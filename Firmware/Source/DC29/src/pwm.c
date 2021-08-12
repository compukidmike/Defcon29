/*
 * pwm.c
 *
 *  Author: compukidmike
 */ 

#include <stdint.h>
#include <asf.h>
#include "pwm.h"

struct tcc_module tcc0_instance;
struct tcc_module tcc1_instance;
struct tcc_module tcc2_instance;
struct tc_module tc3_instance;
struct tc_module tc4_instance;
struct tc_module tc5_instance;

uint8_t ledvalues[12] = {0};

extern bool USBPower;

void pwm_init(void){
	struct tcc_config config_tcc0;
	tcc_get_config_defaults(&config_tcc0, TCC0);
	config_tcc0.counter.period = 256;
	config_tcc0.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	config_tcc0.compare.match[0] = 0; //LED1R
	config_tcc0.pins.enable_wave_out_pin[4] = true;
	config_tcc0.pins.wave_out_pin[4] = PIN_PA22F_TCC0_WO4;
	config_tcc0.pins.wave_out_pin_mux[4] = PINMUX_PA22F_TCC0_WO4;
	config_tcc0.wave.wave_polarity[0] = TCC_WAVE_POLARITY_1;
	config_tcc0.compare.match[1] = 0; //LED2R
	config_tcc0.pins.enable_wave_out_pin[5] = true;
	config_tcc0.pins.wave_out_pin[5] = PIN_PA23F_TCC0_WO5;
	config_tcc0.pins.wave_out_pin_mux[5] = PINMUX_PA23F_TCC0_WO5;
	config_tcc0.wave.wave_polarity[1] = TCC_WAVE_POLARITY_1;
	config_tcc0.compare.match[2] = 0; //LED3R
	config_tcc0.pins.enable_wave_out_pin[6] = true;
	config_tcc0.pins.wave_out_pin[6] = PIN_PA20F_TCC0_WO6;
	config_tcc0.pins.wave_out_pin_mux[6] = PINMUX_PA20F_TCC0_WO6;
	config_tcc0.wave.wave_polarity[2] = TCC_WAVE_POLARITY_1;
	config_tcc0.compare.match[3] = 0; //LED4R
	config_tcc0.pins.enable_wave_out_pin[7] = true;
	config_tcc0.pins.wave_out_pin[7] = PIN_PA21F_TCC0_WO7;
	config_tcc0.pins.wave_out_pin_mux[7] = PINMUX_PA21F_TCC0_WO7;
	config_tcc0.wave.wave_polarity[3] = TCC_WAVE_POLARITY_1;
	config_tcc0.run_in_standby = true;
	config_tcc0.counter.clock_source = GCLK_GENERATOR_0;
	tcc_init(&tcc0_instance, TCC0, &config_tcc0);
	tcc_enable(&tcc0_instance);
	
	struct tcc_config config_tcc1;
	tcc_get_config_defaults(&config_tcc1, TCC1);
	config_tcc1.counter.period = 256;
	config_tcc1.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	config_tcc1.compare.match[0] = 0; //LED1G
	config_tcc1.pins.enable_wave_out_pin[0] = true;
	config_tcc1.pins.wave_out_pin[0] = PIN_PA10E_TCC1_WO0;
	config_tcc1.pins.wave_out_pin_mux[0] = PINMUX_PA10E_TCC1_WO0;
	config_tcc1.wave.wave_polarity[0] = TCC_WAVE_POLARITY_1;
	config_tcc1.compare.match[1] = 0; //LED2G
	config_tcc1.pins.enable_wave_out_pin[1] = true;
	config_tcc1.pins.wave_out_pin[1] = PIN_PA11E_TCC1_WO1;
	config_tcc1.pins.wave_out_pin_mux[1] = PINMUX_PA11E_TCC1_WO1;
	config_tcc1.wave.wave_polarity[1] = TCC_WAVE_POLARITY_1;
	config_tcc1.run_in_standby = true;
	config_tcc1.counter.clock_source = GCLK_GENERATOR_0;
	tcc_init(&tcc1_instance, TCC1, &config_tcc1);
	tcc_enable(&tcc1_instance);

	struct tcc_config config_tcc2;
	tcc_get_config_defaults(&config_tcc2, TCC2);
	config_tcc2.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV256;
	config_tcc2.counter.period = 256;
	config_tcc2.compare.wave_generation = TCC_WAVE_GENERATION_MATCH_FREQ;
	config_tcc2.compare.match[0] = 0; //BUZZER
	config_tcc2.pins.enable_wave_out_pin[0] = true;
	config_tcc2.pins.wave_out_pin[0] = PIN_PA00E_TCC2_WO0;
	config_tcc2.pins.wave_out_pin_mux[0] = PINMUX_PA00E_TCC2_WO0;
	config_tcc2.run_in_standby = true;
	config_tcc2.counter.clock_source = GCLK_GENERATOR_3;
	//config_tcc2.double_buffering_enabled = false;
	tcc_init(&tcc2_instance, TCC2, &config_tcc2);
	tcc_enable(&tcc2_instance);
	tcc_stop_counter(&tcc2_instance);
	
	struct tc_config config_tc3;
	tc_get_config_defaults(&config_tc3);
	config_tc3.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc3.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM_MODE;
	config_tc3.clock_source = GCLK_GENERATOR_0;
	config_tc3.counter_8_bit.period = 255;
	config_tc3.waveform_invert_output = 3; //invert both channels
	config_tc3.pwm_channel[0].enabled = true; //LED3G
	config_tc3.pwm_channel[0].pin_out = PIN_PA18E_TC3_WO0;
	config_tc3.pwm_channel[0].pin_mux = PINMUX_PA18E_TC3_WO0;
	config_tc3.pwm_channel[1].enabled = true; //LED4G
	config_tc3.pwm_channel[1].pin_out = PIN_PA19E_TC3_WO1;
	config_tc3.pwm_channel[1].pin_mux = PINMUX_PA19E_TC3_WO1;
	config_tc3.run_in_standby = true;
	tc_init(&tc3_instance, TC3, &config_tc3);
	tc_enable(&tc3_instance);
	
	struct tc_config config_tc4;
	tc_get_config_defaults(&config_tc4);
	config_tc4.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc4.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM_MODE;
	config_tc4.clock_source = GCLK_GENERATOR_0;
	config_tc4.counter_8_bit.period = 255;
	config_tc4.waveform_invert_output = 3; //invert both channels
	config_tc4.pwm_channel[0].enabled = true; //LED1B
	config_tc4.pwm_channel[0].pin_out = PIN_PB08E_TC4_WO0;
	config_tc4.pwm_channel[0].pin_mux = PINMUX_PB08E_TC4_WO0;
	config_tc4.pwm_channel[1].enabled = true; //LED2B
	config_tc4.pwm_channel[1].pin_out = PIN_PB09E_TC4_WO1;
	config_tc4.pwm_channel[1].pin_mux = PINMUX_PB09E_TC4_WO1;
	config_tc4.run_in_standby = true;
	tc_init(&tc4_instance, TC4, &config_tc4);
	tc_enable(&tc4_instance);
	
	struct tc_config config_tc5;
	tc_get_config_defaults(&config_tc5);
	config_tc5.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc5.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM_MODE;
	config_tc5.clock_source = GCLK_GENERATOR_0;
	config_tc5.counter_8_bit.period = 255;
	config_tc5.waveform_invert_output = 3; //invert both channels
	config_tc5.pwm_channel[0].enabled = true; //LED3B
	config_tc5.pwm_channel[0].pin_out = PIN_PB10E_TC5_WO0;
	config_tc5.pwm_channel[0].pin_mux = PINMUX_PB10E_TC5_WO0;
	config_tc5.pwm_channel[1].enabled = true; //LED4B
	config_tc5.pwm_channel[1].pin_out = PIN_PB11E_TC5_WO1;
	config_tc5.pwm_channel[1].pin_mux = PINMUX_PB11E_TC5_WO1;
	config_tc5.run_in_standby = true;
	tc_init(&tc5_instance, TC5, &config_tc5);
	tc_enable(&tc5_instance);
}

void led_set_brightness(leds led, uint8_t brightness){
	if(!USBPower){
		brightness = brightness/5; //20% brightness to save power
	}
	ledvalues[led] = brightness;
	switch(led){
		case LED1R:
			tcc_set_compare_value(&tcc0_instance, 0, brightness);
			break;
		case LED1G:
			tcc_set_compare_value(&tcc1_instance, 0, brightness);
			break;
		case LED1B:
			tc_set_compare_value(&tc4_instance, 0, brightness);
			break;
		case LED2R:
			tcc_set_compare_value(&tcc0_instance, 1, brightness);
			break;
		case LED2G:
			tcc_set_compare_value(&tcc1_instance, 1, brightness);
			break;
		case LED2B:
			tc_set_compare_value(&tc4_instance, 1, brightness);
			break;
		case LED3R:
			tcc_set_compare_value(&tcc0_instance, 2, brightness);
			break;
		case LED3G:
			tc_set_compare_value(&tc3_instance, 0, brightness);
			break;
		case LED3B:
			tc_set_compare_value(&tc5_instance, 0, brightness);
			break;
		case LED4R:
			tcc_set_compare_value(&tcc0_instance, 3, brightness);
			break;
		case LED4G:
			tc_set_compare_value(&tc3_instance, 1, brightness);
			break;
		case LED4B:
			tc_set_compare_value(&tc5_instance, 1, brightness);
			break;
		default:
			break;
	}
}

void led_on(leds led){
	led_set_brightness(led, 255);
	ledvalues[led] = 255;
}

void led_off(leds led){
	led_set_brightness(led, 0);
	ledvalues[led] = 0;
}

void led_toggle(leds led){
	if(ledvalues[led] > 0){
		ledvalues[led] = 0;
		led_set_brightness(led, 0);
	} else {
		ledvalues[led] = 255;
		led_set_brightness(led, 255);
	}
}

void led_set_color(uint8_t led, uint8_t color[3]){
	ledvalues[((led-1)*3)] = color[0];
	led_set_brightness(((led-1)*3), color[0]);
	ledvalues[((led-1)*3)+1] = color[1];
	led_set_brightness((((led-1)*3)+1), color[1]);
	ledvalues[((led-1)*3)+2] = color[2];
	led_set_brightness((((led-1)*3)+2), color[2]);
}

void buzzer_on(void){
	tcc_set_compare_value(&tcc2_instance, 0, 64);

}

void buzzer_off(void){
	tcc_set_compare_value(&tcc2_instance, 0, 0);
}

void buzzer_set_value(uint8_t value){
	tcc_set_compare_value(&tcc2_instance, 0, value);
}
