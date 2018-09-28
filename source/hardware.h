#ifndef HARDWARE_H
#define HARDWARE_H

#include <nds.h>
#include <fat.h>
#include <sys/unistd.h>

#define RS_BACKUP 0x4b434142

extern uint32 boot;

extern bool flash_card;

struct dsCardData
{
	uint32 data[4];
	char name[12];
};

void do_dump_nds_save_stage_1();
void do_dump_nds_save_stage_2(int size);
void do_restore_nds_save();

bool swap_cart(bool allow_cancel = false);
//u32 get_slot1_type();

// This function was previously found in the main function.
// Return values:
//  0 - no special configuration, WiFi mode
//  1 - GBA game in Slot 2
//  2 - EZFlash 3in1 in Slot 2
//  3 - running in DSi mode
//  4 - running from Slot 2 flash card
//  5 - running from download play or another exploit that does not need a flash card in Slot 1
//
u32 hwDetect();

void hwBackup3in1();
void hwDump3in1(uint32 size, const char *gamename);
void hwRestore3in1();
void hwRestore3in1_b(uint32 size_file);
void hwErase();

void hwBackupDSi();
void hwRestoreDSi();

void hwBackupFTP(bool dlp = false);
void hwRestoreFTP(bool dlp = false);

void hwBackupGBA(u8 type);
void hwRestoreGBA();
void hwEraseGBA();

void hwBackupSlot2();
void hwRestoreSlot2();

void hwFormatNor(uint32 page, uint32 count);

uint32 hwGrab3in1();
void hwRelease3in1(uint32 ime);

void writeGBAtoDS(u8* whatthefuck);

#endif