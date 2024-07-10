// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "xa.h"
#include "xadasm.h"


void xa_cpu_device::add_byte_rd_data8(u8 rd, u8 data8)
{
}

void xa_cpu_device::addc_byte_rd_data8(u8 rd, u8 data8)
{
}

void xa_cpu_device::sub_byte_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::subc_byte_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::cmp_byte_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::and_byte_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::or_byte_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::xor_byte_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::mov_byte_rd_data8(u8 rd, u8 data8)
{}

// ------------------------------------------

void xa_cpu_device::add_byte_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::addc_byte_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::sub_byte_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::subc_byte_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::cmp_byte_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::and_byte_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::or_byte_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::xor_byte_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::mov_byte_indrd_data8(u8 rd, u8 data8)
{
}

// ------------------------------------------

void xa_cpu_device::add_byte_indrdinc_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::addc_byte_indrdinc_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::sub_byte_indrdinc_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::subc_byte_indrdinc_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::cmp_byte_indrdinc_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::and_byte_indrdinc_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::or_byte_indrdinc_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::xor_byte_indrdinc_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::mov_byte_indrdinc_data8(u8 rd, u8 data8)
{}


// ------------------------------------------

void xa_cpu_device::add_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)
{}
void xa_cpu_device::addc_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)
{}
void xa_cpu_device::sub_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)
{}
void xa_cpu_device::subc_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)
{}
void xa_cpu_device::cmp_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)
{}
void xa_cpu_device::and_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)
{}
void xa_cpu_device::or_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)
{}
void xa_cpu_device::xor_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)
{}
void xa_cpu_device::mov_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)
{}

// ------------------------------------------



void xa_cpu_device::add_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)
{}
void xa_cpu_device::addc_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)
{}
void xa_cpu_device::sub_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)
{}
void xa_cpu_device::subc_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)
{}
void xa_cpu_device::cmp_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)
{}
void xa_cpu_device::and_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)
{}
void xa_cpu_device::or_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)
{}
void xa_cpu_device::xor_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)
{}
void xa_cpu_device::mov_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)
{}

// ------------------------------------------

void xa_cpu_device::add_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("ADD.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);
}

void xa_cpu_device::addc_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("ADDC.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);
}

void xa_cpu_device::sub_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("SUB.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);
}

void xa_cpu_device::subc_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("SUBC.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);
}

void xa_cpu_device::cmp_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("CMP.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);
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
	fatalerror("OR.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);
}

void xa_cpu_device::xor_byte_direct_data8(u16 direct, u8 data8)
{
	fatalerror("XOR.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);
}

void xa_cpu_device::mov_byte_direct_data8(u16 direct, u8 data8)
{
	u8 newval = data8;
	// do MOV flags?
	write_direct8(direct, newval);
}

// -----------------------------------------------

void xa_cpu_device::add_word_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::addc_word_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::sub_word_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::subc_word_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::cmp_word_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::and_word_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::or_word_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::xor_word_rd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::mov_word_rd_data8(u8 rd, u8 data8)
{}

// -----------------------------------------------

void xa_cpu_device::add_word_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::addc_word_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::sub_word_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::subc_word_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::cmp_word_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::and_word_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::or_word_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::xor_word_indrd_data8(u8 rd, u8 data8)
{}
void xa_cpu_device::mov_word_indrd_data8(u8 rd, u8 data8)
{}

// -----------------------------------------------

void xa_cpu_device::add_word_indrdinc_data8(u8 rd, u8 data8){}
void xa_cpu_device::addc_word_indrdinc_data8(u8 rd, u8 data8){}
void xa_cpu_device::sub_word_indrdinc_data8(u8 rd, u8 data8){}
void xa_cpu_device::subc_word_indrdinc_data8(u8 rd, u8 data8){}
void xa_cpu_device::cmp_word_indrdinc_data8(u8 rd, u8 data8){}
void xa_cpu_device::and_word_indrdinc_data8(u8 rd, u8 data8){}
void xa_cpu_device::or_word_indrdinc_data8(u8 rd, u8 data8){}
void xa_cpu_device::xor_word_indrdinc_data8(u8 rd, u8 data8){}
void xa_cpu_device::mov_word_indrdinc_data8(u8 rd, u8 data8){}

// -----------------------------------------------

void xa_cpu_device::add_word_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){}
void xa_cpu_device::addc_word_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){}
void xa_cpu_device::sub_word_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){}
void xa_cpu_device::subc_word_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){}
void xa_cpu_device::cmp_word_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){}
void xa_cpu_device::and_word_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){}
void xa_cpu_device::or_word_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){}
void xa_cpu_device::xor_word_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){}
void xa_cpu_device::mov_word_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){}

// -----------------------------------------------

void xa_cpu_device::add_word_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){}
void xa_cpu_device::addc_word_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){}
void xa_cpu_device::sub_word_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){}
void xa_cpu_device::subc_word_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){}
void xa_cpu_device::cmp_word_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){}
void xa_cpu_device::and_word_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){}
void xa_cpu_device::or_word_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){}
void xa_cpu_device::xor_word_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){}
void xa_cpu_device::mov_word_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){}

// -----------------------------------------------

void xa_cpu_device::add_word_direct_data8(u8 rd, u8 data8){}
void xa_cpu_device::addc_word_direct_data8(u8 rd, u8 data8){}
void xa_cpu_device::sub_word_direct_data8(u8 rd, u8 data8){}
void xa_cpu_device::subc_word_direct_data8(u8 rd, u8 data8){}
void xa_cpu_device::cmp_word_direct_data8(u8 rd, u8 data8){}
void xa_cpu_device::and_word_direct_data8(u8 rd, u8 data8){}
void xa_cpu_device::or_word_direct_data8(u8 rd, u8 data8){}
void xa_cpu_device::xor_word_direct_data8(u8 rd, u8 data8){}
void xa_cpu_device::mov_word_direct_data8(u8 rd, u8 data8){}

// -----------------------------------------------



void xa_cpu_device::add_word_rd_data16(u8 rd, u16 data16)
{
}

void xa_cpu_device::addc_word_rd_data16(u8 rd, u16 data16)
{
}

void xa_cpu_device::sub_word_rd_data16(u8 rd, u16 data16)
{
}

void xa_cpu_device::subc_word_rd_data16(u8 rd, u16 data16)
{
}

void xa_cpu_device::cmp_word_rd_data16(u8 rd, u16 data16)
{
}

void xa_cpu_device::and_word_rd_data16(u8 rd, u16 data16)
{
}

void xa_cpu_device::or_word_rd_data16(u8 rd, u16 data16)
{
}

void xa_cpu_device::xor_word_rd_data16(u8 rd, u16 data16)
{
}

void xa_cpu_device::mov_word_rd_data16(u8 rd, u16 data16)
{
	printf("MOV.w %s, #$%04x\n", m_regnames16[rd], data16);
	set_reg16(rd, data16);
}

// -----------------------------------------------

void xa_cpu_device::add_word_indrd_data16(u8 rd, u16 data16){}
void xa_cpu_device::addc_word_indrd_data16(u8 rd, u16 data16){}
void xa_cpu_device::sub_word_indrd_data16(u8 rd, u16 data16){}
void xa_cpu_device::subc_word_indrd_data16(u8 rd, u16 data16){}
void xa_cpu_device::cmp_word_indrd_data16(u8 rd, u16 data16){}
void xa_cpu_device::and_word_indrd_data16(u8 rd, u16 data16){}
void xa_cpu_device::or_word_indrd_data16(u8 rd, u16 data16){}
void xa_cpu_device::xor_word_indrd_data16(u8 rd, u16 data16){}
void xa_cpu_device::mov_word_indrd_data16(u8 rd, u16 data16){}

void xa_cpu_device::add_word_indrdinc_data16(u8 rd, u16 data16){}
void xa_cpu_device::addc_word_indrdinc_data16(u8 rd, u16 data16){}
void xa_cpu_device::sub_word_indrdinc_data16(u8 rd, u16 data16){}
void xa_cpu_device::subc_word_indrdinc_data16(u8 rd, u16 data16){}
void xa_cpu_device::cmp_word_indrdinc_data16(u8 rd, u16 data16){}
void xa_cpu_device::and_word_indrdinc_data16(u8 rd, u16 data16){}
void xa_cpu_device::or_word_indrdinc_data16(u8 rd, u16 data16){}
void xa_cpu_device::xor_word_indrdinc_data16(u8 rd, u16 data16){}
void xa_cpu_device::mov_word_indrdinc_data16(u8 rd, u16 data16){}

void xa_cpu_device::add_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){}
void xa_cpu_device::addc_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){}
void xa_cpu_device::sub_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){}
void xa_cpu_device::subc_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){}
void xa_cpu_device::cmp_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){}
void xa_cpu_device::and_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){}
void xa_cpu_device::or_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){}
void xa_cpu_device::xor_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){}
void xa_cpu_device::mov_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){}

void xa_cpu_device::add_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){}
void xa_cpu_device::addc_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){}
void xa_cpu_device::sub_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){}
void xa_cpu_device::subc_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){}
void xa_cpu_device::cmp_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){}
void xa_cpu_device::and_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){}
void xa_cpu_device::or_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){}
void xa_cpu_device::xor_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){}
void xa_cpu_device::mov_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){}

void xa_cpu_device::add_word_direct_data16(u16 direct, u16 data16){}
void xa_cpu_device::addc_word_direct_data16(u16 direct, u16 data16){}
void xa_cpu_device::sub_word_direct_data16(u16 direct, u16 data16){}
void xa_cpu_device::subc_word_direct_data16(u16 direct, u16 data16){}
void xa_cpu_device::cmp_word_direct_data16(u16 direct, u16 data16){}
void xa_cpu_device::and_word_direct_data16(u16 direct, u16 data16){}
void xa_cpu_device::or_word_direct_data16(u16 direct, u16 data16){}
void xa_cpu_device::xor_word_direct_data16(u16 direct, u16 data16){}
void xa_cpu_device::mov_word_direct_data16(u16 direct, u16 data16){}
