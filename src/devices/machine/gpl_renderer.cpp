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
