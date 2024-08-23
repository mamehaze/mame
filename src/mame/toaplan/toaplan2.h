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

#include "gp9001.h"
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
		, m_maincpu(*this, "maincpu")
		, m_audiocpu(*this, "audiocpu")
		, m_vdp(*this, "gp9001_%u", 0U)
		, m_oki(*this, "oki%u", 1U)
		, m_eeprom(*this, "eeprom")
		, m_gfxdecode(*this, "gfxdecode")
		, m_screen(*this, "screen")
		, m_palette(*this, "palette")
		, m_soundlatch(*this, "soundlatch%u", 1U)
		, m_z80_rom(*this, "audiocpu")
		, m_oki_rom(*this, "oki%u", 1U)
		, m_okibank(*this, "okibank")
	{ }

	void batsugun(machine_config &config);
	void batsugunbl(machine_config &config);
	void dogyuun(machine_config &config);
	void dogyuunto(machine_config &config);
	void enmadaio(machine_config &config);
	void kbash(machine_config &config);
	void kbash2(machine_config &config);
	void pipibibs(machine_config &config);
	void pipibibsbl(machine_config &config);
	void snowbro2(machine_config &config);
	void snowbro2b3(machine_config &config);
	void tekipaki(machine_config &config);
	void vfive(machine_config &config);

	void init_dogyuun();
	void init_enmadaio();
	void init_fixeightbl();
	void init_pipibibsbl();
	void init_vfive();

	int c2map_r();

protected:
	// We encode priority with colour in the tilemaps, so need a larger palette
	static constexpr unsigned T2PALETTE_LENGTH = 0x10000;

	optional_shared_ptr<u8> m_shared_ram; // 8 bit RAM shared between 68K and sound CPU
	optional_shared_ptr<u16> m_mainram;

	required_device<m68000_base_device> m_maincpu;
	optional_device<cpu_device> m_audiocpu;
	optional_device_array<gp9001vdp_device, 2> m_vdp;
	optional_device_array<okim6295_device, 2> m_oki;
	optional_device<eeprom_serial_93cxx_device> m_eeprom;
	optional_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	optional_device_array<generic_latch_8_device, 4> m_soundlatch; // tekipaki, batrider, bgaregga, batsugun

	optional_region_ptr<u8> m_z80_rom;
	optional_region_ptr_array<u8, 2> m_oki_rom;
	optional_memory_bank m_okibank;

	u8 m_sound_reset_bit = 0; /* 0x20 for dogyuun/batsugun, 0x10 for vfive, 0x08 for fixeight */

	bitmap_ind8 m_custom_priority_bitmap;
	bitmap_ind16 m_secondary_render_bitmap;

	u16 video_count_r();
	void coin_w(u8 data);
	void coin_sound_reset_w(u8 data);
	u8 shared_ram_r(offs_t offset);
	void shared_ram_w(offs_t offset, u8 data);
	void sound_reset_w(u8 data);
	void fixeightbl_oki_bankswitch_w(u8 data);

	template<int Chip> void oki_bankswitch_w(u8 data);
	void enmadaio_oki_bank_w(offs_t offset, u16 data, u16 mem_mask = ~0);

	DECLARE_VIDEO_START(toaplan2);
	DECLARE_VIDEO_START(batsugunbl);

	// Teki Paki sound
	u8 tekipaki_cmdavailable_r();

	u32 screen_update_toaplan2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_dogyuun(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_batsugun(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void screen_vblank(int state);
	void cpu_space_fixeightbl_map(address_map &map);
	void cpu_space_pipibibsbl_map(address_map &map);

	void toaplan2_reset(int state);

	void batsugun_68k_mem(address_map &map);
	void batsugunbl_68k_mem(address_map &map);
	void dogyuun_68k_mem(address_map &map);
	void dogyuunto_68k_mem(address_map &map);
	void dogyuunto_sound_z80_mem(address_map &map);
	void enmadaio_68k_mem(address_map &map);
	void enmadaio_oki(address_map &map);
	void fixeightbl_oki(address_map &map);
	void hd647180_io_map(address_map &map);
	void kbash2_68k_mem(address_map &map);
	void kbash_68k_mem(address_map &map);
	void kbash_v25_mem(address_map &map);
	void pipibibi_bootleg_68k_mem(address_map &map);
	void pipibibs_68k_mem(address_map &map);
	void pipibibs_sound_z80_mem(address_map &map);
	void snowbro2_68k_mem(address_map &map);
	void snowbro2b3_68k_mem(address_map &map);
	void tekipaki_68k_mem(address_map &map);
	void v25_mem(address_map &map);
	void vfive_68k_mem(address_map &map);
	void vfive_v25_mem(address_map &map);
};

// with paddle
class ghox_state : public toaplan2_state
{
public:
	ghox_state(const machine_config &mconfig, device_type type, const char *tag)
		: toaplan2_state(mconfig, type, tag)
		, m_io_pad(*this, "PAD%u", 1U)
	{ }

	void ghox(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

private:
	required_ioport_array<2> m_io_pad;

	s8 m_old_paddle_h[2] = {0};

	template<unsigned Which> u16 ghox_h_analog_r();

	void ghox_68k_mem(address_map &map);
	void ghox_hd647180_mem_map(address_map &map);
};


// with RTC, hopper
class pwrkick_state : public toaplan2_state
{
public:
	pwrkick_state(const machine_config &mconfig, device_type type, const char *tag)
		: toaplan2_state(mconfig, type, tag)
		, m_rtc(*this, "rtc")
		, m_hopper(*this, "hopper")
	{ }

	void othldrby(machine_config &config);
	void pwrkick(machine_config &config);

private:
	required_device<upd4992_device> m_rtc;
	optional_device<ticket_dispenser_device> m_hopper;

	void pwrkick_coin_w(u8 data);
	void pwrkick_coin_lockout_w(u8 data);

	void othldrby_68k_mem(address_map &map);
	void pwrkick_68k_mem(address_map &map);
};

#endif // MAME_TOAPLAN_TOAPLAN2_H
