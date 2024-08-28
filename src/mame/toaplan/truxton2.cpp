
//#define TRUXTON2_STEREO       /* Uncomment to hear truxton2 music in stereo */


#include "truxton2.h"


void truxton2_state::toaplan2_reset(int state)
{
	if (m_audiocpu != nullptr)
		m_audiocpu->pulse_input_line(INPUT_LINE_RESET, attotime::zero);
}

u16 truxton2_state::video_count_r()
{
	/* +---------+---------+--------+---------------------------+ */
	/* | /H-Sync | /V-Sync | /Blank |       Scanline Count      | */
	/* | Bit 15  | Bit 14  | Bit 8  |  Bit 7-0 (count from #EF) | */
	/* +---------+---------+--------+---------------------------+ */
	/*************** Control Signals are active low ***************/

	int vpos = m_screen->vpos();

	u16 video_status = 0xff00;    // Set signals inactive

	vpos = (vpos + 15) % 262;

	if (!m_vdp->hsync_r())
		video_status &= ~0x8000;
	if (!m_vdp->vsync_r())
		video_status &= ~0x4000;
	if (!m_vdp->fblank_r())
		video_status &= ~0x0100;
	if (vpos < 256)
		video_status |= (vpos & 0xff);
	else
		video_status |= 0xff;

//  logerror("VC: vpos=%04x hpos=%04x VBL=%04x\n",vpos,hpos,m_screen->vblank());

	return video_status;
}


VIDEO_START_MEMBER(truxton2_state,toaplan2)
{
	/* our current VDP implementation needs this bitmap to work with */
	m_screen->register_screen_bitmap(m_custom_priority_bitmap);
	m_vdp->custom_priority_bitmap = &m_custom_priority_bitmap;
}


u32 truxton2_state::screen_update_toaplan2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);
	m_custom_priority_bitmap.fill(0, cliprect);
	m_vdp->render_vdp(bitmap, cliprect);

	return 0;
}

void truxton2_state::screen_vblank(int state)
{
	// rising edge
	if (state)
	{
		m_vdp->screen_eof();
	}
}


void truxton2_state::coin_w(u8 data)
{
	/* +----------------+------ Bits 7-5 not used ------+--------------+ */
	/* | Coin Lockout 2 | Coin Lockout 1 | Coin Count 2 | Coin Count 1 | */
	/* |     Bit 3      |     Bit 2      |     Bit 1    |     Bit 0    | */

	if (data & 0x0f)
	{
		machine().bookkeeping().coin_lockout_w(0, BIT(~data, 2));
		machine().bookkeeping().coin_lockout_w(1, BIT(~data, 3));
		machine().bookkeeping().coin_counter_w(0, BIT( data, 0));
		machine().bookkeeping().coin_counter_w(1, BIT( data, 1));
	}
	else
	{
		machine().bookkeeping().coin_lockout_global_w(1);    // Lock all coin slots
	}
	if (data & 0xf0)
	{
		logerror("Writing unknown upper bits (%02x) to coin control\n",data);
	}
}

TILE_GET_INFO_MEMBER(truxton2_state::get_text_tile_info)
{
	const u16 attrib = m_tx_videoram[tile_index];
	const u32 tile_number = attrib & 0x3ff;
	const u32 color = attrib >> 10;
	tileinfo.set(0,
			tile_number,
			color,
			0);
}


void truxton2_state::create_tx_tilemap(int dx, int dx_flipped)
{
	m_tx_tilemap = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(truxton2_state::get_text_tile_info)), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);

	m_tx_tilemap->set_scroll_rows(8*32); /* line scrolling */
	m_tx_tilemap->set_scroll_cols(1);
	m_tx_tilemap->set_scrolldx(dx, dx_flipped);
	m_tx_tilemap->set_transparent_pen(0);
}

void truxton2_state::device_post_load()
{
	if (m_tx_gfxram != nullptr)
		m_gfxdecode->gfx(0)->mark_all_dirty();
}


VIDEO_START_MEMBER(truxton2_state,truxton2)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	/* Create the Text tilemap for this game */
	m_gfxdecode->gfx(0)->set_source(reinterpret_cast<u8 *>(m_tx_gfxram.target()));

	create_tx_tilemap(0x1d5, 0x16a);
}

VIDEO_START_MEMBER(truxton2_state,fixeightbl)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	/* Create the Text tilemap for this game */
	create_tx_tilemap();

	/* This bootleg has additional layer offsets on the VDP */
	m_vdp->set_tm_extra_offsets(0, -0x1d6 - 26, -0x1ef - 15, 0, 0);
	m_vdp->set_tm_extra_offsets(1, -0x1d8 - 22, -0x1ef - 15, 0, 0);
	m_vdp->set_tm_extra_offsets(2, -0x1da - 18, -0x1ef - 15, 0, 0);
	m_vdp->set_sp_extra_offsets(8/*-0x1cc - 64*/, 8/*-0x1ef - 128*/, 0, 0);

	m_vdp->init_scroll_regs();
}


VIDEO_START_MEMBER(truxton2_state,bgaregga)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	/* Create the Text tilemap for this game */
	create_tx_tilemap(0x1d4, 0x16b);
}

VIDEO_START_MEMBER(truxton2_state,bgareggabl)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	/* Create the Text tilemap for this game */
	create_tx_tilemap(4, 4);
}

VIDEO_START_MEMBER(truxton2_state,batrider)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	m_vdp->disable_sprite_buffer(); // disable buffering on this game

	/* Create the Text tilemap for this game */
	m_gfxdecode->gfx(0)->set_source(reinterpret_cast<u8 *>(m_tx_gfxram.target()));

	create_tx_tilemap(0x1d4, 0x16b);

	/* Has special banking */
	save_item(NAME(m_gfxrom_bank));
}

void truxton2_state::tx_videoram_w(offs_t offset, u16 data, u16 mem_mask)
{
	COMBINE_DATA(&m_tx_videoram[offset]);
	if (offset < 64*32)
		m_tx_tilemap->mark_tile_dirty(offset);
}

void truxton2_state::tx_linescroll_w(offs_t offset, u16 data, u16 mem_mask)
{
	/*** Line-Scroll RAM for Text Layer ***/
	COMBINE_DATA(&m_tx_linescroll[offset]);

	m_tx_tilemap->set_scrollx(offset, m_tx_linescroll[offset]);
}

void truxton2_state::tx_gfxram_w(offs_t offset, u16 data, u16 mem_mask)
{
	/*** Dynamic GFX decoding for Truxton 2 / FixEight ***/

	const u16 oldword = m_tx_gfxram[offset];

	if (oldword != data)
	{
		COMBINE_DATA(&m_tx_gfxram[offset]);
		m_gfxdecode->gfx(0)->mark_dirty(offset/32);
	}
}

void truxton2_state::batrider_tx_gfxram_w(offs_t offset, u16 data, u16 mem_mask)
{
	/*** Dynamic GFX decoding for Batrider / Battle Bakraid ***/

	const u16 oldword = m_tx_gfxram[offset];

	if (oldword != data)
	{
		COMBINE_DATA(&m_tx_gfxram[offset]);
		m_gfxdecode->gfx(0)->mark_dirty(offset/16);
	}
}

void truxton2_state::batrider_textdata_dma_w(u16 data)
{
	/*** Dynamic Text GFX decoding for Batrider ***/
	/*** Only done once during start-up ***/
	m_dma_space->set_bank(1);
	for (int i = 0; i < (0x8000 >> 1); i++)
	{
		m_dma_space->write16(i, m_mainram[i]);
	}
}

void truxton2_state::batrider_pal_text_dma_w(u16 data)
{
	// FIXME: In batrider and bbakraid, the text layer and palette RAM
	// are probably DMA'd from main RAM by writing here at every vblank,
	// rather than being directly accessible to the 68K like the other games
	m_dma_space->set_bank(0);
	for (int i = 0; i < (0x3400 >> 1); i++)
	{
		m_dma_space->write16(i, m_mainram[i]);
	}
}

void truxton2_state::batrider_objectbank_w(offs_t offset, u8 data)
{
	data &= 0xf;
	if (m_gfxrom_bank[offset] != data)
	{
		m_gfxrom_bank[offset] = data;
		m_vdp->set_dirty();
	}
}

void truxton2_state::batrider_bank_cb(u8 layer, u32 &code)
{
	code = (m_gfxrom_bank[code >> 15] << 15) | (code & 0x7fff);
}


/* fixeightbl and bgareggabl do not use the lineselect or linescroll tables */
u32 truxton2_state::screen_update_bootleg(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	screen_update_toaplan2(screen, bitmap, cliprect);
	m_tx_tilemap->draw(screen, bitmap, cliprect, 0);
	return 0;
}


u32 truxton2_state::screen_update_truxton2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	screen_update_toaplan2(screen, bitmap, cliprect);

	rectangle clip = cliprect;

	/* it seems likely that flipx can be set per line! */
	/* however, none of the games does it, and emulating it in the */
	/* MAME tilemap system without being ultra slow would be tricky */
	m_tx_tilemap->set_flip(m_tx_lineselect[0] & 0x8000 ? 0 : TILEMAP_FLIPX);

	/* line select is used for 'for use in' and '8ing' screen on bbakraid, 'Raizing' logo on batrider */
	for (int y = cliprect.min_y; y <= cliprect.max_y; y++)
	{
		clip.min_y = clip.max_y = y;
		m_tx_tilemap->set_scrolly(0, m_tx_lineselect[y] - y);
		m_tx_tilemap->draw(screen, bitmap, clip, 0);
	}
	return 0;
}



////////////////////////////////

void truxton2_state::machine_start()
{
	save_item(NAME(m_z80_busreq));
}

MACHINE_RESET_MEMBER(truxton2_state, bgaregga)
{
	for (int chip = 0; chip < 2; chip++)
	{
		for (int i = 0; i < 8; i++)
		{
			if (m_raizing_okibank[chip][i] != nullptr)
				m_raizing_okibank[chip][i]->set_entry(0);
		}
	}
}



void truxton2_state::install_raizing_okibank(int chip)
{
	assert(m_oki_rom[chip] && m_raizing_okibank[chip][0]);

	for (int i = 0; i < 4; i++)
	{
		m_raizing_okibank[chip][i]->configure_entries(0, 16, &m_oki_rom[chip][(i * 0x100)], 0x10000);
	}
	m_raizing_okibank[chip][4]->configure_entries(0, 16, &m_oki_rom[chip][0x400], 0x10000);
	for (int i = 5; i < 8; i++)
	{
		m_raizing_okibank[chip][i]->configure_entries(0, 16, &m_oki_rom[chip][0], 0x10000);
	}
}

void truxton2_state::init_bgaregga()
{
	u8 *Z80 = memregion("audiocpu")->base();

	m_audiobank->configure_entries(0, 8, Z80, 0x4000); // Test mode only, Mirror of First 128KB Areas?
	m_audiobank->configure_entries(8, 8, Z80, 0x4000);
	install_raizing_okibank(0);
}







void truxton2_state::shippumd_coin_w(u8 data)
{
	coin_w(data & ~0x10);
	m_oki[0]->set_rom_bank(BIT(data, 4));
}


void truxton2_state::raizing_z80_bankswitch_w(u8 data)
{
	m_audiobank->set_entry(data & 0x0f);
}


// bgaregga and batrider don't actually have a NMK112, but rather a GAL
// programmed to bankswitch the sound ROMs in a similar fashion.
// it may not be a coincidence that the composer and sound designer for
// these two games, Manabu "Santaruru" Namiki, came to Raizing from NMK...

void truxton2_state::raizing_oki_bankswitch_w(offs_t offset, u8 data)
{
	m_raizing_okibank[(offset & 4) >> 2][offset & 3]->set_entry(data & 0xf);
	m_raizing_okibank[(offset & 4) >> 2][4 + (offset & 3)]->set_entry(data & 0xf);
	offset++;
	data >>= 4;
	m_raizing_okibank[(offset & 4) >> 2][offset & 3]->set_entry(data & 0xf);
	m_raizing_okibank[(offset & 4) >> 2][4 + (offset & 3)]->set_entry(data & 0xf);
}


u8 truxton2_state::bgaregga_E01D_r()
{
	// the Z80 reads this address during its IRQ routine,
	// and reads the soundlatch only if the lowest bit is clear.
	return m_soundlatch[0]->pending_r() ? 0 : 1;
}


u16 truxton2_state::batrider_z80_busack_r()
{
	// Bit 0x01 returns the status of BUSAK from the Z80.
	// These accesses are made when the 68K wants to read the Z80
	// ROM code. Failure to return the correct status incurrs a Sound Error.

	return m_z80_busreq;    // Loop BUSRQ to BUSAK
}


void truxton2_state::batrider_z80_busreq_w(u8 data)
{
	m_z80_busreq = (data & 0x01);   // see batrider_z80_busack_r above
}


u16 truxton2_state::batrider_z80rom_r(offs_t offset)
{
	return m_z80_rom[offset];
}

// these two latches are always written together, via a single move.l instruction
void truxton2_state::batrider_soundlatch_w(u8 data)
{
	m_soundlatch[0]->write(data & 0xff);
	m_audiocpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
}


void truxton2_state::batrider_soundlatch2_w(u8 data)
{
	m_soundlatch[1]->write(data & 0xff);
	m_audiocpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
}

void truxton2_state::batrider_unknown_sound_w(u16 data)
{
	// the 68K writes here when it wants a sound acknowledge IRQ from the Z80
	// for bbakraid this is on every sound command; for batrider, only on certain commands
}


void truxton2_state::batrider_clear_sndirq_w(u16 data)
{
	// not sure whether this is correct
	// the 68K writes here during the sound IRQ handler, and nowhere else...
	m_maincpu->set_input_line(m_sndirq_line, CLEAR_LINE);
}


void truxton2_state::batrider_sndirq_w(u8 data)
{
	// if batrider_clear_sndirq_w() is correct, should this be ASSERT_LINE?
	m_maincpu->set_input_line(m_sndirq_line, HOLD_LINE);
}


void truxton2_state::batrider_clear_nmi_w(u8 data)
{
	m_audiocpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
}


u16 truxton2_state::bbakraid_eeprom_r()
{
	// Bit 0x01 returns the status of BUSAK from the Z80.
	// BUSRQ is activated via bit 0x10 on the EEPROM write port.
	// These accesses are made when the 68K wants to read the Z80
	// ROM code. Failure to return the correct status incurrs a Sound Error.

	u8 data;
	data  = ((m_eeprom->do_read() & 0x01) << 4);
	data |= ((m_z80_busreq >> 4) & 0x01);   // Loop BUSRQ to BUSAK

	return data;
}


void truxton2_state::bbakraid_eeprom_w(u8 data)
{
	if (data & ~0x1f)
		logerror("CPU #0 PC:%06X - Unknown EEPROM data being written %02X\n",m_maincpu->pc(),data);

	m_eepromout->write(data, 0xff);

	m_z80_busreq = data & 0x10; // see bbakraid_eeprom_r above
}


INTERRUPT_GEN_MEMBER(truxton2_state::bbakraid_snd_interrupt)
{
	device.execute().set_input_line(0, HOLD_LINE);
}




void truxton2_state::sound_reset_w(u8 data)
{
	m_audiocpu->set_input_line(INPUT_LINE_RESET, (data & m_sound_reset_bit) ? CLEAR_LINE : ASSERT_LINE);
}


void truxton2_state::fixeightbl_oki_bankswitch_w(u8 data)
{
	data &= 7;
	if (data <= 4) m_okibank->set_entry(data);
}

void truxton2_state::fixeightbl_oki(address_map &map)
{
	map(0x00000, 0x2ffff).rom();
	map(0x30000, 0x3ffff).bankr(m_okibank);
}





void truxton2_state::batrider_dma_mem(address_map &map)
{
	map(0x0000, 0x1fff).ram().w(FUNC(truxton2_state::tx_videoram_w)).share(m_tx_videoram);
	map(0x2000, 0x2fff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x3000, 0x31ff).ram().share(m_tx_lineselect);
	map(0x3200, 0x33ff).ram().w(FUNC(truxton2_state::tx_linescroll_w)).share(m_tx_linescroll);
	map(0x3400, 0x7fff).ram();
	map(0x8000, 0xffff).ram().w(FUNC(truxton2_state::batrider_tx_gfxram_w)).share(m_tx_gfxram);
}





void truxton2_state::raizing_sound_z80_mem(address_map &map)
{
	map(0x0000, 0xbfff).rom();
	map(0xc000, 0xdfff).ram().share(m_shared_ram);
	map(0xe000, 0xe001).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0xe004, 0xe004).rw(m_oki[0], FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0xe00e, 0xe00e).w(FUNC(truxton2_state::coin_w));
}





void truxton2_state::fixeight_v25_mem(address_map &map)
{
	map(0x00000, 0x00000).portr("IN1");
	map(0x00002, 0x00002).portr("IN2");
	map(0x00004, 0x00004).portr("IN3");
	map(0x0000a, 0x0000b).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0x0000c, 0x0000c).rw(m_oki[0], FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0x80000, 0x87fff).mirror(0x78000).ram().share(m_shared_ram);
}





void truxton2_state::cpu_space_fixeightbl_map(address_map &map)
{
	map(0xfffff0, 0xffffff).m(m_maincpu, FUNC(m68000_base_device::autovectors_map));
	map(0xfffff5, 0xfffff5).lr8(NAME([this] () { m_maincpu->set_input_line(M68K_IRQ_2, CLEAR_LINE); return m68000_device::autovector(2); }));
}





/* -------------------------- Raizing games ------------------------- */

