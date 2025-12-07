// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "elan_eu3a05_soc.h"

DEFINE_DEVICE_TYPE(ELAN_EU3A05_SOC,     elan_eu3a05_cpu_device,     "elan_eu3a05_cpu_device",     "ELAN EU3A05 (NTSC)")

elan_eu3a05_cpu_device::elan_eu3a05_cpu_device(const machine_config& mconfig, device_type type, const char* tag, device_t* owner, uint32_t clock) :
	m6502_device(mconfig, type, tag, owner, clock),
	m_extbus_config("extbus", ENDIANNESS_LITTLE, 8, 24)
{
	m_extbus_config.m_addr_width = 24;
	m_extbus_config.m_logaddr_width = 24;
	program_config.m_internal_map = address_map_constructor(FUNC(elan_eu3a05_cpu_device::int_map), this);
}

elan_eu3a05_cpu_device::elan_eu3a05_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	elan_eu3a05_cpu_device(mconfig, ELAN_EU3A05_SOC, tag, owner, clock)
{
}

void elan_eu3a05_cpu_device::device_add_mconfig(machine_config &config)
{
}

device_memory_interface::space_config_vector elan_eu3a05_cpu_device::memory_space_config() const
{
	if(has_configured_map(AS_OPCODES))
		return space_config_vector {
			std::make_pair(AS_PROGRAM, &program_config),
			std::make_pair(AS_OPCODES, &sprogram_config),
			std::make_pair(5, &m_extbus_config),
		};
	else
		return space_config_vector {
			std::make_pair(AS_PROGRAM, &program_config),
			std::make_pair(5, &m_extbus_config),
		};
}

void elan_eu3a05_cpu_device::device_reset()
{
	m6502_device::device_reset();
}

void elan_eu3a05_cpu_device::device_start()
{
	m6502_device::device_start();
}

void elan_eu3a05_cpu_device::int_map(address_map &map)
{
	map(0x0000, 0x3fff).ram();
}
