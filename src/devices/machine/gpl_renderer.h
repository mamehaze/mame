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
};

DECLARE_DEVICE_TYPE(GPL_RENDERER, gpl_renderer_device)


#endif // MAME_MACHINE_GPL_RENDERER_H
