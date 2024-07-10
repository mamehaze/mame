// license:BSD-3-Clause
// copyright-holders:David Haywood

#ifndef MAME_CPU_XA_XA_H
#define MAME_CPU_XA_XA_H

#pragma once

#define XA_EXECUTE_PARAMS uint8_t op
#define XA_EXECUTE_CALL_PARAMS op

enum {
	XA_BANK0_R0,
	XA_BANK0_R1,
	XA_BANK0_R2,
	XA_BANK0_R3,

	XA_BANK1_R0,
	XA_BANK1_R1,
	XA_BANK1_R2,
	XA_BANK1_R3,

	XA_BANK2_R0,
	XA_BANK2_R1,
	XA_BANK2_R2,
	XA_BANK2_R3,

	XA_BANK3_R0,
	XA_BANK3_R1,
	XA_BANK3_R2,
	XA_BANK3_R3,

	XA_R4,
	XA_R5,
	XA_R6,
	XA_R7,
};


class xa_cpu_device : public cpu_device
{
public:
	xa_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	xa_cpu_device(const machine_config& mconfig, device_type type, const char* tag, device_t* owner, uint32_t clock, address_map_constructor prg_map);

	virtual void device_start() override;
	virtual void device_reset() override;

	virtual uint32_t execute_min_cycles() const noexcept override { return 1; }
	virtual uint32_t execute_max_cycles() const noexcept override { return 5; }
	virtual uint32_t execute_input_lines() const noexcept override { return 0; }
	virtual void execute_run() override;

	virtual space_config_vector memory_space_config() const override;

	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;

private:
	void internal_map(address_map &map);
	void data_map(address_map &map);
	void sfr_map(address_map &map);

	address_space_config m_program_config;
	address_space_config m_data_config;
	address_space_config m_sfr_config;

	struct mem_info {
		int addr;
		const char *name;
	};


	static const mem_info default_names[];
	void add_names(const mem_info *info);

	std::string get_data_address(u16 arg) const;

	typedef void (xa_cpu_device::*op_func) (XA_EXECUTE_PARAMS);
	static const op_func s_instruction[256];

	const char* m_regnames16[16] = { "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "illegal", "illegal", "illegal", "illegal", "illegal", "illegal", "illegal", "illegal" };
	const char* m_regnames8[16] = { "R0L", "R0H", "R1L", "R1H", "R2L", "R2H", "R3L", "R3H", "R4L", "R4H", "R5L", "R5H", "R6L", "R6H", "R7L", "R7H"};
	const char* m_aluops[16] = { "ADD", "ADDC", "SUB", "SUBC", "CMP", "AND", "OR", "XOR", "MOV", "illegal ALU 0x09", "illegal ALU 0x0a", "illegal ALU 0x0b", "illegal ALU 0x0c", "illegal ALU 0x0d", "illegal ALU 0x0e", "illegal ALU 0x0f"};
	const char* m_branches[15] = { "BCC", "BCS", "BNE", "BEQ", "BNV", "BOV", "BPL", "BMI", "BG", "BL", "BGE", "BLT", "BGT", "BLE", "BR" };
	const char* m_addsmovs[2] = { "ADDS", "MOVS" };
	const char* m_pushpull[4] = { "PUSH", "PUSHU", "POP", "POPU" };
	const char* m_shifts[3] = { "ASL", "ASR", "LSR" };
	const char* m_dwparamsizes[4] = { ".b", "invalid", ".w", ".dw" };

	std::string get_bittext(int bit);
	std::string get_directtext(int bit);
	std::string show_expanded_data4(u16 data4, int size);

	u8 sfr_WDCON_r();

	void do_nz_flags_16(u16 data);
	void do_nz_flags_8(u8 data);

	uint8_t get_n_flag() { return m_nflag; }
	void set_n_flag() { m_nflag = 1; }
	void clear_n_flag() { m_nflag = 0; }

	uint8_t get_z_flag() { return m_zflag; }
	void set_z_flag()  { m_zflag = 1; }
	void clear_z_flag() { m_zflag = 0; }

	uint8_t get_c_flag() { return m_cflag; }
	void set_c_flag()  { m_cflag = 1; }
	void clear_c_flag() { m_cflag = 0; }


	uint8_t get_reg8(int reg);
	void set_reg8(int reg, u8 data);

	u16 get_reg16(int reg);
	void set_reg16(int reg, u16 data);

	void set_pc_in_current_page(u16 addr);
	void push_word_to_stack(u16 data);

	void write_data16(int address, u16 data);

	void sfr_PSWL_w(u8 data);
	void sfr_PSWH_w(u8 data);
	void sfr_PSW51_w(u8 data);

	void sfr_SCR_w(u8 data);
	void sfr_WFEED1_w(u8 data);
	void sfr_WFEED2_w(u8 data);

	void write_direct16(u16 addr, u16 data);
	u8 read_direct8(u16 addr);
	void write_direct8(u16 addr, u8 data);

	void handle_alu_type0(XA_EXECUTE_PARAMS, int alu_op);
	void handle_alu_type1(XA_EXECUTE_PARAMS, uint8_t op2);
	void handle_pushpop_rlist(XA_EXECUTE_PARAMS, int type);
	void handle_adds_movs(XA_EXECUTE_PARAMS, int which);
	void handle_shift(XA_EXECUTE_PARAMS, int shift_type);

	void d_illegal(XA_EXECUTE_PARAMS);

	void d_nop(XA_EXECUTE_PARAMS);
	void d_bitgroup(XA_EXECUTE_PARAMS);
	void d_add(XA_EXECUTE_PARAMS);
	void d_push_rlist(XA_EXECUTE_PARAMS);
	void d_addc(XA_EXECUTE_PARAMS);
	void d_pushu_rlist(XA_EXECUTE_PARAMS);
	void d_sub(XA_EXECUTE_PARAMS);
	void d_pop_rlist(XA_EXECUTE_PARAMS);
	void d_subb(XA_EXECUTE_PARAMS);
	void d_popu_rlist(XA_EXECUTE_PARAMS);
	void d_lea_offset8(XA_EXECUTE_PARAMS);
	void d_lea_offset16(XA_EXECUTE_PARAMS);
	void d_cmp(XA_EXECUTE_PARAMS);
	void d_xch_type1(XA_EXECUTE_PARAMS);
	void d_and(XA_EXECUTE_PARAMS);
	void d_xch_type2(XA_EXECUTE_PARAMS);
	void d_or(XA_EXECUTE_PARAMS);
	void d_xor(XA_EXECUTE_PARAMS);
	void d_movc_rd_rsinc(XA_EXECUTE_PARAMS);
	void d_mov(XA_EXECUTE_PARAMS);
	void d_pushpop_djnz_subgroup(XA_EXECUTE_PARAMS);
	void d_g9_subgroup(XA_EXECUTE_PARAMS);
	void d_alu(XA_EXECUTE_PARAMS);
	void d_jb_mov_subgroup(XA_EXECUTE_PARAMS);
	void d_movdir(XA_EXECUTE_PARAMS);
	void d_adds(XA_EXECUTE_PARAMS);
	void d_movx_subgroup(XA_EXECUTE_PARAMS);
	void d_rr(XA_EXECUTE_PARAMS);
	void d_movs(XA_EXECUTE_PARAMS);
	void d_rrc(XA_EXECUTE_PARAMS);
	void d_lsr_fc(XA_EXECUTE_PARAMS);
	void d_asl_c(XA_EXECUTE_PARAMS);
	void d_asr_c(XA_EXECUTE_PARAMS);
	void d_norm(XA_EXECUTE_PARAMS);
	void d_lsr_fj(XA_EXECUTE_PARAMS);
	void d_asl_j(XA_EXECUTE_PARAMS);
	void d_asr_j(XA_EXECUTE_PARAMS);
	void d_rl(XA_EXECUTE_PARAMS);
	void d_rlc(XA_EXECUTE_PARAMS);
	void d_djnz_cjne(XA_EXECUTE_PARAMS);
	void d_mulu_b(XA_EXECUTE_PARAMS);
	void d_divu_b(XA_EXECUTE_PARAMS);
	void d_mulu_w(XA_EXECUTE_PARAMS);
	void d_divu_w(XA_EXECUTE_PARAMS);
	void d_mul_w(XA_EXECUTE_PARAMS);
	void d_div_w(XA_EXECUTE_PARAMS);
	void d_div_data8(XA_EXECUTE_PARAMS);
	void d_div_d16(XA_EXECUTE_PARAMS);
	void d_divu_d(XA_EXECUTE_PARAMS);
	void d_div_d(XA_EXECUTE_PARAMS);
	void d_cjne_d8(XA_EXECUTE_PARAMS);
	void d_cjne_d16(XA_EXECUTE_PARAMS);
	void d_jz_rel8(XA_EXECUTE_PARAMS);
	void d_jnz_rel8(XA_EXECUTE_PARAMS);
	void d_branch(XA_EXECUTE_PARAMS);
	void d_bkpt(XA_EXECUTE_PARAMS);

	void add_byte_rd_data8(u8 rd, u8 data8); // used
	void addc_byte_rd_data8(u8 rd, u8 data8);
	void sub_byte_rd_data8(u8 rd, u8 data8);
	void subc_byte_rd_data8(u8 rd, u8 data8);
	void cmp_byte_rd_data8(u8 rd, u8 data8);
	void and_byte_rd_data8(u8 rd, u8 data8);
	void or_byte_rd_data8(u8 rd, u8 data8);
	void xor_byte_rd_data8(u8 rd, u8 data8);
	void mov_byte_rd_data8(u8 rd, u8 data8);

	void add_byte_indrd_data8(u8 rd, u8 data8); // used
	void addc_byte_indrd_data8(u8 rd, u8 data8);
	void sub_byte_indrd_data8(u8 rd, u8 data8);
	void subc_byte_indrd_data8(u8 rd, u8 data8);
	void cmp_byte_indrd_data8(u8 rd, u8 data8);
	void and_byte_indrd_data8(u8 rd, u8 data8);
	void or_byte_indrd_data8(u8 rd, u8 data8);
	void xor_byte_indrd_data8(u8 rd, u8 data8);
	void mov_byte_indrd_data8(u8 rd, u8 data8);

	void add_byte_indrdinc_data8(u8 rd, u8 data8); // used
	void addc_byte_indrdinc_data8(u8 rd, u8 data8);
	void sub_byte_indrdinc_data8(u8 rd, u8 data8);
	void subc_byte_indrdinc_data8(u8 rd, u8 data8);
	void cmp_byte_indrdinc_data8(u8 rd, u8 data8);
	void and_byte_indrdinc_data8(u8 rd, u8 data8);
	void or_byte_indrdinc_data8(u8 rd, u8 data8);
	void xor_byte_indrdinc_data8(u8 rd, u8 data8);
	void mov_byte_indrdinc_data8(u8 rd, u8 data8);

	void add_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8); // used
	void addc_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8);
	void sub_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8);
	void subc_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8);
	void cmp_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8);
	void and_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8);
	void or_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8);
	void xor_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8);
	void mov_byte_indrdoff8_data8(u8 rd, u8 offset8, u8 data8);

	void add_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8); // used
	void addc_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8);
	void sub_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8);
	void subc_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8);
	void cmp_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8);
	void and_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8);
	void or_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8);
	void xor_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8);
	void mov_byte_indrdoff16_data8(u8 rd, u16 offset16, u8 data8);

	void add_byte_direct_data8(u16 direct, u8 data8); // used
	void addc_byte_direct_data8(u16 direct, u8 data8);
	void sub_byte_direct_data8(u16 direct, u8 data8);
	void subc_byte_direct_data8(u16 direct, u8 data8);
	void cmp_byte_direct_data8(u16 direct, u8 data8);
	void and_byte_direct_data8(u16 direct, u8 data8);
	void or_byte_direct_data8(u16 direct, u8 data8);
	void xor_byte_direct_data8(u16 direct, u8 data8);
	void mov_byte_direct_data8(u16 direct, u8 data8);



	void add_word_rd_data16(u8 rd, u16 data16); // used
	void addc_word_rd_data16(u8 rd, u16 data16);
	void sub_word_rd_data16(u8 rd, u16 data16);
	void subc_word_rd_data16(u8 rd, u16 data16);
	void cmp_word_rd_data16(u8 rd, u16 data16);
	void and_word_rd_data16(u8 rd, u16 data16);
	void or_word_rd_data16(u8 rd, u16 data16);
	void xor_word_rd_data16(u8 rd, u16 data16);
	void mov_word_rd_data16(u8 rd, u16 data16);

	void add_word_indrd_data16(u8 rd, u16 data16); // used
	void addc_word_indrd_data16(u8 rd, u16 data16);
	void sub_word_indrd_data16(u8 rd, u16 data16);
	void subc_word_indrd_data16(u8 rd, u16 data16);
	void cmp_word_indrd_data16(u8 rd, u16 data16);
	void and_word_indrd_data16(u8 rd, u16 data16);
	void or_word_indrd_data16(u8 rd, u16 data16);
	void xor_word_indrd_data16(u8 rd, u16 data16);
	void mov_word_indrd_data16(u8 rd, u16 data16);

	void add_word_indrdinc_data16(u8 rd, u16 data16); // used
	void addc_word_indrdinc_data16(u8 rd, u16 data16);
	void sub_word_indrdinc_data16(u8 rd, u16 data16);
	void subc_word_indrdinc_data16(u8 rd, u16 data16);
	void cmp_word_indrdinc_data16(u8 rd, u16 data16);
	void and_word_indrdinc_data16(u8 rd, u16 data16);
	void or_word_indrdinc_data16(u8 rd, u16 data16);
	void xor_word_indrdinc_data16(u8 rd, u16 data16);
	void mov_word_indrdinc_data16(u8 rd, u16 data16);

	void add_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16); // used
	void addc_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16);
	void sub_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16);
	void subc_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16);
	void cmp_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16);
	void and_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16);
	void or_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16);
	void xor_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16);
	void mov_word_indrdoff8_data16(u8 rd, u8 offset8, u16 data16);

	void add_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16); // used
	void addc_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16);
	void sub_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16);
	void subc_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16);
	void cmp_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16);
	void and_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16);
	void or_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16);
	void xor_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16);
	void mov_word_indrdoff16_data16(u8 rd, u16 offset16, u16 data16);

	void add_word_direct_data16(u16 direct, u16 data16); // uused
	void addc_word_direct_data16(u16 direct, u16 data16);
	void sub_word_direct_data16(u16 direct, u16 data16);
	void subc_word_direct_data16(u16 direct, u16 data16);
	void cmp_word_direct_data16(u16 direct, u16 data16);
	void and_word_direct_data16(u16 direct, u16 data16);
	void or_word_direct_data16(u16 direct, u16 data16);
	void xor_word_direct_data16(u16 direct, u16 data16);
	void mov_word_direct_data16(u16 direct, u16 data16);

	std::unordered_map<offs_t, const char *> m_names;


	uint8_t m_im;
	uint8_t m_rs;
	uint8_t m_zflag;
	uint8_t m_nflag;
	uint8_t m_vflag;
	uint8_t m_cflag;
	uint8_t m_ac_flag;
	uint8_t m_sm_flag;
	uint8_t m_tm_flag;
	uint8_t m_p_flag;
	uint8_t m_f0_flag;
	uint8_t m_f1_flag;

	uint32_t m_pc;

	bool m_usermode;

	uint16_t m_USP; // user stack pointer
	uint16_t m_SSP; // system stack pointer

	uint8_t m_WDCON;
	uint8_t m_SCR;

	// 16-bit regs R0-R3 can have 4 selectable banks, R4-R7 are global
	// for 8-bit use each register can be seen as High and Low parts
	// R4 when split is R4H and R4L, R4L acts as ACC from i8051 for compatibility, R4H acts as B
	// R6H is DPH, R6L is DPL
	uint16_t m_regs[(4 * 4) + 4];

	address_space *m_program;
	address_space *m_data;
	address_space *m_sfr;
	int m_icount;
};

class mx10exa_cpu_device : public xa_cpu_device
{
public:
	mx10exa_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

private:
	void mx10exa_internal_map(address_map &map);

};


DECLARE_DEVICE_TYPE(XA, xa_cpu_device)
DECLARE_DEVICE_TYPE(MX10EXA, mx10exa_cpu_device)

#endif // MAME_CPU_XA_XA_H
