/***************************************************************************//**
 *   @file   main.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "main.h"
#include "config.h"
#include "ad9361_api.h"
#include "parameters.h"
#include "platform.h"
#ifdef CONSOLE_COMMANDS
#include "command.h"
#include "console.h"
#endif
#ifdef XILINX_PLATFORM
#include <xil_cache.h>
#endif
#if defined XILINX_PLATFORM || defined LINUX_PLATFORM
#include "adc_core.h"
#include "dac_core.h"
#endif

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
#ifdef CONSOLE_COMMANDS
extern command cmd_list[];
extern char cmd_no;
extern cmd_function cmd_functions[11];
unsigned char cmd = 0;
double param[5] = {0, 0, 0, 0, 0};
char param_no = 0;
int cmd_type = -1;
char invalid_cmd = 0;
char received_cmd[30] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

AD9361_InitParam default_init_param = {
/* Identification number */
		0,		//id_no;
		/* Reference Clock */
		40000000UL,	//reference_clk_rate
		/* Base Configuration */
		0,		//two_rx_two_tx_mode_enable *** adi,2rx-2tx-mode-enable
		1,		//one_rx_one_tx_mode_use_rx_num *** adi,1rx-1tx-mode-use-rx-num
		1,		//one_rx_one_tx_mode_use_tx_num *** adi,1rx-1tx-mode-use-tx-num
		1,//frequency_division_duplex_mode_enable *** adi,frequency-division-duplex-mode-enable
		0,//frequency_division_duplex_independent_mode_enable *** adi,frequency-division-duplex-independent-mode-enable
		0,//tdd_use_dual_synth_mode_enable *** adi,tdd-use-dual-synth-mode-enable
		0,		//tdd_skip_vco_cal_enable *** adi,tdd-skip-vco-cal-enable
		0,		//tx_fastlock_delay_ns *** adi,tx-fastlock-delay-ns
		0,		//rx_fastlock_delay_ns *** adi,rx-fastlock-delay-ns
		0,//rx_fastlock_pincontrol_enable *** adi,rx-fastlock-pincontrol-enable
		0,//tx_fastlock_pincontrol_enable *** adi,tx-fastlock-pincontrol-enable
		0,		//external_rx_lo_enable *** adi,external-rx-lo-enable
		0,		//external_tx_lo_enable *** adi,external-tx-lo-enable
		5,//dc_offset_tracking_update_event_mask *** adi,dc-offset-tracking-update-event-mask
		6,//dc_offset_attenuation_high_range *** adi,dc-offset-attenuation-high-range
		5,//dc_offset_attenuation_low_range *** adi,dc-offset-attenuation-low-range
		0x28,	//dc_offset_count_high_range *** adi,dc-offset-count-high-range
		0x32,	//dc_offset_count_low_range *** adi,dc-offset-count-low-range
		0,//tdd_use_fdd_vco_tables_enable *** adi,tdd-use-fdd-vco-tables-enable
		0,	//split_gain_table_mode_enable *** adi,split-gain-table-mode-enable
		MAX_SYNTH_FREF,	//trx_synthesizer_target_fref_overwrite_hz *** adi,trx-synthesizer-target-fref-overwrite-hz
		0,// qec_tracking_slow_mode_enable *** adi,qec-tracking-slow-mode-enable
		/* ENSM Control */
		0,//ensm_enable_pin_pulse_mode_enable *** adi,ensm-enable-pin-pulse-mode-enable
		0,//ensm_enable_txnrx_control_enable *** adi,ensm-enable-txnrx-control-enable
		/* LO Control */
		2450000000UL,//rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz
		2450000000UL,//tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz
		/* Rate & BW Control */
		{ 983040000, 122880000, 61440000, 30720000, 15360000, 7680000 },//uint32_t	rx_path_clock_frequencies[6] *** adi,rx-path-clock-frequencies
		{ 983040000, 122880000, 61440000, 30720000, 15360000, 7680000 },//uint32_t	tx_path_clock_frequencies[6] *** adi,tx-path-clock-frequencies
		4500000,	//rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz
		4500000,	//rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz
		/* RF Port Control */
		0,		//rx_rf_port_input_select *** adi,rx-rf-port-input-select
		0,		//tx_rf_port_input_select *** adi,tx-rf-port-input-select
		/* TX Attenuation Control */
		10000,	//tx_attenuation_mdB *** adi,tx-attenuation-mdB
		0,//update_tx_gain_in_alert_enable *** adi,update-tx-gain-in-alert-enable
		/* Reference Clock Control */
		0,//xo_disable_use_ext_refclk_enable *** adi,xo-disable-use-ext-refclk-enable
		{ 8, 5920 },//dcxo_coarse_and_fine_tune[2] *** adi,dcxo-coarse-and-fine-tune
		DAC_CLK,	//clk_output_mode_select *** adi,clk-output-mode-select
		/* Gain Control */
		2,		//gc_rx1_mode *** adi,gc-rx1-mode
		2,		//gc_rx2_mode *** adi,gc-rx2-mode
		58,	//gc_adc_large_overload_thresh *** adi,gc-adc-large-overload-thresh
		4,		//gc_adc_ovr_sample_size *** adi,gc-adc-ovr-sample-size
		47,	//gc_adc_small_overload_thresh *** adi,gc-adc-small-overload-thresh
		8192,//gc_dec_pow_measurement_duration *** adi,gc-dec-pow-measurement-duration
		0,		//gc_dig_gain_enable *** adi,gc-dig-gain-enable
		800,//gc_lmt_overload_high_thresh *** adi,gc-lmt-overload-high-thresh
		704,	//gc_lmt_overload_low_thresh *** adi,gc-lmt-overload-low-thresh
		24,		//gc_low_power_thresh *** adi,gc-low-power-thresh
		15,		//gc_max_dig_gain *** adi,gc-max-dig-gain
		/* Gain MGC Control */
		2,		//mgc_dec_gain_step *** adi,mgc-dec-gain-step
		2,		//mgc_inc_gain_step *** adi,mgc-inc-gain-step
		0,		//mgc_rx1_ctrl_inp_enable *** adi,mgc-rx1-ctrl-inp-enable
		0,		//mgc_rx2_ctrl_inp_enable *** adi,mgc-rx2-ctrl-inp-enable
		0,//mgc_split_table_ctrl_inp_gain_mode *** adi,mgc-split-table-ctrl-inp-gain-mode
		/* Gain AGC Control */
		10,	//agc_adc_large_overload_exceed_counter *** adi,agc-adc-large-overload-exceed-counter
		2,//agc_adc_large_overload_inc_steps *** adi,agc-adc-large-overload-inc-steps
		0,//agc_adc_lmt_small_overload_prevent_gain_inc_enable *** adi,agc-adc-lmt-small-overload-prevent-gain-inc-enable
		10,	//agc_adc_small_overload_exceed_counter *** adi,agc-adc-small-overload-exceed-counter
		4,		//agc_dig_gain_step_size *** adi,agc-dig-gain-step-size
		3,//agc_dig_saturation_exceed_counter *** adi,agc-dig-saturation-exceed-counter
		1000,// agc_gain_update_interval_us *** adi,agc-gain-update-interval-us
		0,//agc_immed_gain_change_if_large_adc_overload_enable *** adi,agc-immed-gain-change-if-large-adc-overload-enable
		0,//agc_immed_gain_change_if_large_lmt_overload_enable *** adi,agc-immed-gain-change-if-large-lmt-overload-enable
		10,		//agc_inner_thresh_high *** adi,agc-inner-thresh-high
		1,//agc_inner_thresh_high_dec_steps *** adi,agc-inner-thresh-high-dec-steps
		12,		//agc_inner_thresh_low *** adi,agc-inner-thresh-low
		1,//agc_inner_thresh_low_inc_steps *** adi,agc-inner-thresh-low-inc-steps
		10,	//agc_lmt_overload_large_exceed_counter *** adi,agc-lmt-overload-large-exceed-counter
		2,//agc_lmt_overload_large_inc_steps *** adi,agc-lmt-overload-large-inc-steps
		10,	//agc_lmt_overload_small_exceed_counter *** adi,agc-lmt-overload-small-exceed-counter
		5,		//agc_outer_thresh_high *** adi,agc-outer-thresh-high
		2,//agc_outer_thresh_high_dec_steps *** adi,agc-outer-thresh-high-dec-steps
		18,		//agc_outer_thresh_low *** adi,agc-outer-thresh-low
		2,//agc_outer_thresh_low_inc_steps *** adi,agc-outer-thresh-low-inc-steps
		1,//agc_attack_delay_extra_margin_us; *** adi,agc-attack-delay-extra-margin-us
		0,//agc_sync_for_gain_counter_enable *** adi,agc-sync-for-gain-counter-enable
		/* Fast AGC */
		64,	//fagc_dec_pow_measuremnt_duration ***  adi,fagc-dec-pow-measurement-duration
		260,	//fagc_state_wait_time_ns ***  adi,fagc-state-wait-time-ns
		/* Fast AGC - Low Power */
		0,//fagc_allow_agc_gain_increase ***  adi,fagc-allow-agc-gain-increase-enable
		5,//fagc_lp_thresh_increment_time ***  adi,fagc-lp-thresh-increment-time
		1,//fagc_lp_thresh_increment_steps ***  adi,fagc-lp-thresh-increment-steps
		/* Fast AGC - Lock Level */
		10,		//fagc_lock_level ***  adi,fagc-lock-level
		1,//fagc_lock_level_lmt_gain_increase_en ***  adi,fagc-lock-level-lmt-gain-increase-enable
		5,//fagc_lock_level_gain_increase_upper_limit ***  adi,fagc-lock-level-gain-increase-upper-limit
		/* Fast AGC - Peak Detectors and Final Settling */
		1,//fagc_lpf_final_settling_steps ***  adi,fagc-lpf-final-settling-steps
		1,//fagc_lmt_final_settling_steps ***  adi,fagc-lmt-final-settling-steps
		3,		//fagc_final_overrange_count ***  adi,fagc-final-overrange-count
		/* Fast AGC - Final Power Test */
		0,//fagc_gain_increase_after_gain_lock_en ***  adi,fagc-gain-increase-after-gain-lock-enable
		/* Fast AGC - Unlocking the Gain */
		0,//fagc_gain_index_type_after_exit_rx_mode ***  adi,fagc-gain-index-type-after-exit-rx-mode
		1,//fagc_use_last_lock_level_for_set_gain_en ***  adi,fagc-use-last-lock-level-for-set-gain-enable
		1,//fagc_rst_gla_stronger_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-stronger-sig-thresh-exceeded-enable
		5,		//fagc_optimized_gain_offset ***  adi,fagc-optimized-gain-offset
		10,	//fagc_rst_gla_stronger_sig_thresh_above_ll ***  adi,fagc-rst-gla-stronger-sig-thresh-above-ll
		1,//fagc_rst_gla_engergy_lost_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-exceeded-enable
		1,//fagc_rst_gla_engergy_lost_goto_optim_gain_en ***  adi,fagc-rst-gla-engergy-lost-goto-optim-gain-enable
		10,	//fagc_rst_gla_engergy_lost_sig_thresh_below_ll ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-below-ll
		8,//fagc_energy_lost_stronger_sig_gain_lock_exit_cnt ***  adi,fagc-energy-lost-stronger-sig-gain-lock-exit-cnt
		1,//fagc_rst_gla_large_adc_overload_en ***  adi,fagc-rst-gla-large-adc-overload-enable
		1,//fagc_rst_gla_large_lmt_overload_en ***  adi,fagc-rst-gla-large-lmt-overload-enable
		0,//fagc_rst_gla_en_agc_pulled_high_en ***  adi,fagc-rst-gla-en-agc-pulled-high-enable
		0,//fagc_rst_gla_if_en_agc_pulled_high_mode ***  adi,fagc-rst-gla-if-en-agc-pulled-high-mode
		64,	//fagc_power_measurement_duration_in_state5 ***  adi,fagc-power-measurement-duration-in-state5
		/* RSSI Control */
		1,		//rssi_delay *** adi,rssi-delay
		1000,	//rssi_duration *** adi,rssi-duration
		3,		//rssi_restart_mode *** adi,rssi-restart-mode
		0,//rssi_unit_is_rx_samples_enable *** adi,rssi-unit-is-rx-samples-enable
		1,		//rssi_wait *** adi,rssi-wait
		/* Aux ADC Control */
		256,	//aux_adc_decimation *** adi,aux-adc-decimation
		40000000UL,	//aux_adc_rate *** adi,aux-adc-rate
		/* AuxDAC Control */
		1,		//aux_dac_manual_mode_enable ***  adi,aux-dac-manual-mode-enable
		0,		//aux_dac1_default_value_mV ***  adi,aux-dac1-default-value-mV
		0,	//aux_dac1_active_in_rx_enable ***  adi,aux-dac1-active-in-rx-enable
		0,	//aux_dac1_active_in_tx_enable ***  adi,aux-dac1-active-in-tx-enable
		0,//aux_dac1_active_in_alert_enable ***  adi,aux-dac1-active-in-alert-enable
		0,		//aux_dac1_rx_delay_us ***  adi,aux-dac1-rx-delay-us
		0,		//aux_dac1_tx_delay_us ***  adi,aux-dac1-tx-delay-us
		0,		//aux_dac2_default_value_mV ***  adi,aux-dac2-default-value-mV
		0,	//aux_dac2_active_in_rx_enable ***  adi,aux-dac2-active-in-rx-enable
		0,	//aux_dac2_active_in_tx_enable ***  adi,aux-dac2-active-in-tx-enable
		0,//aux_dac2_active_in_alert_enable ***  adi,aux-dac2-active-in-alert-enable
		0,		//aux_dac2_rx_delay_us ***  adi,aux-dac2-rx-delay-us
		0,		//aux_dac2_tx_delay_us ***  adi,aux-dac2-tx-delay-us
		/* Temperature Sensor Control */
		256,	//temp_sense_decimation *** adi,temp-sense-decimation
		1000,//temp_sense_measurement_interval_ms *** adi,temp-sense-measurement-interval-ms
		0xCE,	//temp_sense_offset_signed *** adi,temp-sense-offset-signed
		1,//temp_sense_periodic_measurement_enable *** adi,temp-sense-periodic-measurement-enable
		/* Control Out Setup */
		0xFF,	//ctrl_outs_enable_mask *** adi,ctrl-outs-enable-mask
		0,		//ctrl_outs_index *** adi,ctrl-outs-index
		/* External LNA Control */
		0,		//elna_settling_delay_ns *** adi,elna-settling-delay-ns
		0,		//elna_gain_mdB *** adi,elna-gain-mdB
		0,		//elna_bypass_loss_mdB *** adi,elna-bypass-loss-mdB
		0,	//elna_rx1_gpo0_control_enable *** adi,elna-rx1-gpo0-control-enable
		0,	//elna_rx2_gpo1_control_enable *** adi,elna-rx2-gpo1-control-enable
		0,//elna_gaintable_all_index_enable *** adi,elna-gaintable-all-index-enable
		/* Digital Interface Control */
		2,//digital_interface_tune_skip_mode *** adi,digital-interface-tune-skip-mode
		0,//digital_interface_tune_fir_disable *** adi,digital-interface-tune-fir-disable
		1,		//pp_tx_swap_enable *** adi,pp-tx-swap-enable
		1,		//pp_rx_swap_enable *** adi,pp-rx-swap-enable
		0,		//tx_channel_swap_enable *** adi,tx-channel-swap-enable
		0,		//rx_channel_swap_enable *** adi,rx-channel-swap-enable
		1,		//rx_frame_pulse_mode_enable *** adi,rx-frame-pulse-mode-enable
		0,		//two_t_two_r_timing_enable *** adi,2t2r-timing-enable
		0,		//invert_data_bus_enable *** adi,invert-data-bus-enable
		0,		//invert_data_clk_enable *** adi,invert-data-clk-enable
		0,		//fdd_alt_word_order_enable *** adi,fdd-alt-word-order-enable
		0,		//invert_rx_frame_enable *** adi,invert-rx-frame-enable
		0,		//fdd_rx_rate_2tx_enable *** adi,fdd-rx-rate-2tx-enable
		0,		//swap_ports_enable *** adi,swap-ports-enable
		0,		//single_data_rate_enable *** adi,single-data-rate-enable
		1,		//lvds_mode_enable *** adi,lvds-mode-enable
		0,		//half_duplex_mode_enable *** adi,half-duplex-mode-enable
		0,		//single_port_mode_enable *** adi,single-port-mode-enable
		0,		//full_port_enable *** adi,full-port-enable
		0,	//full_duplex_swap_bits_enable *** adi,full-duplex-swap-bits-enable
		0,		//delay_rx_data *** adi,delay-rx-data
		0,		//rx_data_clock_delay *** adi,rx-data-clock-delay
		4,		//rx_data_delay *** adi,rx-data-delay
		7,		//tx_fb_clock_delay *** adi,tx-fb-clock-delay
		0,		//tx_data_delay *** adi,tx-data-delay
		150,	//lvds_bias_mV *** adi,lvds-bias-mV
		1,//lvds_rx_onchip_termination_enable *** adi,lvds-rx-onchip-termination-enable
		0,	//rx1rx2_phase_inversion_en *** adi,rx1-rx2-phase-inversion-enable
		0xFF,	//lvds_invert1_control *** adi,lvds-invert1-control
		0x0F,	//lvds_invert2_control *** adi,lvds-invert2-control
		/* GPO Control */
		0,//gpo0_inactive_state_high_enable *** adi,gpo0-inactive-state-high-enable
		0,//gpo1_inactive_state_high_enable *** adi,gpo1-inactive-state-high-enable
		0,//gpo2_inactive_state_high_enable *** adi,gpo2-inactive-state-high-enable
		0,//gpo3_inactive_state_high_enable *** adi,gpo3-inactive-state-high-enable
		0,		//gpo0_slave_rx_enable *** adi,gpo0-slave-rx-enable
		0,		//gpo0_slave_tx_enable *** adi,gpo0-slave-tx-enable
		0,		//gpo1_slave_rx_enable *** adi,gpo1-slave-rx-enable
		0,		//gpo1_slave_tx_enable *** adi,gpo1-slave-tx-enable
		0,		//gpo2_slave_rx_enable *** adi,gpo2-slave-rx-enable
		0,		//gpo2_slave_tx_enable *** adi,gpo2-slave-tx-enable
		0,		//gpo3_slave_rx_enable *** adi,gpo3-slave-rx-enable
		0,		//gpo3_slave_tx_enable *** adi,gpo3-slave-tx-enable
		0,		//gpo0_rx_delay_us *** adi,gpo0-rx-delay-us
		0,		//gpo0_tx_delay_us *** adi,gpo0-tx-delay-us
		0,		//gpo1_rx_delay_us *** adi,gpo1-rx-delay-us
		0,		//gpo1_tx_delay_us *** adi,gpo1-tx-delay-us
		0,		//gpo2_rx_delay_us *** adi,gpo2-rx-delay-us
		0,		//gpo2_tx_delay_us *** adi,gpo2-tx-delay-us
		0,		//gpo3_rx_delay_us *** adi,gpo3-rx-delay-us
		0,		//gpo3_tx_delay_us *** adi,gpo3-tx-delay-us
		/* Tx Monitor Control */
		37000,	//low_high_gain_threshold_mdB *** adi,txmon-low-high-thresh
		0,		//low_gain_dB *** adi,txmon-low-gain
		24,		//high_gain_dB *** adi,txmon-high-gain
		0,		//tx_mon_track_en *** adi,txmon-dc-tracking-enable
		0,		//one_shot_mode_en *** adi,txmon-one-shot-mode-enable
		511,	//tx_mon_delay *** adi,txmon-delay
		8192,	//tx_mon_duration *** adi,txmon-duration
		2,		//tx1_mon_front_end_gain *** adi,txmon-1-front-end-gain
		2,		//tx2_mon_front_end_gain *** adi,txmon-2-front-end-gain
		48,		//tx1_mon_lo_cm *** adi,txmon-1-lo-cm
		48,		//tx2_mon_lo_cm *** adi,txmon-2-lo-cm
		/* GPIO definitions */
		-1,		//gpio_resetb *** reset-gpios
		/* MCS Sync */
		-1,		//gpio_sync *** sync-gpios
		-1,		//gpio_cal_sw1 *** cal-sw1-gpios
		-1,		//gpio_cal_sw2 *** cal-sw2-gpios
		/* External LO clocks */
		NULL,	//(*ad9361_rfpll_ext_recalc_rate)()
		NULL,	//(*ad9361_rfpll_ext_round_rate)()
		NULL	//(*ad9361_rfpll_ext_set_rate)()
		};

#if 1

#define SAMPLING_FREQ  7680000
#define RF_BW          4500000

AD9361_TXFIRConfig tx_fir_config = { 3, // tx
		0, // tx gain
		2, // tx int
		{ -5, 0, 4, 23, 36, 40, 18, -13, -36, -26, 11, 48, 46, -2, -60, -72,
				-16, 70, 104, 43, -74, -142, -83, 71, 184, 136, -57, -229, -205,
				26, 272, 291, 25, -312, -396, -102, 342, 520, 211, -357, -664,
				-362, 349, 831, 566, -308, -1024, -842, 215, 1253, 1228, -43,
				-1539, -1796, -269, 1933, 2734, 876, -2624, -4727, -2426, 4784,
				13988, 20393, 20393, 13988, 4784, -2426, -4727, -2624, 876,
				2734, 1933, -269, -1796, -1539, -43, 1228, 1253, 215, -842,
				-1024, -308, 566, 831, 349, -362, -664, -357, 211, 520, 342,
				-102, -396, -312, 25, 291, 272, 26, -205, -229, -57, 136, 184,
				71, -83, -142, -74, 43, 104, 70, -16, -72, -60, -2, 46, 48, 11,
				-26, -36, -13, 18, 40, 36, 23, 4, 0, -5, }, 128, // tx_coef_size
		{ 983040000, 122880000, 61440000, 30720000, 15360000, 7680000 }, // tx_path_clks[6]
		4372840 // tx bandwidth
		};

AD9361_RXFIRConfig rx_fir_config = { 3, //rx
		-6, // rx_gain
		2, // rx_dec
		{ -10, -21, -21, -19, 11, 20, 28, -5, -30, -41, -5, 39, 61, 22, -46,
				-87, -46, 48, 117, 81, -44, -152, -127, 31, 189, 187, -4, -227,
				-261, -40, 262, 352, 105, -290, -460, -197, 308, 586, 323, -307,
				-732, -492, 281, 900, 717, -218, -1093, -1020, 97, 1322, 1438,
				113, -1604, -2053, -485, 1987, 3058, 1197, -2642, -5173, -3014,
				4532, 14372, 21276, 21276, 14372, 4532, -3014, -5173, -2642,
				1197, 3058, 1987, -485, -2053, -1604, 113, 1438, 1322, 97,
				-1020, -1093, -218, 717, 900, 281, -492, -732, -307, 323, 586,
				308, -197, -460, -290, 105, 352, 262, -40, -261, -227, -4, 187,
				189, 31, -127, -152, -44, 81, 117, 48, -46, -87, -46, 22, 61,
				39, -5, -41, -30, -5, 28, 20, 11, -19, -21, -21, -10, }, 128, // rx_coef_size
		{ 983040000, 122880000, 61440000, 30720000, 15360000, 7680000 }, // rx_path_clks[6]
		4694670 //rx_bandwidth
		};
#endif
#if 0

#define SAMPLING_FREQ  30720000
#define RF_BW          18000000

AD9361_TXFIRConfig tx_fir_config = { 3, // tx
		0, // tx gain
		2, // tx int
		{ -5, 0, 4, 23, 36, 39, 18, -13, -36, -26, 11, 48, 46, -2, -60, -72,
				-16, 69, 104, 43, -74, -142, -82, 71, 184, 136, -57, -228, -205,
				26, 272, 291, 25, -311, -395, -101, 342, 519, 210, -357, -663,
				-361, 349, 829, 564, -307, -1022, -841, 216, 1251, 1225, -44,
				-1536, -1793, -268, 1930, 2728, 873, -2622, -4718, -2416, 4788,
				13982, 20380, 20380, 13982, 4788, -2416, -4718, -2622, 873,
				2728, 1930, -268, -1793, -1536, -44, 1225, 1251, 216, -841,
				-1022, -307, 564, 829, 349, -361, -663, -357, 210, 519, 342,
				-101, -395, -311, 25, 291, 272, 26, -205, -228, -57, 136, 184,
				71, -82, -142, -74, 43, 104, 69, -16, -72, -60, -2, 46, 48, 11,
				-26, -36, -13, 18, 39, 36, 23, 4, 0, -5 }, 128, // tx_coef_size
		{ 983040000, 245760000, 245760000, 122880000, 61440000, 30720000 }, // tx_path_clks[6]
		19365438 // tx bandwidth
		};

AD9361_RXFIRConfig rx_fir_config = { 3, //rx
		-6, // rx_gain
		2, // rx_dec
		{ -9, -23, -20, -22, 12, 20, 29, -5, -30, -43, -6, 40, 64, 24, -47, -90,
				-49, 49, 122, 85, -44, -158, -134, 29, 196, 196, -1, -234, -274,
				-46, 270, 368, 114, -298, -481, -212, 315, 612, 344, -312, -762,
				-522, 282, 935, 757, -212, -1133, -1074, 80, 1365, 1510, 149,
				-1646, -2146, -551, 2016, 3173, 1310, -2634, -5298, -3194, 4431,
				14435, 21470, 21470, 14435, 4431, -3194, -5298, -2634, 1310,
				3173, 2016, -551, -2146, -1646, 149, 1510, 1365, 80, -1074,
				-1133, -212, 757, 935, 282, -522, -762, -312, 344, 612, 315,
				-212, -481, -298, 114, 368, 270, -46, -274, -234, -1, 196, 196,
				29, -134, -158, -44, 85, 122, 49, -49, -90, -47, 24, 64, 40, -6,
				-43, -30, -5, 29, 20, 12, -22, -20, -23, -9 }, 128, // rx_coef_size
		{ 983040000, 491520000, 245760000, 122880000, 61440000, 30720000 }, // rx_path_clks[6]
		19365514 //rx_bandwidth
		};
#endif

struct ad9361_rf_phy *ad9361_phy;
#ifdef FMCOMMS5
struct ad9361_rf_phy *ad9361_phy_b;
#endif

/***************************************************************************//**
 * @brief main
 *******************************************************************************/
int initAd9361(void) {

	int Status;
	uint64_t Value;
	uint8_t en_dis;

	/*
	 * NOTE: The user has to choose the GPIO numbers according to desired
	 * carrier board. The following configuration is valid for boards other
	 * than the Fmcomms5.
	 */
	default_init_param.gpio_resetb = GPIO_RESET_PIN;
	default_init_param.gpio_sync = -1;
	default_init_param.gpio_cal_sw1 = -1;
	default_init_param.gpio_cal_sw2 = -1;

	/*
	 * Initialize the GPIO
	 */
	gpio_init(GPIO_DEVICE_ID);
	gpio_direction(default_init_param.gpio_resetb, 1);

	/*
	 * Initialize the SPI
	 */
	spi_init(SPI_DEVICE_ID, 1, 0);

	/*
	 * Initialize AD9361
	 */
	Status = ad9361_init(&ad9361_phy, &default_init_param);
	if (Status != 0) {
		xil_printf("Could not initialize AD9361\r\n");
		xil_printf("Status\t%d\r\n", Status);
		return 1;
	}

	/*
	 * Sampling frequency
	 */
	Status = ad9361_set_tx_sampling_freq(ad9361_phy, SAMPLING_FREQ);
	if (Status != 0) {
		xil_printf("Could not set Tx sampling freq.\r\n");
		return 1;
	}
	Status = ad9361_set_rx_sampling_freq(ad9361_phy, SAMPLING_FREQ);
	if (Status != 0) {
		xil_printf("Could not set Rx sampling freq.\r\n");
		return 1;
	}
	/*
	 * Set Tx and Rx FIR
	 */
	Status = ad9361_set_tx_fir_config(ad9361_phy, tx_fir_config);
	if (Status != 0) {
		xil_printf("Could not set Tx FIR\r\n");
		return 1;
	}
	Status = ad9361_set_rx_fir_config(ad9361_phy, rx_fir_config);
	if (Status != 0) {
		xil_printf("Could not set Rx FIR\r\n");
		return 1;
	}
	// Enable both at the same time
	Status = ad9361_set_trx_fir_en_dis(ad9361_phy, 1);
	if (Status != 0) {
		xil_printf("Could not enable Tx and Rx FIR\r\n");
		return 1;
	}
	// Check status
	Status = ad9361_get_tx_fir_en_dis(ad9361_phy, &en_dis);
	if (Status == 0) {
		xil_printf("Tx FIR status\t %d\r\n", en_dis);
	} else {
		xil_printf("Could not get Tx FIR enable\r\n");
		return 1;
	}
	Status = ad9361_get_rx_fir_en_dis(ad9361_phy, &en_dis);
	if (Status == 0) {
		xil_printf("Rx FIR status\t %d\r\n", en_dis);
	} else {
		xil_printf("Could not get Rx FIR enable\r\n");
		return 1;
	}
	/*
	 * Rf bandwidth
	 */
	Status = ad9361_set_tx_rf_bandwidth(ad9361_phy, RF_BW);
	if (Status != 0) {
		xil_printf("Could not set Tx Rf bandwidth\r\n");
		return 1;
	}
	Status = ad9361_set_rx_rf_bandwidth(ad9361_phy, RF_BW);
	if (Status != 0) {
		xil_printf("Could not set Rx Rf bandwidth\r\n");
		return 1;
	}
	/*
	 * Gain control mode
	 */
	Status = ad9361_set_rx_gain_control_mode(ad9361_phy, 0,
			RF_GAIN_SLOWATTACK_AGC);
	if (Status != 0) {
		xil_printf("Could not set Rx gain control mode for channel 0\r\n");
		return 1;
	}
	/*
	 * Hardware gains
	 */
	Status = ad9361_set_rx_rf_gain(ad9361_phy, 0, -10);
	if (Status != 0) {
		xil_printf("Could not set Rx gain for channel 0\r\n");
		return 1;
	}

	Status = ad9361_get_rx_lo_freq(ad9361_phy, &Value);
	if (Status == 0) {
		xil_printf("LO Frequency\t %u \r\n", Value);
	} else {
		xil_printf("Could not get current LO frequency\r\n");
		return 1;
	}

#if ROE_CPRI_SINK == ROE_SINK_DAC
	dac_init(ad9361_phy, DATA_SEL_DMA, 1);
#endif

#ifndef AXI_ADC_NOT_PRESENT
#if defined XILINX_PLATFORM && defined CAPTURE_SCRIPT
	// NOTE: To prevent unwanted data loss, it's recommended to invalidate
	// cache after each adc_capture() call, keeping in mind that the
	// size of the capture and the start address must be alinged to the size
	// of the cache line.
	mdelay(1000);
	adc_capture(16384, ADC_DDR_BASEADDR);
	Xil_DCacheInvalidateRange(ADC_DDR_BASEADDR, 16384);
#endif
#endif

#ifdef CONSOLE_COMMANDS
	get_help(NULL, 0);

	while (1)
	{
		console_get_command(received_cmd);
		invalid_cmd = 0;
		for (cmd = 0; cmd < cmd_no; cmd++)
		{
			param_no = 0;
			cmd_type = console_check_commands(received_cmd, cmd_list[cmd].name,
					param, &param_no);
			if (cmd_type == UNKNOWN_CMD)
			{
				invalid_cmd++;
			}
			else
			{
				cmd_list[cmd].function(param, param_no);
			}
		}
		if (invalid_cmd == cmd_no)
		{
			console_print("Invalid command!\n");
		}
	}
#endif

	printf("AD9361 Initialization Done.\n");

#ifdef XILINX_PLATFORM
	Xil_DCacheDisable();
	Xil_ICacheDisable();
#endif

	return 0;
}
