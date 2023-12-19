// license:BSD-3-Clause
// copyright-holders:David Haywood
// Video System Sprites
#ifndef MAME_TATSUMI_TATSUMI_ROTATION_SPRITES_H
#define MAME_TATSUMI_TATSUMI_ROTATION_SPRITES_H

#include "emupal.h"

#pragma once

/*** CG10103 **********************************************/

class tatsumi_rot_sprite_deice : public device_t
{
public:
	tatsumi_rot_sprite_deice(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration
	template <typename T> void set_gfxdecode_tag(T &&tag) { m_gfxdecode.set_tag(std::forward<T>(tag)); }
	template <typename T> void set_palette_tag(T &&tag) { m_palette.set_tag(std::forward<T>(tag)); }
	template <typename T> void set_screen_tag(T &&tag) { m_screen.set_tag(std::forward<T>(tag)); }
	void set_romtable_offset(uint32_t offset) { m_romtableoffset = offset; }

	uint16_t cyclwarr_sprite_r(offs_t offset) { return m_spriteram[offset]; }
	void cyclwarr_sprite_w(offs_t offset, uint16_t data, uint16_t mem_mask) { COMBINE_DATA(&m_spriteram[offset]); }

	uint16_t cyclwarr_sprite_control_r(offs_t offset) { return m_sprite_control_ram[offset]; }
	void cyclwarr_sprite_control_w(offs_t offset, uint16_t data, uint16_t mem_mask) { COMBINE_DATA(&m_sprite_control_ram[offset]); }
	/* 0xe0 is bank switch, others unknown */
	//  if ((offset==0xe0 && data&0xefff) || offset!=0xe0)
	//      logerror("%s:  Tatsumi TZB215 sprite control %04x %08x\n", m_maincpu->pc(), offset, data);
	
	uint8_t * get_tile_line_src(uint8_t *src1, uint8_t *src2, int h);

	template<class BitmapClass> void draw_sprites(BitmapClass &bitmap, const rectangle &cliprect, int write_priority_only, int rambank);

	template<class BitmapClass> inline void roundupt_drawgfxzoomrotate( BitmapClass &dest_bmp, const rectangle &clip,
		gfx_element *gfx, uint32_t code,uint32_t color,int flipx,int flipy,uint32_t ssx,uint32_t ssy,
		int scale, int rotate, int write_priority_only, int real_line );
	void update_cluts(int fake_palette_offset, int object_base, int length);


	void draw_alpha_pass(const rectangle &cliprect);
	void draw_regular_pass(bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void draw_alt_pass(bitmap_rgb32 &bitmap, const rectangle &cliprect);

	std::unique_ptr<uint8_t[]> m_shadow_pen_array;
	std::unique_ptr<uint16_t[]> m_spriteram;
	std::unique_ptr<uint16_t[]> m_sprite_control_ram;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<screen_device> m_screen;

	uint8_t *m_rom_sprite_lookup[2];
	uint8_t *m_rom_clut[2];

	uint32_t m_romtableoffset;
};


DECLARE_DEVICE_TYPE(TATSUMI_ROT_SPRITES, tatsumi_rot_sprite_deice)

#endif // MAME_TATSUMI_TATSUMI_ROTATION_SPRITES_H
