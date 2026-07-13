// license:BSD-3-Clause
// copyright-holders:David Haywood
/*****************************************************************************

    GPL16218B
    GPL16238B (this could be GPAC500B)
    GPL16248VB
    GPL16258VB (this could be GPAC800B)

**********************************************************************/

#include "emu.h"
#include "generalplus_gpl162xx_b_soc.h"

generalplus_gpl16218b_device::generalplus_gpl16218b_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock) :
	generalplus_gpl162xx_base_device(mconfig, GPL16250VA, tag, owner, clock, address_map_constructor(FUNC(generalplus_gpl16218b_device::gcm394_internal_map), this))
{
}

DEFINE_DEVICE_TYPE(GPL16218B, generalplus_gpl16218b_device, "gcm394", "GeneralPlus GPL16218B System-on-a-Chip")
