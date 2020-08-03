
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

#pragma once

#include <sys/types.h>

// #define PCI_DEBUG

#define PCI_SELF_CONFIG 0x41000000
#define PCI_CONFIG 0x42000000
#define PCI_DEVICE_BIT_OFFSET 11
#define PCI_DEVICE_STATUS_COMMAND_OFFSET 0x04

#define PCI_VENDOR_ID   0x00    /* 16 bits */
#define PCI_DEVICE_ID   0x02    /* 16 bits */
#define PCI_COMMAND   0x04      /* 16 bits */
#define  PCI_COMMAND_IO   0x1   /* Enable response in I/O space */
#define  PCI_COMMAND_MEMORY 0x2 /* Enable response in Memory space */
#define  PCI_COMMAND_MASTER 0x4 /* Enable bus mastering */
#define  PCI_COMMAND_SPECIAL  0x8       /* Enable response to special cycles */
#define  PCI_COMMAND_INVALIDATE 0x10    /* Use memory write and invalidate */
#define  PCI_COMMAND_VGA_PALETTE 0x20   /* Enable palette snooping */
#define  PCI_COMMAND_PARITY 0x40        /* Enable parity checking */
#define  PCI_COMMAND_WAIT 0x80  /* Enable address/data stepping */
#define  PCI_COMMAND_SERR 0x100 /* Enable SERR */
#define  PCI_COMMAND_FAST_BACK  0x200   /* Enable back-to-back writes */
#define  PCI_COMMAND_INTX_DISABLE 0x400 /* INTx Emulation Disable */

#define PCI_CLASS_REVISION  0x08        /* High 24 bits are class, low 8 revision */
#define PCI_REVISION_ID   0x08  /* Revision ID */
#define PCI_CLASS_PROG    0x09  /* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE  0x0a  /* Device class */
/*
 * Base addresses specify locations in memory or I/O space.
 * Decoded size can be determined by writing a value of
 * 0xffffffff to the register, and reading it back.  Only
 * 1 bits are decoded.
 */
#define PCI_BASE_ADDRESS_0  0x10        /* 32 bits */
#define PCI_BAR_OFFSET 0x04

#define PCI_BAR(x) (PCI_BASE_ADDRESS_0 + (x)*(4))       // 0 <= x <= 5

/* 0x35-0x3b are reserved */
#define PCI_INTERRUPT_LINE  0x3c        /* 8 bits */
#define PCI_INTERRUPT_PIN 0x3d  /* 8 bits */
#define PCI_MIN_GNT   0x3e      /* 8 bits */
#define PCI_MAX_LAT   0x3f      /* 8 bits */


// PCI controller registers
// see Table 4.50 http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0224i/Cacdijji.html
#define SYS_PCICTL 0x10000044
#define PCI_IMAP0  0x10001000
#define PCI_IMAP1  0x10001004
#define PCI_IMAP2  0x10001008
#define PCI_SELFID 0x1000100C
#define PCI_FLAGS  0x10001010
#define PCI_SMAP0  0x10001014
#define PCI_SMAP1  0x10001018
#define PCI_SMAP2  0x1000101C

#define MAX_PCI_DEVICES 21

#define VP_PCI_DEV_ID 0x030010ee
#define VP_PCI_CLASS_ID 0x0b400000

#define PCI_INVALID_VENDOR 0xFFFF

typedef struct
{
  uint32_t config_base;
  uint32_t mem_base;
  uint32_t io_base;
  uint8_t pci_slot_id;
  uint16_t vendor_id;
  uint16_t device_id;
} pci_device_t;


void pci_enable_bus_mastering(uint32_t address);
uint32_t pci_alloc_memory(pci_device_t * device, uint8_t bar);
pci_device_t *pci_get_device(uint16_t vendor_id, uint16_t device_id);
void pci_init(void);
