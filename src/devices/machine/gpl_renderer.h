// license:BSD-3-Clause
// copyright-holders:David Haywood

#ifndef MAME_MACHINE_GPL_RENDERER_H
#define MAME_MACHINE_GPL_RENDERER_H

#pragma once

#include "spg_renderer.h"

class gpl_renderer_device : public spg_renderer_device
{
public:
	gpl_renderer_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void set_cs_video_spaces(address_space* cs_space, uint32_t csbase) { m_cs_space = cs_space; m_csbase = csbase; }

	// used by some hack logic for the gpl16250 rendering for now
	void set_video_reg_7f(uint16_t val) { m_video_regs_7f = val; }
	uint16_t get_video_reg_7f(void) { return m_video_regs_7f; }

protected:
	virtual void device_start() override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;

private:
	virtual void draw_linemap(bool has_extended_tilemaps, const rectangle& cliprect, uint32_t scanline, int priority, uint32_t tilegfxdata_addr, uint16_t* scrollregs, uint16_t* tilemapregs, address_space& spc, uint16_t* paletteram) override;
	virtual uint32_t get_tilegfx_base_address(uint16_t tilegfxdata_addr_msb, uint16_t tilegfxdata_addr) override;
	virtual void get_tilemap_dimensions(const uint32_t attr, uint32_t &total_width, uint32_t &y_mask, uint32_t &screenwidth) override;
	virtual int16_t get_linescroll_value(uint16_t* scrollram, uint32_t logical_scanline, const uint32_t yscroll) override;
	virtual void apply_extra_tilemap_attributes(uint32_t& tile, uint32_t& tileattr, uint32_t& tilectrl, const uint32_t exattributemap_rambase, uint32_t tile_address, const int realx0, address_space& spc) override;
	virtual bool is_tile_skipped(uint32_t tile) override;
	virtual uint32_t get_words_per_text_tile(const uint32_t tile_h, const uint32_t bits_per_row) override;
	virtual inline void get_tile_pixel(bool read_from_csspace, address_space &spc, uint32_t &bits, uint32_t &nbits, uint32_t &m, const uint32_t nc_bpp) override;
	virtual void get_extended_spriteram_attributes(uint16_t* spriteram, uint32_t base_addr, uint32_t& tile, uint8_t& blendlevel, bool& flip_x, bool& flip_y) override;
	virtual void check_direct_sprite_mode(int extended_sprites_mode, uint32_t &words_per_tile, uint32_t &tile) override;
	virtual void check_sprite_extended_palette_mode(int extended_sprites_mode, uint32_t attr, uint32_t palbank, uint32_t &palette_offset) override;
	virtual void check_text_extended_palette_mode(bool has_extended_tilemaps, uint16_t tilegfxdata_addr_msb, uint32_t& palette_offset) override;
	virtual bool check_sprites_enable() override;
	virtual void adjust_sprite_coordinates(int16_t &x, int16_t &y, uint32_t screenwidth, uint32_t screenheight, const uint32_t tile_w, const uint32_t tile_h) override;
	virtual void adjust_sprite_limit(int &sprlimit) override;
	virtual void get_sprite_screenparams(bool highres, uint32_t& screenwidth, uint32_t& screenheight, uint32_t& xmask, uint32_t& ymask) override;

	uint16_t m_video_regs_7f;

	// config
	address_space* m_cs_space;
	uint32_t m_csbase;
};

DECLARE_DEVICE_TYPE(GPL_RENDERER, gpl_renderer_device)


#endif // MAME_MACHINE_GPL_RENDERER_H
