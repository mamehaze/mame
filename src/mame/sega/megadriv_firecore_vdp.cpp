// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "megadriv_firecore_vdp.h"


DEFINE_DEVICE_TYPE(FIRECORE_VDP, megadriv_firecore_vdp_device, "megadriv_firecore_vdp_device", "FireCore / RedKid VDP (MegaDrive clone)")


void megadriv_firecore_vdp_device::device_start()
{
	sega315_5313_device::device_start();
	m_xvram = std::make_unique<uint8_t[]>(0x20000);
	std::fill_n(m_xvram.get(), 0x20000, 0);
	save_pointer(NAME(m_xvram), 0x20000);
	save_item(NAME(m_xcram));
	save_item(NAME(m_xvsram));
	save_item(NAME(m_xreg));
	save_item(NAME(m_xaddr));
	save_item(NAME(m_xcode));
	save_item(NAME(m_xpending));
	save_item(NAME(m_xfill));
}

void megadriv_firecore_vdp_device::device_reset()
{
	sega315_5313_device::device_reset();

	std::fill(std::begin(m_xcram), std::end(m_xcram), 0);
	std::fill(std::begin(m_xvsram), std::end(m_xvsram), 0);
	std::fill(std::begin(m_xreg), std::end(m_xreg), 0);
	m_xaddr = 0;
	m_xcode = 0;
	m_xpending = false;
	m_xfill = false;
}


/*
    Firecore extended video mode

    Enabled with VDP register 0x18 bit 7 (the menus use 0x80 / 0x82 / 0x8a, games switch back to 0x00).
    Reverse engineered from the RedKid menus (all of this is unverified against hardware documentation):

    - 128KiB VRAM, address bit 16 comes from bit 2 of the 2nd VDP command word
    - tiles are 8x8 with 6 bits per pixel (48 bytes per tile, tile n at n * 48), the pixels are packed
      LSB first into consecutive 16-bit VRAM words
    - name table / sprite attribute entries: bits 15-13 palette (8 palettes of 64 colours), bit 12 V flip,
      bit 11 H flip, bits 10-0 tile number, there is no priority bit
    - CRAM holds 512 colours of 2 words each: word 0 = GGBB, word 1 = 00RR
    - 68k -> VDP DMA transfers twice the programmed length (the length is in 32-bit units)
    - the table locations don't follow the standard register formulas; the menus always use (and their
      software copies of the addresses say) plane A at 0x12000, plane B at 0x14000, H scroll at 0x16000 and
      the sprite table at 0x17000, so these are hardcoded for now.  Plane B is drawn above plane A.
*/

uint16_t megadriv_firecore_vdp_device::xvdp_r(offs_t offset, uint16_t mem_mask)
{
	if (!machine().side_effects_disabled() && ((offset & 0x0e) == 0x02)) // control port read clears the pending command
		m_xpending = false;

	return sega315_5313_device::vdp_r(offset, mem_mask);
}

void megadriv_firecore_vdp_device::xvdp_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	if (mem_mask == 0xffff)
	{
		if ((offset & 0x0e) == 0x00)
			xvdp_data_w(data);
		else if ((offset & 0x0e) == 0x02)
			xvdp_ctrl_w(data);
	}

	sega315_5313_device::vdp_w(offset, data, mem_mask);
}

void megadriv_firecore_vdp_device::xvdp_data_w(uint16_t data)
{
	m_xpending = false;

	if (m_xfill)
	{
		m_xfill = false;
		uint32_t length = m_xreg[0x13] | (m_xreg[0x14] << 8);
		if (!length)
			length = 0x10000;

		m_xvram[(m_xaddr & 0x1fffe) | 0] = data >> 8;
		m_xvram[(m_xaddr & 0x1fffe) | 1] = data & 0xff;
		for (uint32_t i = 0; i < length; i++)
		{
			m_xvram[(m_xaddr ^ 1) & 0x1ffff] = data >> 8;
			m_xaddr = (m_xaddr + m_xreg[0x0f]) & 0x1ffff;
		}
		return;
	}

	switch (m_xcode & 0x0f)
	{
	case 0x01: // VRAM
		m_xvram[(m_xaddr & 0x1fffe) | 0] = data >> 8;
		m_xvram[(m_xaddr & 0x1fffe) | 1] = data & 0xff;
		break;
	case 0x03: // CRAM
		m_xcram[(m_xaddr >> 1) & 0x3ff] = data;
		break;
	case 0x05: // VSRAM
		m_xvsram[(m_xaddr >> 1) & 0x3f] = data;
		break;
	}
	m_xaddr = (m_xaddr + m_xreg[0x0f]) & 0x1ffff;
}

void megadriv_firecore_vdp_device::xvdp_ctrl_w(uint16_t data)
{
	if (!m_xpending)
	{
		if ((data & 0xc000) == 0x8000)
		{
			m_xreg[(data >> 8) & 0x1f] = data & 0xff;
			return;
		}

		m_xpending = true;
		m_xcode = (m_xcode & 0x3c) | (data >> 14);
		m_xaddr = (m_xaddr & 0x1c000) | (data & 0x3fff);
		return;
	}

	m_xpending = false;
	m_xcode = (m_xcode & 0x03) | ((data >> 2) & 0x3c);
	m_xaddr = (m_xaddr & 0x03fff) | ((data & 0x03) << 14) | ((data & 0x04) << 14);

	if (!(m_xcode & 0x20) || !BIT(m_xreg[0x01], 4))
		return;

	switch (m_xreg[0x17] >> 6)
	{
	case 0:
	case 1: // 68k -> VDP
	{
		uint32_t length = m_xreg[0x13] | (m_xreg[0x14] << 8);
		if (!length)
			length = 0x10000;
		if (xvdp_enabled())
			length <<= 1;

		uint32_t source = (m_xreg[0x15] | (m_xreg[0x16] << 8) | ((m_xreg[0x17] & 0x7f) << 16)) << 1;
		for (uint32_t i = 0; i < length; i++)
		{
			xvdp_data_w(m_space68k->read_word(source));
			// the standard VDP wraps the source within 128KiB, the extended mode transfers can cross that boundary
			source = xvdp_enabled() ? ((source + 2) & 0xfffffe) : ((source & 0xfe0000) | ((source + 2) & 0x1ffff));
		}
		break;
	}

	case 2: // fill, happens on the next data port write
		m_xfill = true;
		break;

	case 3: // VRAM copy
	{
		uint32_t length = m_xreg[0x13] | (m_xreg[0x14] << 8);
		if (!length)
			length = 0x10000;
		uint32_t source = m_xreg[0x15] | (m_xreg[0x16] << 8);
		for (uint32_t i = 0; i < length; i++)
		{
			m_xvram[(m_xaddr ^ 1) & 0x1ffff] = m_xvram[(source ^ 1) & 0x1ffff];
			source = (source + 1) & 0xffff;
			m_xaddr = (m_xaddr + m_xreg[0x0f]) & 0x1ffff;
		}
		break;
	}
	}
}

uint32_t megadriv_firecore_vdp_device::xvdp_color(int index) const
{
	const uint16_t gr = m_xcram[(index * 2 + 0) & 0x3ff];
	const uint16_t b = m_xcram[(index * 2 + 1) & 0x3ff];
	return rgb_t(gr & 0xff, gr >> 8, b & 0xff);
}

uint8_t megadriv_firecore_vdp_device::xvdp_tile_pixel(uint32_t tile, int x, int y) const
{
	const uint32_t bit = ((y * 8) + x) * 6;
	const uint32_t base = tile * 48;
	const uint32_t word0 = base + ((bit >> 4) << 1);
	const uint32_t word1 = word0 + 2;
	const uint32_t w0 = (m_xvram[word0 & 0x1ffff] << 8) | m_xvram[(word0 + 1) & 0x1ffff];
	const uint32_t w1 = (m_xvram[word1 & 0x1ffff] << 8) | m_xvram[(word1 + 1) & 0x1ffff];
	return ((w0 | (w1 << 16)) >> (bit & 15)) & 0x3f;
}

void megadriv_firecore_vdp_device::xvdp_draw_plane(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint32_t nametable, int plane)
{
	static const int sizes[4] = { 32, 64, 64, 128 };
	const int hsize = sizes[m_xreg[0x10] & 3];
	const int vsize = sizes[(m_xreg[0x10] >> 4) & 3];

	for (int y = cliprect.min_y; y <= cliprect.max_y; y++)
	{
		int hscroll;
		switch (m_xreg[0x0b] & 3)
		{
		default:
		case 0: hscroll = 0x16000; break;
		case 2: hscroll = 0x16000 + (y & ~7) * 4; break;
		case 3: hscroll = 0x16000 + y * 4; break;
		}
		hscroll += plane * 2;
		const int hs = ((m_xvram[hscroll & 0x1ffff] << 8) | m_xvram[(hscroll + 1) & 0x1ffff]) & 0x3ff;
		const int vs = m_xvsram[plane] & 0x3ff;

		uint32_t *const dest = &bitmap.pix(y);
		const int py = (y + vs) & ((vsize * 8) - 1);
		for (int x = cliprect.min_x; x <= cliprect.max_x; x++)
		{
			const int px = (x - hs) & ((hsize * 8) - 1);
			const uint32_t entryaddr = nametable + (((py >> 3) * hsize) + (px >> 3)) * 2;
			const uint16_t entry = (m_xvram[entryaddr & 0x1ffff] << 8) | m_xvram[(entryaddr + 1) & 0x1ffff];
			int tx = px & 7, ty = py & 7;
			if (BIT(entry, 11)) tx ^= 7;
			if (BIT(entry, 12)) ty ^= 7;
			const uint8_t pix = xvdp_tile_pixel(entry & 0x7ff, tx, ty);
			if (pix)
				dest[x] = xvdp_color(((entry >> 13) * 64) + pix);
		}
	}
}

void megadriv_firecore_vdp_device::xvdp_draw_sprites(bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	const uint32_t table = 0x17000;
	const int maxsprites = BIT(m_xreg[0x0c], 0) ? 80 : 64;

	// collect the link list, the first sprite in the list has the highest priority
	int order[80];
	int count = 0;
	int link = 0;
	do
	{
		order[count++] = link;
		const uint32_t a = table + link * 8;
		link = m_xvram[(a + 3) & 0x1ffff] & 0x7f;
	} while (link && (link < maxsprites) && (count < maxsprites));

	for (int i = count - 1; i >= 0; i--)
	{
		const uint32_t a = table + order[i] * 8;
		auto word = [this, a] (int n) { return uint16_t((m_xvram[(a + n * 2) & 0x1ffff] << 8) | m_xvram[(a + n * 2 + 1) & 0x1ffff]); };
		const int ypos = (word(0) & 0x3ff) - 128;
		const int w = ((word(1) >> 10) & 3) + 1;
		const int h = ((word(1) >> 8) & 3) + 1;
		const uint16_t attr = word(2);
		const int xpos = (word(3) & 0x1ff) - 128;
		const bool fx = BIT(attr, 11), fy = BIT(attr, 12);
		const int pal = attr >> 13;

		for (int sy = 0; sy < h * 8; sy++)
		{
			const int y = ypos + sy;
			if (y < cliprect.min_y || y > cliprect.max_y)
				continue;
			uint32_t *const dest = &bitmap.pix(y);
			const int ty = fy ? (h * 8 - 1 - sy) : sy;
			for (int sx = 0; sx < w * 8; sx++)
			{
				const int x = xpos + sx;
				if (x < cliprect.min_x || x > cliprect.max_x)
					continue;
				const int tx = fx ? (w * 8 - 1 - sx) : sx;
				const uint32_t tile = (attr & 0x7ff) + (tx >> 3) * h + (ty >> 3);
				const uint8_t pix = xvdp_tile_pixel(tile & 0x7ff, tx & 7, ty & 7);
				if (pix)
					dest[x] = xvdp_color((pal * 64) + pix);
			}
		}
	}
}

void megadriv_firecore_vdp_device::draw_firecore(bitmap_rgb32& bitmap, const rectangle& cliprect)
{
	if (!BIT(m_xreg[0x01], 6)) // display disabled
		return;

	bitmap.fill(xvdp_color(m_xreg[0x07] & 0x3f), cliprect);
	xvdp_draw_plane(bitmap, cliprect, 0x12000, 0);
	xvdp_draw_plane(bitmap, cliprect, 0x14000, 1);
	xvdp_draw_sprites(bitmap, cliprect);
}
