// license:BSD-3-Clause
// copyright-holders:David Haywood

#ifndef MAME_TVGAMES_ELAN_EP3A19A_SOC_H
#define MAME_TVGAMES_ELAN_EP3A19A_SOC_H

#include "cpu/m6502/m6502.h"
//#include "cpu/m6502/w65c02.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"


class elan_ep3a19a_cpu_device : public m6502_device {
public:
	elan_ep3a19a_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	elan_ep3a19a_cpu_device(const machine_config& mconfig, device_type type, const char* tag, device_t* owner, uint32_t clock);

	virtual void device_add_mconfig(machine_config &config) override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;
	virtual void device_start() override ATTR_COLD;

	// device_memory_interface overrides
	virtual space_config_vector memory_space_config() const override;

	void int_map(address_map &map);
	address_space_config m_extbus_config;
	address_space *m_extbus_space;

private:

};

DECLARE_DEVICE_TYPE(ELAN_EP3A19A_SOC,     elan_ep3a19a_cpu_device)

#endif // MAME_TVGAMES_ELAN_EP3A19A_SOC_H
