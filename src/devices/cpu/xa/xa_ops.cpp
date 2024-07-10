// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "xa.h"
#include "xadasm.h"

void xa_cpu_device::add_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("ADD.b %s, #$%02x", get_directtext(direct), data8);
}

void xa_cpu_device::addc_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("ADDC.b %s, #$%02x", get_directtext(direct), data8);
}

void xa_cpu_device::sub_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("SUB.b %s, #$%02x", get_directtext(direct), data8);
}

void xa_cpu_device::subc_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("SUBC.b %s, #$%02x", get_directtext(direct), data8);
}

void xa_cpu_device::cmp_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("CMP.b %s, #$%02x", get_directtext(direct), data8);
}

void xa_cpu_device::and_byte_direct_data8(u16 direct, u8 data8)
{
	u8 val = read_direct8(direct);
	u8 newval = val & data8;
	// change N flag, change Z flag
	write_direct8(direct, newval);
}

void xa_cpu_device::or_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("OR.b %s, #$%02x", get_directtext(direct), data8);
}

void xa_cpu_device::xor_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("XOR.b %s, #$%02x", get_directtext(direct), data8);
}

void xa_cpu_device::mov_byte_direct_data8(u16 direct, u8 data8)
{
	u8 newval = data8;
	// do MOV flags?
	write_direct8(direct, newval);
}

