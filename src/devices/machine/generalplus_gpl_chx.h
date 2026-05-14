// license:BSD-3-Clause
// copyright-holders:David Haywood
#ifndef MAME_MACHINE_GENERALPLUS_GPL_CHX_H
#define MAME_MACHINE_GENERALPLUS_GPL_CHX_H

#pragma once

class gpl_chx_device : public device_t
{
public:
	gpl_chx_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	u16 cha_ctrl_r();
	void cha_ctrl_w(u16 data);
	u16 cha_data_r();
	void cha_data_w(u16 data);
	u16 cha_fifo_r();
	void cha_fifo_w(u16 data);
	u16 chb_ctrl_r();
	void chb_ctrl_w(u16 data);
	u16 chb_data_r();
	void chb_data_w(u16 data);
	u16 chb_fifo_r();
	void chb_fifo_w(u16 data);

protected:
	virtual void device_start() override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;
	virtual void device_add_mconfig(machine_config &config) override ATTR_COLD;

private:
	u16 m_cha_ctrl;
	u16 m_chb_ctrl;
};

DECLARE_DEVICE_TYPE(GPL_CHX, gpl_chx_device)

#endif // MAME_MACHINE_GENERALPLUS_GPL_CHX_H
