// license:BSD-3-Clause
// copyright-holders:David Haywood

#ifndef MAME_TVGAMES_ELAN_EU3A05_SOC_H
#define MAME_TVGAMES_ELAN_EU3A05_SOC_H


#include "elan_eu3a05_a.h"

#include "cpu/m6502/m6502.h"
//#include "cpu/m6502/w65c02.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"

#include "machine/bankdev.h"
#include "elan_eu3a05gpio.h"
#include "elan_eu3a05sys.h"
#include "elan_eu3a05vid.h"

class elan_eu3a05_cpu_device : public m6502_device {
public:
	elan_eu3a05_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void set_current_bank(uint16_t bank) { m_current_bank = bank; }

	void generate_custom_interrupt(int irq) { m_sys->generate_custom_interrupt(9); }

protected:
	virtual void device_add_mconfig(machine_config &config) override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;
	virtual void device_start() override ATTR_COLD;

	// device_memory_interface overrides
	virtual space_config_vector memory_space_config() const override;

	void int_map(address_map &map);
	address_space_config m_extbus_config;
	address_space *m_extbus_space;

private:
	uint8_t bank_r(offs_t offset);
	void bank_w(offs_t offset, uint8_t data);
	uint8_t read_full_space(offs_t offset);
	uint8_t read_fixed_bank(offs_t offset);

	void bank_change(uint16_t data)
	{
		set_current_bank(data);
	}

	// screen updates
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void sound_end0(int state) { m_sys->generate_custom_interrupt(2); }
	void sound_end1(int state) { m_sys->generate_custom_interrupt(3); }
	void sound_end2(int state) { m_sys->generate_custom_interrupt(4); }
	void sound_end3(int state) { m_sys->generate_custom_interrupt(5); }
	void sound_end4(int state) { m_sys->generate_custom_interrupt(6); }
	void sound_end5(int state) { m_sys->generate_custom_interrupt(7); }

	uint16_t m_current_bank;

	required_device<elan_eu3a05sys_device> m_sys;
	required_device<elan_eu3a05_sound_device> m_sound;
	required_device<elan_eu3a05gpio_device> m_gpio;
	required_device<screen_device> m_screen;
	required_device<elan_eu3a05vid_device> m_vid;
	required_device<palette_device> m_palette;
};

DECLARE_DEVICE_TYPE(ELAN_EU3A05_SOC, elan_eu3a05_cpu_device)

#endif // MAME_TVGAMES_ELAN_EU3A05_SOC_H
