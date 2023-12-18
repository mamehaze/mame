// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "tatsumi_rotation_sprites.h"
#include "screen.h"

DEFINE_DEVICE_TYPE(TATSUMI_ROT_SPRITES, tatsumi_rot_sprite_deice, "tatsumi_rot_spr", "Tatsumi Rotation Sprites")

tatsumi_rot_sprite_deice::tatsumi_rot_sprite_deice(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, TATSUMI_ROT_SPRITES, tag, owner, clock)
	, m_gfxdecode(*this, finder_base::DUMMY_TAG)
	, m_palette(*this, finder_base::DUMMY_TAG)
	, m_screen(*this, finder_base::DUMMY_TAG)
{
}

void tatsumi_rot_sprite_deice::device_start()
{
	m_shadow_pen_array = make_unique_clear<uint8_t[]>(8192);

	m_spriteram = make_unique_clear<uint16_t[]>(0x4000/2);
	m_sprite_control_ram = make_unique_clear<uint16_t[]>(0x200/2);

	uint8_t *dst = memregion("sprites")->base();
	uint8_t *src1 = memregion("sprites_l")->base();
	int len1 = memregion("sprites_l")->bytes();
	uint8_t *src2 = memregion("sprites_h")->base();
	int len2 = memregion("sprites_h")->bytes();

	for (int i = 0; i < len1; i += 32)
	{
		memcpy(dst, src1, 32);
		src1 += 32;
		dst += 32;
		memcpy(dst, src2, 32);
		dst += 32;
		src2 += 32;
	}

	// Copy sprite & palette data out of GFX rom area
	m_rom_sprite_lookup[0] = memregion("sprites_l")->base();
	m_rom_sprite_lookup[1] = memregion("sprites_h")->base();
	m_rom_clut[0] = memregion("sprites_l")->base() + len1 - m_romtableoffset;
	m_rom_clut[1] = memregion("sprites_h")->base() + len2 - m_romtableoffset;
}

void tatsumi_rot_sprite_deice::device_reset()
{
}

template<class BitmapClass>
inline void tatsumi_rot_sprite_deice::roundupt_drawgfxzoomrotate( BitmapClass &dest_bmp, const rectangle &clip,
		gfx_element *gfx, uint32_t code,uint32_t color,int flipx,int flipy,uint32_t ssx,uint32_t ssy,
		int scalex, int scaley, int rotate, int write_priority_only )
{
	if (!scalex || !scaley) return;

	/*
	scalex and scaley are 16.16 fixed point numbers
	1<<15 : shrink to 50%
	1<<16 : uniform scale
	1<<17 : double to 200%
	*/

	/* KW 991012 -- Added code to force clip to bitmap boundary */
	rectangle myclip = clip;
	myclip &= dest_bmp.cliprect();

	if( gfx )
	{
		const pen_t *pal = &m_palette->pen(gfx->colorbase() + gfx->granularity() * (color % gfx->colors()));
		const uint8_t *shadow_pens = m_shadow_pen_array.get() + (gfx->granularity() * (color % gfx->colors()));
		const uint8_t *code_base = gfx->get_data(code % gfx->elements());

		int block_size = 8 * scalex;
		int sprite_screen_height = ((ssy&0xffff)+block_size)>>16;
		int sprite_screen_width = ((ssx&0xffff)+block_size)>>16;

		if (sprite_screen_width && sprite_screen_height)
		{
			/* compute sprite increment per screen pixel */
			int dx = (gfx->width()<<16)/sprite_screen_width;
			int dy = (gfx->height()<<16)/sprite_screen_height;

			int sx;//=ssx>>16;
			int sy;//=ssy>>16;

			int incxx=0x10000;
			int incyx=0x0;

			if (ssx&0x80000000) sx=0-(0x10000 - (ssx>>16)); else sx=ssx>>16;
			if (ssy&0x80000000) sy=0-(0x10000 - (ssy>>16)); else sy=ssy>>16;
			int ex = sx+sprite_screen_width;
			int ey = sy+sprite_screen_height;
			int x_index_base;
			if( flipx )
			{
				x_index_base = (sprite_screen_width-1)*dx;
				dx = -dx;
				incxx=-incxx;
				incyx=-incyx;
			}
			else
			{
				x_index_base = 0;
			}

			int y_index;
			if( flipy )
			{
				y_index = (sprite_screen_height-1)*dy;
				dy = -dy;
			}
			else
			{
				y_index = 0;
			}

			if( sx < myclip.min_x)
			{ /* clip left */
				int pixels = myclip.min_x-sx;
				sx += pixels;
				x_index_base += pixels*dx;
			}
			if( sy < myclip.min_y )
			{ /* clip top */
				int pixels = myclip.min_y-sy;
				sy += pixels;
				y_index += pixels*dy;
			}
			/* NS 980211 - fixed incorrect clipping */
			if( ex > myclip.max_x+1 )
			{ /* clip right */
				int pixels = ex-myclip.max_x-1;
				ex -= pixels;
			}
			if( ey > myclip.max_y+1 )
			{ /* clip bottom */
				int pixels = ey-myclip.max_y-1;
				ey -= pixels;
			}

			if( ex>sx )
			{ /* skip if inner loop doesn't draw anything */
				for( int y=sy; y<ey; y++ )
				{
					uint8_t const *const source = code_base + (y_index>>16) * gfx->rowbytes();
					typename BitmapClass::pixel_t *const dest = &dest_bmp.pix(y);

					int x_index = x_index_base;
					for( int x=sx; x<ex; x++ )
					{
						int c = source[x_index>>16];
						if( c )
						{
							// Only draw shadow pens if writing priority buffer
							if (write_priority_only)
								dest[x]=shadow_pens[c];
							else if (!shadow_pens[c])
								dest[x]=pal[c];
						}
						x_index += dx;
					}

					y_index += dy;
				}
			}
		}
	}
}

template<class BitmapClass>
void tatsumi_rot_sprite_deice::draw_sprites(BitmapClass &bitmap, const rectangle &cliprect, int write_priority_only, int rambank)
{
	// Sprite data is double buffered
	for (int offs = rambank; offs < rambank + 0x800; offs += 6)
	{
		/*
			Sprite RAM itself uses an index into two ROM tables to actually draw the object.

			Sprite RAM format:

			Word 0: 0xf000 - ?
					0x0fff - Index into ROM sprite table
			Word 1: 0x8000 - X Flip
					0x4000 - Y Flip
					0x3000 - ?
					0x0ff8 - Color
					0x0007 - ?
			Word 2: 0xffff - X position
			Word 3: 0xffff - Y position
			Word 4: 0x01ff - Scale
			Word 5: 0x01ff - Rotation

			Sprite ROM table format, alternate lines come from each bank, with the
			very first line indicating control information:

			First bank:
			Byte 0: Y destination offset (in scanlines, unaffected by scale).
			Byte 1: Always 0?
			Byte 2: Number of source scanlines to render from (so unaffected by destination scale).
			Byte 3: Usually 0, sometimes 0x80??

			Other banks:
			Byte 0: Width of line in tiles (-1)
			Byte 1: X offset to start drawing line at (multipled by scale * 8)
			Bytes 2/3: Tile index to start fetching tiles from (increments per tile).

		*/
		int y = m_spriteram[offs + 3];
		int x = m_spriteram[offs + 2];
		int scale = m_spriteram[offs + 4] & 0x1ff;
		int color = m_spriteram[offs + 1] >> 3 & 0x1ff;
		int flip_x = m_spriteram[offs + 1] & 0x8000;
		int flip_y = m_spriteram[offs + 1] & 0x4000;
		int rotate = m_spriteram[offs + 5]; // Todo:  Turned off for now

		if (rotate)
		{
		//	printf("%04x is rotated sprite %04x\n", offs, rotate);
		}

		rotate = 0;

		int index = m_spriteram[offs];

		if (m_spriteram[offs + 1] & 0x3007)
			color = machine().rand() % 0xff;

		/* End of sprite list marker */
		if (index == 0xffff || m_spriteram[offs + 4] == 0xffff) // todo
			return;

		if (index >= 0x4000)
			continue;

		uint8_t const *src1 = m_rom_sprite_lookup[0] + (index * 4);
		uint8_t const *src2 = m_rom_sprite_lookup[1] + (index * 4);

		int lines = src1[2];
		int y_offset = src1[0] & 0xf8;

		lines -= y_offset;

		int render_x = x << 16;
		int render_y = y << 16;
		scale = scale << 9; /* 0x80 becomes 0x10000 */

		if (flip_y)
			render_y -= y_offset * scale;
		else
			render_y += y_offset * scale;

		int extent_x = 0, extent_y = 0;

		src1 += 4;
		int h = 0;

		while (lines > 0)
		{
			int base, x_offs, x_width, x_pos;
			int this_extent = 0;

			/* Odd and even lines come from different banks */
			if (h & 1) {
				x_width = src1[0] + 1;
				x_offs = src1[1] * scale * 8;
				base = src1[2] | (src1[3] << 8);
			}
			else {
				x_width = src2[0] + 1;
				x_offs = src2[1] * scale * 8;
				base = src2[2] | (src2[3] << 8);
			}

			base *= 2;
			
			if (flip_x)
				x_pos = render_x - x_offs - scale * 8;
			else
				x_pos = render_x + x_offs;

			for (int w = 0; w < x_width; w++)
			{

				roundupt_drawgfxzoomrotate(
					bitmap, cliprect, m_gfxdecode->gfx(0),
					base,
					color, flip_x, flip_y, x_pos, render_y,
					scale, scale, 0, write_priority_only);
				base++;

				if (flip_x)
					x_pos -= scale * 8;
				else
					x_pos += scale * 8;

				this_extent += scale * 8;
			}

			if (h & 1)
				src1 += 4;
			else
				src2 += 4;

			if (this_extent > extent_x)
				extent_x = this_extent;
			this_extent = 0;

			if (flip_y)
				render_y -= 8 * scale;
			else
				render_y += 8 * scale;
			extent_y += 8 * scale;

			h++;
			lines -= 8;			
		}
	}
}

/*
 * Object palettes are build from a series of cluts stored in the object roms.
 *
 *  We update 'Mame palettes' from the clut here in order to simplify the
 *  draw routines.  We also note down any uses of the 'shadow' pen (index 255).
 */
void tatsumi_rot_sprite_deice::update_cluts(int fake_palette_offset, int object_base, int length)
{

	const uint8_t* bank1 = m_rom_clut[0];
	const uint8_t* bank2 = m_rom_clut[1];
	for (int i=0; i<length; i+=8)
	{
		m_palette->set_pen_color(fake_palette_offset+i+0,m_palette->pen_color(bank1[1]+object_base));
		m_shadow_pen_array[i+0]=(bank1[1]==255);
		m_palette->set_pen_color(fake_palette_offset+i+1,m_palette->pen_color(bank1[0]+object_base));
		m_shadow_pen_array[i+1]=(bank1[0]==255);
		m_palette->set_pen_color(fake_palette_offset+i+2,m_palette->pen_color(bank1[3]+object_base));
		m_shadow_pen_array[i+2]=(bank1[3]==255);
		m_palette->set_pen_color(fake_palette_offset+i+3,m_palette->pen_color(bank1[2]+object_base));
		m_shadow_pen_array[i+3]=(bank1[2]==255);

		m_palette->set_pen_color(fake_palette_offset+i+4,m_palette->pen_color(bank2[1]+object_base));
		m_shadow_pen_array[i+4]=(bank2[1]==255);
		m_palette->set_pen_color(fake_palette_offset+i+5,m_palette->pen_color(bank2[0]+object_base));
		m_shadow_pen_array[i+5]=(bank2[0]==255);
		m_palette->set_pen_color(fake_palette_offset+i+6,m_palette->pen_color(bank2[3]+object_base));
		m_shadow_pen_array[i+6]=(bank2[3]==255);
		m_palette->set_pen_color(fake_palette_offset+i+7,m_palette->pen_color(bank2[2]+object_base));
		m_shadow_pen_array[i+7]=(bank2[2]==255);

		bank1+=4;
		bank2+=4;
	}
}


void tatsumi_rot_sprite_deice::draw_alpha_pass( const rectangle &cliprect)
{
	draw_sprites(m_screen->priority(),cliprect,1,(m_sprite_control_ram[0xe0]&0x1000) ? 0x1000 : 0); // Alpha pass only
}

void tatsumi_rot_sprite_deice::draw_regular_pass(bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	draw_sprites(bitmap,cliprect,0,(m_sprite_control_ram[0xe0]&0x1000) ? 0x1000 : 0);
}

void tatsumi_rot_sprite_deice::draw_alt_pass(bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	draw_sprites(bitmap, cliprect, 0, (m_sprite_control_ram[0x20] & 0x1000) ? 0x1000 : 0);
}
