
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

#include "kernel/pci/pci.h"
#include "kernel/pci/pci_mmio.h"
#include "kernel/mmio.h"
#include "kernel/logger/logger.h"
#include "kernel/memory.h"
#include "kernel/network/network_task.h"
#include "kernel/network/pdu_handler.h"
#include "kernel/network/ethernet.h"
#include "kernel/network/network_io.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

static e1000_device_t *e1000;

void
write_command(uint16_t address, uint32_t value)
{
  pci_write32(e1000->mem_base + address, value);
}

uint32_t
read_command(uint32_t address)
{
  return pci_read32(e1000->mem_base + address);
}

/*
 * Checks whether the e1000 is using an eeprom and stores it in e1000 object.
 * Writes to the eeprom register first and performs a set of reads to check.
 */
void
detect_eeprom()
{
  write_command(REG_EEPROM, 0x1);

  uint32_t val = 0;
  for (uint16_t i = 0; i < 1000 && !e1000->eeprom_exists; i++)
    {
      val = read_command(REG_EEPROM);
      if (val & 0x10)
        e1000->eeprom_exists = true;
      else
        e1000->eeprom_exists = false;
    }
#ifdef E1000_DEBUG
  LOG_DEBUG("EEPROM exists: %x", e1000->eeprom_exists);
#endif
}

/*
 * Reads the hardware address using the pci bus and stores it in host byte order in
 * the e1000 object.
 */
void
read_e1000_hardware_address()
{
  uint32_t mem_base_mac = e1000->mem_base + MAC_OFFSET;
  uint64_t mac_data = pci_read64(mem_base_mac);
  if (mac_data << 16 != 0)
    {
      mac_address_t mac_network;
      memcpy(&mac_network, &mac_data, 6);
      e1000->mac = ntoh_mac(mac_network);
#ifdef E1000_DEBUG
      LOG_DEBUG("MAC: %s", mac_to_str(e1000->mac));
#endif
    }
}

mac_address_t
e1000_get_mac()
{
  return e1000->mac;
}

/*
 * Initializes the receive descriptors of the e1000 and tells it where to find them.
 * For each descriptor space in the systems memory is allocated, to allow the
 * e1000 to write the packet data into it.
 * IMPORTANT: The addresses have to be 16-byte aligned!
 * See header file for information on the configured settings.
 *
 * A receive descriptor contains information on the received packets like status, checksum and length.
 * The e1000 uses a ring buffer to store the descriptors.
 */
void
init_receive_descriptors()
{
  for (int i = 0; i < E1000_NUM_RX_DESC; i++)
    {
      e1000->rx_descs[i].addr =
        (uint32_t) malloc(MAX_PACKET_SIZE + sizeof(e1000_rx_desc_t));
      e1000->rx_descs[i].status = 0;
    }

  write_command(REG_RXDESCLO, (uint32_t) e1000->rx_descs);
  write_command(REG_RXDESCHI, 0);

  write_command(REG_RXDESCLEN, E1000_NUM_RX_DESC * sizeof(e1000_rx_desc_t));

  write_command(REG_RXDESCHEAD, 0);
  write_command(REG_RXDESCTAIL, E1000_NUM_RX_DESC - 1);
  e1000->rx_cur = 0;
  write_command(REG_RCTRL,
                RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE |
                RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_8192);
}

/*
 * Initializes the transmit descriptors of the e1000 and tells it where to find them.
 * See header file for information on the configured settings.
 *
 * A transmit descriptor contains information on the packets to be transmitted like transmit status.
 * The e1000 uses a ring buffer to store the descriptors.
 */
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
  write_command(REG_TIPG, 0x0060200A);
}

void
enable_interrupt()
{
  // according to E1000 documentation p. 297
  write_command(REG_IMASK, 0x1F2D4);
}

void
start_link(void)
{
  uint32_t flags = read_command(REG_CTRL);
  write_command(REG_CTRL, flags | ECTRL_SLU);
}

void
start_e1000(void)
{
  detect_eeprom();
  read_e1000_hardware_address();
  start_link();
  enable_interrupt();
  init_receive_descriptors();
  init_transmit_descriptors();
}

/*
 * Fills the next transmit buffer with information on the data to send.
 * Tells the e1000 that a new packet is available for send and waits until it is sent.
 */
uint32_t
e1000_send_packet(const void *p_data, uint16_t p_len)
{
  if (!e1000_is_available())
    return -1;
#ifdef E1000_DEBUG
  LOG_DEBUG("Sending packet with len %i", p_len);
#endif
  e1000_tx_desc_t *curr = &(e1000->tx_descs[e1000->tx_cur]);
  curr->addr = (uint32_t) p_data;
  curr->length = p_len;
  curr->cmd = CMD_EOP | CMD_IFCS | CMD_RS;
  curr->status = 0;
  e1000->tx_cur = (e1000->tx_cur + 1) % E1000_NUM_TX_DESC;

  write_command(REG_TXDESCTAIL, e1000->tx_cur);

  while (!(curr->status));
#ifdef E1000_DEBUG
  LOG_DEBUG("Packet send!");
#endif
  return 0;
}

/*
 * Receives all currently available packets from the e1000.
 * Inserts each packet into the network task receives queue to receive them
 * asyncronically, as this method is called in an interrupt_handler.
 * Clears the receive descriptor on receive.
 */
void
receive_packet()
{
  if (!e1000_is_available())
    return;

  uint16_t old_cur;

  while (e1000->rx_descs[e1000->rx_cur].status & 0x1)
    {
      uint8_t *buf =
        (uint8_t *) ((uint32_t) e1000->rx_descs[e1000->rx_cur].addr);
      uint16_t len = e1000->rx_descs[e1000->rx_cur].length;

#ifdef E1000_DEBUG
      LOG_DEBUG("Received packet with length: %i", len);
#endif

      insert_packet(buf, len);

      e1000->rx_descs[e1000->rx_cur].status = 0;
      old_cur = e1000->rx_cur;
      e1000->rx_cur = (e1000->rx_cur + 1) % E1000_NUM_RX_DESC;
      write_command(REG_RXDESCTAIL, old_cur);
    }
}

bool
e1000_is_available(void)
{
  return e1000->pci_device != (void *) 0;
}

void
e1000_irq_handler(void)
{
  // clear interrupt on device
  write_command(REG_IMASK, 0x1);
  uint32_t cause = read_command(REG_INT_CAUSE);
  if (cause & LSC)
    start_link();
  if (cause & RXDMT0)
    ;                           // good threshold
  if (cause & RXT0)
    receive_packet();
}

/*
 * Allocates memory for the e100 and configures it as a pci device.
 * Starts the e1000 on success.
 */
void
e1000_init(void)
{
  e1000 = (e1000_device_t *) malloc(sizeof(e1000_device_t));
  if (e1000 == NULL)
    {
      LOG_ERROR("No memory for e1000 struct left!");
      return;
    }

#ifdef E1000_DEBUG
  LOG_DEBUG("Initializing driver.");
#endif
  e1000->pci_device = pci_get_device(INTEL_VEND, E1000_DEV);

  if (!e1000_is_available())
    {
      LOG_ERROR("Network card not found!");
      return;
    }
#ifdef E1000_DEBUG
  LOG_DEBUG("Network card found!");
#endif

  e1000->mem_base = pci_alloc_memory(e1000->pci_device, 0);
  e1000->io_base = pci_alloc_memory(e1000->pci_device, 1);
  pci_enable_bus_mastering(e1000->pci_device->config_base);
#ifdef E1000_DEBUG
  LOG_DEBUG("Membase: 0x%x iobase: 0x%x", e1000->mem_base, e1000->io_base);
#endif
  start_e1000();
}
