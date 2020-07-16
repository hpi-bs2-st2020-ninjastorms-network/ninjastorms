
/******************************************************************************
 *       ninjastorms - shuriken operating system                              *
 *                                                                            *
 *    Copyright (C) 2013 - 2016  Andreas Grapentin et al.                     *
 *                                                                            *
 *    This program is free software: you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation, either version 3 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ******************************************************************************/
 
#include "e1000.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "kernel/pci/pci.h"
#include "kernel/pci/pci_mmio.h"
#include "kernel/mmio.h"
#include "kernel/logger/logger.h"
#include "kernel/memory.h"
#include "network_task.h"
#include "kernel/network/pdu_handler.h"

// get some memory for our e1000
e1000_device_t* e1000;

void
write_command(uint16_t address, uint32_t value)
{
  pci_write32(e1000->mem_base+address, value);
}

uint32_t
read_command(uint32_t address)
{
  return pci_read32(e1000->mem_base+address);
}

void
detect_eeprom()
{
  write_command(REG_EEPROM, 0x1);

  uint32_t val = 0;
  for(uint16_t i = 0; i < 1000 && ! e1000->eeprom_exists; i++)
    {
      val = read_command(REG_EEPROM);
      if(val & 0x10)
        e1000->eeprom_exists = 1;
      else
        e1000->eeprom_exists = 0;
    }
#ifdef E1000_DEBUG
  printf("[E1000] eeprom exists: %x\n", e1000->eeprom_exists);
#endif
}

uint8_t
read_mac()
{ 
  uint32_t mem_base_mac = e1000->mem_base + MAC_OFFSET;
  if(pci_read32(mem_base_mac) != 0)
    {
      printf("[E1000] MAC ");
      e1000->mac[0] = pci_read8(mem_base_mac);
      printf("%x", e1000->mac[0]);
      for(uint16_t i = 1; i < 6; i++)
        {
          e1000->mac[i] = pci_read8(mem_base_mac + i);
          printf(":%x", e1000->mac[i]);
        }
      printf("\n");
    }
  else
    return 0;
  return 1;
}

void
init_receive_descriptors()
{
  for(int i = 0; i < E1000_NUM_RX_DESC; i++) 
    {
      e1000->rx_descs[i].addr = (uint32_t) malloc(MAX_PACKET_SIZE + sizeof(e1000_rx_desc_t));
      e1000->rx_descs[i].status = 0;
    }

  write_command(REG_RXDESCLO, (uint32_t)e1000->rx_descs);
  write_command(REG_RXDESCHI, 0);

  write_command(REG_RXDESCLEN, E1000_NUM_RX_DESC * sizeof(e1000_rx_desc_t));

  write_command(REG_RXDESCHEAD, 0);
  write_command(REG_RXDESCTAIL, E1000_NUM_RX_DESC-1);
  e1000->rx_cur = 0;
  write_command(REG_RCTRL, RCTL_EN | RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_8192);

}

void
init_transmit_descriptors()
{
  write_command(REG_TXDESCLO, (uint32_t) e1000->tx_descs);
  write_command(REG_TXDESCHI, 0);

  //now setup total length of descriptors

  write_command(REG_TXDESCLEN, E1000_NUM_TX_DESC * sizeof(e1000_tx_desc_t));

  //setup numbers
  write_command(REG_TXDESCHEAD, 0);
  write_command(REG_TXDESCTAIL, 0);
  e1000->tx_cur = 0;
  // works with e1000 only
  write_command(REG_TCTRL, read_command(REG_TCTRL) | TCTL_EN | TCTL_PSP);
  write_command(REG_TIPG,  0x0060200A);
}

void
enable_interrupt()
{
  // according to E1000 documentation p. 297
  write_command(REG_IMASK, 0x1F2D4);
}

void start_link(void)
{
  uint32_t flags = read_command(REG_CTRL);
  write_command(REG_CTRL, flags | ECTRL_SLU);
}

void
irq_handler_e1000(void)
{
  // clear interrupt on device
  write_command(REG_IMASK, 0x1);
  uint32_t cause = read_command(REG_INT_CAUSE);
  if (cause & LSC)
    start_link();
  if (cause & RXDMT0)
    ; // good threshold
  if (cause & RXT0)
    receive_packet();
}

void
start_e1000(void)
{
  detect_eeprom();
  read_mac();
  start_link();
  enable_interrupt();
  init_receive_descriptors();
  init_transmit_descriptors();
}

uint32_t
send_packet(const void *p_data, uint16_t p_len)
{
  if(!is_e1000_available()) return -1;

  printf("[E1000] Sending packet data <%s> with len %i\n", p_data, p_len);
  e1000_tx_desc_t *curr = &(e1000->tx_descs[e1000->tx_cur]);
  curr->addr = (uint32_t) p_data;
  curr->length = p_len;
  curr->cmd = CMD_EOP | CMD_IFCS | CMD_RS;
  curr->status = 0;
  uint8_t old_cur = e1000->tx_cur;
  e1000->tx_cur = (e1000->tx_cur + 1) % E1000_NUM_TX_DESC;

  write_command(REG_TXDESCTAIL, e1000->tx_cur);

  while(!(curr->status));
  printf("[E1000] Packet send!\n"); 
  return 0;
}

void
receive_packet()
{
  if(!is_e1000_available()) return;
  
  uint16_t old_cur;

  while(e1000->rx_descs[e1000->rx_cur].status & 0x1)
    {
      uint8_t* buf = (uint8_t *) ((uint32_t) e1000->rx_descs[e1000->rx_cur].addr);
      uint16_t len = e1000->rx_descs[e1000->rx_cur].length;

      log_debug("received packet with length: %i", len)

      insert_packet(buf, len);

      e1000->rx_descs[e1000->rx_cur].status = 0;
      old_cur = e1000->rx_cur;
      e1000->rx_cur = (e1000->rx_cur + 1) % E1000_NUM_RX_DESC;
      write_command(REG_RXDESCTAIL, old_cur);
    }
}

uint8_t
is_e1000_available(void)
{
  return e1000->pci_device != (void*) 0;
}

void
init_e1000(void)
{
  e1000 = (e1000_device_t*) malloc(sizeof(e1000_device_t));
  if(e1000 == NULL)
    {
      printf("[E1000] No memory for e1000 struct left!\n");
      return;
    }

  printf("[E1000] Initializing driver.\n");
  e1000->pci_device = get_pci_device(INTEL_VEND, E1000_DEV);
  
  if(!is_e1000_available())
    {
      printf("[E1000] Network card not found!\n");
      return;
    }
  printf("[E1000] Network card found!\n");

  e1000->mem_base = alloc_pci_memory(e1000->pci_device, 0);
  e1000->io_base = alloc_pci_memory(e1000->pci_device, 1);
  enable_bus_mastering(e1000->pci_device->config_base);
  printf("[E1000] membase: 0x%x iobase: 0x%x\n", e1000->mem_base, e1000->io_base);
  start_e1000();
}
