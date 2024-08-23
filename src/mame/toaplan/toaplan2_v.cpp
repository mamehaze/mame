// license:BSD-3-Clause
// copyright-holders:Quench, Yochizo, David Haywood
/***************************************************************************

 Functions to emulate additional video hardware on several Toaplan2 games.
 The main video is handled by the GP9001 (see gp9001.cpp)

 Extra-text RAM format

 Truxton 2, Fixeight and Raizing games have an extra-text layer.

  Text RAM format      $0000-1FFF (actually its probably $0000-0FFF)
  ---- --xx xxxx xxxx = Tile number
  xxxx xx-- ---- ---- = Color (0 - 3Fh) + 40h

  Line select / flip   $0000-01EF (some games go to $01FF (excess?))
  ---x xxxx xxxx xxxx = Line select for each line
  x--- ---- ---- ---- = X flip for each line ???

  Line scroll          $0000-01EF (some games go to $01FF (excess?))
  ---- ---x xxxx xxxx = X scroll for each line


***************************************************************************/

#include "emu.h"
#include "toaplan2.h"


/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/


VIDEO_START_MEMBER(toaplan2_state,toaplan2)
{
	/* our current VDP implementation needs this bitmap to work with */
	m_screen->register_screen_bitmap(m_custom_priority_bitmap);

	if (m_vdp[0] != nullptr)
	{
		m_secondary_render_bitmap.reset();
		m_vdp[0]->custom_priority_bitmap = &m_custom_priority_bitmap;
	}

	if (m_vdp[1] != nullptr)
	{
		m_screen->register_screen_bitmap(m_secondary_render_bitmap);
		m_vdp[1]->custom_priority_bitmap = &m_custom_priority_bitmap;
	}
}





VIDEO_START_MEMBER(toaplan2_state, batsugunbl)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	// This bootleg has additional layer offsets. TODO: further refinement needed
	m_vdp[0]->set_tm_extra_offsets(0, 0, 0, 0, 0);
	m_vdp[0]->set_tm_extra_offsets(1, 0, 0, 0, 0);
	m_vdp[0]->set_tm_extra_offsets(2, 0, 0, 0, 0);
	m_vdp[0]->set_sp_extra_offsets(0x37, 0x07, 0, 0);

	m_vdp[1]->set_tm_extra_offsets(0, -0x05, 0x07, 0, 0);
	m_vdp[1]->set_tm_extra_offsets(1, -0x05, 0x07, 0, 0);
	m_vdp[1]->set_tm_extra_offsets(2, 0, 0, 0, 0);
	m_vdp[1]->set_sp_extra_offsets(0x39, 0x12, 0, 0);

	m_vdp[0]->init_scroll_regs();
	m_vdp[1]->init_scroll_regs();
}


// Dogyuun doesn't appear to require fancy mixing?
u32 toaplan2_state::screen_update_dogyuun(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);
	if (m_vdp[1])
	{
		m_custom_priority_bitmap.fill(0, cliprect);
		m_vdp[1]->render_vdp(bitmap, cliprect);
	}
	if (m_vdp[0])
	{
		m_custom_priority_bitmap.fill(0, cliprect);
		m_vdp[0]->render_vdp(bitmap, cliprect);
	}

	return 0;
}


// renders to 2 bitmaps, and mixes output
u32 toaplan2_state::screen_update_batsugun(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
//  bitmap.fill(0, cliprect);
//  gp9001_custom_priority_bitmap->fill(0, cliprect);

	if (m_vdp[0])
	{
		bitmap.fill(0, cliprect);
		m_custom_priority_bitmap.fill(0, cliprect);
		m_vdp[0]->render_vdp(bitmap, cliprect);
	}
	if (m_vdp[1])
	{
		m_secondary_render_bitmap.fill(0, cliprect);
		m_custom_priority_bitmap.fill(0, cliprect);
		m_vdp[1]->render_vdp(m_secondary_render_bitmap, cliprect);
	}

	// key test places in batsugun
	// level 2 - the two layers of clouds (will appear under background, or over ships if wrong)
	// level 3 - the special effect 'layer' which should be under everything (will appear over background if wrong)
	// level 4(?) - the large clouds (will obscure player if wrong)
	// high score entry - letters will be missing if wrong
	// end credits - various issues if wrong, clouds like level 2
	//
	// when implemented based directly on the PAL equation it doesn't work, however, my own equations roughly based
	// on that do.
	//

	if (m_vdp[0] && m_vdp[1])
	{
		for (int y = cliprect.min_y; y <= cliprect.max_y; y++)
		{
			u16 *const src_vdp0 = &bitmap.pix(y);
			u16 const *const src_vdp1 = &m_secondary_render_bitmap.pix(y);

			for (int x = cliprect.min_x; x <= cliprect.max_x; x++)
			{
				const u16 GPU0_LUTaddr = src_vdp0[x];
				const u16 GPU1_LUTaddr = src_vdp1[x];

				// these equations is derived from the PAL, but doesn't seem to work?

				const bool COMPARISON = ((GPU0_LUTaddr & 0x0780) > (GPU1_LUTaddr & 0x0780));

				// note: GPU1_LUTaddr & 0x000f - transparency check for vdp1? (gfx are 4bpp, the low 4 bits of the lookup would be the pixel data value)
#if 0
				int result =
							((GPU0_LUTaddr & 0x0008) & !COMPARISON)
						| ((GPU0_LUTaddr & 0x0008) & !(GPU1_LUTaddr & 0x000f))
						| ((GPU0_LUTaddr & 0x0004) & !COMPARISON)
						| ((GPU0_LUTaddr & 0x0004) & !(GPU1_LUTaddr & 0x000f))
						| ((GPU0_LUTaddr & 0x0002) & !COMPARISON)
						| ((GPU0_LUTaddr & 0x0002) & !(GPU1_LUTaddr & 0x000f))
						| ((GPU0_LUTaddr & 0x0001) & !COMPARISON)
						| ((GPU0_LUTaddr & 0x0001) & !(GPU1_LUTaddr & 0x000f));

				if (result) src_vdp0[x] = GPU0_LUTaddr;
				else src_vdp0[x] = GPU1_LUTaddr;
#endif
				// this seems to work tho?
				if (!(GPU1_LUTaddr & 0x000f))
				{
					src_vdp0[x] = GPU0_LUTaddr;
				}
				else
				{
					if (!(GPU0_LUTaddr & 0x000f))
					{
						src_vdp0[x] = GPU1_LUTaddr; // bg pen
					}
					else
					{
						if (COMPARISON)
						{
							src_vdp0[x] = GPU1_LUTaddr;
						}
						else
						{
							src_vdp0[x] = GPU0_LUTaddr;
						}

					}
				}
			}
		}
	}

	return 0;
}


u32 toaplan2_state::screen_update_toaplan2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);
	m_custom_priority_bitmap.fill(0, cliprect);
	m_vdp[0]->render_vdp(bitmap, cliprect);

	return 0;
}



void toaplan2_state::screen_vblank(int state)
{
	// rising edge
	if (state)
	{
		if (m_vdp[0]) m_vdp[0]->screen_eof();
		if (m_vdp[1]) m_vdp[1]->screen_eof();
	}
}
