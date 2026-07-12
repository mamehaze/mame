// license:BSD-3-Clause
// copyright-holders:David Haywood
/*****************************************************************************

  SunPlus "GCM394" (based on die pictures)

**********************************************************************/

#ifndef MAME_MACHINE_GENERALPLUS_GPL162XX_B_SOC_H
#define MAME_MACHINE_GENERALPLUS_GPL162XX_B_SOC_H

#pragma once

#include "generalplus_gpl162xx_soc.h"

class generalplus_gpl16218b_device : public generalplus_gpl162xx_base_device
{
public:
	template <typename T>
	generalplus_gpl16218b_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock, T &&screen_tag) :
		generalplus_gpl16218b_device(mconfig, tag, owner, clock)
	{
		m_screen.set_tag(std::forward<T>(screen_tag));
	}

	generalplus_gpl16218b_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);
};

DECLARE_DEVICE_TYPE(GPL16218A, generalplus_gpl16218b_device)


#endif // MAME_MACHINE_GENERALPLUS_GPL162XX_B_SOC_H
