
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







































/* -------------------------- Raizing games ------------------------- */

