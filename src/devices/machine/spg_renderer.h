// license:BSD-3-Clause
// copyright-holders:David Haywood, Ryan Holtz

#ifndef MAME_MACHINE_SPG_RENDERER_H
#define MAME_MACHINE_SPG_RENDERER_H

#pragma once

#include "screen.h"

class spg_renderer_device : public device_t
{
public:
	spg_renderer_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);
	spg_renderer_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void draw_sprites(bool read_from_csspace, int extended_sprites_mode, uint32_t palbank, bool highres, const rectangle& cliprect, uint32_t scanline, int priority, uint32_t spritegfxdata_addr, address_space& spc, uint16_t* paletteram, uint16_t* spriteram, int sprlimit);
	void draw_page(bool read_from_csspace, bool has_extended_tilemaps, uint32_t palbank, const rectangle& cliprect, uint32_t scanline, int priority, uint16_t tilegfxdata_addr_msb, uint16_t tilegfxdata_addr, uint16_t* scrollregs, uint16_t* tilemapregs, address_space& spc, uint16_t* paletteram, uint16_t* scrollram, uint32_t which);
	void new_line(const rectangle& cliprect);

	void apply_saturation_and_fade(bitmap_rgb32& bitmap, const rectangle& cliprect, int scanline);

	void set_video_reg_1c(uint16_t val) { m_video_regs_1c = val; update_vcmp_table(); }
	void set_video_reg_1d(uint16_t val) { m_video_regs_1d = val; update_vcmp_table(); }
	void set_video_reg_1e(uint16_t val) { m_video_regs_1e = val; update_vcmp_table(); }
	void set_video_reg_2a(uint16_t val) { m_video_regs_2a = val; }
	void set_video_reg_30(uint16_t val)
	{
		if (m_video_regs_30 != val)
			m_brightness_or_saturation_dirty = true;

		m_video_regs_30 = val;
	}
	void set_video_reg_3c(uint16_t val)
	{
		if (m_video_regs_3c != val)
			m_brightness_or_saturation_dirty = true;

		m_video_regs_3c = val;
	}

	uint16_t get_video_reg_1c(void) { return m_video_regs_1c; }
	uint16_t get_video_reg_1d(void) { return m_video_regs_1d; }
	uint16_t get_video_reg_1e(void) { return m_video_regs_1e; }
	uint16_t get_video_reg_2a(void) { return m_video_regs_2a; }
	uint16_t get_video_reg_30(void) { return m_video_regs_30; }
	uint16_t get_video_reg_3c(void) { return m_video_regs_3c; }

	void set_video_reg_42(uint16_t val) { m_video_regs_42 = val; }
	uint16_t get_video_reg_42(void) { return m_video_regs_42; }

	auto space_read_callback() { return m_space_read_cb.bind(); }
	void set_video_spaces(address_space* cpuspace) { m_cpuspace = cpuspace; }

protected:

	virtual void device_start() override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;

	template<bool Blend, bool FlipX>
	inline void draw_tilestrip(bool read_from_csspace, uint32_t screenwidth, uint32_t drawwidthmask, const rectangle& cliprect, uint32_t tile_h, uint32_t tile_w, uint32_t tilegfxdata_addr, uint32_t tile, uint32_t tile_scanline, int drawx, bool flip_y, uint32_t palette_offset, const uint32_t nc_bpp, const uint32_t bits_per_row, const uint32_t words_per_tile, address_space &spc, uint16_t* palette, uint8_t blendlevel);

	inline void draw_tilestrip(bool read_from_csspace, uint32_t screenwidth, uint32_t drawwidthmask, bool blend, bool flip_x, const rectangle& cliprect, uint32_t tile_h, uint32_t tile_w, uint32_t tilegfxdata_addr, uint32_t tile, uint32_t tile_scanline, int drawx, bool flip_y, uint32_t palette_offset, const uint32_t nc_bpp, const uint32_t bits_per_row, const uint32_t words_per_tile, address_space& spc, uint16_t* paletteram, uint8_t blendlevel);

	inline void draw_sprite(bool read_from_csspace, int extended_sprites_mode, uint32_t palbank, bool highres, const rectangle& cliprect, uint32_t scanline, int priority, uint32_t spritegfxdata_addr, uint32_t base_addr, address_space& spc, uint16_t* paletteram, uint16_t* spriteram);

	virtual void get_tilemap_dimensions(const uint32_t attr, uint32_t &total_width, uint32_t &y_mask, uint32_t &screenwidth);
	virtual uint32_t get_tilegfx_base_address(uint16_t tilegfxdata_addr_msb, uint16_t tilegfxdata_addr);
	virtual int16_t get_linescroll_value(uint16_t* scrollram, uint32_t logical_scanline, const uint32_t yscroll);
	virtual void draw_linemap(bool has_extended_tilemaps, const rectangle& cliprect, uint32_t scanline, int priority, uint32_t tilegfxdata_addr, uint16_t* scrollregs, uint16_t* tilemapregs, address_space& spc, uint16_t* paletteram);
	virtual void apply_extra_tilemap_attributes(uint32_t& tile, uint32_t& tileattr, uint32_t& tilectrl, const uint32_t exattributemap_rambase, uint32_t tile_address, const int realx0, address_space& spc);
	virtual bool is_tile_skipped(uint32_t tile);
	virtual uint32_t get_words_per_text_tile(const uint32_t tile_h, const uint32_t bits_per_row);
	virtual inline void get_tile_pixel(bool read_from_csspace, address_space& spc, uint32_t& bits, uint32_t& nbits, uint32_t& m, const uint32_t nc_bpp)
	{
		uint16_t b = spc.read_word(m++ & 0x3fffff);
		b = (b << 8) | (b >> 8);
		bits |= b << (nc_bpp - nbits);
		nbits += 16;
	}

	virtual void get_extended_spriteram_attributes(uint16_t* spriteram, uint32_t base_addr, uint32_t &tile, uint8_t &blendlevel, bool &flip_x, bool &flip_y) { /* doesn't have extended attributes */ }
	virtual void get_sprite_screenparams(bool highres, uint32_t &screenwidth, uint32_t &screenheight, uint32_t &xmask, uint32_t &ymask);	
	virtual void adjust_sprite_coordinates(int16_t &x, int16_t &y, uint32_t screenwidth, uint32_t screenheight, const uint32_t tile_w, const uint32_t tile_h);
	virtual void check_direct_sprite_mode(int extended_sprites_mode, uint32_t &words_per_tile, uint32_t &tile);
	virtual void check_sprite_extended_palette_mode(int extended_sprites_mode, uint32_t attr, uint32_t palbank, uint32_t &palette_offset);
	virtual bool check_sprites_enable();
	virtual void adjust_sprite_limit(int &sprlimit);
	virtual void check_text_extended_palette_mode(bool has_extended_tilemaps, uint16_t tilegfxdata_addr_msb, uint32_t &palette_offset);

	inline uint8_t mix_channel(uint8_t a, uint8_t b, uint8_t alpha);

	uint8_t m_rgb5_to_rgb8[32];
	uint32_t m_rgb555_to_rgb888[0x8000];
	uint32_t m_rgb555_to_rgb888_current[0x8000];

	void update_vcmp_table();

	// for vcmp
	uint16_t m_video_regs_1c;
	uint16_t m_video_regs_1d;
	uint16_t m_video_regs_1e;

	uint16_t m_video_regs_2a;

	uint16_t m_video_regs_30;
	uint16_t m_video_regs_3c;

	uint16_t m_video_regs_42;

	uint32_t m_ycmp_table[480];

	devcb_read16 m_space_read_cb;

	address_space* m_cpuspace;

	bool m_brightness_or_saturation_dirty;
	void update_palette_lookup(void);
	uint16_t m_linebuf[640];
};

DECLARE_DEVICE_TYPE(SPG_RENDERER, spg_renderer_device)


#endif // MAME_MACHINE_SPG_RENDERER_H
