// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "xa.h"
#include "xadasm.h"


void xa_cpu_device::add_byte_rd_data8(u8 rd, u8 data8) { fatalerror( "ADD.b %s, #$%02x (RD, DATA8)", m_regnames8[rd], data8 ); }
void xa_cpu_device::addc_byte_rd_data8(u8 rd, u8 data8){ fatalerror( "ADDC.b %s, #$%02x (RD, DATA8)", m_regnames8[rd], data8 ); }
void xa_cpu_device::sub_byte_rd_data8(u8 rd, u8 data8) { fatalerror( "SUB.b %s, #$%02x (RD, DATA8)", m_regnames8[rd], data8 ); }
void xa_cpu_device::subc_byte_rd_data8(u8 rd, u8 data8){ fatalerror( "SUBC.b %s, #$%02x (RD, DATA8)", m_regnames8[rd], data8 ); }
void xa_cpu_device::cmp_byte_rd_data8(u8 rd, u8 data8) { fatalerror( "CMP.b %s, #$%02x (RD, DATA8)", m_regnames8[rd], data8 ); }
void xa_cpu_device::and_byte_rd_data8(u8 rd, u8 data8) { fatalerror( "AND.b %s, #$%02x (RD, DATA8)", m_regnames8[rd], data8 ); }
void xa_cpu_device::or_byte_rd_data8(u8 rd, u8 data8)  { fatalerror( "OR.b %s, #$%02x (RD, DATA8)", m_regnames8[rd], data8 ); }
void xa_cpu_device::xor_byte_rd_data8(u8 rd, u8 data8) { fatalerror( "XOR.b %s, #$%02x (RD, DATA8)", m_regnames8[rd], data8 ); }
void xa_cpu_device::mov_byte_rd_data8(u8 rd, u8 data8) { fatalerror( "MOV.b %s, #$%02x (RD, DATA8)", m_regnames8[rd], data8 ); }

// ------------------------------------------

void xa_cpu_device::add_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "ADD.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::addc_byte_indrd_data8(u8 rd, u8 data8){ fatalerror( "ADDC.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::sub_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "SUB.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::subc_byte_indrd_data8(u8 rd, u8 data8){ fatalerror( "SUBC.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::cmp_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "CMP.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::and_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "AND.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::or_byte_indrd_data8(u8 rd, u8 data8)  { fatalerror( "OR.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::xor_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "XOR.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::mov_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "MOV.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }

// ------------------------------------------

void xa_cpu_device::add_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "ADD.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::addc_byte_indrdinc_data8(u8 rd, u8 data8){ fatalerror( "ADDC.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::sub_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "SUB.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::subc_byte_indrdinc_data8(u8 rd, u8 data8){ fatalerror( "SUBC.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::cmp_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "CMP.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::and_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "AND.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::or_byte_indrdinc_data8(u8 rd, u8 data8)  { fatalerror( "OR.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::xor_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "XOR.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::mov_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "MOV.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }

// ------------------------------------------

void xa_cpu_device::add_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "ADD.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::addc_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){ fatalerror( "ADDC.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::sub_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "SUB.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::subc_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){ fatalerror( "SUBC.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::cmp_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "CMP.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::and_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "AND.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::or_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)  { fatalerror( "OR.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::xor_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "XOR.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::mov_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "MOV.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }

// ------------------------------------------

void xa_cpu_device::add_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "ADD.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::addc_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){ fatalerror( "ADDC.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::sub_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "SUB.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::subc_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){ fatalerror( "SUBC.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::cmp_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "CMP.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::and_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "AND.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::or_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)  { fatalerror( "OR.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::xor_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "XOR.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::mov_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "MOV.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }

// ------------------------------------------

void xa_cpu_device::add_byte_direct_data8(u16 direct, u8 data8) { fatalerror("ADD.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8); }
void xa_cpu_device::addc_byte_direct_data8(u16 direct, u8 data8){ fatalerror("ADDC.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);}
void xa_cpu_device::sub_byte_direct_data8(u16 direct, u8 data8) { fatalerror("SUB.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8); }
void xa_cpu_device::subc_byte_direct_data8(u16 direct, u8 data8){ fatalerror("SUBC.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);}
void xa_cpu_device::cmp_byte_direct_data8(u16 direct, u8 data8) { fatalerror("CMP.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8); }

void xa_cpu_device::and_byte_direct_data8(u16 direct, u8 data8)
{
	u8 val = read_direct8(direct);
	u8 newval = val & data8;
	// change N flag, change Z flag
	write_direct8(direct, newval);
}

void xa_cpu_device::or_byte_direct_data8(u16 direct, u8 data8)  { fatalerror("OR.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8); }
void xa_cpu_device::xor_byte_direct_data8(u16 direct, u8 data8) { fatalerror("XOR.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8); }

void xa_cpu_device::mov_byte_direct_data8(u16 direct, u8 data8)
{
	u8 newval = data8;
	// do MOV flags?
	write_direct8(direct, newval);
}

// -----------------------------------------------

void xa_cpu_device::add_word_rd_data16(u8 rd, u16 data16) { fatalerror("ADD.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::addc_word_rd_data16(u8 rd, u16 data16){ fatalerror("ADDC.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::sub_word_rd_data16(u8 rd, u16 data16) { fatalerror("SUB.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::subc_word_rd_data16(u8 rd, u16 data16){ fatalerror("SUBC.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::cmp_word_rd_data16(u8 rd, u16 data16) { fatalerror("CMP.w % s, #$ % 04x(RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::and_word_rd_data16(u8 rd, u16 data16) { fatalerror("AND.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::or_word_rd_data16(u8 rd, u16 data16)  { fatalerror("OR.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::xor_word_rd_data16(u8 rd, u16 data16) { fatalerror("XOR.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }

void xa_cpu_device::mov_word_rd_data16(u8 rd, u16 data16){
	printf("MOV.w %s, #$%04x\n", m_regnames16[rd], data16);
	set_reg16(rd, data16);
}

// -----------------------------------------------

void xa_cpu_device::add_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "ADD.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::addc_word_indrd_data16(u8 rd, u16 data16){ fatalerror( "ADDC.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::sub_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "SUB.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::subc_word_indrd_data16(u8 rd, u16 data16){ fatalerror( "SUBC.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::cmp_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "CMP.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::and_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "AND.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::or_word_indrd_data16(u8 rd, u16 data16)  { fatalerror( "OR.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::xor_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "XOR.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::mov_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "MOV.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }

void xa_cpu_device::add_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "ADD.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::addc_word_indrdinc_data16(u8 rd, u16 data16){ fatalerror( "ADDC.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::sub_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "SUB.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::subc_word_indrdinc_data16(u8 rd, u16 data16){ fatalerror( "SUBC.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::cmp_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "CMP.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::and_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "AND.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::or_word_indrdinc_data16(u8 rd, u16 data16)  { fatalerror( "OR.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::xor_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "XOR.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::mov_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "MOV.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }

void xa_cpu_device::add_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "ADD.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::addc_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){ fatalerror( "ADDC.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::sub_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "SUB.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::subc_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){ fatalerror( "SUBC.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::cmp_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "CMP.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::and_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "AND.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::or_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16)  { fatalerror( "OR.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::xor_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "XOR.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::mov_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "MOV.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }

void xa_cpu_device::add_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "ADD.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::addc_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){ fatalerror( "ADDC.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::sub_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "SUB.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::subc_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){ fatalerror( "SUBC.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::cmp_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "CMP.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::and_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "AND.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::or_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16)  { fatalerror( "OR.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::xor_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "XOR.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::mov_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "MOV.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }

void xa_cpu_device::add_word_direct_data16(u16 direct, u16 data16) { fatalerror( "ADD.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::addc_word_direct_data16(u16 direct, u16 data16){ fatalerror( "ADDC.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::sub_word_direct_data16(u16 direct, u16 data16) { fatalerror( "SUB.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::subc_word_direct_data16(u16 direct, u16 data16){ fatalerror( "SUBC.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::cmp_word_direct_data16(u16 direct, u16 data16) { fatalerror( "CMP.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::and_word_direct_data16(u16 direct, u16 data16) { fatalerror( "AND.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::or_word_direct_data16(u16 direct, u16 data16)  { fatalerror( "OR.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::xor_word_direct_data16(u16 direct, u16 data16) { fatalerror( "XOR.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::mov_word_direct_data16(u16 direct, u16 data16) { fatalerror( "MOV.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }

// ------------------------------------------
// ------------------------------------------
// ------------------------------------------


// ALUOP.w Rd, Rs
void xa_cpu_device::aluop_word_rd_rs(int alu_op, u8 rd, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_word_rd_rs(rd, rs); break;
	case 0x1: addc_word_rd_rs(rd, rs); break;
	case 0x2: sub_word_rd_rs(rd, rs); break;
	case 0x3: subc_word_rd_rs(rd, rs); break;
	case 0x4: cmp_word_rd_rs(rd, rs); break;
	case 0x5: and_word_rd_rs(rd, rs); break;
	case 0x6: or_word_rd_rs(rd, rs); break;
	case 0x7: xor_word_rd_rs(rd, rs); break;
	case 0x8: mov_word_rd_rs(rd, rs); break;
	default: fatalerror("UNK_ALUOP.w %s, %s", m_regnames16[rd], m_regnames16[rs]); // ALUOP.w Rd, Rs
	}
}
void xa_cpu_device::add_word_rd_rs(u8 rd, u8 rs) { fatalerror("ADD.w %s, %s", m_regnames16[rd], m_regnames16[rs]); }
void xa_cpu_device::addc_word_rd_rs(u8 rd, u8 rs){ fatalerror("ADDC.w %s, %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_word_rd_rs(u8 rd, u8 rs) { fatalerror("SUB.w %s, %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::subc_word_rd_rs(u8 rd, u8 rs){ fatalerror("SUBC.w %s, %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_word_rd_rs(u8 rd, u8 rs) { fatalerror("CMP.w %s, %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::and_word_rd_rs(u8 rd, u8 rs) { fatalerror("AND.w %s, %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::or_word_rd_rs(u8 rd, u8 rs)  { fatalerror("OR.w %s, %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_word_rd_rs(u8 rd, u8 rs) { fatalerror("XOR.w %s, %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_word_rd_rs(u8 rd, u8 rs) { fatalerror("MOV.w %s, %s", m_regnames16[rd], m_regnames16[rs]);}


// ALUOP.b Rd, Rs
void xa_cpu_device::aluop_byte_rd_rs(int alu_op, u8 rd, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_byte_rd_rs(rd, rs); break;
	case 0x1: addc_byte_rd_rs(rd, rs); break;
	case 0x2: sub_byte_rd_rs(rd, rs); break;
	case 0x3: subc_byte_rd_rs(rd, rs); break;
	case 0x4: cmp_byte_rd_rs(rd, rs); break;
	case 0x5: and_byte_rd_rs(rd, rs); break;
	case 0x6: or_byte_rd_rs(rd, rs); break;
	case 0x7: xor_byte_rd_rs(rd, rs); break;
	case 0x8: mov_byte_rd_rs(rd, rs); break;
	default: fatalerror("UNK_ALUOP.b %s, %s", m_regnames8[rd], m_regnames8[rs]); // ALUOP.b Rd, Rs
	}
}
void xa_cpu_device::add_byte_rd_rs(u8 rd, u8 rs) { fatalerror("ADD.b %s, %s", m_regnames8[rd], m_regnames8[rs]); }
void xa_cpu_device::addc_byte_rd_rs(u8 rd, u8 rs){ fatalerror("ADDC.b %s, %s", m_regnames8[rd], m_regnames8[rs]);}
void xa_cpu_device::sub_byte_rd_rs(u8 rd, u8 rs) { fatalerror("SUB.b %s, %s", m_regnames8[rd], m_regnames8[rs]);}
void xa_cpu_device::subc_byte_rd_rs(u8 rd, u8 rs){ fatalerror("SUBC.b %s, %s", m_regnames8[rd], m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_rd_rs(u8 rd, u8 rs) { fatalerror("CMP.b %s, %s", m_regnames8[rd], m_regnames8[rs]);}
void xa_cpu_device::and_byte_rd_rs(u8 rd, u8 rs) { fatalerror("AND.b %s, %s", m_regnames8[rd], m_regnames8[rs]);}
void xa_cpu_device::or_byte_rd_rs(u8 rd, u8 rs)  { fatalerror("OR.b %s, %s", m_regnames8[rd], m_regnames8[rs]);}
void xa_cpu_device::xor_byte_rd_rs(u8 rd, u8 rs) { fatalerror("XOR.b %s, %s", m_regnames8[rd], m_regnames8[rs]);}
void xa_cpu_device::mov_byte_rd_rs(u8 rd, u8 rs) { fatalerror("MOV.b %s, %s", m_regnames8[rd], m_regnames8[rs]);}


// ALUOP.w Rd, [Rs]
void xa_cpu_device::aluop_word_rd_indrs(int alu_op, u8 rd, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_word_rd_indrs(rd, rs); break;
	case 0x1: addc_word_rd_indrs(rd, rs); break;
	case 0x2: sub_word_rd_indrs(rd, rs); break;
	case 0x3: subc_word_rd_indrs(rd, rs); break;
	case 0x4: cmp_word_rd_indrs(rd, rs); break;
	case 0x5: and_word_rd_indrs(rd, rs); break;
	case 0x6: or_word_rd_indrs(rd, rs); break;
	case 0x7: xor_word_rd_indrs(rd, rs); break;
	case 0x8: mov_word_rd_indrs(rd, rs); break;
	default: fatalerror("UNK_ALUOP.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]); // ALUOP.w Rd, [Rs]
	}
}
void xa_cpu_device::add_word_rd_indrs(u8 rd, u8 rs) { fatalerror("ADD.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::addc_word_rd_indrs(u8 rd, u8 rs){ fatalerror("ADDC.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_word_rd_indrs(u8 rd, u8 rs) { fatalerror("SUB.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::subc_word_rd_indrs(u8 rd, u8 rs){ fatalerror("SUBC.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_word_rd_indrs(u8 rd, u8 rs) { fatalerror("CMP.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::and_word_rd_indrs(u8 rd, u8 rs) { fatalerror("AND.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::or_word_rd_indrs(u8 rd, u8 rs)  { fatalerror("OR.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_word_rd_indrs(u8 rd, u8 rs) { fatalerror("XOR.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_word_rd_indrs(u8 rd, u8 rs) { fatalerror("MOV.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]);}


// ALUOP.b Rd, [Rs]
void xa_cpu_device::aluop_byte_rd_indrs(int alu_op, u8 rd, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_byte_rd_indrs(rd, rs); break;
	case 0x1: addc_byte_rd_indrs(rd, rs); break;
	case 0x2: sub_byte_rd_indrs(rd, rs); break;
	case 0x3: subc_byte_rd_indrs(rd, rs); break;
	case 0x4: cmp_byte_rd_indrs(rd, rs); break;
	case 0x5: and_byte_rd_indrs(rd, rs); break;
	case 0x6: or_byte_rd_indrs(rd, rs); break;
	case 0x7: xor_byte_rd_indrs(rd, rs); break;
	case 0x8: mov_byte_rd_indrs(rd, rs); break;
	default: fatalerror("UNK_ALUOP.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]); // ALUOP.b Rd, [Rs]
	}
}
void xa_cpu_device::add_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("ADD.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::addc_byte_rd_indrs(u8 rd, u8 rs){ fatalerror("ADDC.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("SUB.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::subc_byte_rd_indrs(u8 rd, u8 rs){ fatalerror("SUBC.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("CMP.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::and_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("AND.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::or_byte_rd_indrs(u8 rd, u8 rs)  { fatalerror("OR.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("XOR.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("MOV.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}


// ALUOP.w [Rd], Rs
void xa_cpu_device::add_word_indrd_rs(u8 rd, u8 rs) { fatalerror("ADD.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::addc_word_indrd_rs(u8 rd, u8 rs){ fatalerror("ADDC.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_word_indrd_rs(u8 rd, u8 rs) { fatalerror("SUB.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::subc_word_indrd_rs(u8 rd, u8 rs){ fatalerror("SUBC.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_word_indrd_rs(u8 rd, u8 rs) { fatalerror("CMP.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::and_word_indrd_rs(u8 rd, u8 rs) { fatalerror("AND.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::or_word_indrd_rs(u8 rd, u8 rs)  { fatalerror("OR.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_word_indrd_rs(u8 rd, u8 rs) { fatalerror("XOR.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_word_indrd_rs(u8 rd, u8 rs) { fatalerror("MOV.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}


// ALUOP.b [Rd], Rs
void xa_cpu_device::add_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("ADD.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::addc_byte_indrd_rs(u8 rd, u8 rs){ fatalerror("ADDC.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::sub_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("SUB.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::subc_byte_indrd_rs(u8 rd, u8 rs){ fatalerror("SUBC.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("CMP.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::and_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("AND.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::or_byte_indrd_rs(u8 rd, u8 rs)  { fatalerror("OR.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::xor_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("XOR.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::mov_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("MOV.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}

// ALUOP.w Rd, [Rs+]
void xa_cpu_device::add_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("ADD.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::addc_word_rd_indrsinc(u8 rd, u8 rs){ fatalerror("ADDC.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("SUB.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::subc_word_rd_indrsinc(u8 rd, u8 rs){ fatalerror("SUBC.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("CMP.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::and_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("AND.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::or_word_rd_indrsinc(u8 rd, u8 rs)  { fatalerror("OR.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("XOR.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("MOV.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}

// ALUOP.b Rd, [Rs+]
void xa_cpu_device::add_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("ADD.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::addc_byte_rd_indrsinc(u8 rd, u8 rs){ fatalerror("ADDC.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("SUB.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::subc_byte_rd_indrsinc(u8 rd, u8 rs){ fatalerror("SUBC.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("CMP.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::and_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("AND.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::or_byte_rd_indrsinc(u8 rd, u8 rs)  { fatalerror("OR.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("XOR.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("MOV.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}

// ALUOP.w [Rd+], Rs
void xa_cpu_device::add_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("ADD.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::addc_word_indrdinc_rs(u8 rd, u8 rs){ fatalerror("ADDC.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("SUB.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::subc_word_indrdinc_rs(u8 rd, u8 rs){ fatalerror("SUBC.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("CMP.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::and_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("AND.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::or_word_indrdinc_rs(u8 rd, u8 rs)  { fatalerror("OR.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("XOR.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("MOV.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}


// ALUOP.b [Rd+], Rs
void xa_cpu_device::add_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("ADD.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::addc_byte_indrdinc_rs(u8 rd, u8 rs){ fatalerror("ADDC.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::sub_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("SUB.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::subc_byte_indrdinc_rs(u8 rd, u8 rs){ fatalerror("SUBC.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("CMP.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::and_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("AND.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::or_byte_indrdinc_rs(u8 rd, u8 rs)  { fatalerror("OR.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::xor_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("XOR.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::mov_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("MOV.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}


// ALUOP.w Rd, [Rs+off8]
void xa_cpu_device::add_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("ADD.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::addc_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8){ fatalerror("ADDC.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::sub_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("SUB.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::subc_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8){ fatalerror("SUBC.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::cmp_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("CMP.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::and_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("AND.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::or_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8)  { fatalerror("OR.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::xor_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("XOR.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::mov_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("MOV.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}

// ALUOP.b Rd, [Rs+off8]
void xa_cpu_device::add_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("ADD.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::addc_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8){ fatalerror("ADDC.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::sub_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("SUB.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::subc_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8){ fatalerror("SUBC.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::cmp_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("CMP.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::and_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("AND.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::or_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8)  { fatalerror("OR.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::xor_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("XOR.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::mov_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("MOV.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}

// ALUOP.w [Rd+off8], Rs
void xa_cpu_device::add_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("ADD.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::addc_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs){ fatalerror("ADDC.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::sub_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("SUB.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::subc_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs){ fatalerror("SUBC.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::cmp_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("CMP.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::and_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("AND.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::or_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs)  { fatalerror("OR.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::xor_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("XOR.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::mov_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("MOV.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}


// ALUOP.b [Rd+off8], Rs
void xa_cpu_device::add_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("ADD.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::addc_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs){ fatalerror("ADDC.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::sub_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("SUB.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::subc_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs){ fatalerror("SUBC.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("CMP.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::and_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("AND.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::or_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs)  { fatalerror("OR.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::xor_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("XOR.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::mov_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("MOV.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}


// ALUOP.w Rd, [Rs+off16]
void xa_cpu_device::add_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("ADD.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::addc_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16){ fatalerror("ADDC.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::sub_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("SUB.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::subc_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16){ fatalerror("SUBC.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::cmp_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("CMP.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::and_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("AND.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::or_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16)  { fatalerror("OR.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::xor_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("XOR.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::mov_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("MOV.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}

// ALUOP.b Rd, [Rs+off16]
void xa_cpu_device::add_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("ADD.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::addc_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16){ fatalerror("ADDC.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::sub_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("SUB.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::subc_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16){ fatalerror("SUBC.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::cmp_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("CMP.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::and_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("AND.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::or_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16)  { fatalerror("OR.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::xor_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("XOR.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::mov_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("MOV.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}

// ALUOP.w [Rd+off16], Rs
void xa_cpu_device::add_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("ADD.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::addc_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs){ fatalerror("ADDC.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::sub_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("SUB.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::subc_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs){ fatalerror("SUBC.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::cmp_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("CMP.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::and_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("AND.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::or_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs)  { fatalerror("OR.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::xor_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("XOR.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::mov_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("MOV.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}

// ALUOP.b [Rd+off16], Rs
void xa_cpu_device::add_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("ADD.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::addc_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs){ fatalerror("ADDC.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::sub_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("SUB.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::subc_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs){ fatalerror("SUBC.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("CMP.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::and_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("AND.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::or_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs)  { fatalerror("OR.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::xor_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("XOR.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::mov_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("MOV.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}

// ALUOP.w Rd, Direct
void xa_cpu_device::add_word_rd_direct(u8 rd, u16 direct) { fatalerror("ADD.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::addc_word_rd_direct(u8 rd, u16 direct){ fatalerror("ADDC.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::sub_word_rd_direct(u8 rd, u16 direct) { fatalerror("SUB.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::subc_word_rd_direct(u8 rd, u16 direct){ fatalerror("SUBC.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::cmp_word_rd_direct(u8 rd, u16 direct) { fatalerror("CMP.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::and_word_rd_direct(u8 rd, u16 direct) { fatalerror("AND.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::or_word_rd_direct(u8 rd, u16 direct)  { fatalerror("OR.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::xor_word_rd_direct(u8 rd, u16 direct) { fatalerror("XOR.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::mov_word_rd_direct(u8 rd, u16 direct) { fatalerror("MOV.w %s, %s", m_regnames16[rd], get_directtext(direct));}

// ALUOP.b Rd, Direct
void xa_cpu_device::add_byte_rd_direct(u8 rd, u16 direct) { fatalerror("ADD.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::addc_byte_rd_direct(u8 rd, u16 direct){ fatalerror("ADDC.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::sub_byte_rd_direct(u8 rd, u16 direct) { fatalerror("SUB.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::subc_byte_rd_direct(u8 rd, u16 direct){ fatalerror("SUBC.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::cmp_byte_rd_direct(u8 rd, u16 direct) { fatalerror("CMP.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::and_byte_rd_direct(u8 rd, u16 direct) { fatalerror("AND.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::or_byte_rd_direct(u8 rd, u16 direct)  { fatalerror("OR.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::xor_byte_rd_direct(u8 rd, u16 direct) { fatalerror("XOR.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::mov_byte_rd_direct(u8 rd, u16 direct) { fatalerror("MOV.b %s, %s", m_regnames8[rd], get_directtext(direct));}

// ALUOP.w Direct, Rs
void xa_cpu_device::add_word_direct_rs(u16 direct, u8 rs) { fatalerror("ADD.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::addc_word_direct_rs(u16 direct, u8 rs){ fatalerror("ADDC.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::sub_word_direct_rs(u16 direct, u8 rs) { fatalerror("SUB.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::subc_word_direct_rs(u16 direct, u8 rs){ fatalerror("SUBC.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::cmp_word_direct_rs(u16 direct, u8 rs) { fatalerror("CMP.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::and_word_direct_rs(u16 direct, u8 rs) { fatalerror("AND.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::or_word_direct_rs(u16 direct, u8 rs)  { fatalerror("OR.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::xor_word_direct_rs(u16 direct, u8 rs) { fatalerror("XOR.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::mov_word_direct_rs(u16 direct, u8 rs) { fatalerror("MOV.w %s, %s", get_directtext(direct), m_regnames16[rs]);}

// ALUOP.b Direct, Rs
void xa_cpu_device::add_byte_direct_rs(u16 direct, u8 rs) { fatalerror("ADD.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::addc_byte_direct_rs(u16 direct, u8 rs){ fatalerror("ADDC.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::sub_byte_direct_rs(u16 direct, u8 rs) { fatalerror("SUB.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::subc_byte_direct_rs(u16 direct, u8 rs){ fatalerror("SUBC.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_direct_rs(u16 direct, u8 rs) { fatalerror("CMP.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::and_byte_direct_rs(u16 direct, u8 rs) { fatalerror("AND.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::or_byte_direct_rs(u16 direct, u8 rs)  { fatalerror("OR.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::xor_byte_direct_rs(u16 direct, u8 rs) { fatalerror("XOR.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::mov_byte_direct_rs(u16 direct, u8 rs) { fatalerror("MOV.b %s, %s", get_directtext(direct), m_regnames8[rs]);}


// ADDS.w / MOVS.w Rd, #data4
void xa_cpu_device::movs_word_rd_data4(u8 rd, u8 data4) { fatalerror("MOVS.w %s, %s", m_regnames16[rd], show_expanded_data4(data4, 1)); }
void xa_cpu_device::adds_word_rd_data4(u8 rd, u8 data4) { fatalerror("ADDS.w %s, %s", m_regnames16[rd], show_expanded_data4(data4, 1)); }

// ADDS.b / MOVS.b Rd, #data4
void xa_cpu_device::movs_byte_rd_data4(u8 rd, u8 data4){ u8 data = util::sext(data4, 4); set_reg8(rd, data);	do_nz_flags_8(data); }
void xa_cpu_device::adds_byte_rd_data4(u8 rd, u8 data4){ fatalerror("ADDS.b %s, %s", m_regnames8[rd], show_expanded_data4(data4, 0)); }

// ADDS.w / MOVS.w [Rd], #data4
void xa_cpu_device::movs_word_indrd_data4(u8 rd, u8 data4){ fatalerror("MOVS.w [%s], %s", m_regnames16[rd], show_expanded_data4(data4, 1)); }
void xa_cpu_device::adds_word_indrd_data4(u8 rd, u8 data4){ fatalerror("ADDS.w [%s], %s", m_regnames16[rd], show_expanded_data4(data4, 1)); }

// ADDS.b / MOVS.b [Rd], #data4
void xa_cpu_device::movs_byte_indrd_data4(u8 rd, u8 data4){ fatalerror("MOVS.b [%s], %s", m_regnames16[rd], show_expanded_data4(data4, 0)); }
void xa_cpu_device::adds_byte_indrd_data4(u8 rd, u8 data4){ fatalerror("ADDS.b [%s], %s", m_regnames16[rd], show_expanded_data4(data4, 0)); }

// ADDS.w / MOVS.w [Rd+], #data4
void xa_cpu_device::movs_word_indrdinc_data4(u8 rd, u8 data4){ u16 data = util::sext(data4, 4); u16 regval = get_reg16(rd); write_data16(regval, data); regval += 2; do_nz_flags_16(regval); set_reg16(rd, regval); }
void xa_cpu_device::adds_word_indrdinc_data4(u8 rd, u8 data4){ fatalerror("ADDS.w [%s+], %s", m_regnames16[rd], show_expanded_data4(data4, 1)); }

// ADDS.b / MOVS.w [Rd+], #data4
void xa_cpu_device::movs_byte_indrdinc_data4(u8 rd, u8 data4){ fatalerror("MOVS.b [%s+], %s", m_regnames16[rd], show_expanded_data4(data4, 0)); }
void xa_cpu_device::adds_byte_indrdinc_data4(u8 rd, u8 data4){ fatalerror("ADDS.b [%s+], %s", m_regnames16[rd], show_expanded_data4(data4, 0)); }

// ADDS.w / MOVS.w [Rd+off8], #data4
void xa_cpu_device::movs_word_indrdoff8_data4(u8 rd, u8 off8, u8 data4){ fatalerror("MOVS.w [%s+$%02x], %s", m_regnames16[rd], off8, show_expanded_data4(data4, 1)); }
void xa_cpu_device::adds_word_indrdoff8_data4(u8 rd, u8 off8, u8 data4){ fatalerror("ADDS.w [%s+$%02x], %s", m_regnames16[rd], off8, show_expanded_data4(data4, 1)); }

// ADDS.b / MOVS.b [Rd+off8], #data4
void xa_cpu_device::movs_byte_indrdoff8_data4(u8 rd, u8 off8, u8 data4){ fatalerror("MOVS.b [%s+$%02x], %s", m_regnames16[rd], off8, show_expanded_data4(data4, 0));  }
void xa_cpu_device::adds_byte_indrdoff8_data4(u8 rd, u8 off8, u8 data4){ fatalerror("ADDS.b [%s+$%02x], %s", m_regnames16[rd], off8, show_expanded_data4(data4, 0));  }

// ADDS.w / MOVS.w [Rd+off16], #data4
void xa_cpu_device::movs_word_indrdoff16_data4(u8 rd, u16 off16, u8 data4){ fatalerror("MOVS.w [%s+$%04x], %s", m_regnames16[rd], off16, show_expanded_data4(data4, 1)); }
void xa_cpu_device::adds_word_indrdoff16_data4(u8 rd, u16 off16, u8 data4){ fatalerror("ADDS.w [%s+$%04x], %s", m_regnames16[rd], off16, show_expanded_data4(data4, 1)); }

// ADDS.b / MOVS.b [Rd+off16], #data4
void xa_cpu_device::movs_byte_indrdoff16_data4(u8 rd, u16 off16, u8 data4){ fatalerror("MOVS.b [%s+$%04x], %s", m_regnames16[rd], off16, show_expanded_data4(data4, 0)); }
void xa_cpu_device::adds_byte_indrdoff16_data4(u8 rd, u16 off16, u8 data4){ fatalerror("ADDS.b [%s+$%04x], %s", m_regnames16[rd], off16, show_expanded_data4(data4, 0)); }

// ADDS.w / MOVS.w DIRECT, #data4
void xa_cpu_device::movs_word_direct_data4(u16 direct, u8 data4){ u16 data = util::sext(data4, 4); do_nz_flags_16(data); write_direct16(direct, data); }
void xa_cpu_device::adds_word_direct_data4(u16 direct, u8 data4){ fatalerror("ADDS.w %s, %s\n", get_directtext(direct), show_expanded_data4(data4, 0)); }

// ADDS.b / MOVS.b DIRECT, #data4
void xa_cpu_device::movs_byte_direct_data4(u16 direct, u8 data4){ u8  data = util::sext(data4, 4); do_nz_flags_8(data);  write_direct8(direct, data); }
void xa_cpu_device::adds_byte_direct_data4(u16 direct, u8 data4){ fatalerror("ADDS.b %s, %s\n", get_directtext(direct), show_expanded_data4(data4, 0)); }

// CALL rel16
void xa_cpu_device::call_rel16(u16 rel16) { push_word_to_stack(m_pc); set_pc_in_current_page(expand_rel16(rel16)); }

// BCC rel8                    Branch if the carry flag is clear                                       2 6t/3nt    1111 0000  rrrr rrrr
void xa_cpu_device::bcc_rel8(u8 rel8) { fatalerror("BCC %04x\n", expand_rel8(rel8)); }

// BCS rel8                    Branch if the carry flag is set                                         2 6t/3nt    1111 0001  rrrr rrrr
void xa_cpu_device::bcs_rel8(u8 rel8) { fatalerror("BCS %04x\n", expand_rel8(rel8)); }

// BNE rel8                    Branch if the zero flag is not set                                      2 6t/3nt    1111 0010  rrrr rrrr
void xa_cpu_device::bne_rel8(u8 rel8) { fatalerror("BNE %04x\n", expand_rel8(rel8)); }

// BEQ rel8                    Branch if the zero flag is set                                          2 6t/3nt    1111 0011  rrrr rrrr
void xa_cpu_device::beq_rel8(u8 rel8) { if (get_z_flag()) { set_pc_in_current_page(expand_rel8(rel8)); } }

// BNV rel8                    Branch if overflow flag is clear                                        2 6t/3nt    1111 0100  rrrr rrrr
void xa_cpu_device::bnv_rel8(u8 rel8) { fatalerror("BNV %04x\n", expand_rel8(rel8)); }

// BOV rel8                    Branch if overflow flag is set                                          2 6t/3nt    1111 0101  rrrr rrrr
void xa_cpu_device::bov_rel8(u8 rel8) { fatalerror("BOV %04x\n", expand_rel8(rel8)); }

// BPL rel8                    Branch if the negative flag is clear                                    2 6t/3nt    1111 0110  rrrr rrrr
void xa_cpu_device::bpl_rel8(u8 rel8) { fatalerror("BPL %04x\n", expand_rel8(rel8)); }

// BMI rel8                    Branch if the negative flag is set                                      2 6t/3nt    1111 0111  rrrr rrrr
void xa_cpu_device::bmi_rel8(u8 rel8) { fatalerror("BMI %04x\n", expand_rel8(rel8)); }

// BG rel8                     Branch if greater than (unsigned)                                       2 6t/3nt    1111 1000  rrrr rrrr
void xa_cpu_device::bg_rel8(u8 rel8) { fatalerror("BG %04x\n", expand_rel8(rel8)); }

// BL rel8                     Branch if less than or equal to (unsigned)                              2 6t/3nt    1111 1001  rrrr rrrr
void xa_cpu_device::bl_rel8(u8 rel8) { fatalerror("BL %04x\n", expand_rel8(rel8)); }

// BGE rel8                    Branch if greater than or equal to (signed)                             2 6t/3nt    1111 1010  rrrr rrrr
void xa_cpu_device::bge_rel8(u8 rel8) { fatalerror("BGE %04x\n", expand_rel8(rel8)); }

// BLT rel8                    Branch if less than (signed)                                            2 6t/3nt    1111 1011  rrrr rrrr
void xa_cpu_device::blt_rel8(u8 rel8) { fatalerror("BLT %04x\n", expand_rel8(rel8)); }

// BGT rel8                    Branch if greater than (signed)                                         2 6t/3nt    1111 1100  rrrr rrrr
void xa_cpu_device::bgt_rel8(u8 rel8) { fatalerror("BGT %04x\n", expand_rel8(rel8)); }

// BLE rel8                    Branch if less than or equal to (signed)                                2 6t/3nt    1111 1101  rrrr rrrr
void xa_cpu_device::ble_rel8(u8 rel8) { fatalerror("BLE %04x\n", expand_rel8(rel8)); }

// BR rel8                     Short unconditional branch                                              2 6         1111 1110  rrrr rrrr
void xa_cpu_device::br_rel8(u8 rel8) { fatalerror("BR %04x\n", expand_rel8(rel8)); }

// ASL.b Rd, #data4              Logical left shift reg by the 4-bit imm value                           2 a*        1101 SS01  dddd iiii
void xa_cpu_device::asl_byte_rd_imm4(u8 rd, u8 amount) { fatalerror("ASL.b %s, %d", m_regnames8[rd], amount); }
void xa_cpu_device::asl_word_rd_imm4(u8 rd, u8 amount) { fatalerror("ASL.w %s, %d", m_regnames16[rd], amount); }
void xa_cpu_device::asl_dword_rd_imm5(u8 rd, u8 amount) { fatalerror("ASL.dw %s, %d", m_regnames16[rd], amount); }

void xa_cpu_device::asr_byte_rd_imm4(u8 rd, u8 amount) { fatalerror("ASR.b %s, %d", m_regnames8[rd], amount); }
void xa_cpu_device::asr_word_rd_imm4(u8 rd, u8 amount) { fatalerror("ASR.w %s, %d", m_regnames16[rd], amount); }
void xa_cpu_device::asr_dword_rd_imm5(u8 rd, u8 amount) { fatalerror("ASR.dw %s, %d", m_regnames16[rd], amount); }

void xa_cpu_device::lsr_byte_rd_imm4(u8 rd, u8 amount) { fatalerror("LSR.b %s, %d", m_regnames8[rd], amount); }
void xa_cpu_device::lsr_word_rd_imm4(u8 rd, u8 amount) { fatalerror("LSR.w %s, %d", m_regnames16[rd], amount); }
void xa_cpu_device::lsr_dword_rd_imm5(u8 rd, u8 amount) { fatalerror("LSR.dw %s, %d", m_regnames16[rd], amount); }

