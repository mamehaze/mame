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

	/* the 6502 core sets the default stack value to 0x01bd
	   and Tetris does not initialize it to anything else

	   Tetris stores the playfield data at 0x100 - 0x1c7 and
	   has a clear routine that will erase that range and
	   trash the stack

	   It seems likely this 6502 sets it to 0x1ff by default
	   at least.

	   According to
	   http://mametesters.org/view.php?id=6486
	   this isn't right for known 6502 types either
	*/
	set_state_int(M6502_S, 0x1ff);
}

void elan_eu3a05_cpu_device::device_start()
{
	m6502_device::device_start();
}

void elan_eu3a05_cpu_device::int_map(address_map &map)
{
	map(0x0000, 0x3fff).ram();
}
