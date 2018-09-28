// borrowed from ds save manager

#ifndef SPI_BUS_H
#define SPI_BUS_H

#include <nds.h>

// This is a handy typedef for nonstandard SPI buses, i.e. games with some
//  extra hardware on the cartridge. The following values mean:
// AUXSPI_DEFAULT: A regular game with no exotic hardware.
// AUXSPI_INFRARED: A game with an infrared transceiver.
//  Games known to use this hardware:
//  - Personal Trainer: Walking (aka Laufrhytmus DS, Walk With Me, ...)
//  - Pokemon HeartGold/SoulSilver/Black/White
// AUXSPI_BBDX: A game with what seems to be an extra protection against reading
//  out the chip. Exclusively found on Band Brothers DX.
// AUXSPI_BLUETOOTH: A game with a Bluetooth transceiver. The only game using this
//  hardware is Pokemon Typing DS.
//
// NOTE: This library does *not* support BBDX (I do have the game, but did not find the
//  time to reverse engineer this; besides, a separate homebrew for this game already exists),
//  and also *not* BLUETOOTH (the game is Japan-only, and I am from Europe. Plus I can't
//  read Japanese. And it is unlikely that this game will ever make it here.)
//
typedef enum {
	AUXSPI_DEFAULT,
	AUXSPI_INFRARED,
	AUXSPI_BBDX,
	AUXSPI_BLUETOOTH,
	AUXSPI_FLASH_CARD = 999
} auxspi_extra;

// These functions reimplement relevant parts of "card.cpp", in a way that is easier to modify.
uint8 auxspi_save_type(auxspi_extra extra = AUXSPI_DEFAULT);
uint32 auxspi_save_size(auxspi_extra extra = AUXSPI_DEFAULT);
uint8 auxspi_save_size_log_2(auxspi_extra extra = AUXSPI_DEFAULT);
uint32 auxspi_save_jedec_id(auxspi_extra extra = AUXSPI_DEFAULT);
uint8 auxspi_save_status_register(auxspi_extra extra = AUXSPI_DEFAULT);
void auxspi_read_data(uint32 addr, uint8* buf, uint16 cnt, uint8 type = 0,auxspi_extra extra = AUXSPI_DEFAULT);
void auxspi_write_data(uint32 addr, uint8 *buf, uint16 cnt, uint8 type = 0,auxspi_extra extra = AUXSPI_DEFAULT);
void auxspi_erase(auxspi_extra extra = AUXSPI_DEFAULT);
void auxspi_erase_sector(u32 sector, auxspi_extra extra = AUXSPI_DEFAULT);

// These functions are used to identify exotic hardware.
auxspi_extra auxspi_has_extra();
//bool auxspi_has_infrared();

void auxspi_disable_extra(auxspi_extra extra = AUXSPI_DEFAULT);
void auxspi_disable_infrared();
void auxspi_disable_big_protection();

// The following function returns true if this is a type 3 save (big saves, Flash memory), but
//  the JEDEC ID is not known.
bool auxspi_is_unknown_type3(auxspi_extra extra = AUXSPI_DEFAULT);

#endif