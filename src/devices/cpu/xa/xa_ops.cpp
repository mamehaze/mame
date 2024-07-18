// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "xa.h"
#include "xadasm.h"

void xa_cpu_device::do_nop() { }

u8 xa_cpu_device::do_subb_8(u8 val1, u8 val2)
{
	return do_sub_8_helper(val1, val2, get_c_flag());
}

u8 xa_cpu_device::do_sub_8(u8 val1, u8 val2)
{
	return do_sub_8_helper(val1, val2, 0);
}

u8 xa_cpu_device::do_sub_8_helper(u8 val1, u8 val2, u8 c)
{
	u16 result = val1 - (val2 + c);
	s16 result1 = (s8)val1 - (s8)(val2 + c);
	u16 result3 = (val1 & 0x0f) - ((val2 & 0x0f) + c);
	if (result & 0x100) set_c_flag(); else clear_c_flag();
	if (result3 & 0x10) set_ac_flag(); else clear_ac_flag();
	if ((result1 < -128 || result1 > 127)) set_v_flag(); else clear_v_flag();

	do_nz_flags_8((u8)result);
	return (u8)result;
}

u8 xa_cpu_device::do_addc_8(u8 val1, u8 val2)
{
	return do_add_8_helper(val1, val2, get_c_flag());
}

u8 xa_cpu_device::do_add_8(u8 val1, u8 val2)
{
	return do_add_8_helper(val1, val2, 0);
}


u8 xa_cpu_device::do_add_8_helper(u8 val1, u8 val2, u8 c)
{
	u16 result = val1 + val2 + c;
	s16 result1 = (s8)val1 + (s8)val2 + c;
	u16 result3 = (val1 & 0x0f) + (val2 & 0x0f) + c;
	if (result & 0x100) set_c_flag(); else clear_c_flag();
	if (result3 & 0x10) set_ac_flag(); else clear_ac_flag();
	if ((result1 < -128 || result1 > 127)) set_v_flag(); else clear_v_flag();

	do_nz_flags_8((u8)result);
	return (u8)result;
}

u8 xa_cpu_device::do_xor_8(u8 val1, u8 val2)
{
	u8 result = val1 ^ val2;
	do_nz_flags_8(result);
	return result;
}

u8 xa_cpu_device::do_or_8(u8 val1, u8 val2)
{
	u8 result = val1 | val2;
	do_nz_flags_8(result);
	return result;
}

u8 xa_cpu_device::do_and_8(u8 val1, u8 val2)
{
	u8 result = val1 & val2;
	do_nz_flags_8(result);
	return result;
}



u8 xa_cpu_device::do_cjne_8_helper(u8 val1, u8 val2)
{
	u16 result = val1 - val2;
	do_nz_flags_8((u8)result);
	if (result & 0x100) set_c_flag(); else clear_c_flag();
	return (u8)result;
}


// ALUOP.b Rd, data8
void xa_cpu_device::aluop_byte_rd_data8(int alu_op, u8 rd, u8 data8)
{
	switch (alu_op)
	{
	case 0x0: add_byte_rd_data8(rd, data8); break;
	case 0x1: addc_byte_rd_data8(rd, data8); break;
	case 0x2: sub_byte_rd_data8(rd, data8); break;
	case 0x3: subb_byte_rd_data8(rd, data8); break;
	case 0x4: cmp_byte_rd_data8(rd, data8); break;
	case 0x5: and_byte_rd_data8(rd, data8); break;
	case 0x6: or_byte_rd_data8(rd, data8); break;
	case 0x7: xor_byte_rd_data8(rd, data8); break;
	case 0x8: mov_byte_rd_data8(rd, data8); break;
	default: logerror("UNK_ALUOP.b %s, #$%02x", m_regnames8[rd], data8); do_nop(); break;
	}
}

void xa_cpu_device::add_byte_rd_data8(u8 rd, u8 data8) { u8 rdval = get_reg8(rd); u8 result = do_add_8(rdval, data8); set_reg8(rd, result); }
void xa_cpu_device::addc_byte_rd_data8(u8 rd, u8 data8){ u8 rdval = get_reg8(rd); u8 result = do_addc_8(rdval, data8); set_reg8(rd, result); }
void xa_cpu_device::sub_byte_rd_data8(u8 rd, u8 data8) { u8 rdval = get_reg8(rd); u8 result = do_sub_8(rdval, data8); set_reg8(rd, result); }
void xa_cpu_device::subb_byte_rd_data8(u8 rd, u8 data8){ u8 rdval = get_reg8(rd); u8 result = do_subb_8(rdval, data8); set_reg8(rd, result); }
void xa_cpu_device::cmp_byte_rd_data8(u8 rd, u8 data8) { u8 rdval = get_reg8(rd); do_sub_8(rdval, data8); }
void xa_cpu_device::and_byte_rd_data8(u8 rd, u8 data8) { u8 rdval = get_reg8(rd); u8 result = do_and_8(rdval, data8); set_reg8(rd, result); }
void xa_cpu_device::or_byte_rd_data8(u8 rd, u8 data8)  { u8 rdval = get_reg8(rd); u8 result = do_or_8(rdval, data8); set_reg8(rd, result); }
void xa_cpu_device::xor_byte_rd_data8(u8 rd, u8 data8) { u8 rdval = get_reg8(rd); u8 result = do_xor_8(rdval, data8); set_reg8(rd, result); }
void xa_cpu_device::mov_byte_rd_data8(u8 rd, u8 data8) { u8 result = data8; do_nz_flags_8(result); set_reg8(rd, result); }

// ------------------------------------------

// ALUOP.b [Rd], data8
void xa_cpu_device::aluop_byte_indrd_data8(int alu_op, u8 rd, u8 data8)
{
	switch (alu_op)
	{
	case 0x0: add_byte_indrd_data8(rd, data8); break;
	case 0x1: addc_byte_indrd_data8(rd, data8); break;
	case 0x2: sub_byte_indrd_data8(rd, data8); break;
	case 0x3: subb_byte_indrd_data8(rd, data8); break;
	case 0x4: cmp_byte_indrd_data8(rd, data8); break;
	case 0x5: and_byte_indrd_data8(rd, data8); break;
	case 0x6: or_byte_indrd_data8(rd, data8); break;
	case 0x7: xor_byte_indrd_data8(rd, data8); break;
	case 0x8: mov_byte_indrd_data8(rd, data8); break;
	default: logerror("UNK_ALUOP.b [%s], #$%02x", m_regnames16[rd], data8); do_nop(); break;
	}
}
void xa_cpu_device::add_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "ADD.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::addc_byte_indrd_data8(u8 rd, u8 data8){ fatalerror( "ADDC.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::sub_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "SUB.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::subb_byte_indrd_data8(u8 rd, u8 data8){ fatalerror( "SUBB.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::cmp_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "CMP.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::and_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "AND.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::or_byte_indrd_data8(u8 rd, u8 data8)  { fatalerror( "OR.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::xor_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "XOR.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::mov_byte_indrd_data8(u8 rd, u8 data8) { fatalerror( "MOV.b [%s], #$%02x ([RD], DATA8)", m_regnames16[rd], data8 ); }

// ------------------------------------------
// ALUOP.b [Rd+], data8
void xa_cpu_device::aluop_byte_indrdinc_data8(int alu_op, u8 rd, u8 data8)
{
	switch (alu_op)
	{
	case 0x0: add_byte_indrdinc_data8(rd, data8); break;
	case 0x1: addc_byte_indrdinc_data8(rd, data8); break;
	case 0x2: sub_byte_indrdinc_data8(rd, data8); break;
	case 0x3: subb_byte_indrdinc_data8(rd, data8); break;
	case 0x4: cmp_byte_indrdinc_data8(rd, data8); break;
	case 0x5: and_byte_indrdinc_data8(rd, data8); break;
	case 0x6: or_byte_indrdinc_data8(rd, data8); break;
	case 0x7: xor_byte_indrdinc_data8(rd, data8); break;
	case 0x8: mov_byte_indrdinc_data8(rd, data8); break;
	default: logerror("UNK_ALUOP.b [%s+], #$%02x", m_regnames16[rd], data8); do_nop(); break;
	}
}
void xa_cpu_device::add_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "ADD.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::addc_byte_indrdinc_data8(u8 rd, u8 data8){ fatalerror( "ADDC.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::sub_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "SUB.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::subb_byte_indrdinc_data8(u8 rd, u8 data8){ fatalerror( "SUBB.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::cmp_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "CMP.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::and_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "AND.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::or_byte_indrdinc_data8(u8 rd, u8 data8)  { fatalerror( "OR.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::xor_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "XOR.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }
void xa_cpu_device::mov_byte_indrdinc_data8(u8 rd, u8 data8) { fatalerror( "MOV.b [%s+], #$%02x ([RD+], DATA8)", m_regnames16[rd], data8 ); }

// ------------------------------------------
// ALUOP.b [Rd+offs8], data8
void xa_cpu_device::aluop_byte_rdoff8_data8(int alu_op, u8 rd, u8 offset8, u8 data8)
{
	switch (alu_op)
	{
	case 0x0: add_byte_indrdoff8_data8(rd, offset8, data8); break;
	case 0x1: addc_byte_indrdoff8_data8(rd, offset8, data8); break;
	case 0x2: sub_byte_indrdoff8_data8(rd, offset8, data8); break;
	case 0x3: subb_byte_indrdoff8_data8(rd, offset8, data8); break;
	case 0x4: cmp_byte_indrdoff8_data8(rd, offset8, data8); break;
	case 0x5: and_byte_indrdoff8_data8(rd, offset8, data8); break;
	case 0x6: or_byte_indrdoff8_data8(rd, offset8, data8); break;
	case 0x7: xor_byte_indrdoff8_data8(rd, offset8, data8); break;
	case 0x8: mov_byte_indrdoff8_data8(rd, offset8, data8); break;
	default: logerror("UNK_ALUOP.b [%s+#$%02x], #$%02x", m_regnames16[rd], offset8, data8); do_nop(); break;
	}
}
void xa_cpu_device::add_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "ADD.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::addc_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){ fatalerror( "ADDC.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::sub_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "SUB.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::subb_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8){ fatalerror( "SUBB.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::cmp_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "CMP.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::and_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "AND.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::or_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8)  { fatalerror( "OR.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::xor_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "XOR.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }
void xa_cpu_device::mov_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8) { fatalerror( "MOV.b [%s+#$%02x], #$%02x ([RD+offs8], DATA8)", m_regnames16[rd], offset8, data8 ); }

// ------------------------------------------
// ALUOP.b [Rd+offs16], data8
void xa_cpu_device::aluop_byte_rdoff16_data8(int alu_op, u8 rd, u16 offset16, u8 data8)
{
	switch (alu_op)
	{
	case 0x0: add_byte_indrdoff16_data8(rd, offset16, data8); break;
	case 0x1: addc_byte_indrdoff16_data8(rd, offset16, data8); break;
	case 0x2: sub_byte_indrdoff16_data8(rd, offset16, data8); break;
	case 0x3: subb_byte_indrdoff16_data8(rd, offset16, data8); break;
	case 0x4: cmp_byte_indrdoff16_data8(rd, offset16, data8); break;
	case 0x5: and_byte_indrdoff16_data8(rd, offset16, data8); break;
	case 0x6: or_byte_indrdoff16_data8(rd, offset16, data8); break;
	case 0x7: xor_byte_indrdoff16_data8(rd, offset16, data8); break;
	case 0x8: mov_byte_indrdoff16_data8(rd, offset16, data8); break;
	default: logerror("UNK_ALUOP.b [%s+#$%04x], #$%02d", m_regnames16[rd], offset16, data8); do_nop(); break;
	}
}
void xa_cpu_device::add_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "ADD.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::addc_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){ fatalerror( "ADDC.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::sub_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "SUB.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::subb_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8){ fatalerror( "SUBB.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::cmp_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "CMP.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::and_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "AND.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::or_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8)  { fatalerror( "OR.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::xor_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "XOR.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }
void xa_cpu_device::mov_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8) { fatalerror( "MOV.b [%s+#$%04x], #$%02x ([RD+offs16], DATA8)", m_regnames16[rd], offset16, data8 ); }

// ------------------------------------------
 // ALUOP.b DIRECT, data8
void xa_cpu_device::aluop_byte_direct_data8(int alu_op, u16 direct, u8 data8)
{
	switch (alu_op)
	{
	case 0x0: add_byte_direct_data8(direct, data8); break;
	case 0x1: addc_byte_direct_data8(direct, data8); break;
	case 0x2: sub_byte_direct_data8(direct, data8); break;
	case 0x3: subb_byte_direct_data8(direct, data8); break;
	case 0x4: cmp_byte_direct_data8(direct, data8); break;
	case 0x5: and_byte_direct_data8(direct, data8); break;
	case 0x6: or_byte_direct_data8(direct, data8); break;
	case 0x7: xor_byte_direct_data8(direct, data8); break;
	case 0x8: mov_byte_direct_data8(direct, data8); break;
	default: logerror("UNK_ALUOP.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8); do_nop(); break;
	}
}
void xa_cpu_device::add_byte_direct_data8(u16 direct, u8 data8) { fatalerror("ADD.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8); }
void xa_cpu_device::addc_byte_direct_data8(u16 direct, u8 data8){ fatalerror("ADDC.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);}
void xa_cpu_device::sub_byte_direct_data8(u16 direct, u8 data8) { fatalerror("SUB.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8); }
void xa_cpu_device::subb_byte_direct_data8(u16 direct, u8 data8){ fatalerror("SUBB.b %s, #$%02x (DIRECT, DATA8)", get_directtext(direct), data8);}
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
// ALUOP.w Rd, data16
void xa_cpu_device::aluop_byte_rd_data16(int alu_op, u8 rd, u16 data16)
{
	switch (alu_op)
	{
	case 0x0: add_word_rd_data16(rd, data16); break;
	case 0x1: addc_word_rd_data16(rd, data16); break;
	case 0x2: sub_word_rd_data16(rd, data16); break;
	case 0x3: subb_word_rd_data16(rd, data16); break;
	case 0x4: cmp_word_rd_data16(rd, data16); break;
	case 0x5: and_word_rd_data16(rd, data16); break;
	case 0x6: or_word_rd_data16(rd, data16); break;
	case 0x7: xor_word_rd_data16(rd, data16); break;
	case 0x8: mov_word_rd_data16(rd, data16); break;
	default: logerror("UNK_ALUOP.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); do_nop(); break;
	}
}
void xa_cpu_device::add_word_rd_data16(u8 rd, u16 data16) { fatalerror("ADD.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::addc_word_rd_data16(u8 rd, u16 data16){ fatalerror("ADDC.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::sub_word_rd_data16(u8 rd, u16 data16) { fatalerror("SUB.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::subb_word_rd_data16(u8 rd, u16 data16){ fatalerror("SUBB.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::cmp_word_rd_data16(u8 rd, u16 data16) { fatalerror("CMP.w % s, #$ % 04x(RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::and_word_rd_data16(u8 rd, u16 data16) { fatalerror("AND.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::or_word_rd_data16(u8 rd, u16 data16)  { fatalerror("OR.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::xor_word_rd_data16(u8 rd, u16 data16) { fatalerror("XOR.w %s, #$%04x (RD, DATA16)", m_regnames16[rd], data16); }

void xa_cpu_device::mov_word_rd_data16(u8 rd, u16 data16){
	printf("MOV.w %s, #$%04x\n", m_regnames16[rd], data16);
	set_reg16(rd, data16);
}

// -----------------------------------------------
// ALUOP.w [Rd], data16
void xa_cpu_device::aluop_byte_indrd_data16(int alu_op, u8 rd, u16 data16)
{
	switch (alu_op)
	{
	case 0x0: add_word_indrd_data16(rd, data16); break;
	case 0x1: addc_word_indrd_data16(rd, data16); break;
	case 0x2: sub_word_indrd_data16(rd, data16); break;
	case 0x3: subb_word_indrd_data16(rd, data16); break;
	case 0x4: cmp_word_indrd_data16(rd, data16); break;
	case 0x5: and_word_indrd_data16(rd, data16); break;
	case 0x6: or_word_indrd_data16(rd, data16); break;
	case 0x7: xor_word_indrd_data16(rd, data16); break;
	case 0x8: mov_word_indrd_data16(rd, data16); break;
	default: logerror("UNK_ALUOP.w [%s], #$%04x", m_regnames16[rd], data16); do_nop(); break;
	}
}
void xa_cpu_device::add_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "ADD.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::addc_word_indrd_data16(u8 rd, u16 data16){ fatalerror( "ADDC.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::sub_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "SUB.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::subb_word_indrd_data16(u8 rd, u16 data16){ fatalerror( "SUBB.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::cmp_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "CMP.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::and_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "AND.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::or_word_indrd_data16(u8 rd, u16 data16)  { fatalerror( "OR.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::xor_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "XOR.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::mov_word_indrd_data16(u8 rd, u16 data16) { fatalerror( "MOV.w [%s], #$%04x ([RD], DATA16)", m_regnames16[rd], data16); }

// ALUOP.w [Rd+], data16
void xa_cpu_device::aluop_byte_indrdinc_data16(int alu_op, u8 rd, u16 data16)
{
	switch (alu_op)
	{
	case 0x0: add_word_indrdinc_data16(rd, data16); break;
	case 0x1: addc_word_indrdinc_data16(rd, data16); break;
	case 0x2: sub_word_indrdinc_data16(rd, data16); break;
	case 0x3: subb_word_indrdinc_data16(rd, data16); break;
	case 0x4: cmp_word_indrdinc_data16(rd, data16); break;
	case 0x5: and_word_indrdinc_data16(rd, data16); break;
	case 0x6: or_word_indrdinc_data16(rd, data16); break;
	case 0x7: xor_word_indrdinc_data16(rd, data16); break;
	case 0x8: mov_word_indrdinc_data16(rd, data16); break;
	default: logerror("UNK_ALUOP.w [%s+], #$%04x", m_regnames16[rd], data16); do_nop(); break;
	}
}
void xa_cpu_device::add_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "ADD.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::addc_word_indrdinc_data16(u8 rd, u16 data16){ fatalerror( "ADDC.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::sub_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "SUB.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::subb_word_indrdinc_data16(u8 rd, u16 data16){ fatalerror( "SUBB.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::cmp_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "CMP.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::and_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "AND.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::or_word_indrdinc_data16(u8 rd, u16 data16)  { fatalerror( "OR.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::xor_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "XOR.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }
void xa_cpu_device::mov_word_indrdinc_data16(u8 rd, u16 data16) { fatalerror( "MOV.w [%s+], #$%04x ([RD+], DATA16)", m_regnames16[rd], data16); }

// ALPOP.w [Rd+offs8], data16
void xa_cpu_device::aluop_byte_rdoff8_data16(int alu_op, u8 rd, u8 offset8, u16 data16)
{
	switch (alu_op)
	{
	case 0x0: add_word_indrdoff8_data16(rd, offset8, data16); break;
	case 0x1: addc_word_indrdoff8_data16(rd, offset8, data16); break;
	case 0x2: sub_word_indrdoff8_data16(rd, offset8, data16); break;
	case 0x3: subb_word_indrdoff8_data16(rd, offset8, data16); break;
	case 0x4: cmp_word_indrdoff8_data16(rd, offset8, data16); break;
	case 0x5: and_word_indrdoff8_data16(rd, offset8, data16); break;
	case 0x6: or_word_indrdoff8_data16(rd, offset8, data16); break;
	case 0x7: xor_word_indrdoff8_data16(rd, offset8, data16); break;
	case 0x8: mov_word_indrdoff8_data16(rd, offset8, data16); break;
	default: logerror("UNK_ALUOP.w [%s+#$%02x], #$%04x", m_regnames16[rd], offset8, data16); do_nop(); break;
	}
}
void xa_cpu_device::add_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "ADD.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::addc_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){ fatalerror( "ADDC.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::sub_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "SUB.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::subb_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16){ fatalerror( "SUBB.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::cmp_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "CMP.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::and_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "AND.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::or_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16)  { fatalerror( "OR.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::xor_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "XOR.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }
void xa_cpu_device::mov_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16) { fatalerror( "MOV.w [%s+#$%02x], #$%04x ([RD+offs8], DATA16)", m_regnames16[rd], offset8, data16); }

 // ALUOP.w [Rd+offs16], data16
void xa_cpu_device::aluop_byte_rdoff16_data16(int alu_op, u8 rd, u16 offset16, u16 data16)
{
	switch (alu_op)
	{
	case 0x0: add_word_indrdoff16_data16(rd, offset16, data16); break;
	case 0x1: addc_word_indrdoff16_data16(rd, offset16, data16); break;
	case 0x2: sub_word_indrdoff16_data16(rd, offset16, data16); break;
	case 0x3: subb_word_indrdoff16_data16(rd, offset16, data16); break;
	case 0x4: cmp_word_indrdoff16_data16(rd, offset16, data16); break;
	case 0x5: and_word_indrdoff16_data16(rd, offset16, data16); break;
	case 0x6: or_word_indrdoff16_data16(rd, offset16, data16); break;
	case 0x7: xor_word_indrdoff16_data16(rd, offset16, data16); break;
	case 0x8: mov_word_indrdoff16_data16(rd, offset16, data16); break;
	default: logerror("UNK_ALUOP.w [%s+#$%04x], #$%04x", m_regnames16[rd], offset16, data16); do_nop(); break;
	}
}
void xa_cpu_device::add_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "ADD.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::addc_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){ fatalerror( "ADDC.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::sub_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "SUB.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::subb_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16){ fatalerror( "SUBB.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::cmp_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "CMP.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::and_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "AND.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::or_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16)  { fatalerror( "OR.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::xor_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "XOR.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }
void xa_cpu_device::mov_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16) { fatalerror( "MOV.w [%s+#$%04x], #$%04x ([RD+offs16], DATA16)", m_regnames16[rd], offset16, data16); }

 // ALUOP.w DIRECT, data16
void xa_cpu_device::aluop_byte_direct_data16(int alu_op, u16 direct, u16 data16)
{
	switch (alu_op)
	{
	case 0x0: add_word_direct_data16(direct, data16); break;
	case 0x1: addc_word_direct_data16(direct, data16); break;
	case 0x2: sub_word_direct_data16(direct, data16); break;
	case 0x3: subb_word_direct_data16(direct, data16); break;
	case 0x4: cmp_word_direct_data16(direct, data16); break;
	case 0x5: and_word_direct_data16(direct, data16); break;
	case 0x6: or_word_direct_data16(direct, data16); break;
	case 0x7: xor_word_direct_data16(direct, data16); break;
	case 0x8: mov_word_direct_data16(direct, data16); break;
	default: logerror("UNK_ALUOP.w %s, #$%04x", get_directtext(direct), data16); do_nop(); break;
	}
}
void xa_cpu_device::add_word_direct_data16(u16 direct, u16 data16) { fatalerror( "ADD.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::addc_word_direct_data16(u16 direct, u16 data16){ fatalerror( "ADDC.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::sub_word_direct_data16(u16 direct, u16 data16) { fatalerror( "SUB.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
void xa_cpu_device::subb_word_direct_data16(u16 direct, u16 data16){ fatalerror( "SUBB.w %s, #$%04x (DIRECT, DATA16)", get_directtext(direct), data16); }
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
	case 0x3: subb_word_rd_rs(rd, rs); break;
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
void xa_cpu_device::subb_word_rd_rs(u8 rd, u8 rs){ fatalerror("SUBB.w %s, %s", m_regnames16[rd], m_regnames16[rs]);}
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
	case 0x3: subb_byte_rd_rs(rd, rs); break;
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
void xa_cpu_device::subb_byte_rd_rs(u8 rd, u8 rs){ fatalerror("SUBB.b %s, %s", m_regnames8[rd], m_regnames8[rs]);}
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
	case 0x3: subb_word_rd_indrs(rd, rs); break;
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
void xa_cpu_device::subb_word_rd_indrs(u8 rd, u8 rs){ fatalerror("SUBB.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]);}
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
	case 0x3: subb_byte_rd_indrs(rd, rs); break;
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
void xa_cpu_device::subb_byte_rd_indrs(u8 rd, u8 rs){ fatalerror("SUBB.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("CMP.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::and_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("AND.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::or_byte_rd_indrs(u8 rd, u8 rs)  { fatalerror("OR.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("XOR.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("MOV.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]);}


// ALUOP.w [Rd], Rs
void xa_cpu_device::aluop_word_indrd_rs(int alu_op, u8 rd, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_word_indrd_rs(rd, rs); break;
	case 0x1: addc_word_indrd_rs(rd, rs); break;
	case 0x2: sub_word_indrd_rs(rd, rs); break;
	case 0x3: subb_word_indrd_rs(rd, rs); break;
	case 0x4: cmp_word_indrd_rs(rd, rs); break;
	case 0x5: and_word_indrd_rs(rd, rs); break;
	case 0x6: or_word_indrd_rs(rd, rs); break;
	case 0x7: xor_word_indrd_rs(rd, rs); break;
	case 0x8: mov_word_indrd_rs(rd, rs); break;
	default: logerror("UNK_ALUOP.w [%s], %s", m_regnames16[rd], m_regnames16[rs]); do_nop(); break;
	}
}

void xa_cpu_device::add_word_indrd_rs(u8 rd, u8 rs) { fatalerror("ADD.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::addc_word_indrd_rs(u8 rd, u8 rs){ fatalerror("ADDC.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_word_indrd_rs(u8 rd, u8 rs) { fatalerror("SUB.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::subb_word_indrd_rs(u8 rd, u8 rs){ fatalerror("SUBB.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_word_indrd_rs(u8 rd, u8 rs) { fatalerror("CMP.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::and_word_indrd_rs(u8 rd, u8 rs) { fatalerror("AND.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::or_word_indrd_rs(u8 rd, u8 rs)  { fatalerror("OR.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_word_indrd_rs(u8 rd, u8 rs) { fatalerror("XOR.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_word_indrd_rs(u8 rd, u8 rs) { fatalerror("MOV.w [%s], %s", m_regnames16[rd], m_regnames16[rs]);}


// ALUOP.b [Rd], Rs
void xa_cpu_device::aluop_byte_indrd_rs(int alu_op, u8 rd, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_byte_indrd_rs(rd, rs); break;
	case 0x1: addc_byte_indrd_rs(rd, rs); break;
	case 0x2: sub_byte_indrd_rs(rd, rs); break;
	case 0x3: subb_byte_indrd_rs(rd, rs); break;
	case 0x4: cmp_byte_indrd_rs(rd, rs); break;
	case 0x5: and_byte_indrd_rs(rd, rs); break;
	case 0x6: or_byte_indrd_rs(rd, rs); break;
	case 0x7: xor_byte_indrd_rs(rd, rs); break;
	case 0x8: mov_byte_indrd_rs(rd, rs); break;
	default: logerror("UNK_ALUOP.b [%s], %s", m_regnames16[rd], m_regnames8[rs]); do_nop(); break;
	}
}
void xa_cpu_device::add_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("ADD.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::addc_byte_indrd_rs(u8 rd, u8 rs){ fatalerror("ADDC.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::sub_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("SUB.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::subb_byte_indrd_rs(u8 rd, u8 rs){ fatalerror("SUBB.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("CMP.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::and_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("AND.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::or_byte_indrd_rs(u8 rd, u8 rs)  { fatalerror("OR.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::xor_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("XOR.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::mov_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("MOV.b [%s], %s", m_regnames16[rd], m_regnames8[rs]);}

// ALUOP.w Rd, [Rs+]
void xa_cpu_device::aluop_word_rd_indrsinc(int alu_op, u8 rd, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_word_rd_indrsinc(rd, rs); break;
	case 0x1: addc_word_rd_indrsinc(rd, rs); break;
	case 0x2: sub_word_rd_indrsinc(rd, rs); break;
	case 0x3: subb_word_rd_indrsinc(rd, rs); break;
	case 0x4: cmp_word_rd_indrsinc(rd, rs); break;
	case 0x5: and_word_rd_indrsinc(rd, rs); break;
	case 0x6: or_word_rd_indrsinc(rd, rs); break;
	case 0x7: xor_word_rd_indrsinc(rd, rs); break;
	case 0x8: mov_word_rd_indrsinc(rd, rs); break;
	default: logerror("UNK_ALUOP.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]); do_nop(); break;
	}
}
void xa_cpu_device::add_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("ADD.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::addc_word_rd_indrsinc(u8 rd, u8 rs){ fatalerror("ADDC.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("SUB.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::subb_word_rd_indrsinc(u8 rd, u8 rs){ fatalerror("SUBB.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("CMP.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::and_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("AND.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::or_word_rd_indrsinc(u8 rd, u8 rs)  { fatalerror("OR.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("XOR.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("MOV.w %s, [%s+]", m_regnames16[rd], m_regnames16[rs]);}

// ALUOP.b Rd, [Rs+]
void xa_cpu_device::aluop_byte_rd_indrsinc(int alu_op, u8 rd, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_byte_rd_indrsinc(rd, rs); break;
	case 0x1: addc_byte_rd_indrsinc(rd, rs); break;
	case 0x2: sub_byte_rd_indrsinc(rd, rs); break;
	case 0x3: subb_byte_rd_indrsinc(rd, rs); break;
	case 0x4: cmp_byte_rd_indrsinc(rd, rs); break;
	case 0x5: and_byte_rd_indrsinc(rd, rs); break;
	case 0x6: or_byte_rd_indrsinc(rd, rs); break;
	case 0x7: xor_byte_rd_indrsinc(rd, rs); break;
	case 0x8: mov_byte_rd_indrsinc(rd, rs); break;
	default: logerror("UNK_ALUOP.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]); do_nop(); break;
	}
}

void xa_cpu_device::add_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("ADD.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::addc_byte_rd_indrsinc(u8 rd, u8 rs){ fatalerror("ADDC.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("SUB.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::subb_byte_rd_indrsinc(u8 rd, u8 rs){ fatalerror("SUBB.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("CMP.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::and_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("AND.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::or_byte_rd_indrsinc(u8 rd, u8 rs)  { fatalerror("OR.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("XOR.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_byte_rd_indrsinc(u8 rd, u8 rs) { fatalerror("MOV.b %s, [%s+]", m_regnames8[rd], m_regnames16[rs]);}

// ALUOP.w [Rd+], Rs
void xa_cpu_device::aluop_word_indrdinc_rs(int alu_op, u8 rd, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_word_indrdinc_rs(rd, rs); break;
	case 0x1: addc_word_indrdinc_rs(rd, rs); break;
	case 0x2: sub_word_indrdinc_rs(rd, rs); break;
	case 0x3: subb_word_indrdinc_rs(rd, rs); break;
	case 0x4: cmp_word_indrdinc_rs(rd, rs); break;
	case 0x5: and_word_indrdinc_rs(rd, rs); break;
	case 0x6: or_word_indrdinc_rs(rd, rs); break;
	case 0x7: xor_word_indrdinc_rs(rd, rs); break;
	case 0x8: mov_word_indrdinc_rs(rd, rs); break;
	default: logerror("UNK_ALUOP.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]); do_nop(); break;
	}
}

void xa_cpu_device::add_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("ADD.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::addc_word_indrdinc_rs(u8 rd, u8 rs){ fatalerror("ADDC.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::sub_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("SUB.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::subb_word_indrdinc_rs(u8 rd, u8 rs){ fatalerror("SUBB.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::cmp_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("CMP.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::and_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("AND.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::or_word_indrdinc_rs(u8 rd, u8 rs)  { fatalerror("OR.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::xor_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("XOR.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}
void xa_cpu_device::mov_word_indrdinc_rs(u8 rd, u8 rs) { fatalerror("MOV.w [%s+], %s", m_regnames16[rd], m_regnames16[rs]);}


// ALUOP.b [Rd+], Rs
void xa_cpu_device::aluop_byte_indrdinc_rs(int alu_op, u8 rd, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_byte_indrdinc_rs(rd, rs); break;
	case 0x1: addc_byte_indrdinc_rs(rd, rs); break;
	case 0x2: sub_byte_indrdinc_rs(rd, rs); break;
	case 0x3: subb_byte_indrdinc_rs(rd, rs); break;
	case 0x4: cmp_byte_indrdinc_rs(rd, rs); break;
	case 0x5: and_byte_indrdinc_rs(rd, rs); break;
	case 0x6: or_byte_indrdinc_rs(rd, rs); break;
	case 0x7: xor_byte_indrdinc_rs(rd, rs); break;
	case 0x8: mov_byte_indrdinc_rs(rd, rs); break;
	default: logerror("UNK_ALUOP.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]); do_nop(); break;
	}
}
void xa_cpu_device::add_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("ADD.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::addc_byte_indrdinc_rs(u8 rd, u8 rs){ fatalerror("ADDC.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::sub_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("SUB.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::subb_byte_indrdinc_rs(u8 rd, u8 rs){ fatalerror("SUBB.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("CMP.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::and_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("AND.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::or_byte_indrdinc_rs(u8 rd, u8 rs)  { fatalerror("OR.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::xor_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("XOR.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}
void xa_cpu_device::mov_byte_indrdinc_rs(u8 rd, u8 rs) { fatalerror("MOV.b [%s+], %s", m_regnames16[rd], m_regnames8[rs]);}


// ALUOP.w Rd, [Rs+off8]
void xa_cpu_device::aluop_word_rd_rsoff8(int alu_op, u8 rd, u8 rs, u8 offset8)
{
	switch (alu_op)
	{
	case 0x0: add_word_rd_rsoff8(rd, rs, offset8); break;
	case 0x1: addc_word_rd_rsoff8(rd, rs, offset8); break;
	case 0x2: sub_word_rd_rsoff8(rd, rs, offset8); break;
	case 0x3: subb_word_rd_rsoff8(rd, rs, offset8); break;
	case 0x4: cmp_word_rd_rsoff8(rd, rs, offset8); break;
	case 0x5: and_word_rd_rsoff8(rd, rs, offset8); break;
	case 0x6: or_word_rd_rsoff8(rd, rs, offset8); break;
	case 0x7: xor_word_rd_rsoff8(rd, rs, offset8); break;
	case 0x8: mov_word_rd_rsoff8(rd, rs, offset8); break;
	default: logerror("UNK_ALUOP.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8); do_nop(); break;
	}
}

void xa_cpu_device::add_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("ADD.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::addc_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8){ fatalerror("ADDC.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::sub_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("SUB.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::subb_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8){ fatalerror("SUBB.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::cmp_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("CMP.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::and_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("AND.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::or_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8)  { fatalerror("OR.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::xor_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("XOR.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::mov_word_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("MOV.w %s, [%s+#$%02x]", m_regnames16[rd], m_regnames16[rs], offset8);}

// ALUOP.b Rd, [Rs+off8]
void xa_cpu_device::aluop_byte_rd_rsoff8(int alu_op, u8 rd, u8 rs, u8 offset8)
{
	switch (alu_op)
	{
	case 0x0: add_byte_rd_rsoff8(rd, rs, offset8); break;
	case 0x1: addc_byte_rd_rsoff8(rd, rs, offset8); break;
	case 0x2: sub_byte_rd_rsoff8(rd, rs, offset8); break;
	case 0x3: subb_byte_rd_rsoff8(rd, rs, offset8); break;
	case 0x4: cmp_byte_rd_rsoff8(rd, rs, offset8); break;
	case 0x5: and_byte_rd_rsoff8(rd, rs, offset8); break;
	case 0x6: or_byte_rd_rsoff8(rd, rs, offset8); break;
	case 0x7: xor_byte_rd_rsoff8(rd, rs, offset8); break;
	case 0x8: mov_byte_rd_rsoff8(rd, rs, offset8); break;
	default: logerror("UNK_ALUOP.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8); do_nop(); break;
	}
}

void xa_cpu_device::add_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("ADD.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::addc_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8){ fatalerror("ADDC.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::sub_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("SUB.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::subb_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8){ fatalerror("SUBB.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::cmp_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("CMP.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::and_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("AND.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::or_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8)  { fatalerror("OR.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::xor_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("XOR.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}
void xa_cpu_device::mov_byte_rd_rsoff8(u8 rd, u8 rs, u8 offset8) { fatalerror("MOV.b %s, [%s+#$%02x]", m_regnames8[rd], m_regnames16[rs], offset8);}

// ALUOP.w [Rd+off8], Rs
void xa_cpu_device::aluop_word_rdoff8_rs(int alu_op, u8 rd, u8 offset8, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_word_rdoff8_rs(rd, offset8, rs); break;
	case 0x1: addc_word_rdoff8_rs(rd, offset8, rs); break;
	case 0x2: sub_word_rdoff8_rs(rd, offset8, rs); break;
	case 0x3: subb_word_rdoff8_rs(rd, offset8, rs); break;
	case 0x4: cmp_word_rdoff8_rs(rd, offset8, rs); break;
	case 0x5: and_word_rdoff8_rs(rd, offset8, rs); break;
	case 0x6: or_word_rdoff8_rs(rd, offset8, rs); break;
	case 0x7: xor_word_rdoff8_rs(rd, offset8, rs); break;
	case 0x8: mov_word_rdoff8_rs(rd, offset8, rs); break;
	default: logerror("UNK_ALUOP.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]); do_nop(); break;
	}
}

void xa_cpu_device::add_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("ADD.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::addc_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs){ fatalerror("ADDC.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::sub_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("SUB.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::subb_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs){ fatalerror("SUBB.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::cmp_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("CMP.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::and_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("AND.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::or_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs)  { fatalerror("OR.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::xor_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("XOR.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}
void xa_cpu_device::mov_word_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("MOV.w [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames16[rs]);}


// ALUOP.b [Rd+off8], Rs
void xa_cpu_device::aluop_byte_rdoff8_rs(int alu_op, u8 rd, u8 offset8, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_byte_rdoff8_rs(rd, offset8, rs); break;
	case 0x1: addc_byte_rdoff8_rs(rd, offset8, rs); break;
	case 0x2: sub_byte_rdoff8_rs(rd, offset8, rs); break;
	case 0x3: subb_byte_rdoff8_rs(rd, offset8, rs); break;
	case 0x4: cmp_byte_rdoff8_rs(rd, offset8, rs); break;
	case 0x5: and_byte_rdoff8_rs(rd, offset8, rs); break;
	case 0x6: or_byte_rdoff8_rs(rd, offset8, rs); break;
	case 0x7: xor_byte_rdoff8_rs(rd, offset8, rs); break;
	case 0x8: mov_byte_rdoff8_rs(rd, offset8, rs); break;
	default: logerror("UNK_ALUOP.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]); do_nop(); break;
	}
}

void xa_cpu_device::add_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("ADD.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::addc_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs){ fatalerror("ADDC.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::sub_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("SUB.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::subb_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs){ fatalerror("SUBB.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("CMP.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::and_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("AND.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::or_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs)  { fatalerror("OR.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::xor_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("XOR.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}
void xa_cpu_device::mov_byte_rdoff8_rs(u8 rd, u8 offset8, u8 rs) { fatalerror("MOV.b [%s+#$%02x], %s", m_regnames16[rd], offset8, m_regnames8[rs]);}


// ALUOP.w Rd, [Rs+off16]
void xa_cpu_device::aluop_word_rsoff16(int alu_op, u8 rd, u8 rs, u16 offset16)
{
	switch (alu_op)
	{
	case 0x0: add_word_rd_rsoff16(rd, rs, offset16); break;
	case 0x1: addc_word_rd_rsoff16(rd, rs, offset16); break;
	case 0x2: sub_word_rd_rsoff16(rd, rs, offset16); break;
	case 0x3: subb_word_rd_rsoff16(rd, rs, offset16); break;
	case 0x4: cmp_word_rd_rsoff16(rd, rs, offset16); break;
	case 0x5: and_word_rd_rsoff16(rd, rs, offset16); break;
	case 0x6: or_word_rd_rsoff16(rd, rs, offset16); break;
	case 0x7: xor_word_rd_rsoff16(rd, rs, offset16); break;
	case 0x8: mov_word_rd_rsoff16(rd, rs, offset16); break;
	default: logerror("UNK_ALUOP.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16); do_nop(); break;
	}
}
void xa_cpu_device::add_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("ADD.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::addc_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16){ fatalerror("ADDC.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::sub_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("SUB.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::subb_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16){ fatalerror("SUBB.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::cmp_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("CMP.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::and_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("AND.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::or_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16)  { fatalerror("OR.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::xor_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("XOR.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::mov_word_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("MOV.w %s, [%s+#$%04x]", m_regnames16[rd], m_regnames16[rs], offset16);}

// ALUOP.b Rd, [Rs+off16]
void xa_cpu_device::aluop_byte_rsoff16(int alu_op, u8 rd, u8 rs, u16 offset16)
{
	switch (alu_op)
	{
	case 0x0: add_byte_rd_rsoff16(rd, rs, offset16); break;
	case 0x1: addc_byte_rd_rsoff16(rd, rs, offset16); break;
	case 0x2: sub_byte_rd_rsoff16(rd, rs, offset16); break;
	case 0x3: subb_byte_rd_rsoff16(rd, rs, offset16); break;
	case 0x4: cmp_byte_rd_rsoff16(rd, rs, offset16); break;
	case 0x5: and_byte_rd_rsoff16(rd, rs, offset16); break;
	case 0x6: or_byte_rd_rsoff16(rd, rs, offset16); break;
	case 0x7: xor_byte_rd_rsoff16(rd, rs, offset16); break;
	case 0x8: mov_byte_rd_rsoff16(rd, rs, offset16); break;
	default: logerror("UNK_ALUOP.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16); do_nop(); break;
	}
}

void xa_cpu_device::add_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("ADD.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::addc_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16){ fatalerror("ADDC.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::sub_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("SUB.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::subb_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16){ fatalerror("SUBB.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::cmp_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("CMP.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::and_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("AND.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::or_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16)  { fatalerror("OR.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::xor_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("XOR.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}
void xa_cpu_device::mov_byte_rd_rsoff16(u8 rd, u8 rs, u16 offset16) { fatalerror("MOV.b %s, [%s+#$%04x]", m_regnames8[rd], m_regnames16[rs], offset16);}

// ALUOP.w [Rd+off16], Rs
void xa_cpu_device::aluop_word_rdoff16_rs(int alu_op, u8 rd, u16 offset16, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_word_rdoff16_rs(rd, offset16, rs); break;
	case 0x1: addc_word_rdoff16_rs(rd, offset16, rs); break;
	case 0x2: sub_word_rdoff16_rs(rd, offset16, rs); break;
	case 0x3: subb_word_rdoff16_rs(rd, offset16, rs); break;
	case 0x4: cmp_word_rdoff16_rs(rd, offset16, rs); break;
	case 0x5: and_word_rdoff16_rs(rd, offset16, rs); break;
	case 0x6: or_word_rdoff16_rs(rd, offset16, rs); break;
	case 0x7: xor_word_rdoff16_rs(rd, offset16, rs); break;
	case 0x8: mov_word_rdoff16_rs(rd, offset16, rs); break;
	default: logerror("UNK_ALUOP.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]); do_nop(); break;
	}
}
void xa_cpu_device::add_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("ADD.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::addc_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs){ fatalerror("ADDC.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::sub_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("SUB.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::subb_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs){ fatalerror("SUBB.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::cmp_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("CMP.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::and_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("AND.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::or_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs)  { fatalerror("OR.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::xor_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("XOR.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}
void xa_cpu_device::mov_word_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("MOV.w [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames16[rs]);}

// ALUOP.b [Rd+off16], Rs
void xa_cpu_device::aluop_byte_rdoff16_rs(int alu_op, u8 rd, u16 offset16, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_byte_rdoff16_rs(rd, offset16, rs); break;
	case 0x1: addc_byte_rdoff16_rs(rd, offset16, rs); break;
	case 0x2: sub_byte_rdoff16_rs(rd, offset16, rs); break;
	case 0x3: subb_byte_rdoff16_rs(rd, offset16, rs); break;
	case 0x4: cmp_byte_rdoff16_rs(rd, offset16, rs); break;
	case 0x5: and_byte_rdoff16_rs(rd, offset16, rs); break;
	case 0x6: or_byte_rdoff16_rs(rd, offset16, rs); break;
	case 0x7: xor_byte_rdoff16_rs(rd, offset16, rs); break;
	case 0x8: mov_byte_rdoff16_rs(rd, offset16, rs); break;
	default: logerror("UNK_ALUOP.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]); do_nop(); break;
	}
}

void xa_cpu_device::add_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("ADD.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::addc_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs){ fatalerror("ADDC.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::sub_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("SUB.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::subb_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs){ fatalerror("SUBB.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("CMP.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::and_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("AND.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::or_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs)  { fatalerror("OR.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::xor_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("XOR.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}
void xa_cpu_device::mov_byte_rdoff16_rs(u8 rd, u16 offset16, u8 rs) { fatalerror("MOV.b [%s+#$%04x], %s", m_regnames16[rd], offset16, m_regnames8[rs]);}

// ALUOP.w Rd, Direct
void xa_cpu_device::aluop_word_rd_direct(int alu_op, u8 rd, u16 direct)
{
	switch (alu_op)
	{
	case 0x0: add_word_rd_direct(rd, direct); break;
	case 0x1: addc_word_rd_direct(rd, direct); break;
	case 0x2: sub_word_rd_direct(rd, direct); break;
	case 0x3: subb_word_rd_direct(rd, direct); break;
	case 0x4: cmp_word_rd_direct(rd, direct); break;
	case 0x5: and_word_rd_direct(rd, direct); break;
	case 0x6: or_word_rd_direct(rd, direct); break;
	case 0x7: xor_word_rd_direct(rd, direct); break;
	case 0x8: mov_word_rd_direct(rd, direct); break;
	default: logerror("UNK_ALUOP.w %s, %s", m_regnames16[rd], get_directtext(direct)); do_nop(); break;
	}
}

void xa_cpu_device::add_word_rd_direct(u8 rd, u16 direct) { fatalerror("ADD.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::addc_word_rd_direct(u8 rd, u16 direct){ fatalerror("ADDC.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::sub_word_rd_direct(u8 rd, u16 direct) { fatalerror("SUB.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::subb_word_rd_direct(u8 rd, u16 direct){ fatalerror("SUBB.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::cmp_word_rd_direct(u8 rd, u16 direct) { fatalerror("CMP.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::and_word_rd_direct(u8 rd, u16 direct) { fatalerror("AND.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::or_word_rd_direct(u8 rd, u16 direct)  { fatalerror("OR.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::xor_word_rd_direct(u8 rd, u16 direct) { fatalerror("XOR.w %s, %s", m_regnames16[rd], get_directtext(direct));}
void xa_cpu_device::mov_word_rd_direct(u8 rd, u16 direct) { fatalerror("MOV.w %s, %s", m_regnames16[rd], get_directtext(direct));}

// ALUOP.b Rd, Direct
void xa_cpu_device::aluop_byte_rd_direct(int alu_op, u8 rd, u16 direct)
{
	switch (alu_op)
	{
	case 0x0: add_byte_rd_direct(rd, direct); break;
	case 0x1: addc_byte_rd_direct(rd, direct); break;
	case 0x2: sub_byte_rd_direct(rd, direct); break;
	case 0x3: subb_byte_rd_direct(rd, direct); break;
	case 0x4: cmp_byte_rd_direct(rd, direct); break;
	case 0x5: and_byte_rd_direct(rd, direct); break;
	case 0x6: or_byte_rd_direct(rd, direct); break;
	case 0x7: xor_byte_rd_direct(rd, direct); break;
	case 0x8: mov_byte_rd_direct(rd, direct); break;
	default: logerror("UNK_ALUOP.b %s, %s", m_regnames8[rd], get_directtext(direct)); do_nop(); break;
	}
}

void xa_cpu_device::add_byte_rd_direct(u8 rd, u16 direct) { fatalerror("ADD.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::addc_byte_rd_direct(u8 rd, u16 direct){ fatalerror("ADDC.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::sub_byte_rd_direct(u8 rd, u16 direct) { fatalerror("SUB.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::subb_byte_rd_direct(u8 rd, u16 direct){ fatalerror("SUBB.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::cmp_byte_rd_direct(u8 rd, u16 direct) { fatalerror("CMP.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::and_byte_rd_direct(u8 rd, u16 direct) { fatalerror("AND.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::or_byte_rd_direct(u8 rd, u16 direct)  { fatalerror("OR.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::xor_byte_rd_direct(u8 rd, u16 direct) { fatalerror("XOR.b %s, %s", m_regnames8[rd], get_directtext(direct));}
void xa_cpu_device::mov_byte_rd_direct(u8 rd, u16 direct) { fatalerror("MOV.b %s, %s", m_regnames8[rd], get_directtext(direct));}

// ALUOP.w Direct, Rs
void xa_cpu_device::aluop_word_direct_rs(int alu_op, u16 direct, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_word_direct_rs(direct, rs); break;
	case 0x1: addc_word_direct_rs(direct, rs); break;
	case 0x2: sub_word_direct_rs(direct, rs); break;
	case 0x3: subb_word_direct_rs(direct, rs); break;
	case 0x4: cmp_word_direct_rs(direct, rs); break;
	case 0x5: and_word_direct_rs(direct, rs); break;
	case 0x6: or_word_direct_rs(direct, rs); break;
	case 0x7: xor_word_direct_rs(direct, rs); break;
	case 0x8: mov_word_direct_rs(direct, rs); break;
	default: logerror("UNK_ALUOP.w %s, %s", get_directtext(direct), m_regnames16[rs]); do_nop(); break;
	}
}

void xa_cpu_device::add_word_direct_rs(u16 direct, u8 rs) { fatalerror("ADD.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::addc_word_direct_rs(u16 direct, u8 rs){ fatalerror("ADDC.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::sub_word_direct_rs(u16 direct, u8 rs) { fatalerror("SUB.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::subb_word_direct_rs(u16 direct, u8 rs){ fatalerror("SUBB.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::cmp_word_direct_rs(u16 direct, u8 rs) { fatalerror("CMP.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::and_word_direct_rs(u16 direct, u8 rs) { fatalerror("AND.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::or_word_direct_rs(u16 direct, u8 rs)  { fatalerror("OR.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::xor_word_direct_rs(u16 direct, u8 rs) { fatalerror("XOR.w %s, %s", get_directtext(direct), m_regnames16[rs]);}
void xa_cpu_device::mov_word_direct_rs(u16 direct, u8 rs) { fatalerror("MOV.w %s, %s", get_directtext(direct), m_regnames16[rs]);}

// ALUOP.b Direct, Rs
void xa_cpu_device::aluop_byte_direct_rs(int alu_op, u16 direct, u8 rs)
{
	switch (alu_op)
	{
	case 0x0: add_byte_direct_rs(direct, rs); break;
	case 0x1: addc_byte_direct_rs(direct, rs); break;
	case 0x2: sub_byte_direct_rs(direct, rs); break;
	case 0x3: subb_byte_direct_rs(direct, rs); break;
	case 0x4: cmp_byte_direct_rs(direct, rs); break;
	case 0x5: and_byte_direct_rs(direct, rs); break;
	case 0x6: or_byte_direct_rs(direct, rs); break;
	case 0x7: xor_byte_direct_rs(direct, rs); break;
	case 0x8: mov_byte_direct_rs(direct, rs); break;
	default: logerror("UNK_ALUOP.b %s, %s", get_directtext(direct), m_regnames8[rs]); do_nop(); break;
	}
}

void xa_cpu_device::add_byte_direct_rs(u16 direct, u8 rs) { fatalerror("ADD.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::addc_byte_direct_rs(u16 direct, u8 rs){ fatalerror("ADDC.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::sub_byte_direct_rs(u16 direct, u8 rs) { fatalerror("SUB.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::subb_byte_direct_rs(u16 direct, u8 rs){ fatalerror("SUBB.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::cmp_byte_direct_rs(u16 direct, u8 rs) { fatalerror("CMP.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::and_byte_direct_rs(u16 direct, u8 rs) { fatalerror("AND.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::or_byte_direct_rs(u16 direct, u8 rs)  { fatalerror("OR.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::xor_byte_direct_rs(u16 direct, u8 rs) { fatalerror("XOR.b %s, %s", get_directtext(direct), m_regnames8[rs]);}
void xa_cpu_device::mov_byte_direct_rs(u16 direct, u8 rs) { fatalerror("MOV.b %s, %s", get_directtext(direct), m_regnames8[rs]);}


// ADDS.w / MOVS.w Rd, #data4
void xa_cpu_device::movs_word_rd_data4(u8 rd, u8 data4) { fatalerror("MOVS.w %s, %s", m_regnames16[rd], show_expanded_data4(data4, 1)); }
void xa_cpu_device::adds_word_rd_data4(u8 rd, u8 data4) { fatalerror("ADDS.w %s, %s", m_regnames16[rd], show_expanded_data4(data4, 1)); }

// ADDS.b / MOVS.b Rd, #data4
void xa_cpu_device::movs_byte_rd_data4(u8 rd, u8 data4){ u8 data = util::sext(data4, 4); set_reg8(rd, data);    do_nz_flags_8(data); }
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

void xa_cpu_device::asl_byte_rd_rs(u8 rd, u8 rs) { fatalerror("ASL.b %s, %d", m_regnames8[rd], m_regnames8[rs]); }
void xa_cpu_device::asl_word_rd_rs(u8 rd, u8 rs) { fatalerror("ASL.w %s, %d", m_regnames16[rd], m_regnames8[rs]); }
void xa_cpu_device::asl_dword_rd_rs(u8 rd, u8 rs) { fatalerror("ASL.dw %s, %d", m_regnames16[rd], m_regnames8[rs]); }

void xa_cpu_device::asr_byte_rd_rs(u8 rd, u8 rs) { fatalerror("ASR.b %s, %d", m_regnames8[rd], m_regnames8[rs]); }
void xa_cpu_device::asr_word_rd_rs(u8 rd, u8 rs) { fatalerror("ASR.w %s, %d", m_regnames16[rd], m_regnames8[rs]); }
void xa_cpu_device::asr_dword_rd_rs(u8 rd, u8 rs) { fatalerror("ASR.dw %s, %d", m_regnames16[rd], m_regnames8[rs]); }

void xa_cpu_device::lsr_byte_rd_rs(u8 rd, u8 rs) { fatalerror("LSR.b %s, %d", m_regnames8[rd], m_regnames8[rs]); }
void xa_cpu_device::lsr_word_rd_rs(u8 rd, u8 rs) { fatalerror("LSR.w %s, %d", m_regnames16[rd], m_regnames8[rs]); }
void xa_cpu_device::lsr_dword_rd_rs(u8 rd, u8 rs) { fatalerror("LSR.dw %s, %d", m_regnames16[rd], m_regnames8[rs]); }

void xa_cpu_device::norm_byte_rd_rs(u8 rd, u8 rs) { fatalerror("NORM.b %s, %d", m_regnames8[rd], m_regnames8[rs]); }
void xa_cpu_device::norm_word_rd_rs(u8 rd, u8 rs) { fatalerror("NORM.w %s, %d", m_regnames16[rd], m_regnames8[rs]); }
void xa_cpu_device::norm_dword_rd_rs(u8 rd, u8 rs) { fatalerror("NORM.dw %s, %d", m_regnames16[rd], m_regnames8[rs]); }


//MULU.b Rd, Rs
void xa_cpu_device::mulu_byte_rd_rs(u8 rd, u8 rs) { fatalerror( "MULU.b %s, %s", m_regnames8[rd], m_regnames8[rs]); }
//DIVU.b Rd, Rs
void xa_cpu_device::divu_byte_rd_rs(u8 rd, u8 rs) { fatalerror( "DIVU.b %s, %s", m_regnames8[rd], m_regnames8[rs]); }
//MULU.w Rd, Rs
void xa_cpu_device::mulu_word_rd_rs(u8 rd, u8 rs) { fatalerror( "MULU.w %s, %s", m_regnames16[rd], m_regnames16[rs]); }
//DIVU.w Rd, Rs
void xa_cpu_device::divu_word_rd_rs(u8 rd, u8 rs) { fatalerror( "DIVU.w %s, %s", m_regnames16[rd], m_regnames16[rs]); }
// MUL.w Rd, Rs
void xa_cpu_device::mul_word_rd_rs(u8 rd, u8 rs) { fatalerror( "MUL.w %s, %s", m_regnames16[rd], m_regnames16[rs]); }
// DIV.w Rd, Rs
void xa_cpu_device::div_word_rd_rs(u8 rd, u8 rs) { fatalerror( "DIV.w %s, %s", m_regnames16[rd], m_regnames16[rs]); }

//DIV.w Rd, #data8
void xa_cpu_device::div_word_rd_data8(u8 rd, u8 data8) { fatalerror( "DIV.w %s, #$%02x", m_regnames8[rd], data8); }
//DIVU.b Rd, #data8
void xa_cpu_device::divu_byte_rd_data8(u8 rd, u8 data8) { fatalerror( "DIVU.b %s, #$%02x", m_regnames8[rd], data8); }
//DIVU.w Rd, #data8
void xa_cpu_device::divu_word_rd_data8(u8 rd, u8 data8) { fatalerror( "DIVU.w %s, #$%02x", m_regnames8[rd], data8); }
//MULU.b Rd, #data8
void xa_cpu_device::mulu_byte_rd_data8(u8 rd, u8 data8) { fatalerror( "MULU.b %s, #$%02x", m_regnames8[rd], data8); }

//MULU.w Rd, #data16
void xa_cpu_device::mulu_word_rd_data16(u8 rd, u16 data16) { fatalerror( "MULU.w %s, #$%04x", m_regnames16[rd], data16); }
//DIVU.d Rd, #data16
void xa_cpu_device::divu_dword_rd_data16(u8 rd, u16 data16) { fatalerror( "DIVU.d %s, #$%04x", m_regnames16[rd], data16); }
//MUL.w Rd, #data16
void xa_cpu_device::mul_word_rd_data16(u8 rd, u16 data16) { fatalerror( "MUL.w %s, #$%04x", m_regnames16[rd], data16); }
//DIV.d Rd, #data16
void xa_cpu_device::div_dword_rd_data16(u8 rd, u16 data16) { fatalerror( "DIV.d %s, #$%04x", m_regnames16[rd], data16); }

//DIVU.d Rd, Rs
void xa_cpu_device::divu_dword_rd_rs(u8 rd, u8 rs) { fatalerror( "DIVU.d %s, %s", m_regnames16[rd], m_regnames16[rs]); }
//DIV.d Rd, Rs
void xa_cpu_device::div_dword_rd_rs(u8 rd, u8 rs) { fatalerror( "DIV.d %s, %s", m_regnames16[rd], m_regnames16[rs]); }


void xa_cpu_device::clr_bit(u16 bit) { fatalerror( "CLR %s", get_bittext(bit) ); }
void xa_cpu_device::setb_bit(u16 bit) { fatalerror( "SETB %s", get_bittext(bit) ); }
void xa_cpu_device::mov_c_bit(u16 bit) { fatalerror( "MOV C, %s", get_bittext(bit) ); }
void xa_cpu_device::mov_bit_c(u16 bit) { fatalerror( "MOV %s, C", get_bittext(bit) ); }
void xa_cpu_device::anl_c_bit(u16 bit) { fatalerror( "ANL C, %s", get_bittext(bit) ); }
void xa_cpu_device::anl_c_notbit(u16 bit) { fatalerror( "ANL C, /%s", get_bittext(bit) ); }
void xa_cpu_device::orl_c_bit(u16 bit) { fatalerror( "ORL C, %s", get_bittext(bit) ); }
void xa_cpu_device::orl_c_notbit(u16 bit) { fatalerror( "ORL C, /%s", get_bittext(bit) ); }

// LEA Rd, Rs+offset8          Load 16-bit effective address w/ 8-bit offs to reg                      3 3         0100 0000  0ddd 0sss  oooo oooo
void xa_cpu_device::lea_word_rd_rs_off8(u8 rd, u8 rs, u8 offs8) { fatalerror("LEA %s, %s+#$%02x", m_regnames16[rd], m_regnames16[rs], offs8); }

// LEA Rd, Rs+offset16         Load 16-bit effective address w/ 16-bit offs to reg                     4 3         0100 1000  0ddd 0sss  oooo oooo  oooo oooo
void xa_cpu_device::lea_word_rd_rs_off16(u8 rd, u8 rs, u16 offs16) { fatalerror( "LEA %s, %s+#$%04x", m_regnames16[rd], m_regnames16[rs], offs16); }

// XCH Rd, [Rs]                Exchange contents of a reg-ind address w/ a reg                         2 6         0101 S000  dddd 0sss
void xa_cpu_device::xch_word_rd_indrs(u8 rd, u8 rs) { fatalerror("XCH.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]); }
void xa_cpu_device::xch_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("XCH.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]); }

// XCH Rd, Rs                  Exchange contents of two regs                                           2 5         0110 S000  dddd ssss
void xa_cpu_device::xch_word_rd_rs(u8 rd, u8 rs) { fatalerror("XCH.w %s, %s", m_regnames16[rd], m_regnames16[rs]); }
void xa_cpu_device::xch_byte_rd_rs(u8 rd, u8 rs) { fatalerror("XCH.b %s, %s", m_regnames8[rd], m_regnames8[rs]); }

// MOVC Rd, [Rs+]              Move data from WS:Rs address of code mem to reg w/ autoinc              2 4         1000 S000  dddd 0sss
void xa_cpu_device::movc_word_rd_indrsinc(u8 rd, u8 rs) { fatalerror("MOVC.w %s, [%s+]\n", m_regnames16[rd], m_regnames16[rs]); }
void xa_cpu_device::movc_byte_rd_indrsinc(u8 rd, u8 rs) { u16 ptr = get_reg16(rs); u8 data = m_program->read_byte(ptr); ptr++; set_reg16(rs, ptr); do_nz_flags_8(data); set_reg8(rd, data); }

// DJNZ Rd,rel8                Decrement reg and jump if not zero                                      3 8t/5nt    1000 S111  dddd 1000  rrrr rrrr
void xa_cpu_device::djnz_word_rd_rel8(u8 rd, u8 rel8) { u16 regval = get_reg16(rd); regval--; set_reg16(rd, regval); if (regval != 0) { set_pc_in_current_page(expand_rel8(rel8)); } }
void xa_cpu_device::djnz_byte_rd_rel8(u8 rd, u8 rel8) { fatalerror("DJNZ.b %s, $%04x", m_regnames8[rd], expand_rel8(rel8)); }

void xa_cpu_device::popu_word_direct(u16 direct) { fatalerror("POPU.w %s", get_directtext(direct)); }
void xa_cpu_device::popu_byte_direct(u16 direct) { fatalerror("POPU.b %s", get_directtext(direct)); }
void xa_cpu_device::pop_word_direct(u16 direct) { fatalerror("POP.w %s", get_directtext(direct)); }
void xa_cpu_device::pop_byte_direct(u16 direct) { fatalerror("POP.b %s", get_directtext(direct)); }
void xa_cpu_device::pushu_word_direct(u16 direct) { fatalerror("PUSHU.w %s", get_directtext(direct)); }
void xa_cpu_device::pushu_byte_direct(u16 direct) { fatalerror("PUSHU.b %s", get_directtext(direct)); }
void xa_cpu_device::push_word_direct(u16 direct) { fatalerror("PUSH.w %s", get_directtext(direct)); }
void xa_cpu_device::push_byte_direct(u16 direct) { fatalerror("PUSH.b %s", get_directtext(direct)); }

// MOV [Rd+], [Rs+]            Move reg-ind to reg-ind, both pointers autoinc                          2 6         1001 S000  0ddd 0sss
void xa_cpu_device::mov_word_indrdinc_indrsinc(u8 rd, u8 rs) { fatalerror("MOV.w [%s+], [%s+]", m_regnames16[rd], m_regnames16[rs]); }
void xa_cpu_device::mov_byte_indrdinc_indrsinc(u8 rd, u8 rs) { fatalerror("MOV.b [%s+], [%s+]", m_regnames16[rd], m_regnames16[rs]); }

void xa_cpu_device::da_rd(u8 rd) { fatalerror( "DA %s", m_regnames8[rd]); }
void xa_cpu_device::sext_word_rd(u8 rd) { fatalerror("SEXT.w %s", m_regnames16[rd]); }
void xa_cpu_device::sext_byte_rd(u8 rd) { fatalerror("SEXT.b %s", m_regnames8[rd]); }
void xa_cpu_device::cpl_word_rd(u8 rd) { fatalerror("CPL.w %s", m_regnames16[rd]); }
void xa_cpu_device::cpl_byte_rd(u8 rd) { fatalerror("CPL.b %s", m_regnames8[rd]); }
void xa_cpu_device::neg_word_rd(u8 rd) { fatalerror("NEG.w %sx", m_regnames16[rd]); }
void xa_cpu_device::neg_byte_rd(u8 rd) { fatalerror("NEG.b %sx", m_regnames8[rd]); }
void xa_cpu_device::movc_a_apc() { fatalerror( "MOVC A, [A+PC]"); }
void xa_cpu_device::movc_a_adptr() { fatalerror( "MOVC A, [A+DPTR]"); }
void xa_cpu_device::mov_rd_usp(u8 rd) { fatalerror( "MOV %s, USP", m_regnames16[rd]); }
void xa_cpu_device::mov_usp_rs(u8 rs) { fatalerror( "MOV USP, %s", m_regnames16[rs]); }

//JB bit,rel8                 Jump if bit set                                                         4 10t/6nt   1001 0111  1000 00bb  bbbb bbbb  rrrr rrrr
//JNB bit,rel8                Jump if bit not set                                                     4 10t/6nt   1001 0111  1010 00bb  bbbb bbbb  rrrr rrrr
//JBC bit,rel8                Jump if bit set and then clear the bit                                  4 11t/7nt   1001 0111  1100 00bb  bbbb bbbb  rrrr rrrr

void xa_cpu_device::jb_bit_rel8(u16 bit, u8 rel8) { fatalerror( "JB %s, $%02x", get_bittext(bit), expand_rel8(rel8) ); }
void xa_cpu_device::jnb_bit_rel8(u16 bit, u8 rel8) { fatalerror( "JNB %s, $%02x", get_bittext(bit), expand_rel8(rel8) ); }
void xa_cpu_device::jbc_bit_rel8(u16 bit, u8 rel8) { fatalerror( "JBC %s, $%02x", get_bittext(bit), expand_rel8(rel8) ); }

// MOV direct, direct          Move mem to mem                                                         4 4         1001 S111  0DDD 0ddd  DDDD DDDD  dddd dddd
void xa_cpu_device::mov_word_direct_direct(u16 direct_d, u16 direct_s) { fatalerror("MOV.w %s, %s", get_directtext(direct_d), get_directtext(direct_s)); }
void xa_cpu_device::mov_byte_direct_direct(u16 direct_d, u16 direct_s) { fatalerror("MOV.b %s, %s", get_directtext(direct_d), get_directtext(direct_s)); }

// XCH Rd, direct              Exchange contents of mem w/ a reg                                       3 6         1010 S000  dddd 1DDD  DDDD DDDD
void xa_cpu_device::xch_word_rd_direct(u8 rd, u16 direct) { fatalerror("XCH.w %s, %s", m_regnames16[rd], get_directtext(direct)); }
void xa_cpu_device::xch_byte_rd_direct(u8 rd, u16 direct) { fatalerror("XCH.b %s, %s", m_regnames8[rd], get_directtext(direct)); }

// MOV direct, [Rs]            Move reg-ind to mem                                                     3 4         1010 S000  1sss 0DDD  DDDD DDDD
void xa_cpu_device::mov_word_direct_indrs(u16 direct, u8 rs) { fatalerror("MOV.w %s, [%s]", get_directtext(direct), m_regnames16[rs]); }
void xa_cpu_device::mov_byte_direct_indrs(u16 direct, u8 rs) { fatalerror("MOV.b %s, [%s]", get_directtext(direct), m_regnames16[rs]); }

// MOV [Rd], direct            Move mem to reg-ind                                                     3 4         1010 S000  0ddd 0DDD  DDDD DDDD
void xa_cpu_device::mov_word_indrd_direct(u8 rd, u16 direct) { fatalerror("MOV.w [%s], %s", m_regnames16[rd], get_directtext(direct)); }
void xa_cpu_device::mov_byte_indrd_direct(u8 rd, u16 direct) { fatalerror("MOV.b [%s], %s", m_regnames16[rd], get_directtext(direct)); }

// MOVX [Rd], Rs               Move external data from reg to mem                                      2 6         1010 S111  ssss 1ddd
void xa_cpu_device::movx_word_indrd_rs(u8 rd, u8 rs) { fatalerror("MOVX.w [%s], %s", m_regnames16[rd], m_regnames16[rs]); }
void xa_cpu_device::movx_byte_indrd_rs(u8 rd, u8 rs) { fatalerror("MOVX.b [%s], %s", m_regnames16[rd], m_regnames8[rs]); }

// MOVX Rd, [Rs]               Move external data from mem to reg                                      2 6         1010 S111  dddd 0sss
void xa_cpu_device::movx_word_rd_indrs(u8 rd, u8 rs) { fatalerror("MOVX.w %s, [%s]", m_regnames16[rd], m_regnames16[rs]); }
void xa_cpu_device::movx_byte_rd_indrs(u8 rd, u8 rs) { fatalerror("MOVX.b %s, [%s]", m_regnames8[rd], m_regnames16[rs]); }

//RR Rd, #data4               Rotate right reg by the 4-bit imm value                                 2 a*        1011 S000  dddd iiii
void xa_cpu_device::rr_word_rd_data4(u8 rd, u8 data4) { fatalerror("RR.w %s, %d", m_regnames16[rd], data4); }
void xa_cpu_device::rr_byte_rd_data4(u8 rd, u8 data4) { fatalerror("RR.b %s, %d", m_regnames8[rd], data4); }

//RRC Rd, #data4              Rotate right reg though carry by the 4-bit imm value                    2 a*        1011 S111  dddd iiii
void xa_cpu_device::rrc_word_rd_data4(u8 rd, u8 data4) { fatalerror("RRC.w %s, %d", m_regnames16[rd], data4); }
void xa_cpu_device::rrc_byte_rd_data4(u8 rd, u8 data4) { fatalerror("RRC.b %s, %d", m_regnames8[rd], data4); }

//RL Rd, #data4               Rotate left reg by the 4-bit imm value                                  2 a*        1101 S011  dddd iiii
void xa_cpu_device::rl_word_rd_data4(u8 rd, u8 data4) { fatalerror("RL.w %d, %d", m_regnames16[rd], data4); }
void xa_cpu_device::rl_byte_rd_data4(u8 rd, u8 data4) { fatalerror("RL.b %d, %d", m_regnames8[rd], data4); }

//RLC Rd, #data4              Rotate left reg though carry by the 4-bit imm value                     2 a*        1101 S111  dddd iiii
void xa_cpu_device::rlc_word_rd_data4(u8 rd, u8 data4) { fatalerror( "RLC.w Rd, %d", m_regnames16[rd], data4); }
void xa_cpu_device::rlc_byte_rd_data4(u8 rd, u8 data4) { fatalerror( "RLC.b Rd, %d", m_regnames8[rd], data4); }

// FCALL addr24                Far call (full 24-bit address space)                                    4 12/8(PZ)  1100 0100  aaaa aaaa  AAAA AAAA  AAAA AAAA
void xa_cpu_device::fcall_addr24(u32 addr24) { fatalerror("FCALL $%06x", addr24); }

// CALL [Rs]                   Subroutine call ind w/ a reg                                            2 8/5(PZ)   1100 0110  0000 0sss
void xa_cpu_device::call_indrs(u8 rs) { fatalerror("CALL [%s]", m_regnames16[rs]); }

// FJMP addr24                 Far jump (full 24-bit address space)                                    4 6         1101 0100  aaaa aaaa  AAAA AAAA  AAAA AAAA
void xa_cpu_device::fjmp_addr24(u32 addr24) { fatalerror( "FJMP $%06x", addr24); }

// JMP rel16                   Long unconditional branch                                               3 6         1101 0101  rrrr rrrr  rrrr rrrr
void xa_cpu_device::jmp_rel16(u16 rel16) { fatalerror( "JMP $%04x", expand_rel16(rel16)); }

// DJNZ direct,rel8            Decrement mem and jump if not zero                                      4 9t/5nt    1110 S010  0000 1DDD  DDDD DDDD  rrrr rrrr

void xa_cpu_device::djnz_word_direct_rel8(u16 direct, u8 rel8) { fatalerror("DJNZ.w %s, $%04x", get_directtext(direct), expand_rel8(rel8)); }
void xa_cpu_device::djnz_byte_direct_rel8(u16 direct, u8 rel8) { fatalerror("DJNZ.b %s, $%04x", get_directtext(direct), expand_rel8(rel8)); }

// CJNE Rd,direct,rel8         Compare dir byte to reg and jump if not equal                           4 10t/7nt   1110 S010  dddd 0DDD  DDDD DDDD  rrrr rrrr
void xa_cpu_device::cjne_word_rd_direct_rel8(u8 rd, u16 direct, u8 rel8) { fatalerror("CJNE.w %s, %s, $%04x", m_regnames16[rd], get_directtext(direct), expand_rel8(rel8)); }
void xa_cpu_device::cjne_byte_rd_direct_rel8(u8 rd, u16 direct, u8 rel8) { fatalerror("CJNE.b %s, %s, $%04x", m_regnames8[rd], get_directtext(direct), expand_rel8(rel8)); }

// CJNE [Rd],#data8,rel8       Compare imm word to reg-ind and jump if not equal                       4 10t/7nt   1110 0011  0ddd 1000  rrrr rrrr  iiii iiii
void xa_cpu_device::cjne_indrd_data8_rel8(u8 rd, u8 data8, u8 rel8) { fatalerror( "CJNE [%s], #$%02x, $%04x", m_regnames16[rd], data8, expand_rel8(rel8));}

// CJNE Rd,#data8,rel8         Compare imm byte to reg and jump if not equal                           4 9t/6nt    1110 0011  dddd 0000  rrrr rrrr  iiii iiii
void xa_cpu_device::cjne_rd_data8_rel8(u8 rd, u8 data8, u8 rel8) { u8 regval = get_reg8(rd); do_cjne_8_helper(regval, data8); if (!get_z_flag()) { set_pc_in_current_page(expand_rel8(rel8)); } }

// CJNE [Rd],#data16,rel8      Compare imm word to reg-ind and jump if not equal                       5 10t/7nt   1110 1011  0ddd 1000  rrrr rrrr  iiii iiii  iiii iiii
void xa_cpu_device::cjne_indrd_data16_rel8(u8 rd, u16 data16, u8 rel8) { fatalerror( "CJNE [%s], #$%04x, $%04x", m_regnames16[rd], data16, expand_rel8(rel8));}

// CJNE Rd,#data16,rel8        Compare imm word to reg and jump if not equal                           5 9t/6nt    1110 1011  dddd 0000  rrrr rrrr  iiii iiii  iiii iiii
void xa_cpu_device::cjne_rd_data16_rel8(u8 rd, u16 data16, u8 rel8) { fatalerror( "CJNE %s, #$%04x, $%04x", m_regnames8[rd], data16, expand_rel8(rel8)); }

// RESET                       Causes a hardware Reset (same as external Reset)                        2 18        1101 0110  0001 0000
void xa_cpu_device::reset() { fatalerror( "RESET"); }

// TRAP #data4                 Causes 1 of 16 hardware traps to be executed                            2 23/19(PZ) 1101 0110  0011 tttt
void xa_cpu_device::trap_data4(u8 data4) { fatalerror( "TRAP %d", data4); }

// JMP [A+DPTR]                Jump ind relative to the DPTR                                           2 5         1101 0110  0100 0110
void xa_cpu_device::jmp_ind_adptr() { fatalerror( "JMP [A+DPTR]"); }

// JMP [[Rs+]]                 Jump double-ind to the address (pointer to a pointer)                   2 8         1101 0110  0110 0sss
void xa_cpu_device::jmp_dblindrs(u8 rs) { fatalerror( "JMP [[%s+]]", m_regnames16[rs]); }

// JMP [Rs]                    Jump ind to the address in the reg (64K)                                2 7         1101 0110  0111 0sss
void xa_cpu_device::jmp_indrs(u8 rs) { fatalerror( "JMP [%s]", m_regnames16[rs]); }

// RET                         Return from subroutine                                                  2 8/6(PZ)   1101 0110  1000 0000
void xa_cpu_device::ret() { fatalerror( "RET"); }

// RETI                        Return from interrupt                                                   2 10/8(PZ)  1101 0110  1001 0000
void xa_cpu_device::reti() { fatalerror( "RETI"); }

// JZ rel8                     Jump if accumulator equals zero                                         2 6t/3nt    1110 1100  rrrr rrrr
void xa_cpu_device::jz_rel8(u8 rel8) { fatalerror( "JZ $%04x", expand_rel8(rel8)); }

// JNZ rel8                    Jump if accumulator not equal zero                                      2 6t/3nt    1110 1110  rrrr rrrr
void xa_cpu_device::jnz_rel8(u8 rel8) { fatalerror( "JNZ $%04x", expand_rel8(rel8)); }

// PUSH Rlist                  Push regs (b/w) onto the current stack                                  2 b*        0H00 S111  LLLL LLLL
void xa_cpu_device::push_word_rlist(u8 bitfield, int h) { fatalerror("PUSH.w %s", get_word_reglist(bitfield)); }
void xa_cpu_device::push_byte_rlist(u8 bitfield, int h) { push_byte_reglist(bitfield, h, false); }

// PUSHU Rlist                 Push regs (b/w) from the user stack                                     2 b*        0H01 S111  LLLL LLLL
void xa_cpu_device::pushu_word_rlist(u8 bitfield, int h) { fatalerror("PUSHU.w %s", get_word_reglist(bitfield)); }
void xa_cpu_device::pushu_byte_rlist(u8 bitfield, int h) { push_byte_reglist(bitfield, h, true); }

// POP Rlist                   Pop regs (b/w) from the current stack                                   2 c*        0H10 S111  LLLL LLLL
void xa_cpu_device::pop_word_rlist(u8 bitfield, int h) { fatalerror("POP.w %s", get_word_reglist(bitfield)); }
void xa_cpu_device::pop_byte_rlist(u8 bitfield, int h) { fatalerror("POP.b %s", get_byte_reglist(bitfield, h)); }

// POPU Rlist                  Pop regs (b/w) from the user stack                                      2 c*        0H11 S111  LLLL LLLL
void xa_cpu_device::popu_word_rlist(u8 bitfield, int h) { fatalerror("POPU.w %s", get_word_reglist(bitfield)); }
void xa_cpu_device::popu_byte_rlist(u8 bitfield, int h) { fatalerror("POPU.b %s", get_byte_reglist(bitfield, h)); }
