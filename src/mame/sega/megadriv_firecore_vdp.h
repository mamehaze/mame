// license:BSD-3-Clause
// copyright-holders:David Haywood
/* */


#ifndef MAME_MISC_INDER_SB_H
#define MAME_VIDEO_MEGADRIV_FIRECORE_H

#pragma once

#include "video/315_5313.h"

DECLARE_DEVICE_TYPE(FIRECORE_VDP, megadriv_firecore_vdp_device)


class megadriv_firecore_vdp_device : public sega315_5313_device
{
public:
	// construction/destruction
	template <typename T>
	megadriv_firecore_vdp_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock, T &&cpu_tag)
		: megadriv_firecore_vdp_device(mconfig, tag, owner, clock)
	{
		m_cpu68k.set_tag(std::forward<T>(cpu_tag));
	}

	megadriv_firecore_vdp_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: sega315_5313_device(mconfig, FIRECORE_VDP, tag, owner, clock)
	{
	}

protected:
	virtual void device_start() override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;

private:

};

#endif // MAME_VIDEO_MEGADRIV_FIRECORE_H
