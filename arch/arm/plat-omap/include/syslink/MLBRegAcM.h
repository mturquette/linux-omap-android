/*
 * MLBRegAcM.h
 *
 * DSP-BIOS Bridge driver support functions for TI OMAP processors.
 *
 * Copyright (C) 2007 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _MLB_REG_ACM_H
#define _MLB_REG_ACM_H

#include <syslink/GlobalTypes.h>
#include <syslink/EasiGlobal.h>
#include <syslink/MLBAccInt.h>

#if defined(USE_LEVEL_1_MACROS)

#define MLBMAILBOX_SYSCONFIGReadRegister32(base_address)\
(_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_SYSCONFIGReadRegister32),\
RD_MEM_32_VOLATILE(((u32)(base_address))+ \
MLB_MAILBOX_SYSCONFIG_OFFSET))


#define MLBMAILBOX_SYSCONFIGWriteRegister32(base_address, value)\
do {\
	const u32 offset = MLB_MAILBOX_SYSCONFIG_OFFSET;\
	register u32 newValue = ((u32)(value));\
	_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_SYSCONFIGWriteRegister32);\
	WR_MEM_32_VOLATILE(((u32)(base_address))+offset, newValue);\
} while (0)


#define MLBMAILBOX_SYSCONFIGSIdleModeRead32(base_address)\
(_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_SYSCONFIGSIdleModeRead32),\
(((RD_MEM_32_VOLATILE((((u32)(base_address))+\
(MLB_MAILBOX_SYSCONFIG_OFFSET)))) &\
MLB_MAILBOX_SYSCONFIG_SIdleMode_MASK) >>\
MLB_MAILBOX_SYSCONFIG_SIdleMode_OFFSET))


#define MLBMAILBOX_SYSCONFIGSIdleModeWrite32(base_address, value)\
do {\
	const u32 offset = MLB_MAILBOX_SYSCONFIG_OFFSET;\
	register u32 data = RD_MEM_32_VOLATILE(((u32)(base_address)) +\
				offset);\
	register u32 newValue = ((u32)(value));\
	_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_SYSCONFIGSIdleModeWrite32);\
	data &= ~(MLB_MAILBOX_SYSCONFIG_SIdleMode_MASK);\
	newValue <<= MLB_MAILBOX_SYSCONFIG_SIdleMode_OFFSET;\
	newValue &= MLB_MAILBOX_SYSCONFIG_SIdleMode_MASK;\
	newValue |= data;\
	WR_MEM_32_VOLATILE((u32)(base_address)+offset, newValue);\
} while (0)


#define MLBMAILBOX_SYSCONFIGSoftResetWrite32(base_address, value)\
do {\
	const u32 offset = MLB_MAILBOX_SYSCONFIG_OFFSET;\
	register u32 data =\
	RD_MEM_32_VOLATILE(((u32)(base_address))+offset);\
	register u32 newValue = ((u32)(value));\
	printk(KERN_ALERT "In SYSCONFIG MACOR line %i    file %s",  \
	__LINE__, __FILE__);\
	_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_SYSCONFIGSoftResetWrite32);\
	printk(KERN_ALERT "******************BEFORE DATA WRITE");\
	data &= ~(MLB_MAILBOX_SYSCONFIG_SoftReset_MASK);\
	printk(KERN_ALERT "line %i    file %s", __LINE__, __FILE__);\
	newValue <<= MLB_MAILBOX_SYSCONFIG_SoftReset_OFFSET;\
	newValue &= MLB_MAILBOX_SYSCONFIG_SoftReset_MASK;\
	newValue |= data;\
	printk(KERN_ALERT "line %i    file %s", __LINE__, __FILE__);\
	WR_MEM_32_VOLATILE((u32)(base_address)+offset, newValue);\
	printk(KERN_ALERT "line %i    file %s", __LINE__, __FILE__);\
} while (0)


#define MLBMAILBOX_SYSCONFIGAutoIdleRead32(base_address)\
(_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_SYSCONFIGAutoIdleRead32),\
(((RD_MEM_32_VOLATILE((((u32)(base_address))+\
(MLB_MAILBOX_SYSCONFIG_OFFSET)))) &\
MLB_MAILBOX_SYSCONFIG_AutoIdle_MASK) >>\
MLB_MAILBOX_SYSCONFIG_AutoIdle_OFFSET))


#define MLBMAILBOX_SYSCONFIGAutoIdleWrite32(base_address, value)\
{\
	const u32 offset = MLB_MAILBOX_SYSCONFIG_OFFSET;\
	register u32 data =\
	RD_MEM_32_VOLATILE(((u32)(base_address))+offset);\
	register u32 newValue = ((u32)(value));\
	_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_SYSCONFIGAutoIdleWrite32);\
	data &= ~(MLB_MAILBOX_SYSCONFIG_AutoIdle_MASK);\
	newValue <<= MLB_MAILBOX_SYSCONFIG_AutoIdle_OFFSET;\
	newValue &= MLB_MAILBOX_SYSCONFIG_AutoIdle_MASK;\
	newValue |= data;\
	WR_MEM_32_VOLATILE((u32)(base_address)+offset, newValue);\
}


#define MLBMAILBOX_SYSSTATUSResetDoneRead32(base_address)\
(_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_SYSSTATUSResetDoneRead32),\
(((RD_MEM_32_VOLATILE((((u32)(base_address))+\
(MLB_MAILBOX_SYSSTATUS_OFFSET)))) &\
MLB_MAILBOX_SYSSTATUS_ResetDone_MASK) >>\
MLB_MAILBOX_SYSSTATUS_ResetDone_OFFSET))


#define MLBMAILBOX_MESSAGE___0_15ReadRegister32(base_address, bank)\
(_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_MESSAGE___0_15ReadRegister32),\
RD_MEM_32_VOLATILE(((u32)(base_address))+\
(MLB_MAILBOX_MESSAGE___REGSET_0_15_OFFSET +\
MLB_MAILBOX_MESSAGE___0_15_OFFSET+(\
(bank)*MLB_MAILBOX_MESSAGE___REGSET_0_15_STEP))))


#define MLBMAILBOX_MESSAGE___0_15WriteRegister32(base_address, bank, value)\
do {\
	const u32 offset = MLB_MAILBOX_MESSAGE___REGSET_0_15_OFFSET +\
	MLB_MAILBOX_MESSAGE___0_15_OFFSET +\
	((bank)*MLB_MAILBOX_MESSAGE___REGSET_0_15_STEP);\
	register u32 newValue = ((u32)(value));\
	_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_MESSAGE___0_15WriteRegister32);\
	WR_MEM_32_VOLATILE(((u32)(base_address))+offset, newValue);\
} while (0)


#define MLBMAILBOX_FIFOSTATUS___0_15ReadRegister32(base_address, bank)\
(_DEBUG_LEVEL_1_EASI(\
EASIL1_MLBMAILBOX_FIFOSTATUS___0_15ReadRegister32),\
RD_MEM_32_VOLATILE(((u32)(base_address))+\
(MLB_MAILBOX_FIFOSTATUS___REGSET_0_15_OFFSET +\
MLB_MAILBOX_FIFOSTATUS___0_15_OFFSET+\
((bank)*MLB_MAILBOX_FIFOSTATUS___REGSET_0_15_STEP))))


#define MLBMAILBOX_FIFOSTATUS___0_15FifoFullMBmRead32(base_address, bank)\
(_DEBUG_LEVEL_1_EASI(\
EASIL1_MLBMAILBOX_FIFOSTATUS___0_15FifoFullMBmRead32),\
(((RD_MEM_32_VOLATILE(((u32)(base_address))+\
(MLB_MAILBOX_FIFOSTATUS___REGSET_0_15_OFFSET +\
MLB_MAILBOX_FIFOSTATUS___0_15_OFFSET+\
((bank)*MLB_MAILBOX_FIFOSTATUS___REGSET_0_15_STEP)))) &\
MLB_MAILBOX_FIFOSTATUS___0_15_FifoFullMBm_MASK) >>\
MLB_MAILBOX_FIFOSTATUS___0_15_FifoFullMBm_OFFSET))


#define MLBMAILBOX_MSGSTATUS___0_15NbOfMsgMBmRead32(base_address, bank)\
(_DEBUG_LEVEL_1_EASI(\
EASIL1_MLBMAILBOX_MSGSTATUS___0_15NbOfMsgMBmRead32),\
(((RD_MEM_32_VOLATILE(((u32)(base_address))+\
(MLB_MAILBOX_MSGSTATUS___REGSET_0_15_OFFSET +\
MLB_MAILBOX_MSGSTATUS___0_15_OFFSET+\
((bank)*MLB_MAILBOX_MSGSTATUS___REGSET_0_15_STEP)))) &\
MLB_MAILBOX_MSGSTATUS___0_15_NbOfMsgMBm_MASK) >>\
MLB_MAILBOX_MSGSTATUS___0_15_NbOfMsgMBm_OFFSET))


#define MLBMAILBOX_IRQSTATUS___0_3ReadRegister32(base_address, bank)\
(_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_IRQSTATUS___0_3ReadRegister32),\
RD_MEM_32_VOLATILE(((u32)(base_address))+\
(MLB_MAILBOX_IRQSTATUS___REGSET_0_3_OFFSET +\
MLB_MAILBOX_IRQSTATUS___0_3_OFFSET+\
((bank)*MLB_MAILBOX_IRQSTATUS___REGSET_0_3_STEP))))


#define MLBMAILBOX_IRQSTATUS___0_3WriteRegister32(base_address, bank, value)\
do {\
	const u32 offset = MLB_MAILBOX_IRQSTATUS___REGSET_0_3_OFFSET +\
	MLB_MAILBOX_IRQSTATUS___0_3_OFFSET +\
	((bank)*MLB_MAILBOX_IRQSTATUS___REGSET_0_3_STEP);\
	register u32 newValue = ((u32)(value));\
	_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_IRQSTATUS___0_3WriteRegister32);\
	WR_MEM_32_VOLATILE(((u32)(base_address))+offset, newValue);\
} while (0)


#define MLBMAILBOX_IRQENABLE___0_3ReadRegister32(base_address, bank)\
(_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_IRQENABLE___0_3ReadRegister32),\
RD_MEM_32_VOLATILE(((u32)(base_address))+\
(MLB_MAILBOX_IRQENABLE___REGSET_0_3_OFFSET +\
MLB_MAILBOX_IRQENABLE___0_3_OFFSET+\
((bank)*MLB_MAILBOX_IRQENABLE___REGSET_0_3_STEP))))


#define MLBMAILBOX_IRQENABLE___0_3WriteRegister32(base_address, bank, value)\
do {\
	const u32 offset = MLB_MAILBOX_IRQENABLE___REGSET_0_3_OFFSET +\
	MLB_MAILBOX_IRQENABLE___0_3_OFFSET +\
	((bank)*MLB_MAILBOX_IRQENABLE___REGSET_0_3_STEP);\
	register u32 newValue = ((u32)(value));\
	_DEBUG_LEVEL_1_EASI(EASIL1_MLBMAILBOX_IRQENABLE___0_3WriteRegister32);\
	WR_MEM_32_VOLATILE(((u32)(base_address))+offset, newValue);\
} while (0)


#endif	/* USE_LEVEL_1_MACROS */

#endif /* _MLB_REG_ACM_H */
