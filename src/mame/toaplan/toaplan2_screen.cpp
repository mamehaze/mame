// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "toaplan2_screen.h"

DEFINE_DEVICE_TYPE(TOAPLAN2_SCREEN, toaplan2_screen_device, "toaplan2_screen", "Toaplan 2 Screen")

toaplan2_screen_device::toaplan2_screen_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, TOAPLAN2_SCREEN, tag, owner, clock)
	, m_screen(*this, finder_base::DUMMY_TAG)
{
}

void toaplan2_screen_device::device_start()
{
}

void toaplan2_screen_device::device_reset()
{
}
