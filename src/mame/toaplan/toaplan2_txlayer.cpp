// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "toaplan2_txlayer.h"

DEFINE_DEVICE_TYPE(TOAPLAN2_TXLAYER, toaplan2_txlayer_device, "toaplan2_txlayer", "Toaplan 2 Text Layer")

toaplan2_txlayer_device::toaplan2_txlayer_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, TOAPLAN2_TXLAYER, tag, owner, clock)
	, m_screen(*this, finder_base::DUMMY_TAG)
{
}

void toaplan2_txlayer_device::device_start()
{
}

void toaplan2_txlayer_device::device_reset()
{
}
