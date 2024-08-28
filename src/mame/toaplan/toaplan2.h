// license:BSD-3-Clause
// copyright-holders:Quench, Yochizo, David Haywood
#ifndef MAME_TOAPLAN_TOAPLAN2_H
#define MAME_TOAPLAN_TOAPLAN2_H

#pragma once

#include "cpu/m68000/m68000.h"
#include "machine/bankdev.h"
#include "machine/eepromser.h"
#include "machine/gen_latch.h"
#include "machine/ticket.h"
#include "machine/upd4992.h"

#include "toaplan2_screen.h"
#include "toaplan2_txlayer.h"

#include "sound/okim6295.h"
#include "emupal.h"
#include "screen.h"
#include "tilemap.h"

/**************** Machine stuff ******************/

class toaplan2_state : public driver_device
{
public:
	toaplan2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_shared_ram(*this, "shared_ram")
		, m_mainram(*this, "mainram")
		, m_audiocpu(*this, "audiocpu")
		, m_oki(*this, "oki%u", 1U)
		, m_eeprom(*this, "eeprom")
		, m_gfxdecode(*this, "gfxdecode")
		, m_soundlatch(*this, "soundlatch%u", 1U)
		, m_z80_rom(*this, "audiocpu")
		, m_oki_rom(*this, "oki%u", 1U)
		, m_okibank(*this, "okibank")
	{ }

	optional_shared_ptr<u8> m_shared_ram; // 8 bit RAM shared between 68K and sound CPU
	optional_shared_ptr<u16> m_mainram;

	optional_device<cpu_device> m_audiocpu;
	optional_device_array<okim6295_device, 2> m_oki;
	optional_device<eeprom_serial_93cxx_device> m_eeprom;
	optional_device<gfxdecode_device> m_gfxdecode;
	optional_device_array<generic_latch_8_device, 4> m_soundlatch; // tekipaki, batrider, bgaregga, batsugun

	optional_region_ptr<u8> m_z80_rom;
	optional_region_ptr_array<u8, 2> m_oki_rom;
	optional_memory_bank m_okibank;
};



#endif // MAME_TOAPLAN_TOAPLAN2_H
