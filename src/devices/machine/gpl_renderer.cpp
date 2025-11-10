// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "gpl_renderer.h"

DEFINE_DEVICE_TYPE(GPL_RENDERER, gpl_renderer_device, "gpl_renderer", "GeneralPlus video rendering")

gpl_renderer_device::gpl_renderer_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	spg_renderer_device(mconfig, GPL_RENDERER, tag, owner, clock)
{
}

void gpl_renderer_device::device_start()
{
	spg_renderer_device::device_start();
}

void gpl_renderer_device::device_reset()
{
	spg_renderer_device::device_reset();
}

uint32_t gpl_renderer_device::get_tilegfx_base_address(uint16_t tilegfxdata_addr_msb, uint16_t tilegfxdata_addr)
{
	if (m_video_regs_7f & 0x0040) // FREE == 1
	{
		return ((tilegfxdata_addr_msb & 0x07ff) << 16) | tilegfxdata_addr;
	}
	else // FREE == 0 (default / legacy)
	{
		return tilegfxdata_addr * 0x40;
	}
}

void gpl_renderer_device::get_tilemap_dimensions(const uint32_t attr, uint32_t &total_width, uint32_t &y_mask, uint32_t &screenwidth)
{
	if (attr & 0x8000)
	{
		total_width = 1024;
		y_mask = 0x200;
		screenwidth = 640;
	}
	else
	{
		total_width = 512;
		y_mask = 0x100;
		screenwidth = 320;
	}

	if (attr & 0x4000)
	{
		y_mask <<= 1; // double height tilemap?
	}
}

void gpl_renderer_device::apply_extra_tilemap_attributes(uint32_t &tile, uint32_t &tileattr, uint32_t &tilectrl, const uint32_t exattributemap_rambase, uint32_t tile_address, const int realx0, address_space& spc)
{
	if (m_video_regs_7f & 0x0004) // TX_DIRECT
	{
		uint16_t exattribute = (tilectrl & 0x0004) ? spc.read_word(exattributemap_rambase) : spc.read_word(exattributemap_rambase + tile_address / 2);
		if (realx0 & 1)
			exattribute >>= 8;
		else
			exattribute &= 0x00ff;

		// when TX_DIRECT is used the attributes become extra addressing bits (smartfp)
		tile |= (exattribute & 0xff) << 16;
		//blendlevel = 0x1f; // hack
	}
	else
	{
		spg_renderer_device::apply_extra_tilemap_attributes(tile, tileattr, tilectrl, exattributemap_rambase, tile_address, realx0, spc);
	}
}

bool gpl_renderer_device::is_tile_skipped(uint32_t tile)
{
	if (!tile)
	{
		if (m_video_regs_7f & 0x0002)
		{
			// Galaga in paccon won't render '0' characters in the scoring table if you skip empty tiles, so maybe GPL16250 doesn't skip? - extra tile bits from extended read make no difference

			// probably not based on register m_video_regs_7f, but paccon galaga needs no skip, jak_gtg and jak_hmhsm needs to skip
			//49 0100 1001  no skip (paccon galaga)
			//4b 0100 1011  skip    (paccon pacman)
			//53 0101 0011  skip    (jak_gtg, jak_hmhsm)
			return true;
		}
	}

	return false;
}

int16_t gpl_renderer_device::get_linescroll_value(uint16_t* scrollram, uint32_t logical_scanline, const uint32_t yscroll)
{
	// the logic seems to be different on GPL16250 compared to SPG2xx
	// see Galaxian in paccon and Crazy Moto in myac220, is this mode be selected or did behavior just change?
	return (int16_t)scrollram[logical_scanline & 0xff];
}

void gpl_renderer_device::draw_linemap(bool has_extended_tilemaps, const rectangle& cliprect, uint32_t scanline, int priority, uint32_t tilegfxdata_addr, uint16_t* scrollregs, uint16_t* tilemapregs, address_space& spc, uint16_t* paletteram)
{
	uint32_t ctrl = tilemapregs[1];

	if (0)
	{
		if (ctrl & 0x0010)
			popmessage("bitmap mode %08x with rowscroll\n", tilegfxdata_addr);
		else
			popmessage("bitmap mode %08x\n", tilegfxdata_addr);
	}

	// note, in interlace modes it appears every other line is unused? (480 entry table, but with blank values)
	// and furthermore the rowscroll and rowzoom tables only have 240 entries, not enough for every line
	// the end of the rowscroll table (entries 240-255) contain something else, maybe garbage data as it's offscreen, maybe not
	uint32_t tilemap = tilemapregs[2];
	uint32_t palette_map = tilemapregs[3];

	uint32_t linebase = spc.read_word(tilemap + scanline); // every other word is unused, but there are only enough entries for 240 lines then, sometimes to do with interlace mode?
	uint16_t palette = spc.read_word(palette_map + (scanline / 2));

	if (scanline & 1)
		palette >>= 8;
	else
		palette &= 0xff;

	if (!linebase)
		return;

	linebase = linebase | (palette << 16);

	int upperpalselect = 0;
	if (has_extended_tilemaps && (tilegfxdata_addr & 0x80000000))
		upperpalselect = 1;

	tilegfxdata_addr &= 0x7ffffff;

	// this logic works for jak_s500 and the test modes to get the correct base, doesn't seem to work for jak_car2 ingame, maybe data is copied to wrong place?
	int gfxbase = (tilegfxdata_addr & 0x7ffffff) + (linebase & 0x7ffffff);

	for (int i = 0; i < 160; i++) // will have to be 320 for jak_car2 ingame, jak_s500 lines are wider than screen, and zoomed
	{
		uint16_t pix;
		const int addr = gfxbase & 0x7ffffff;
		if (addr < m_csbase)
		{
			pix = m_cpuspace->read_word(addr);
		}
		else
		{
			pix = m_cs_space->read_word(addr - m_csbase);
		}
		gfxbase++;

		int xx;
		uint16_t pal;

		if ((scanline >= 0) && (scanline < 480))
		{
			xx = i * 2;

			pal = (pix & 0xff) | 0x100;

			if (upperpalselect)
				pal |= 0x200;

			if (xx >= 0 && xx <= cliprect.max_x)
			{
				uint16_t rgb = paletteram[pal];

				if (!(rgb & 0x8000))
				{
					m_linebuf[xx] = rgb;
				}
			}

			xx = (i * 2) + 1;
			pal = (pix >> 8) | 0x100;

			if (upperpalselect)
				pal |= 0x200;

			if (xx >= 0 && xx <= cliprect.max_x)
			{
				uint16_t rgb = paletteram[pal];

				if (!(rgb & 0x8000))
				{
					m_linebuf[xx] = rgb;
				}
			}
		}
	}
}
