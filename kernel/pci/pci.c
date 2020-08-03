
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

#include "pci.h"

#include "kernel/mmio.h"
#include "kernel/logger/logger.h"

#include <stdio.h>
#include <sys/types.h>
#include <stddef.h>

// https://github.com/autoas/as/blob/master/com/as.infrastructure/arch/versatilepb/bsp/pci.c

pci_device_t pci_devices[MAX_PCI_DEVICES] = { 0 };

/*
 * Enables PCI bus mastering for the device stored at address by enabling
 * the corresponding bits in the PCI_COMMAND register.
 */
void
pci_enable_bus_mastering(uint32_t address)
{
  uint16_t val = read16(address + PCI_COMMAND);
  val |= PCI_COMMAND_MASTER | PCI_COMMAND_IO | PCI_COMMAND_MEMORY;
  write16(address + PCI_COMMAND, val);
}

/*
 * Returns the type of the bar with number `number`.
 */
uint8_t
get_bar_type(uint32_t base, uint8_t number)
{
  return read32(base + PCI_BAR(number)) & 1;
}

/*
 * Returns the requested size for bar number.
 *
 * Refer to page 204 in PCI 2.2 Spec 
 * https://www.ics.uci.edu/~harris/ics216/pci/PCI_22.pdf
 */
uint32_t
get_bar_size(uint32_t base, uint8_t number)
{
  // read value before to restore later
  uint32_t bar_addr = base + PCI_BAR(number);
  uint32_t before = read32(bar_addr);
  // write all 1s to get encoded size information
  write32(bar_addr, 0xFFFFFFFF);
  // read encoded size information
  uint32_t encoded_size = read32(bar_addr);
  // restore old value
  write32(bar_addr, before);

  // I/O space base address register | memory space base address register
  uint32_t bit_mask = (before & 1) ? 0xFFFFFFFC : 0xFFFFFFF0;
  uint32_t decoded_size = (~(encoded_size & bit_mask)) + 1;

  return decoded_size;
}

/*
 * Allocates memory for the device in its bar in the PCI memory space.
 * Currently there is only one pci device thus there is no memory management.
 */
uint32_t
pci_alloc_memory(pci_device_t * device, uint8_t bar)
{
  uint8_t type = get_bar_type(device->config_base, bar);
  uint32_t address = 0;
  if (type)
    {
      // io space
      address = 0x43000000;
    }
  else
    {
      // memory space     
      address = 0x50000000;
    }
  write32(device->config_base + PCI_BAR(bar), address);
  return address;
}

/*
 * Enumerates all pci devices by iterating over all available slots.
 * Writes the found information in the pci_devices array at the corresponding index.
 */
void
enumerate_pci_devices(void)
{
#ifdef PCI_DEBUG
  LOG_DEBUG("Enumerating PCI devices:");
#endif
  for (int i = 11; i < 32; ++i)
    {
      uint32_t device_addr = (PCI_CONFIG + ((i) << PCI_DEVICE_BIT_OFFSET));
      int32_t vendor_id = read16(device_addr + PCI_VENDOR_ID);
      int32_t device_id = read16(device_addr + PCI_DEVICE_ID);

      if (vendor_id == PCI_INVALID_VENDOR)
        continue;

      pci_device_t *device = &pci_devices[i - 11];
      device->config_base = device_addr;
      device->pci_slot_id = i;
      device->vendor_id = vendor_id;
      device->device_id = device_id;

#ifdef PCI_DEBUG
      LOG_DEBUG("Device slot: %i at: 0x%x DeviceID: 0x%x VendorID: 0x%x", i,
                device_addr, vendor_id, device_id);
#endif
    }
}

/*
 * Discovers and configures the PCI core of the board.
 * This is needed before the main PCI probing is performed.
 * Makes sure that accesses to the PCI mapped memory can be translated correctly.
 *
 * Refer to https://developer.arm.com/documentation/dui0224/i/programmer-s-reference/pci-controller/pci-configuration?lang=en
 */
int32_t
configure_board(void)
{
  // Setup imap registers for memory translation
  write32(PCI_IMAP0, 0x44000000 >> 28);
  // non-prefetchable memory
  write32(PCI_IMAP1, 0x50000000 >> 28);
  // prefetchable memory
  write32(PCI_IMAP2, 0x60000000 >> 28);

  uint8_t slot = 0;
  for (int i = 11; i < 32; ++i)
    {
      if (read32((PCI_SELF_CONFIG + (i << PCI_DEVICE_BIT_OFFSET)) +
                 PCI_VENDOR_ID) == VP_PCI_DEV_ID
          && read32((PCI_SELF_CONFIG + (i << PCI_DEVICE_BIT_OFFSET)) +
                    PCI_CLASS_REVISION) == VP_PCI_CLASS_ID)
        {
          slot = i;
          break;
        }
    }
  if (slot == 0)
    {
      LOG_ERROR("Cannot find PCI core!");
      return -1;
    }
  uint32_t pci_config_base = PCI_CONFIG + (slot << PCI_DEVICE_BIT_OFFSET);

  // write id of board
  write32(PCI_SELFID, slot);

  // set command status register to enable initiation
  uint32_t val = read32(pci_config_base + PCI_COMMAND);
  val |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE;
  write32(pci_config_base + PCI_COMMAND, val);

  return 0;
}

/*
 * Iterates over all slots in the pci_devices and returns a pointer to the device
 * identified by the vendor_id and device_id.
 * If the device cannot be found, NULL is returned.
 */
pci_device_t *
pci_get_device(uint16_t vendor_id, uint16_t device_id)
{
  for (uint8_t i = 0; i < MAX_PCI_DEVICES; ++i)
    {
      pci_device_t *device = &pci_devices[i];
      if (device->vendor_id == vendor_id && device->device_id == device_id)
        return device;
    }
  return NULL;
}

/*
 * Initializes the PCI bus by first configuring the board itself and
 * enumerating all PCI devices afterwards.
 */
void
pci_init(void)
{
#ifdef PCI_DEBUG
  LOG_DEBUG("Initiating PCI");
#endif
  configure_board();
  enumerate_pci_devices();
}
