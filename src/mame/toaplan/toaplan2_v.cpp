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
