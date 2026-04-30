// license:BSD-3-Clause
// copyright-holders:David Haywood

#ifndef MAME_MACHINE_GENERALPLUS_GPL951XX_SOC_H
#define MAME_MACHINE_GENERALPLUS_GPL951XX_SOC_H

#pragma once

#include "generalplus_gpl162xx_soc.h"

class generalplus_gpl951xx_device : public sunplus_gcm394_base_device
{
public:
	template <typename T>
	generalplus_gpl951xx_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock, T &&screen_tag) :
		generalplus_gpl951xx_device(mconfig, tag, owner, clock)
	{
		m_screen.set_tag(std::forward<T>(screen_tag));
		//m_csbase = 0x30000;
		// TODO: is cs_space even used by this type?
		m_csbase = 0xffffffff;
	}

	generalplus_gpl951xx_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	void gpspi_direct_internal_map(address_map &map) ATTR_COLD;

	virtual void device_start() override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;

private:
	// SPIFC interface

	u16 spifc_ctrl_r();
	void spifc_ctrl_w(u16 data);
	u16 spifc_cmd_r();
	void spifc_cmd_w(u16 data);
	u16 spifc_para_r();
	void spifc_para_w(u16 data);
	u16 spifc_addrl_r();
	void spifc_addrl_w(u16 data);
	u16 spifc_addrh_r();
	void spifc_addrh_w(u16 data);
	u16 spifc_txdat_r();
	void spifc_txdat_w(u16 data);
	u16 spifc_rxdat_r();
	void spifc_rxdat_w(u16 data);
	u16 spifc_tx_bc_r();
	void spifc_tx_bc_w(u16 data);
	u16 spifc_rx_bc_r();
	void spifc_rx_bc_w(u16 data);
	u16 spifc_timing_r();
	void spifc_timing_w(u16 data);
	u16 spifc_ctrl2_r();
	void spifc_ctrl2_w(u16 data);

	u16 pllsel_r();
	void pllsel_w(uint16_t data);

	// Byte swap etc.
	u16 byte_swap_r();
	void byte_swap_w(u16 data);

	// RTC
	u16 rtc_readdata_r();
	u16 rtc_ready_r();

	// Timers (different compared to GPL162xx)
	u16 gpl951xx_timerg_ctrl_r();
	void gpl951xx_timerg_ctrl_w(u16 data);
	u16 gpl951xx_timerg_preload_r();
	void gpl951xx_timerg_preload_w(u16 data);
	u16 gpl951xx_timerh_ctrl_r();
	void gpl951xx_timerh_ctrl_w(u16 data);
	u16 gpl951xx_timerh_preload_r();
	void gpl951xx_timerh_preload_w(u16 data);

	u16 m_byteswap;

	u16 m_gpl951xx_timerg_preload;
	u16 m_gpl951xx_timerg_ctrl;
	u16 m_gpl951xx_timerh_preload;
	u16 m_gpl951xx_timerh_ctrl;

	u16 m_spifc_ctrl;
};

DECLARE_DEVICE_TYPE(GPL951XX, generalplus_gpl951xx_device)

#endif // MAME_MACHINE_GENERALPLUS_GPL951XX_SOC_H
