/**
 * \file
 *
 * \brief Main functions for Keyboard example
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <stdbool.h>
#include "main.h"
#include "conf_usb.h"
#include "udi_hid_kbd.h"
#include "ui.h"

#include <pinmux.h>
#include <nvm.h>
#include "pwm.h"
#include <rww_eeprom.h>
#include "keys.h"
#include "serialconsole.h"
#include "games.h"



static volatile bool main_b_kbd_enable = false;

enum status_code eepromstatus;

/**
 * RTC Interrupt timing definition
 */
#define TIME_PERIOD_1MSEC 33u
#define TIME_PERIOD_500MSEC 16500u
/**
 * Variables
 */

uint32_t serialnum[4];

bool USBPower = false;


uint8_t rotor_state;
uint8_t rotor_position;
volatile uint8_t PWM_Count;
volatile uint32_t touch_time_counter = 0u;
struct rtc_module rtc_instance;

volatile uint32_t millis = 0;

volatile uint32_t buzzer_counter = 0;
volatile uint32_t buzzer_overflow = 250;
volatile bool buzzer_state = false;
volatile bool buzzer_skip = false;

bool main_b_cdc_enable = false;

uint8_t led1color[3];
uint8_t led2color[3];
uint8_t led3color[3];
uint8_t led4color[3];
uint8_t led1pressedcolor[3];
uint8_t led2pressedcolor[3];
uint8_t led3pressedcolor[3];
uint8_t led4pressedcolor[3];

volatile bool wait_for_sof = false;

volatile bool button1 = false;
volatile bool button2 = false;
volatile bool button3 = false;
volatile bool button4 = false;

#define DEBOUNCE_TIME 200
volatile uint32_t lastButton1Press = 0;
volatile uint32_t lastButton2Press = 0;
volatile uint32_t lastButton3Press = 0;
volatile uint32_t lastButton4Press = 0;

uint8_t keymap[231];
uint8_t keymaplength;
uint8_t keymapstarts[6];

uint8_t fwversion[1];

serial serialnumlist;

//Challenge challengedata;
ChallengeStruct challengedata;

//Game gamedata;
GameStruct gamedata;

GameModes gamemode;

extern struct tcc_module tcc2_instance;

volatile bool games_buzzer_off = true;
volatile uint32_t last_usb_comms = 0;

/* Macros */

/**
 * \def GET_SENSOR_STATE(SENSOR_NUMBER)
 * \brief To get the sensor state that it is in detect or not
 * \param SENSOR_NUMBER for which the state to be detected
 * \return Returns either 0 or 1
 * If the bit value is 0, it is not in detect
 * If the bit value is 1, it is in detect
 * Alternatively, the individual sensor state can be directly accessed using
 * p_qm_measure_data->p_sensor_states[(SENSOR_NUMBER/8)] variable.
 */
 #define GET_SELFCAP_SENSOR_STATE(SENSOR_NUMBER) p_selfcap_measure_data-> \
	p_sensor_states[(SENSOR_NUMBER / \
	8)] & (1 << (SENSOR_NUMBER % 8))

/**
 * \def GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER)
 * \brief To get the rotor angle or slider position.
 * These values are valid only when the sensor state for
 * corresponding rotor or slider shows in detect.
 * \param ROTOR_SLIDER_NUMBER for which the position to be known
 * \return Returns rotor angle or sensor position
 */
#define GET_SELFCAP_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) \
	p_selfcap_measure_data->p_rotor_slider_values[ \
		ROTOR_SLIDER_NUMBER]


/* Interrupt on "pin change" from push button to do wakeup on USB
 * Note:
 * This interrupt is enable when the USB host enable remote wakeup feature
 * This interrupt wakeup the CPU if this one is in idle mode
 */
/*static void ui_wakeup_handler(void)
{
	// It is a wakeup then send wakeup USB 
	udc_remotewakeup();
	//LED_On(PIN_PA19);
}*/

void vbus_handler(void){
	if(port_pin_get_input_level(USB_VBUS_PIN)){
		// Start USB stack to authorize VBus monitoring
		disable_usart_top();
		udc_start();
		USBPower = true;
		//configure_usart_top_default();
	} else {
		//disable_usart_top();
		udc_stop();
		configure_usart_top_usb();
		USBPower = false;
	}
}

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	//Get chip serial number. We only use the last 4 bytes to save EEPROM space and to make comms faster
	serialnum[0] = *(volatile uint32_t *)0x0080A040;
	serialnum[1] = *(volatile uint32_t *)0x0080A044;
	serialnum[2] = *(volatile uint32_t *)0x0080A048;
	serialnum[3] = *(volatile uint32_t *)0x0080A00C;
	
	eepromstatus = rww_eeprom_emulator_init();
	if(eepromstatus == STATUS_ERR_BAD_FORMAT){
		rww_eeprom_emulator_erase_memory();
		rww_eeprom_emulator_init();
	}

	rww_eeprom_emulator_read_buffer(EEP_FIRMWARE_VERSION, fwversion, 1);
	
	if(fwversion[0] != FIRMWARE_VERSION){ //Clear eeprom and re-init with good values
		reset_eeprom();
	}
	
	read_eeprom();
	
	
	//uint8_t button1_state;
	//uint8_t button2_state;
	uint8_t slider_state;
	uint8_t slider_position;
	uint8_t last_slider_position;
	
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	//sleepmgr_init();

#if !SAM0
	sysclk_init();
	board_init();
#else
	system_init();
#endif

	//Enable GCLK Generator 3
	SYSCTRL->VREG.bit.RUNSTDBY = 1;
	
	// Configure GCLK3's divider - in this case, no division - so just divide by one /
	GCLK->GENDIV.reg =
	GCLK_GENDIV_ID(3) |
	GCLK_GENDIV_DIV(1);

	// Setup GCLK3 using the internal 8 MHz oscillator /
	GCLK->GENCTRL.reg =
	GCLK_GENCTRL_ID(3) |
	GCLK_GENCTRL_SRC_OSC8M |
	// Improve the duty cycle. /
	GCLK_GENCTRL_IDC |
	GCLK_GENCTRL_GENEN |
	GCLK_GENCTRL_RUNSTDBY;

	// Wait for the write to complete /
	while(GCLK->STATUS.bit.SYNCBUSY) {};
		
	// Connect GCLK3 to SERCOM0 /
	GCLK->CLKCTRL.reg =
	GCLK_CLKCTRL_CLKEN |
	GCLK_CLKCTRL_GEN_GCLK3 |
	GCLK_CLKCTRL_ID_SERCOM0_CORE;

	// Wait for the write to complete. /
	while (GCLK->STATUS.bit.SYNCBUSY) {};
	
	delay_init();
	sleepmgr_init();
	
	
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	

	/* Set buttons as inputs */
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(BUTTON1, &pin_conf);
	port_pin_set_config(BUTTON2, &pin_conf);
	port_pin_set_config(BUTTON3, &pin_conf);
	port_pin_set_config(BUTTON4, &pin_conf);
	
	port_pin_set_config(MATRIX, &pin_conf);
	port_pin_set_config(MAX, &pin_conf);
	port_pin_set_config(ALIENS, &pin_conf);
	
	pin_conf.input_pull = PORT_PIN_PULL_DOWN;
	port_pin_set_config(USB_VBUS_PIN, &pin_conf);
	
	
	//Setup Pin Interrupts
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	
	//VBUS pin interrupt
	config_extint_chan.gpio_pin            = PIN_PA01A_EIC_EXTINT1;
	config_extint_chan.gpio_pin_mux        = PINMUX_PA01A_EIC_EXTINT1;
	config_extint_chan.gpio_pin_pull       = EXTINT_PULL_DOWN;
	config_extint_chan.filter_input_signal = true;
	config_extint_chan.detection_criteria  = EXTINT_DETECT_BOTH;
	extint_chan_set_config(1, &config_extint_chan);
	extint_register_callback(vbus_handler, 1, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(1,EXTINT_CALLBACK_TYPE_DETECT);

	//Button 1 interrupt
	config_extint_chan.gpio_pin            = PIN_PA04A_EIC_EXTINT4;
	config_extint_chan.gpio_pin_mux        = PINMUX_PA04A_EIC_EXTINT4;
	config_extint_chan.gpio_pin_pull       = EXTINT_PULL_UP;
	config_extint_chan.filter_input_signal = true;
	config_extint_chan.detection_criteria  = EXTINT_DETECT_FALLING;
	extint_chan_set_config(4, &config_extint_chan);
	extint_register_callback(button1_handler, 4, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(4,EXTINT_CALLBACK_TYPE_DETECT);
	
	//Button 2 interrupt
	config_extint_chan.gpio_pin            = PIN_PA05A_EIC_EXTINT5;
	config_extint_chan.gpio_pin_mux        = PINMUX_PA05A_EIC_EXTINT5;
	config_extint_chan.gpio_pin_pull       = EXTINT_PULL_UP;
	config_extint_chan.filter_input_signal = true;
	config_extint_chan.detection_criteria  = EXTINT_DETECT_FALLING;
	extint_chan_set_config(5, &config_extint_chan);
	extint_register_callback(button2_handler, 5, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(5,EXTINT_CALLBACK_TYPE_DETECT);
	
	//Button 3 interrupt
	config_extint_chan.gpio_pin            = PIN_PA06A_EIC_EXTINT6;
	config_extint_chan.gpio_pin_mux        = PINMUX_PA06A_EIC_EXTINT6;
	config_extint_chan.gpio_pin_pull       = EXTINT_PULL_UP;
	config_extint_chan.filter_input_signal = true;
	config_extint_chan.detection_criteria  = EXTINT_DETECT_FALLING;
	extint_chan_set_config(6, &config_extint_chan);
	extint_register_callback(button3_handler, 6, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(6,EXTINT_CALLBACK_TYPE_DETECT);
	
	//Button 4 interrupt
	config_extint_chan.gpio_pin            = PIN_PA07A_EIC_EXTINT7;
	config_extint_chan.gpio_pin_mux        = PINMUX_PA07A_EIC_EXTINT7;
	config_extint_chan.gpio_pin_pull       = EXTINT_PULL_UP;
	config_extint_chan.filter_input_signal = true;
	config_extint_chan.detection_criteria  = EXTINT_DETECT_FALLING;
	extint_chan_set_config(7, &config_extint_chan);
	extint_register_callback(button4_handler, 7, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(7,EXTINT_CALLBACK_TYPE_DETECT);
	
	timer_init();
	
	configure_usart();
	configure_usart_callbacks();
	
	
	if(port_pin_get_input_level(USB_VBUS_PIN)){
		// Start USB stack to authorize VBus monitoring
		//disable_usart_top();
		udc_start();
		USBPower = true;
		//configure_usart_top_default();
		} else {
		//disable_usart_top();
		//udc_stop();
		configure_usart_top_usb();
		USBPower = false;
	}
	
	GCLK->GENCTRL.bit.RUNSTDBY = 1;  //GCLK run standby
	
	touch_sensors_init();
	
	pwm_init();
	
	//Startup LED Sequence
	uint8_t delaytime = 40;
	led_on(LED1R);
	delay_cycles_ms(delaytime);
	led_on(LED2R);
	delay_cycles_ms(delaytime);
	led_on(LED3R);
	delay_cycles_ms(delaytime);
	led_on(LED4R);
	delay_cycles_ms(delaytime);
	led_off(LED1R);
	led_on(LED1G);
	delay_cycles_ms(delaytime);
	led_off(LED2R);
	led_on(LED2G);
	delay_cycles_ms(delaytime);
	led_off(LED3R);
	led_on(LED3G);
	delay_cycles_ms(delaytime);
	led_off(LED4R);
	led_on(LED4G);
	delay_cycles_ms(delaytime);
	led_off(LED1G);
	led_on(LED1B);
	delay_cycles_ms(delaytime);
	led_off(LED2G);
	led_on(LED2B);
	delay_cycles_ms(delaytime);
	led_off(LED3G);
	led_on(LED3B);
	delay_cycles_ms(delaytime);
	led_off(LED4G);
	led_on(LED4B);
	delay_cycles_ms(delaytime);
	led_off(LED1B);
	delay_cycles_ms(delaytime);
	led_off(LED2B);
	delay_cycles_ms(delaytime);
	led_off(LED3B);
	delay_cycles_ms(delaytime);
	led_off(LED4B);
	
	if(!port_pin_get_input_level(BUTTON1)){
		gamemode = SIMON_SOLO;
		led_set_color(1,LED_COLOR_OFF);
		led_set_color(2,LED_COLOR_OFF);
		led_set_color(3,LED_COLOR_OFF);
		led_set_color(4,LED_COLOR_OFF);
		while(port_pin_get_input_level(BUTTON1) == false);
		button1 = false;
	}

	uart_event = millis;
	
	led_set_color(1,led1color);
	led_set_color(2,led2color);
	led_set_color(3,led3color);
	led_set_color(4,led4color);

	
	while(1){
		//Check Buttons
		if(gamemode == IDLE){ //Game modes check the buttons themselves
			if(USBPower && ((millis - last_usb_comms) < 100)){ //Only send keys when connected to a computer
				if(button1){
					button1 = false;
					//if(main_b_kbd_enable) send_keys(1);
					send_keys(1);
				}
				if(button2){
					button2 = false;
					//if(main_b_kbd_enable) send_keys(2);
					send_keys(2);
				}
				if(button3){
					button3 = false;
					//if(main_b_kbd_enable) send_keys(3);
					send_keys(3);
				}
				if(button4){
					button4 = false;
					//if(main_b_kbd_enable) send_keys(4);
					send_keys(4);
				}
			
				//Check Touch Sensors
				touch_sensors_measure();
				if (p_selfcap_measure_data->measurement_done_touch == 1u) {

					p_selfcap_measure_data->measurement_done_touch = 0u;
					slider_state = GET_SELFCAP_SENSOR_STATE(0);
					if(slider_state)
					{
						slider_position = GET_SELFCAP_ROTOR_SLIDER_POSITION(0);
						if(slider_position > last_slider_position + 10){
							last_slider_position = slider_position;
							send_keys(6);//sliding down
						}
						if(slider_position < last_slider_position - 10){
							last_slider_position = slider_position;
							send_keys(5);//sliding up
						}
					}
				}
			}
		}
		
		//Update USB serial console if we got data
		if(main_b_cdc_enable){
			if(udi_cdc_get_nb_received_data()){
				updateSerialConsole();
			}
		}
		
		//Go to sleep to save battery
		//if (!USBPower && ((uart_event + 1000) < millis)) {
		if (!USBPower && ((millis - uart_event) > 1000) && gamemode != SIMON_SOLO) {
			standby_sleep();
		}
		
		
		//Check serial ports
		check_comms();
		
		run_games();
		
		play_sounds();
	}
}


void send_data(void){
	const uint16_t data = 1;
	uint8_t delay = 50;
	usart_write_job(&usart_top_instance, &data);
	delay_cycles_ms(delay);
	usart_write_job(&usart_right_instance, &data);
	delay_cycles_ms(delay);
	usart_write_job(&usart_bottom_instance, &data);
	delay_cycles_ms(delay);
	usart_write_job(&usart_left_instance, &data);
	delay_cycles_ms(delay);
	usart_write_job(&usart_usba_instance, &data);
	delay_cycles_ms(delay);
	usart_write_job(&usart_usbc_instance, &data);
	delay_cycles_ms(delay);
}

void main_suspend_action(void)
{
	//ui_powerdown();
}

void main_resume_action(void) 
{
	//ui_wakeup();
}

//void main_sof_action(void)
void user_callback_sof_action(void)
{
	wait_for_sof = false;
	last_usb_comms = millis;
}

void main_remotewakeup_enable(void)
{
	//ui_wakeup_enable();
}

void main_remotewakeup_disable(void)
{
	//ui_wakeup_disable();
}

//bool main_kbd_enable(void)
void user_callback_vbus_action(bool b_vbus_high)
{
	main_b_kbd_enable = b_vbus_high;
	if(main_b_kbd_enable){
		disable_usart_top();
		configure_usart_top_default();
	} else {
		disable_usart_top();
		configure_usart_top_usb();
	}
}

void main_kbd_disable(void)
{
	main_b_kbd_enable = false;
}

/*! \brief Configure the RTC timer overflow callback
 *
 */
void rtc_overflow_callback(void)
{
	millis ++;
	
	/* Do something on RTC overflow here */
	if(touch_time_counter == touch_time.measurement_period_ms)
	{
		touch_time.time_to_measure_touch = 1;
		touch_time.current_time_ms = touch_time.current_time_ms +
		touch_time.measurement_period_ms;
		touch_time_counter = 0u;
	}
	else
	{
		touch_time_counter++;
	}
	
	if(gamemode == SIMON_SOLO || gamemode == SIMON_MULTI_PRIMARY || gamemode == SIMON_MULTI_SECONDARY){
		if(!port_pin_get_input_level(BUTTON1)){
			tcc_restart_counter(&tcc2_instance);
			tcc_set_compare_value(&tcc2_instance,0,219); //415Hz
			
			led_set_color(1,LED_COLOR_GREEN);
		}
		if(!port_pin_get_input_level(BUTTON2)){
			tcc_restart_counter(&tcc2_instance);
			tcc_set_compare_value(&tcc2_instance,0,293); //310Hz
			
			led_set_color(2,LED_COLOR_RED);
		}
		if(!port_pin_get_input_level(BUTTON3)){
			tcc_restart_counter(&tcc2_instance);
			tcc_set_compare_value(&tcc2_instance,0,360); //252Hz
			
			led_set_color(3,LED_COLOR_YELLOW);
		}
		if(!port_pin_get_input_level(BUTTON4)){
			tcc_restart_counter(&tcc2_instance);
			tcc_set_compare_value(&tcc2_instance,0,435); //209Hz
			
			led_set_color(4,LED_COLOR_BLUE);
		}
	
		if(port_pin_get_input_level(BUTTON1) & port_pin_get_input_level(BUTTON2) & port_pin_get_input_level(BUTTON3) & port_pin_get_input_level(BUTTON4) & games_buzzer_off){
			//tcc_set_compare_value(&tcc2_instance,0,0); //off
			tcc_stop_counter(&tcc2_instance);
			led_set_color(1,LED_COLOR_OFF);
			led_set_color(2,LED_COLOR_OFF);
			led_set_color(3,LED_COLOR_OFF);
			led_set_color(4,LED_COLOR_OFF);
		}
	}
}

/*! \brief Configure the RTC timer callback
 *
 */
void configure_rtc_callbacks(void)
{
	/* register callback */
	rtc_count_register_callback(&rtc_instance,
			rtc_overflow_callback, RTC_COUNT_CALLBACK_OVERFLOW);
	/* Enable callback */
	rtc_count_enable_callback(&rtc_instance,RTC_COUNT_CALLBACK_OVERFLOW);
}

/*! \brief Configure the RTC timer count after which interrupts comes
 *
 */
void configure_rtc_count(void)
{
	struct rtc_count_config config_rtc_count;
	rtc_count_get_config_defaults(&config_rtc_count);

	config_rtc_count.prescaler           = RTC_COUNT_PRESCALER_DIV_1;
	config_rtc_count.mode                = RTC_COUNT_MODE_16BIT;
	config_rtc_count.continuously_update = true;
	/* initialize rtc */
	rtc_count_init(&rtc_instance,RTC,&config_rtc_count);

	/* enable rtc */
	rtc_count_enable(&rtc_instance);
}
/*! \brief Initialize timer
 *
 */
void timer_init(void)
{
	/* Configure and enable RTC */
	configure_rtc_count();

	/* Configure and enable callback */
	configure_rtc_callbacks();

	/* Set Timer Period */

	rtc_count_set_period(&rtc_instance,TIME_PERIOD_1MSEC);
}

void button1_handler(void){
	if(button1 == false){
		if((millis - lastButton1Press) > DEBOUNCE_TIME){
			lastButton1Press = millis;
			button1 = true;
			uart_event = millis;
		}
	}
}

void button2_handler(void){
	if(button2 == false){
		if((millis - lastButton2Press) > DEBOUNCE_TIME){
			lastButton2Press = millis;
			button2 = true;
		}
	}
}

void button3_handler(void){
	if(button3 == false){
		if((millis - lastButton3Press) > DEBOUNCE_TIME){
			lastButton3Press = millis;
			button3 = true;
		}
	}
}

void button4_handler(void){
	if(button4 == false){
		if((millis - lastButton4Press) > DEBOUNCE_TIME){
			lastButton4Press = millis;
			button4 = true;
		}
	}
}

bool main_cdc_enable(uint8_t port)
{
	main_b_cdc_enable = true;
	// Open communication
	//uart_open(port);
	return true;
}

void main_cdc_disable(uint8_t port)
{
	main_b_cdc_enable = false;
	// Close communication
	//uart_close(port);
}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{
	if (b_enable) {
		// Host terminal has open COM
		//ui_com_open(port);
		}else{
		// Host terminal has close COM
		//ui_com_close(port);
	}
}

void reset_eeprom(void){ //reset entire EEPROM including challenge and game data
	rww_eeprom_emulator_erase_memory();
	rww_eeprom_emulator_init();
	fwversion[0] = FIRMWARE_VERSION;
	rww_eeprom_emulator_write_buffer(EEP_FIRMWARE_VERSION, fwversion, 1);
	uint8_t eepdata[1];
	eepdata[0] = 0;
	for(int x=1; x<EEP_LED_BRIGHTNESS; x++){
		rww_eeprom_emulator_write_buffer(x, eepdata, 1);
	}
	uint8_t leddata[3];
	leddata[0] = 255; //full brightness as default
	rww_eeprom_emulator_write_buffer(EEP_LED_BRIGHTNESS, leddata, 1);
	leddata[0] = 255;
	leddata[1] = 0;
	leddata[2] = 0;
	rww_eeprom_emulator_write_buffer(EEP_LED_1_COLOR, leddata, 3);
	leddata[0] = 0;
	leddata[1] = 255;
	leddata[2] = 0;
	rww_eeprom_emulator_write_buffer(EEP_LED_2_COLOR, leddata, 3);
	leddata[0] = 0;
	leddata[1] = 0;
	leddata[2] = 255;
	rww_eeprom_emulator_write_buffer(EEP_LED_3_COLOR, leddata, 3);
	leddata[0] = 127;
	leddata[1] = 127;
	leddata[2] = 127;
	rww_eeprom_emulator_write_buffer(EEP_LED_4_COLOR, leddata, 3);
	leddata[0] = 0;
	leddata[1] = 127;
	leddata[2] = 127;
	rww_eeprom_emulator_write_buffer(EEP_LED_1_PRESSED_COLOR, leddata, 3);
	leddata[0] = 127;
	leddata[1] = 0;
	leddata[2] = 127;
	rww_eeprom_emulator_write_buffer(EEP_LED_2_PRESSED_COLOR, leddata, 3);
	leddata[0] = 127;
	leddata[1] = 127;
	leddata[2] = 0;
	rww_eeprom_emulator_write_buffer(EEP_LED_3_PRESSED_COLOR, leddata, 3);
	leddata[0] = 0;
	leddata[1] = 0;
	leddata[2] = 0;
	rww_eeprom_emulator_write_buffer(EEP_LED_4_PRESSED_COLOR, leddata, 3);
	
	rww_eeprom_emulator_write_buffer(EEP_KEY_MAP, default_keymap, sizeof(default_keymap));
	rww_eeprom_emulator_commit_page_buffer();
}

void reset_user_eeprom(void){ //Reset the LED and keymap data
	#if DEBUG > 0 //Reset badge challenge info in debug mode
	uint8_t bytes[80] = {0};
	rww_eeprom_emulator_write_buffer(EEP_CHALLENGE_CONNECTED_TYPES, bytes, 5);
	rww_eeprom_emulator_write_buffer(EEP_CHALLENGE_BADGE_SERIALS, bytes, 80);
	rww_eeprom_emulator_write_buffer(EEP_GAME_SIMON_SOLO_HIGH_SCORE, bytes, 18);
	#endif
	uint8_t leddata[3];
	leddata[0] = 255; //full brightness as default
	rww_eeprom_emulator_write_buffer(EEP_LED_BRIGHTNESS, leddata, 1);
	leddata[0] = 255;
	leddata[1] = 0;
	leddata[2] = 0;
	rww_eeprom_emulator_write_buffer(EEP_LED_1_COLOR, leddata, 3);
	leddata[0] = 0;
	leddata[1] = 255;
	leddata[2] = 0;
	rww_eeprom_emulator_write_buffer(EEP_LED_2_COLOR, leddata, 3);
	leddata[0] = 0;
	leddata[1] = 0;
	leddata[2] = 255;
	rww_eeprom_emulator_write_buffer(EEP_LED_3_COLOR, leddata, 3);
	leddata[0] = 127;
	leddata[1] = 127;
	leddata[2] = 127;
	rww_eeprom_emulator_write_buffer(EEP_LED_4_COLOR, leddata, 3);
	leddata[0] = 0;
	leddata[1] = 127;
	leddata[2] = 127;
	rww_eeprom_emulator_write_buffer(EEP_LED_1_PRESSED_COLOR, leddata, 3);
	leddata[0] = 127;
	leddata[1] = 0;
	leddata[2] = 127;
	rww_eeprom_emulator_write_buffer(EEP_LED_2_PRESSED_COLOR, leddata, 3);
	leddata[0] = 127;
	leddata[1] = 127;
	leddata[2] = 0;
	rww_eeprom_emulator_write_buffer(EEP_LED_3_PRESSED_COLOR, leddata, 3);
	leddata[0] = 0;
	leddata[1] = 0;
	leddata[2] = 0;
	rww_eeprom_emulator_write_buffer(EEP_LED_4_PRESSED_COLOR, leddata, 3);
	
	rww_eeprom_emulator_write_buffer(EEP_KEY_MAP, default_keymap, sizeof(default_keymap));
	rww_eeprom_emulator_commit_page_buffer();
	
	rww_eeprom_emulator_read_buffer(EEP_LED_1_COLOR, led1color, 3);
	led_set_color(1,led1color);
	rww_eeprom_emulator_read_buffer(EEP_LED_2_COLOR, led2color, 3);
	led_set_color(2,led2color);
	rww_eeprom_emulator_read_buffer(EEP_LED_3_COLOR, led3color, 3);
	led_set_color(3,led3color);
	rww_eeprom_emulator_read_buffer(EEP_LED_4_COLOR, led4color, 3);
	led_set_color(4,led4color);
}

void read_eeprom(void){
	uint8_t bytes[18];
	rww_eeprom_emulator_read_buffer(EEP_CHALLENGE_CONNECTED_TYPES,bytes, 5);
	challengedata.badgetypes = bytes[0];
	challengedata.numconnected = bytes[1] | (bytes[2] << 8);
	challengedata.numshared = bytes[3] | (bytes[4] << 8);
	
	rww_eeprom_emulator_read_buffer(EEP_CHALLENGE_BADGE_SERIALS,serialnumlist.bytes, 80);
	
	rww_eeprom_emulator_read_buffer(EEP_GAME_SIMON_SOLO_HIGH_SCORE,bytes, 18);
	gamedata.simon_solo_high_score = bytes[0] | (bytes[1]<<8);
	gamedata.simon_multi_high_score = bytes[2] | (bytes[3]<<8);
	gamedata.simon_multi_connections = bytes[4] | (bytes[5]<<8);
	gamedata.simon_multi_games_played = bytes[6] | (bytes[7]<<8);
	gamedata.wam_solo_high_score = bytes[8] | (bytes[9]<<8);
	gamedata.wam_multi_high_score = bytes[10] | (bytes[11]<<8);
	gamedata.wam_multi_connections = bytes[12] | (bytes[13]<<8);
	gamedata.wam_multi_games_played = bytes[14] | (bytes[15]<<8);
	gamedata.wam_multi_wins = bytes[16] | (bytes[17]<<8);
	
	rww_eeprom_emulator_read_buffer(EEP_LED_1_COLOR, led1color, 3);
	rww_eeprom_emulator_read_buffer(EEP_LED_2_COLOR, led2color, 3);
	rww_eeprom_emulator_read_buffer(EEP_LED_3_COLOR, led3color, 3);
	rww_eeprom_emulator_read_buffer(EEP_LED_4_COLOR, led4color, 3);
	rww_eeprom_emulator_read_buffer(EEP_LED_1_PRESSED_COLOR, led1pressedcolor, 3);
	rww_eeprom_emulator_read_buffer(EEP_LED_2_PRESSED_COLOR, led2pressedcolor, 3);
	rww_eeprom_emulator_read_buffer(EEP_LED_3_PRESSED_COLOR, led3pressedcolor, 3);
	rww_eeprom_emulator_read_buffer(EEP_LED_4_PRESSED_COLOR, led4pressedcolor, 3);

	get_keymap();
}



RAMFUNC void standby_sleep(void)
{
	led_set_color(1,LED_COLOR_OFF);
	led_set_color(2,LED_COLOR_OFF);
	led_set_color(3,LED_COLOR_OFF);
	led_set_color(4,LED_COLOR_OFF);
	
	while(usart_get_job_status(&usart_top_instance, USART_TRANSCEIVER_TX) == STATUS_BUSY);
	while(usart_get_job_status(&usart_right_instance, USART_TRANSCEIVER_TX) == STATUS_BUSY);
	while(usart_get_job_status(&usart_bottom_instance, USART_TRANSCEIVER_TX) == STATUS_BUSY);
	while(usart_get_job_status(&usart_left_instance, USART_TRANSCEIVER_TX) == STATUS_BUSY);
	while(usart_get_job_status(&usart_usba_instance, USART_TRANSCEIVER_TX) == STATUS_BUSY);
	while(usart_get_job_status(&usart_usbc_instance, USART_TRANSCEIVER_TX) == STATUS_BUSY);
	
	tcc_stop_counter(&tcc2_instance); //Disable buzzer

	//Slow down RTC interrupts
	rtc_count_set_period(&rtc_instance,TIME_PERIOD_500MSEC);

	sleepmgr_sleep(SLEEPMGR_STANDBY);
	
	rtc_count_set_period(&rtc_instance,TIME_PERIOD_1MSEC);
	rtc_count_set_count(&rtc_instance,0);

	millis += 500; //Account for time while sleeping
	
	send_heartbeats();
}

void usba_pin_interrupt_handler(void){
	uart_event = millis;
}

/**
 * \mainpage ASF USB Device HID Keyboard
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device HID Keyboard
 * on Atmel MCU with USB module.
 * The application note AVR4904 http://ww1.microchip.com/downloads/en/appnotes/doc8446.pdf
 * provides information about this implementation.
 *
 * \section startup Startup
 * The example uses the buttons or sensors available on the board
 * to simulate a standard keyboard.
 * After loading firmware, connect the board (EVKxx,Xplain,...) to the USB Host.
 * When connected to a USB host system this application provides a keyboard application
 * in the Unix/Mac/Windows operating systems.
 * This example uses the native HID driver for these operating systems.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and HID modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/hid/
 *   <br>services/usb/class/hid/keyboard/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 */
