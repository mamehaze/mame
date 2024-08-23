// license:BSD-3-Clause
// copyright-holders:David Haywood

#ifndef MAME_TOAPLAN_TOAPLAN2_TXLAYER_H
#define MAME_TOAPLAN_TOAPLAN2_TXLAYER_H

#pragma once

#include "screen.h"

class toaplan2_txlayer_device : public device_t
{
public:
	toaplan2_txlayer_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	template <typename T> void set_screen(T &&tag) { m_screen.set_tag(std::forward<T>(tag)); }

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	required_device<screen_device> m_screen;

};

DECLARE_DEVICE_TYPE(TOAPLAN2_TXLAYER, toaplan2_txlayer_device)


#endif // MAME_TOAPLAN_TOAPLAN2_TXLAYER_H
