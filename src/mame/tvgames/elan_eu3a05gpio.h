// license:BSD-3-Clause
// copyright-holders:David Haywood

#ifndef MAME_TVGAMES_ELAN_EU3A05GPIO_H
#define MAME_TVGAMES_ELAN_EU3A05GPIO_H


class elan_eu3a05gpio_device : public device_t
{
public:
	elan_eu3a05gpio_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto write_0_callback() { return m_write_callback[0].bind(); }
	auto write_1_callback() { return m_write_callback[1].bind(); }
	auto write_2_callback() { return m_write_callback[2].bind(); }
	auto read_0_callback() { return m_read_callback[0].bind(); }
	auto read_1_callback() { return m_read_callback[1].bind(); }
	auto read_2_callback() { return m_read_callback[2].bind(); }

	uint8_t gpio_r(offs_t offset);
	void gpio_w(offs_t offset, uint8_t data);

	void gpio_unk_w(offs_t offset, uint8_t data);

protected:
	// device-level overrides
	virtual void device_start() override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;

private:
	devcb_read8::array<3> m_read_callback;
	devcb_write8::array<3> m_write_callback;

	uint8_t read_port_data(int which);
	uint8_t read_direction(int which);
	void write_port_data(int which, uint8_t data);
	void write_direction(int which, uint8_t data);

	uint8_t m_ddr[3];
	uint8_t m_unk[3];
};

DECLARE_DEVICE_TYPE(ELAN_EU3A05_GPIO, elan_eu3a05gpio_device)

#endif // MAME_TVGAMES_RAD_EU3A05GPIO_H
