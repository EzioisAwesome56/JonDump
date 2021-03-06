// adapted from github page listed in gba.harderr
#include <nds.h>
#include <fat.h>
#include <nds/arm9/dldi.h>

#include <nds/interrupts.h>
#include <nds/arm9/console.h>
#include <sys/dir.h>
#include <sys/unistd.h>
#include <dswifi9.h>
#include <nds/card.h>

#include <stdio.h>
#include <algorithm>

#include "dsCard.h"
#include "gba.h"
#include "globals.h"

#include "hardware.h"
// reinclude this
#include "auxspi.h"

using namespace std;

static u32 pitch = 0x40000;

// DS save game loader
// Loads gba save from ds card
void loadDSSave(){
	uint8 type = auxspi_save_type(slot_1_type);
	u8 *uwat;
	uwat = (u8*)malloc(size_buf);
	// get save size of inserted gba game
	uint32 size = gbaGetSaveSize(type);
	iprintf("preparing to read save...\n");
	int size_blocks = 1 << max(0, (int8(size) - 18)); // ... in units of 0x40000 bytes - that's 256 kB
	if (size < 16){
		size_blocks = 1;
	} else {
		size_blocks = 1 << (size - 16);
	}
	u32 LEN = min(1 << size, 1 << 16);
	for (int i = 0; i < size_blocks; i++) {
		auxspi_read_data(i << 8, data, LEN, type, slot_1_type);
		*uwat = *uwat + *data;
	}
	// then just store everything into data again
	*data = *uwat;
	iprintf("Preparing to write...\n");
	// and then restore the save to the gba cart
	hwRestoreGBA();
	iprintf("Done!\n");
	// and thats all folks!
}

// Work in progress
// GBA rom Dumper
void DumpGBARom(){
	bool pressa = false;
	// get the size of the rom
	u32 size = getGameSize();
	iprintf("Detected gane size:\n%i\n", size);
	// set the intial address
	u8 gbarom = 0x08000000;
	
	s32 add = 0x00000000;
	// keep doing it until the game is no more
	while (size > 0){
		// message that tells the user to epic switch
		iprintf("Please insert a DS gamecard\nThat can hold 512kb of save data\nand press A to dump\n");
		iprintf("If you have already used a card\nplease dump its save on a Dsi or 3ds\nand reinsert it\n");
		while (!pressa){
			// wait a very very long time
			swiWaitForVBlank();
			scanKeys();
			if (keysHeld() & KEY_A){
				// break out of the loop
				pressa = true;
			}
		}
		// reset the flag
		pressa = false;
		// then get to dumping
		iprintf("Reading 512kb...\n");
		// then dump it, 512kb at a time baby!
		// (void*)0x08000000
		// clear out data maybe?
		data = 0;
		iprintf("current offset: %X\n", 0x08000000 + add);
		memcpy(data, (void*)(0x08000000 + add), 524288);
		// write the dumped rom data to the ds game card
		iprintf("Writing 512kb...\n");
		writeGBAtoDS(data);
		size = size - 524288;
		// add a bunch of numbers to the offset
		add = add + 0x80000;
	}
}

// some form of writer thingy
void writeGBAtoDS(u8* whatthefuck) {
	slot_1_type = auxspi_has_extra();
	uint8 size = auxspi_save_size_log_2(slot_1_type);
	int size_blocks = 1 << max(0, (int8(size) - 18)); // ... in units of 0x40000 bytes - that's 256 kB
	uint8 type = auxspi_save_type(slot_1_type);
	// format game if required
	if (type == 3) {
		//displayStateF(STR_HW_FORMAT_GAME);
		auxspi_erase(slot_1_type);
	}
	// alright, lets right this shit to the ds game!
	//displayStateF(STR_HW_WRITE_GAME);
	u32 LEN = 0, num_blocks = 0, shift = 0;
	switch (type) {
	case 1:
		shift = 4; // 16 bytes
		break;
	case 2:
		shift = 5; // 32 bytes
		break;
	case 3:
		shift = 8; // 256 bytes
		break;
	default:
		return;
	}
	LEN = 1 << shift;
	num_blocks = 1 << (size - shift);
	for (unsigned int i = 0; i < num_blocks; i++) {
		if (i % (num_blocks >> 6) == 0)
			//displayProgressBar(i+1, num_blocks);
		sysSetBusOwners(true, true);
		auxspi_write_data(i << shift, whatthefuck, LEN, type, slot_1_type);
	}
}


// ---------------------------------------------------------------------
u8 log2trunc(u32 size0)
{	
	u8 size = 1;
	while (size0 > (u32)(1 << size)) {
		size++;
	}
	return size;
}


// This function is called on boot; it detects the hardware configuration and selects the mode.
u32 hwDetect()
{
	
	// Okay, maybe it is a regular GBA game instead
	// make sure its a gba cart
	if (gbaIsGame())
		return 1;
	
	// Nothing unique found, so enter WiFi mode
	return 0;
}

// --------------------------------------------------------
// uncomment this to enable slot 1 operations... but implement "dsiUnlockSlot1()" before!
// the following two functions are *untested* and may not work at all!
//#define SLOT_1_UNLOCKED

void hwBackupGBA(u8 type)
{
	if ((type == 0) || (type > 5))
		return;

	if ((type == 1) || (type == 2)) {
		// This is not to be translated, it will be removed at some point.
		iprintf("Yo i cant fucking read this shit!\n");
		return;
	}
	
	// we arent going to be saving to a file now, we have no where to go!
	// will instead save to another console later
	
	//char path[256];
	//char fname[256] = "";
	//fileSelect("/", path, fname, 0, true, false);
	
	/*if (!fname[0]) {
		find_unused_filename((char*)0x080000a0, path, fname);
	}
	char fullpath[512];
	sprintf(fullpath, "%s/%s", path, fname);*/
	
	uint32 size = gbaGetSaveSize(type);
	iprintf("SAVE SIZE: %x\n", size);
	iprintf("Dumping save...\n");
	gbaReadSave(data, 0, size, type);
	
	/*//displayStateF(STR_HW_WRITE_FILE, fullpath);
	FILE *file = fopen(fullpath, "wb");
	fwrite(data, 1, size, file);
	fclose(file);*/

	iprintf("Save dumped!\n");
	//while(1);
}

// we will be reusing the shit below at a later date.
void hwRestoreGBA()
{
	u8 type = gbaGetSaveType();
	if ((type == 0) || (type > 5))
		return;
	
	if ((type == 1) || (type == 2)) {
		// This is not to be translated, it will be removed at some point.
		iprintf("I can't write this save type\nyet. Please use Rudolphs tool\ninstead.");
		return;
	}
	
	uint32 size = gbaGetSaveSize(type);
	
	// we dont need this!
	/*char path[256];
	char fname[256] = "";
	fileSelect("/", path, fname, 0);
	char fullpath[512];
	sprintf(fullpath, "%s/%s", path, fname);*/

	//displayStateF(STR_HW_READ_FILE, fname);
	/*FILE *file = fopen(fullpath, "rb");
	fread(data, 1, size, file);
	fclose(file);*/
	
	if ((type == 4) || (type == 5)) {
		//displayStateF(STR_HW_FORMAT_GAME);
		gbaFormatSave(type);
	}
	
	//displayStateF(STR_HW_WRITE_GAME);
	gbaWriteSave(0, data, size, type);

	// i dont like this line, so i commented it out
	//displayStateF(STR_STR, "Done!");

}
/*
void hwEraseGBA()
{
	u8 type = gbaGetSaveType();
	if ((type == 0) || (type > 5))
		return;

	//displayStateF(STR_HW_WARN_DELETE);
	while (!(keysCurrent() & (KEY_UP | KEY_R | KEY_Y))) {};
	gbaFormatSave(type);
	//displayStateF(STR_HW_DID_DELETE);
	while (1);

}*/