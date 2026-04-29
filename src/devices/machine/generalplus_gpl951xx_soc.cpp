// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "generalplus_gpl951xx_soc.h"


void generalplus_gpl951xx_device::ramwrite_w(offs_t offset, uint16_t data)
{
	m_mainram[offset] = data;
}

uint16_t generalplus_gpl951xx_device::ramread_r(offs_t offset)
{
	return m_mainram[offset];
}

uint16_t generalplus_gpl951xx_device::spi_direct_7b40_r()
{
	return 0xffff; // doesn't care for now
}

uint16_t generalplus_gpl951xx_device::spi_direct_79f5_r()
{
	return 0xffff; // hangs if returning 0
}

uint16_t generalplus_gpl951xx_device::spi_direct_7b46_r()
{
	int i = machine().rand();

	if (i & 1) return 0x01;
	else return 0x02;
}

uint16_t generalplus_gpl951xx_device::spi_direct_79f4_r()
{
	// status bits?
	return machine().rand();
}


uint16_t generalplus_gpl951xx_device::spi_direct_7af0_r()
{
	return m_7af0;
}

void generalplus_gpl951xx_device::spi_direct_7af0_w(uint16_t data)
{
	// words read from ROM are written here during the checksum routine in RAM, and must
	// be shifted for the checksum to pass.
	m_7af0 = data >> 8;
}


uint16_t generalplus_gpl951xx_device::spi_direct_78e8_r()
{
	return machine().rand();
}

void generalplus_gpl951xx_device::device_start()
{
	sunplus_gcm394_base_device::device_start();
	save_item(NAME(m_7af0));
}

void generalplus_gpl951xx_device::device_reset()
{
	sunplus_gcm394_base_device::device_reset();
	m_7af0 = 0;
}

void generalplus_gpl951xx_device::spi_direct_78e8_w(uint16_t data)
{
	logerror("%s: spi_direct_78e8_w %04x\n", machine().describe_context(), data);
}

void generalplus_gpl951xx_device::gpspi_direct_internal_map(address_map& map)
{
	map(0x000000, 0x0027ff).ram().share("mainram");

	map(0x007000, 0x007007).rw(m_spg_video, FUNC(gcm394_base_video_device::tmap2_regs_r), FUNC(gcm394_base_video_device::tmap2_regs_w)); // 7000 - Tx3_X_Position

	map(0x007010, 0x007015).rw(m_spg_video, FUNC(gcm394_base_video_device::tmap0_regs_r), FUNC(gcm394_base_video_device::tmap0_regs_w));
	map(0x007016, 0x00701b).rw(m_spg_video, FUNC(gcm394_base_video_device::tmap1_regs_r), FUNC(gcm394_base_video_device::tmap1_regs_w));
	map(0x00701c, 0x00701c).w(m_spg_video, FUNC(gcm394_base_video_device::video_701c_w)); // 701c - VComValue
	map(0x00701d, 0x00701d).w(m_spg_video, FUNC(gcm394_base_video_device::video_701d_w)); // 701d - VComOffset
	map(0x00701e, 0x00701e).w(m_spg_video, FUNC(gcm394_base_video_device::video_701e_w)); // 701e - VComStep

	map(0x007020, 0x007020).rw(m_spg_video, FUNC(gcm394_base_video_device::tmap0_tilebase_lsb_r), FUNC(gcm394_base_video_device::tmap0_tilebase_lsb_w));           // 7020 - Segment_Tx1
	map(0x007021, 0x007021).rw(m_spg_video, FUNC(gcm394_base_video_device::tmap1_tilebase_lsb_r), FUNC(gcm394_base_video_device::tmap1_tilebase_lsb_w));           // 7021 - Segment_Tx2
	map(0x007022, 0x007022).rw(m_spg_video, FUNC(gcm394_base_video_device::sprite_7022_gfxbase_lsb_r), FUNC(gcm394_base_video_device::sprite_7022_gfxbase_lsb_w)); // 7022 - Segment_sp
	map(0x007023, 0x007023).rw(m_spg_video, FUNC(gcm394_base_video_device::tmap2_tilebase_lsb_r), FUNC(gcm394_base_video_device::tmap2_tilebase_lsb_w));           // 7023 - Segment_Tx3

	//
	// 
	// 
	// 
	//
	map(0x00702a, 0x00702a).w(m_spg_video, FUNC(gcm394_base_video_device::video_702a_w)); // 702a - Blending
	map(0x00702b, 0x00702b).rw(m_spg_video, FUNC(gcm394_base_video_device::tmap0_tilebase_msb_r), FUNC(gcm394_base_video_device::tmap0_tilebase_msb_w));           // 702b - Segment_Tx1H
	map(0x00702c, 0x00702c).rw(m_spg_video, FUNC(gcm394_base_video_device::tmap1_tilebase_msb_r), FUNC(gcm394_base_video_device::tmap1_tilebase_msb_w));           // 702c - Segment_Tx2H
	map(0x00702d, 0x00702d).rw(m_spg_video, FUNC(gcm394_base_video_device::sprite_702d_gfxbase_msb_r), FUNC(gcm394_base_video_device::sprite_702d_gfxbase_msb_w)); // 702d - Segment_spH
	map(0x00702e, 0x00702e).rw(m_spg_video, FUNC(gcm394_base_video_device::tmap2_tilebase_msb_r), FUNC(gcm394_base_video_device::tmap2_tilebase_msb_w));           // 702e - Segment_Tx3H
		
	//
	map(0x007030, 0x007030).rw(m_spg_video, FUNC(gcm394_base_video_device::video_7030_brightness_r), FUNC(gcm394_base_video_device::video_7030_brightness_w)); // 7030 - Fade_Control
	//
	map(0x00703a, 0x00703a).rw(m_spg_video, FUNC(gcm394_base_video_device::video_703a_palettebank_r), FUNC(gcm394_base_video_device::video_703a_palettebank_w)); // 703a - Palette_Control
	//
	map(0x007042, 0x007042).rw(m_spg_video, FUNC(gcm394_base_video_device::sprite_7042_extra_r), FUNC(gcm394_base_video_device::sprite_7042_extra_w)); // 7042 - SControl
	//
	// 7050 - TFT_Ctrl
	// 7051 - TFT_V_Width
	// 7052 - TFT_VSync_Setup
	// 7053 - TFT_V_Start
	// 7054 - TFT_V_End
	// 7055 - TFT_H_Width
	// 7056 - TFT_HSync_Setup
	// 7057 - TFT_H_Start
	// 7058 - TFT_H_End
	// 7059 - TFT_RGB_Ctrl
	// 705a - TFT_Status
	// 705b - TFT_MemMode_WCmd
	// 705c - TFT_MemMode_RCmd
	//
	// 705e - STN_PIC_SEG
	// 705f - STN_Ctrl1
	//
	map(0x007062, 0x007062).rw(m_spg_video, FUNC(gcm394_base_video_device::videoirq_source_enable_r), FUNC(gcm394_base_video_device::videoirq_source_enable_w));             // 7062 - TFT_INT_EN
	map(0x007063, 0x007063).rw(m_spg_video, FUNC(gcm394_base_video_device::video_7063_videoirq_source_r), FUNC(gcm394_base_video_device::video_7063_videoirq_source_ack_w)); // 7063 - TFT_INT_CLR
	// 7064 - US_Ctrl
	// 7065 - US_Hscaling
	// 7066 - US_Vscaling
	// 7067 - US_Width
	// 7068 - US_Height
	// 7069 - US_Hoffset
	// 706a - US_Voffset
	//
	// 706c - TFT_V_Show_Start
	// 706d - TFT_V_Show_End
	// 706e - TFT_H_Show_Start
	// 706f - TFT_H_Show_End
	//
	map(0x007070, 0x007070).w(m_spg_video, FUNC(gcm394_base_video_device::video_dma_source_w)); // 7070 - SPDMA_Source
	map(0x007071, 0x007071).w(m_spg_video, FUNC(gcm394_base_video_device::video_dma_dest_w));   // 7071 - SPDMA_Target
	map(0x007072, 0x007072).rw(m_spg_video, FUNC(gcm394_base_video_device::video_dma_size_busy_r), FUNC(gcm394_base_video_device::video_dma_size_trigger_w)); // 7072 - SPDMA_Number
	// 7073 - HB_Ctrl
	// 7074 - HB_GO
	//
	// 707d - BLD_Color
	//
	map(0x00707e, 0x00707e).w(m_spg_video, FUNC(gcm394_base_video_device::video_707e_spritebank_w));    // 707e - PPU_RAM_BANK
	map(0x00707f, 0x00707f).rw(m_spg_video, FUNC(gcm394_base_video_device::video_707f_r), FUNC(gcm394_base_video_device::video_707f_w));// 707f - PPU_Enable
	//
	// 7080 - STN_SEG
	// 7081 - STN_COM
	// 7082 - STN_PIC_COM
	// 7083 - STN_CPWait
	// 7084 - STN_Ctrl2
	// 7085 - STN_GTG_SEG
	// 7086 - STN_GTG_COM
	//
	// 70b4 - Tx1_N_PTRH
	// 70b5 - Tx1_A_PTRH
	// 70b6 - Tx2_N_PTRH
	// 70b7 - Tx2_A_PTRH
	// 70b8 - Tx3_N_PTRH
	// 70b9 - Tx3_A_PTRH
	//
	// 70db - Free_Height
	// 70dc - Free_Width
	//
	map(0x0070e0, 0x0070e0).r(m_spg_video, FUNC(gcm394_base_video_device::video_70e0_prng_r)); // 70e0 - Random0 (15-bit)
	// 70e1 - Random1 (15-bit)
	//
	map(0x007100, 0x0071ff).ram().share("rowscroll"); // 7100 to 71ff - Tx_Hvoffset
	map(0x007200, 0x0072ff).ram().share("rowzoom"); // 7200 to 72ff - HCMValue
	map(0x007300, 0x0073ff).rw(m_spg_video, FUNC(gcm394_base_video_device::palette_r), FUNC(gcm394_base_video_device::palette_w)); // 7300 to 73ff - Palette (banked)
	map(0x007400, 0x0077ff).rw(m_spg_video, FUNC(gcm394_base_video_device::spriteram_r), FUNC(gcm394_base_video_device::spriteram_w)); // 7400 to 74ff - Sprite Ram (banked)
	//
	// 7800 - BodyID
	// 7801 - unused
	// 7802 - PwrKey_State
	// 7803 - SYS_CTRL
	// 7804 - CLK_Ctrl0
	// 7805 - CLK_Ctrl1
	// 7806 - Reset_Flag
	// 7807 - Clock_Ctrl
	// 7808 - LVR_Ctrl
	// 7809 - PM_Ctrl
	// 780a - Watchdog_Ctrl
	map(0x00780b, 0x00780b).nopw(); // Watchdog_Clear
	// 780c - WAIT
	// 780d - HALT
	// 780e - unused
	// 780f - Power_State
	// 7810 - BankSwitch
	// 7811 - unused
	// 7812 - unused
	// 7813 - unused
	// 7814 - unused
	// 7815 - unused
	// 7816 - unused
	// 7817 - PLL_Sel
	// 7818 - PLLWaitCLK
	// 7819 - Cache_Ctrl
	// 781a - Cache_HitRate
	// 781b - unused
	// 781c - unused
	// 781d - unused
	// 781e - unused
	// 781f - SYS_Misc

	// 7825 - Unexpect_Flag

	// 7830 - CHECKSUM0_LB
	// 7831 - CHECKSUM1_LB
	// 7832 - CHECKSUM0_HB
	// 7833 - CHECKSUM1_HB
	//
	// 7848 - ECC_LPRL_HB
	// 7849 - ECC_LPRH_HB
	// 784a - ECC_CPR_HB
	// 784b - ECC_LPR_CKL_HB
	// 784c - ECC_LPR_CKH_HB
	// 784d - ECC_CPCKR_HB
	// 784e - ECC_ERR0_HB
	// 784f - ECC_ERR1_HB

	// 7850 - NF_Ctrl
	// 7851 - NF_CMD
	// 7852 - NF_AddrL
	// 7853 - NF_AddrH
	// 7854 - NF_Data
	// 7855 - NF_INT_Ctrl
	// 7856 - unused            or BCH_Control
	// 7857 - ECC_Ctrl
	// 7858 - ECC_LPRL_LB     or BCH_Error
	// 7859 - ECC_LPRH_LB     or BCH_Parity0
	// 785a - ECC_CPR_LB      or BCH_Parity1
	// 785b - ECC_LPR_CKL_LB  or BCH_Parity2
	// 785c - ECC_LPR_CKH_LB  or BCH_Parity3
	// 785d - ECC_CPCKR_LB    or BCH_Parity4
	// 785e - ECC_ERR0_LB     or BCH_Parity5
	// 785f - ECC_ERR1_LB     or BCH_Parity6

	map(0x007860, 0x007860).rw(FUNC(generalplus_gpl951xx_device::ioarea_7860_porta_r), FUNC(generalplus_gpl951xx_device::ioarea_7860_porta_w));                     // 7860 - IOA_Data
	map(0x007861, 0x007861).rw(FUNC(generalplus_gpl951xx_device::ioarea_7861_porta_buffer_r), FUNC(generalplus_gpl951xx_device::ioarea_7861_porta_buffer_w));       // 7861 - IOA_Buffer
	map(0x007862, 0x007862).rw(FUNC(generalplus_gpl951xx_device::ioarea_7862_porta_direction_r), FUNC(generalplus_gpl951xx_device::ioarea_7862_porta_direction_w)); // 7862 - IOA_Dir
	map(0x007863, 0x007863).rw(FUNC(generalplus_gpl951xx_device::ioarea_7863_porta_attribute_r), FUNC(generalplus_gpl951xx_device::ioarea_7863_porta_attribute_w)); // 7863 - IOA_Attrib
	// 7864 - IOA_Drv
	// 7865 - IOA_Mux
	// 7866 - IOA_Latch
	// 7867 - IOA_KeyEN

	map(0x007868, 0x007868).rw(FUNC(generalplus_gpl951xx_device::ioarea_7868_portb_r), FUNC(generalplus_gpl951xx_device::ioarea_7868_portb_w));                     // 7868 - IOB_Data
	map(0x007869, 0x007869).rw(FUNC(generalplus_gpl951xx_device::ioarea_7869_portb_buffer_r), FUNC(generalplus_gpl951xx_device::ioarea_7869_portb_buffer_w));       // 7869 - IOB_Buffer
	map(0x00786a, 0x00786a).rw(FUNC(generalplus_gpl951xx_device::ioarea_786a_portb_direction_r), FUNC(generalplus_gpl951xx_device::ioarea_786a_portb_direction_w)); // 786a - IOB_Dir
	map(0x00786b, 0x00786b).rw(FUNC(generalplus_gpl951xx_device::ioarea_786b_portb_attribute_r), FUNC(generalplus_gpl951xx_device::ioarea_786b_portb_attribute_w)); // 786b - IOB_Attrib
	// 786c - IOB_Drv
	// 786d - IOB_Mux
	// 786e - IOB_Latch
	// 786f - IOB_KeyEN

	map(0x007870, 0x007870).rw(FUNC(generalplus_gpl951xx_device::ioarea_7870_portc_r) ,FUNC(generalplus_gpl951xx_device::ioarea_7870_portc_w));                     // 7870 - IOC_Data
	map(0x007871, 0x007871).rw(FUNC(generalplus_gpl951xx_device::ioarea_7871_portc_buffer_r), FUNC(generalplus_gpl951xx_device::ioarea_7871_portc_buffer_w));       // 7871 - IOC_Buffer
	map(0x007872, 0x007872).rw(FUNC(generalplus_gpl951xx_device::ioarea_7872_portc_direction_r), FUNC(generalplus_gpl951xx_device::ioarea_7872_portc_direction_w)); // 7872 - IOC_Dir
	map(0x007873, 0x007873).rw(FUNC(generalplus_gpl951xx_device::ioarea_7873_portc_attribute_r), FUNC(generalplus_gpl951xx_device::ioarea_7873_portc_attribute_w)); // 7873 - IOC_Attrib	
	// 7874 - IOC_Drv
	// 7875 - IOC_Mux
	// 7876 - IOC_Latch
	// 7877 - IOC_KeyEN

	map(0x007878, 0x007878).rw(FUNC(generalplus_gpl951xx_device::ioarea_7878_portd_r) ,FUNC(generalplus_gpl951xx_device::ioarea_7878_portd_w));                     // 7878 - IOD_Data
	map(0x007879, 0x007879).rw(FUNC(generalplus_gpl951xx_device::ioarea_7879_portd_buffer_r), FUNC(generalplus_gpl951xx_device::ioarea_7879_portd_buffer_w));       // 7879 - IOD_Buffer
	map(0x00787a, 0x00787a).rw(FUNC(generalplus_gpl951xx_device::ioarea_787a_portd_direction_r), FUNC(generalplus_gpl951xx_device::ioarea_787a_portd_direction_w)); // 787a - IOD_Dir
	map(0x00787b, 0x00787b).rw(FUNC(generalplus_gpl951xx_device::ioarea_787b_portd_attribute_r), FUNC(generalplus_gpl951xx_device::ioarea_787b_portd_attribute_w)); // 787b - IOD_Attrib
	// 787c - IOD_Drv
	// 787d - IOD_Mux

	// 7880 - IOE_Data
	// 7881 - IOE_Buffer
	// 7882 - IOE_Dir
	// 7883 - IOE_Attrib
	// 7884 - IOE_Drv
	// 7885 - IOE_Mux
	// 7886 - IOE_Latch
	// 7877 - IOE_KeyEN

	// 7888 - IOF_Data
	// 7889 - IOF_Buffer
	// 788a - IOF_Dir
	// 788b - IOF_Attrib
	// 788c - IOF_Drv
	// 788d - IOF_Mux
	// 788e - IOF_Latch
	// 788f - IOF_KeyEN

	map(0x0078a0, 0x0078a0).rw(FUNC(generalplus_gpl951xx_device::unkarea_78a0_r), FUNC(generalplus_gpl951xx_device::unkarea_78a0_w)); // 78a0 - INT_Status1
	map(0x0078a1, 0x0078a1).r(FUNC(generalplus_gpl951xx_device::unkarea_78a1_r)); // 78a1 - INT_Status2
	// 78a2 - INT_Status3
	// 78a3 - INT_Priority1
	map(0x0078a4, 0x0078a4).w(FUNC(generalplus_gpl951xx_device::unkarea_78a4_w)); // 78a4 - INT_Priority2
	map(0x0078a5, 0x0078a5).w(FUNC(generalplus_gpl951xx_device::unkarea_78a5_w)); // 78a5 - INT_Priority3
	map(0x0078a6, 0x0078a6).w(FUNC(generalplus_gpl951xx_device::unkarea_78a6_w)); // 78a6 - MINT_Ctrl
	// 78a7 - IOAB_KCIEN
	map(0x0078a8, 0x0078a8).w(FUNC(generalplus_gpl951xx_device::unkarea_78a8_w)); // 78a8 - IOC_KCIEN
	// 78a9 - IOE_KCIEN
	// 78aa - IOF_KCIEN
	// 78ab - IOAB_KCIFC
	// 78ac - IOC_ KCIFC
	// 78ad - IOE_ KCIFC
	// 78ae - IOF_ KCIFC

	map(0x0078b0, 0x0078b0).w(FUNC(generalplus_gpl951xx_device::unkarea_78b0_w)); // 78b0 - TimeBaseA_Ctrl
	map(0x0078b1, 0x0078b1).w(FUNC(generalplus_gpl951xx_device::unkarea_78b1_w)); // 78b1 - TimeBaseB_Ctrl
	map(0x0078b2, 0x0078b2).rw(FUNC(generalplus_gpl951xx_device::unkarea_78b2_r), FUNC(generalplus_gpl951xx_device::unkarea_78b2_w)); // 78b2 - TimeBaseC_Ctrl

	map(0x0078b8, 0x0078b8).w(FUNC(generalplus_gpl951xx_device::unkarea_78b8_w)); // 78b8 - TimeBase_Reset

	// 78c0 - I2C_Ctrl
	// 78c1 - I2C_Status
	// 78c2 - I2C_Address
	// 78c3 - I2C_Data
	// 78c4 - I2C_Debounce
	// 78c5 - I2C_Clk
	// 78c6 - I2C_MISC

	// 78e0 - TimerG_Ctrl
	// 78e1
	// 78e2 - TimerG_Preload
	// 78e3
	// 78e4 - TimerG_UpCount
	// 78e5
	// 78e6
	// 78e7
	map(0x0078e8, 0x0078e8).rw(FUNC(generalplus_gpl951xx_device::spi_direct_78e8_r), FUNC(generalplus_gpl951xx_device::spi_direct_78e8_w)); // TimerH_Ctrl
	// 78e9
	// 78ea - TimerH_Preload
	// 78eb
	// 78ec - TimerH_UpCount
	// 78ed
	// 78ee
	// 78ef

	// 78f0 - CHA_Ctrl
	// 78f1 - CHA_Data
	// 78f2 - CHA_FIFO
	// 78f3
	// 78f4
	// 78f5
	// 78f6
	// 78f7
	// 78f8 - CHB_Ctrl
	// 78f9 - CHB_Data
	// 78fa - CHB_FIFO
	// 78fb
	// 78fc
	// 78fd
	// 78fe
	// 78ff

	// 7900 - UART_Data
	// 7901 - UART_RXStatus
	// 7902 - UAR_Ctrl
	// 7903 - UART_BaudRate
	// 7904 - UART_Status
	// 7905 - UART_FIFO
	// 7906 - UART_TXDelay

	// 7920 - SPI1_Ctrl
	// 7921 - SPI1_TXStatus
	// 7922 - SPI1_TXData
	// 7923 - SPI1_RXStatus
	// 7924 - SPI1_RXData
	// 7925 - SPI1_Misc

	// 7940 - SPI0_Ctrl
	// 7941 - SPI0_TXStatus
	// 7942 - SPI0_TXData
	// 7943 - SPI0_RXStatus
	// 7944 - SPI0_RXData
	// 7945 - SPI0_Misc

	// 79a0 - ADC_Setup
	// 79a1 - MADC_Ctrl
	// 79a2 - MADC_Data
	// 79a3 - ASADC_Ctrl
	// 79a4 - ASDAC_Data
	// 79a5
	// 79a6 - ADC_LineCH_En
	// 79a7 - ADC_SH_Wait

	// 79b0 - MICADC_Setup
	// 79b1 - MICGAIN_Ctrl
	// 79b2
	// 79b3 - ASMICADC_Ctrl
	// 79b4 - ASMICDAC_Data
	// 79b5 - MICAGC_UpThres
	// 79b6
	// 79b7 - MICADC_SH_WAIT
	// 79b8 - MICADC_DataMAX
	// 79b9 - MICADC_DataMIN
	// 79ba - MICADC_FLAG
	// 79bb - MICADC_GAIN
	// 79bc - MICAGC_Ctrl
	// 79bd - MICAGC_Time
	// 79be - MICAGC_Enable
	// 79bf - MICAGC_Status

	// 79f0 - RTC_Ctrl
	// 79f1 - RTC_Addr
	// 79f2 - RTC_WriteData
	// 79f3 - RTC_Request
	map(0x0079f4, 0x0079f4).r(FUNC(generalplus_gpl951xx_device::spi_direct_79f4_r)); // RTC_Ready
	map(0x0079f5, 0x0079f5).r(FUNC(generalplus_gpl951xx_device::spi_direct_79f5_r)); // RTC_ReadData
	// 79f6
	// 79f7
	// 79f8
	// 79fa
	// 79fb - RTC_ClkDiv

	// 7a00 - TimerA_Ctr
	// 7a01 - TimerA_CCPB_Ctrl
	// 7a02 - TimerA_Preload
	// 7a03 - TimerA_CCPB_Reg
	// 7a04 - TimerA_UpCount

	// 7a08 - TimerB_Ctrl
	// 7a09 - TimerB_CCPB_Ctrl
	// 7a0a - TimerB_Preload
	// 7a0b - TimerB_CCPB_Reg
	// 7a0c - TimerB_UpCount

	// 7a10 - TimerC_Ctrl
	// 7a11 - TimerC_CCPB_Ctrl
	// 7a12 - TimerC_Preload
	// 7a13 - TimerC_CCPB_Reg
	// 7a14 - TimerC_UpCount

	// 7a18 - TimerD_Ctrl
	// 7a19 - TimerD_CCPB_Ctrl
	// 7a1a - TimerD_Preload
	// 7a1b - TimerD_CCPB_Reg
	// 7a1c - TimerD_UpCount

	// 7a20 - TimerE_Ctrl
	// 7a21 - TimerF_Ctrl
	// 7a22 - TimerE_CCPB_Ctrl
	// 7a23 - TimerF_CCPB_Ctrl
	// 7a24 - TimerE_Preload
	// 7a25 - TimerF_Preload
	// 7a26 - TimerEF_CCPB4_Reg
	// 7a27 - TimerEF_CCPB5_Reg
	// 7a28 - TimerEF_CCPB6_Reg
	// 7a29 - TimerEF_CCPB7_Reg
	// 7a2a - TimerE_UpCount
	// 7a2b - TimerF_UpCount
	// 7a2c - TimerEF_CCPB_Se

	// 7a40 - USBD_Config
	// 7a41 - USBD_Function
	// 7a42 - USBD_PMR
	// 7a43 - USBD_EP0Data
	// 7a44 - USBD_BIData
	// 7a45 - USBD_BOData
	// 7a46 - USBD_INTINData
	// 7a47 - USBD_EPEvent
	// 7a48 - USBD_GLOINT
	// 7a49 - USBD_INTEN
	// 7a4a - USBD_INT
	// 7a4b - USBD_SCI NTEN
	// 7a4c - USBD_SCINT
	// 7a4d - USBD_EPAutoSet
	// 7a4e - USBD_EPSetStall
	// 7a4f - USBD_EPBufClear
	// 7a50 - USBD_EPEvntClear
	// 7a51 - USBD_EP0WrtCount
	// 7a52 - USBD_BOWrtCount
	// 7a53 - USBD_EP0BufPointer
	// 7a54 - USBD_BIBufPointer
	// 7a55 - USBD_BOBufPointer
	// 7a56 - USBD_EP0RTR
	// 7a57 - USBD_EP0RR
	// 7a58 - USBD_ EP0VR
	// 7a59 - USBD_ EP0IR
	// 7a5a - USBD_ EP0LR
	//
	// 7a60 - USBD_DMAWrtCountL
	// 7a61 - USBD_DMAWrtCountH
	// 7a62 - USBD_DMAAckL
	// 7a63 - USBD_DMAAckH
	// 7a64 - USBD_EPStall
	//
	// 7a67 - USBD_Device
	// 7a68 - USBD_NullPkt
	// 7a69 - USBD_DMAINT
	//
	// 7a6c - USBD_INTF

	map(0x007a80, 0x007a87).rw(FUNC(generalplus_gpl951xx_device::system_dma_params_channel0_r), FUNC(generalplus_gpl951xx_device::system_dma_params_channel0_w));
	map(0x007a88, 0x007a8f).rw(FUNC(generalplus_gpl951xx_device::system_dma_params_channel1_r), FUNC(generalplus_gpl951xx_device::system_dma_params_channel1_w));
	//
	// 7ab0 - DMA_SPRISize0
	// 7ab1 - DMA_SPRISize1
	//
	// 7abd - DMA_LineLength
	map(0x007abe, 0x007abe).rw(FUNC(generalplus_gpl951xx_device::system_dma_memtype_r), FUNC(generalplus_gpl951xx_device::system_dma_memtype_w)); // 7abe - DMA_SS
	map(0x007abf, 0x007abf).rw(FUNC(generalplus_gpl951xx_device::system_dma_status_r), FUNC(generalplus_gpl951xx_device::system_dma_7abf_unk_w)); // 7abf - DMA_INT
	//
	// 7ac0 - CTS_Ctrl1
	// 7ac1 - CTS_CH
	// 7ac2 - CTS_DIV
	// 7ac3 - CTS_CYCLE
	// 7ac4 - CTS_Ctrl2
	// 7ac5 - CTS_Status
	// 7ac6 - CTS_Ctrl3
	//
	// 7ac8 - CTS_FIFOLevel
	// 7ac9 - CTS_CNT

	map(0x007af0, 0x007af0).rw(FUNC(generalplus_gpl951xx_device::spi_direct_7af0_r), FUNC(generalplus_gpl951xx_device::spi_direct_7af0_w)); // Byte_Swap
	// 7af1 - Nibble_Swap
	// 7af2 - TwoBit_Swap
	// 7af3 - Bit_Reverse

	// 7b20 - KS_Ctrl1
	// 7b21 - KS_Ctrl2
	//
	// 7b28 - KS_Data0
	// 7b29 - KS_Data1
	// 7b2a - KS_Data2
	// 7b2b - KS_Data3
	// 7b2c - KS_Data4
	// 7b2d - KS_Data5
	// 7b2e - KS_Data6
	// 7b2f - KS_Data7
	// 7b30 - KS_Data8
	// 7b31 - KS_Data9
	// 7b32 - KS_Data10

	//map(0x007b40, 0x007b40).r(FUNC(generalplus_gpl951xx_device::spi_direct_7b40_r)).nopw();; // SPIFC_Ctrl1
	//map(0x007b41, 0x007b41).nopw(); // SPIFC_CMD
	//map(0x007b42, 0x007b42).nopw(); // SPIFC_PARA
	// 7b43 - SPIFC_ADDRL
	// 7b44 - SPIFC_ADDRH
	// 7b45 - SPIFC_TX_Dat
	map(0x007b46, 0x007b46).ram(); // SPIFC_RX_Data - values must be written and read from here, but is there any transformation?
	//map(0x007b47, 0x007b47).nopw(); // SPIFC_TX_BC
	//map(0x007b48, 0x007b48).nopw(); // SPIFC_RX_BC
	//map(0x007b49, 0x007b49).ram(); // SPIFC_TIMING

	// 7b4b - SPIFC_Ctrl2

	map(0x007b80, 0x007bbf).rw(m_spg_audio, FUNC(sunplus_gcm394_audio_device::control_r), FUNC(sunplus_gcm394_audio_device::control_w));
	map(0x007c00, 0x007dff).rw(m_spg_audio, FUNC(sunplus_gcm394_audio_device::audio_r), FUNC(sunplus_gcm394_audio_device::audio_w));
	map(0x007e00, 0x007fff).rw(m_spg_audio, FUNC(sunplus_gcm394_audio_device::audio_phase_r), FUNC(sunplus_gcm394_audio_device::audio_phase_w));

	// 8000 - 8fff internal boot ROM (same on all devices of the same type, not OTP)

	map(0x009000, 0x3fffff).rom().region("spidirect", 0);
}


DEFINE_DEVICE_TYPE(GPL951XX, generalplus_gpl951xx_device, "gpl951xx", "GeneralPlus GPL951xx")

generalplus_gpl951xx_device::generalplus_gpl951xx_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	sunplus_gcm394_base_device(mconfig, GPL951XX, tag, owner, clock, address_map_constructor(FUNC(generalplus_gpl951xx_device::gpspi_direct_internal_map), this))
{
}

