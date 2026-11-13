// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "megadriv_firecore_vdp.h"


DEFINE_DEVICE_TYPE(FIRECORE_VDP, megadriv_firecore_vdp_device, "megadriv_firecore_vdp_device", "FireCore / RedKid VDP (MegaDrive clone)")


void megadriv_firecore_vdp_device::device_start()
{
	sega315_5313_device::device_start();
}

void megadriv_firecore_vdp_device::device_reset()
{
	sega315_5313_device::device_reset();
}
