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
};

DECLARE_DEVICE_TYPE(GPL_RENDERER, gpl_renderer_device)


#endif // MAME_MACHINE_GPL_RENDERER_H
