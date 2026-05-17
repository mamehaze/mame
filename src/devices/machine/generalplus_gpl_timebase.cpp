// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "generalplus_gpl_timebase.h"

#define LOG_TIMEBASE (1U << 1)

#define VERBOSE     (0)

#include "logmacro.h"

DEFINE_DEVICE_TYPE(GPL_TIMEBASE, gpl_timebase_device, "gpl_timebase", "Generalplus GPL162xx / GPL951xx System Timebase")

gpl_timebase_device::gpl_timebase_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, GPL_TIMEBASE, tag, owner, clock),
	m_timebase_a(*this, "timebase_a"),
	m_timebase_b(*this, "timebase_b"),
	m_timebase_c(*this, "timebase_c"),
	m_updateirqs_cb(*this)
{
}

void gpl_timebase_device::device_start()
{
	save_item(NAME(m_timebase_ctrl));
	save_item(NAME(m_timebase_reset));
}

void gpl_timebase_device::device_reset()
{
	m_timebase_ctrl[0] = 0x0000;
	m_timebase_ctrl[1] = 0x0000;
	m_timebase_ctrl[2] = 0x0000;
	m_timebase_reset = 0x0000;
}

attotime gpl_timebase_device::get_timer_period(int timer, int ctrlval)
{
	int which = (timer << 2) | ctrlval;

	if (which != 0) // timebase_a, fequency 0 is 'off'
	{
		int period = 1 << (which - 1);
		return attotime::from_hz(period);
	}

	return attotime::never;
}

// Timebase ('fixed' frequency timers)
// (each can select from 3 different frequencies, different for each timer)

TIMER_DEVICE_CALLBACK_MEMBER( gpl_timebase_device::timebase_a_cb )
{
	// sets bit 15 in m_timebase_ctrl[2], also visible (as read only) in P_INT_Status2, bit 8
	// uses IRQ7 (FIQ option not available)
	m_timebase_ctrl[0] |= 0x8000;
	m_updateirqs_cb(1);
}

// P_TimeBaseA_Ctrl
//
// 15  TMBAIF/C
// 14  TMBAIE
// 13  TMBAEN
// 12
//
// 11
// 10
//  9
//  8
//  7
//  6
//  5
//  4
//  3
//  2
//  1  TMBAS[1]
//  0  TMBAS[0]  00 = Reserved, 01 = 1Hz, 10 = 2Hz, 11 = 4Hz

u16 gpl_timebase_device::timebasea_ctrl_r()
{
	LOGMASKED(LOG_TIMEBASE, "%s:sunplus_gcm394_base_device::timebasea_ctrl_r\n", machine().describe_context());
	return m_timebase_ctrl[0];
}

void gpl_timebase_device::timebasea_ctrl_w(u16 data)
{
	LOGMASKED(LOG_TIMEBASE, "%s:sunplus_gcm394_base_device::timebasea_ctrl_w %04x\n", machine().describe_context(), data);

	if (data & 0x8000)
	{
		m_timebase_ctrl[0] = data & 0x7fff;
		m_updateirqs_cb(1);
	}
	else
	{
		m_timebase_ctrl[0] = data;
	}

	if (m_timebase_ctrl[0] & 0x2000)
	{
		attotime period = get_timer_period(0, m_timebase_ctrl[0] & 0x0003);
		m_timebase_a->adjust(period, 0, period);
	}
	else
	{
		m_timebase_a->adjust(attotime::never);
	}
}

TIMER_DEVICE_CALLBACK_MEMBER( gpl_timebase_device::timebase_b_cb )
{
	// sets bit 15 in m_timebase_ctrl[1], also visible (as read only) in P_INT_Status2, bit 9
	// uses IRQ7 (FIQ option not available)
	m_timebase_ctrl[1] |= 0x8000;
	m_updateirqs_cb(1);
}

u16 gpl_timebase_device::timebaseb_ctrl_r()
{
	LOGMASKED(LOG_TIMEBASE, "%s:sunplus_gcm394_base_device::timebaseb_ctrl_r\n", machine().describe_context());
	return m_timebase_ctrl[1];
}

void gpl_timebase_device::timebaseb_ctrl_w(u16 data)
{
	LOGMASKED(LOG_TIMEBASE, "%s:sunplus_gcm394_base_device::timebaseb_ctrl_w %04x\n", machine().describe_context(), data);

	if (data & 0x8000)
	{
		m_timebase_ctrl[1] = data & 0x7fff;
		m_updateirqs_cb(1);
	}
	else
	{
		m_timebase_ctrl[1] = data;
	}

	if (m_timebase_ctrl[1] & 0x2000)
	{
		attotime period = get_timer_period(1, m_timebase_ctrl[1] & 0x0003);
		m_timebase_b->adjust(period, 0, period);	
	}
	else
	{
		m_timebase_b->adjust(attotime::never);
	}
}

TIMER_DEVICE_CALLBACK_MEMBER( gpl_timebase_device::timebase_c_cb )
{
	// sets bit 15 in m_timebase_ctrl[2], also visible (as read only) in P_INT_Status2, bit 10
	// uses IRQ6 (FIQ option not available)
	m_timebase_ctrl[2] |= 0x8000;
	m_updateirqs_cb(1);
}

u16 gpl_timebase_device::timebasec_ctrl_r()
{
	LOGMASKED(LOG_TIMEBASE, "%s:sunplus_gcm394_base_device::timebasec_ctrl_r\n", machine().describe_context());
	return m_timebase_ctrl[2];
}

void gpl_timebase_device::timebasec_ctrl_w(u16 data)
{
	LOGMASKED(LOG_TIMEBASE, "%s:sunplus_gcm394_base_device::timebasec_ctrl_w %04x\n", machine().describe_context(), data);

	if (data & 0x8000)
	{
		m_timebase_ctrl[2] = data & 0x7fff;
		m_updateirqs_cb(1);
	}
	else
	{
		m_timebase_ctrl[2] = data;
	}

	if (m_timebase_ctrl[2] & 0x2000)
	{
		attotime period = get_timer_period(2, m_timebase_ctrl[2] & 0x0003);
		m_timebase_c->adjust(period, 0, period);	
	}
	else
	{
		m_timebase_c->adjust(attotime::never);
	}
}

void gpl_timebase_device::timebase_reset_w(u16 data)
{
	LOGMASKED(LOG_TIMEBASE, "%s:sunplus_gcm394_base_device::timebase_reset_w %04x\n", machine().describe_context(), data);
	m_timebase_reset = data;
}

void gpl_timebase_device::device_add_mconfig(machine_config &config)
{
	TIMER(config, "timebase_a").configure_generic(FUNC(gpl_timebase_device::timebase_a_cb));
	TIMER(config, "timebase_b").configure_generic(FUNC(gpl_timebase_device::timebase_b_cb));
	TIMER(config, "timebase_c").configure_generic(FUNC(gpl_timebase_device::timebase_c_cb));
}
