/*
 * comms.c
 *
 *  Author: compukidmike
 */ 
#include <asf.h>
#include <stdio.h>
#include "comms.h"


extern bool main_b_kbd_enable;
extern volatile uint32_t millis;
extern ChallengeStruct challengedata;
extern serial serialnumlist;
extern bool main_b_cdc_enable;
extern uint32_t serialnum[4];
extern GameStruct gamedata;
extern GameModes gamemode;
extern uint8_t gamestate;
extern bool badge_count_ready;
extern uint16_t incoming_badge_number;
extern uint8_t incoming_badge_button;
extern bool incoming_button_press_ready;
extern bool incoming_sequence_packet;
extern uint8_t led1color[3];
extern uint8_t led2color[3];
extern uint8_t led3color[3];
extern uint8_t led4color[3];
extern bool USBPower;


volatile uint8_t rx_top_buffer[RX_BUFFER_LENGTH];
volatile uint8_t rx_right_buffer[RX_BUFFER_LENGTH];
volatile uint8_t rx_bottom_buffer[RX_BUFFER_LENGTH];
volatile uint8_t rx_left_buffer[RX_BUFFER_LENGTH];
volatile uint8_t rx_usba_buffer[RX_BUFFER_LENGTH];
volatile uint8_t rx_usbc_buffer[RX_BUFFER_LENGTH];

volatile uint8_t rx_top_buffer_length = 0;
volatile uint8_t rx_right_buffer_length = 0;
volatile uint8_t rx_bottom_buffer_length = 0;
volatile uint8_t rx_left_buffer_length = 0;
volatile uint8_t rx_usba_buffer_length = 0;
volatile uint8_t rx_usbc_buffer_length = 0;

volatile uint8_t rx_top_buffer_read_index = 0;
volatile uint8_t rx_right_buffer_read_index = 0;
volatile uint8_t rx_bottom_buffer_read_index = 0;
volatile uint8_t rx_left_buffer_read_index = 0;
volatile uint8_t rx_usba_buffer_read_index = 0;
volatile uint8_t rx_usbc_buffer_read_index = 0;

volatile uint8_t rx_top_buffer_write_index = 0;
volatile uint8_t rx_right_buffer_write_index = 0;
volatile uint8_t rx_bottom_buffer_write_index = 0;
volatile uint8_t rx_left_buffer_write_index = 0;
volatile uint8_t rx_usba_buffer_write_index = 0;
volatile uint8_t rx_usbc_buffer_write_index = 0;

volatile uint16_t rx_top_temp_buffer = 0;
volatile uint16_t rx_right_temp_buffer = 0;
volatile uint16_t rx_bottom_temp_buffer = 0;
volatile uint16_t rx_left_temp_buffer = 0;
volatile uint16_t rx_usba_temp_buffer = 0;
volatile uint16_t rx_usbc_temp_buffer = 0;


volatile uint16_t usart_top_last_msg_time = 0;
volatile uint16_t usart_right_last_msg_time = 0;
volatile uint16_t usart_bottom_last_msg_time = 0;
volatile uint16_t usart_left_last_msg_time = 0;
volatile uint16_t usart_usba_last_msg_time = 0;
volatile uint16_t usart_usbc_last_msg_time = 0;

volatile uint8_t usart_top_state = 0;

uint8_t rxstate[6];
bool escapechar[6];
uint8_t serialnumcounter[6];
uint32_t incmoingserialnum[6];
bool newbadgetype[6];
bool newserialnum[6];
uint32_t last_heartbeat_message_time[6];
uint32_t last_ack_message_time[6];
uint8_t missed_heartbeat_counter[6];


uint8_t heartbeat_message[5] = {29,29,29,29,0};
uint8_t ack_message[5] = {29,29,29,29,255};
uint8_t hello_message[12];
bool hello_sent[6];
uint8_t synccounter[6] = {0};
uint8_t get_badge_count_message[7] = {29,29,29,29,6,0,0};
uint8_t badge_count_response_message[7] = {29,29,29,29,7,0,0};
uint8_t incoming_byte_counter[6] = {0};
uint16_t badge_counter = 0;
uint16_t badge_counter_response = 0;
uint16_t sequence_badge_number = 0;
uint8_t simon_sequence_message[8] = {29,29,29,29,8,0,0,0};
uint8_t simon_button_message[8] = {29,29,29,29,9,0,0,0};
uint8_t incoming_simon_sequence_message[8] = {29,29,29,29,8,0,0,0};
uint8_t incoming_simon_button_message[8] = {29,29,29,29,9,0,0,0};
uint8_t simon_game_over_message[7] = {29,29,29,29,10,0,0};
uint16_t simon_multi_score;
bool isconnected[6];
uint8_t challenge_status_message[28] = {0};
uint32_t last_challenge_status_request = -2000;


//! [rx_buffer_var]

//! [callback_funcs]
void usart_top_read_callback(struct usart_module *const usart_module)
{
	usart_top_last_msg_time = millis;
	if(rx_top_buffer_length == RX_BUFFER_LENGTH){
		return;
	}
	rx_top_buffer[rx_top_buffer_write_index] = rx_top_temp_buffer;
	rx_top_buffer_write_index ++;
	if(rx_top_buffer_write_index == RX_BUFFER_LENGTH) rx_top_buffer_write_index = 0;
	rx_top_buffer_length ++;

	uint32_t try_time = millis;
	while(usart_read_job(&usart_top_instance, (uint16_t *)&rx_top_temp_buffer)){
		if(millis - try_time > 100) break;
	}
	
	uart_event = millis;
}

void usart_top_write_callback(struct usart_module *const usart_module)
{
	//port_pin_toggle_output_level(LED_0_PIN);
}

void usart_right_read_callback(struct usart_module *const usart_module)
{
	usart_right_last_msg_time = millis;
	if(rx_right_buffer_length == RX_BUFFER_LENGTH){
		return;
	}
	rx_right_buffer[rx_right_buffer_write_index] = rx_right_temp_buffer;
	rx_right_buffer_write_index ++;
	if(rx_right_buffer_write_index == RX_BUFFER_LENGTH) rx_right_buffer_write_index = 0;
	rx_right_buffer_length ++;
	uint32_t try_time = millis;
	while(usart_read_job(&usart_right_instance, (uint16_t *)&rx_right_temp_buffer)){
		if(millis - try_time > 100) break;
	}
	uart_event = millis;
}

void usart_right_write_callback(struct usart_module *const usart_module)
{
	//port_pin_toggle_output_level(LED_0_PIN);
}

void usart_bottom_read_callback(struct usart_module *const usart_module)
{
	usart_bottom_last_msg_time = millis;
	if(rx_bottom_buffer_length == RX_BUFFER_LENGTH){
		return;
	}
	rx_bottom_buffer[rx_bottom_buffer_write_index] = rx_bottom_temp_buffer;
	rx_bottom_buffer_write_index ++;
	if(rx_bottom_buffer_write_index == RX_BUFFER_LENGTH) rx_bottom_buffer_write_index = 0;
	rx_bottom_buffer_length ++;
	uint32_t try_time = millis;
	while(usart_read_job(&usart_bottom_instance, (uint16_t *)&rx_bottom_temp_buffer)){
		if(millis - try_time > 100) break;
	}
	uart_event = millis;
}

void usart_bottom_write_callback(struct usart_module *const usart_module)
{
	//port_pin_toggle_output_level(LED_0_PIN);
}

void usart_left_read_callback(struct usart_module *const usart_module)
{
	usart_left_last_msg_time = millis;
	usart_left_last_msg_time = millis;
	if(rx_left_buffer_length == RX_BUFFER_LENGTH){
		return;
	}
	rx_left_buffer[rx_left_buffer_write_index] = rx_left_temp_buffer;
	rx_left_buffer_write_index ++;
	if(rx_left_buffer_write_index == RX_BUFFER_LENGTH) rx_left_buffer_write_index = 0;
	rx_left_buffer_length ++;
	uint32_t try_time = millis;
	while(usart_read_job(&usart_left_instance, (uint16_t *)&rx_left_temp_buffer)){
		if(millis - try_time > 100) break;
	}
	uart_event = millis;
}

void usart_left_write_callback(struct usart_module *const usart_module)
{
	//port_pin_toggle_output_level(LED_0_PIN);
}

void usart_usba_read_callback(struct usart_module *const usart_module)
{
	usart_usba_last_msg_time = millis;
	usart_usba_last_msg_time = millis;
	if(rx_usba_buffer_length == RX_BUFFER_LENGTH){
		return;
	}
	rx_usba_buffer[rx_usba_buffer_write_index] = rx_usba_temp_buffer;
	rx_usba_buffer_write_index ++;
	if(rx_usba_buffer_write_index == RX_BUFFER_LENGTH) rx_usba_buffer_write_index = 0;
	rx_usba_buffer_length ++;
	uint32_t try_time = millis;
	while(usart_read_job(&usart_usba_instance, (uint16_t *)&rx_usba_temp_buffer)){
		if(millis - try_time > 100) break;
	}
	uart_event = millis;
}

void usart_usba_write_callback(struct usart_module *const usart_module)
{
	//port_pin_toggle_output_level(LED_0_PIN);
}

void usart_usbc_read_callback(struct usart_module *const usart_module)
{
	usart_usbc_last_msg_time = millis;
	usart_usbc_last_msg_time = millis;
	if(rx_usbc_buffer_length == RX_BUFFER_LENGTH){
		return;
	}
	rx_usbc_buffer[rx_usbc_buffer_write_index] = rx_usbc_temp_buffer;
	rx_usbc_buffer_write_index ++;
	if(rx_usbc_buffer_write_index == RX_BUFFER_LENGTH) rx_usbc_buffer_write_index = 0;
	rx_usbc_buffer_length ++;
	uint32_t try_time = millis;
	while(usart_read_job(&usart_usbc_instance, (uint16_t *)&rx_usbc_temp_buffer)){
		if(millis - try_time > 100) break;
	}
	uart_event = millis;
}

void usart_usbc_write_callback(struct usart_module *const usart_module)
{
	//port_pin_toggle_output_level(LED_0_PIN);
}

void usart_top_error_callback(struct usart_module *const usart_module){
	uart_event = millis;
}
void usart_right_error_callback(struct usart_module *const usart_module){
	uart_event = millis;
}
void usart_bottom_error_callback(struct usart_module *const usart_module){
	uart_event = millis;
}
void usart_left_error_callback(struct usart_module *const usart_module){
	uart_event = millis;
}
void usart_usbc_error_callback(struct usart_module *const usart_module){
	uart_event = millis;
}
void usart_usba_error_callback(struct usart_module *const usart_module){
	uart_event = millis;
}
//! [callback_funcs]

//! [setup]
void configure_usart(void)
{
	//! [setup_config]
	struct usart_config config_top_usart;
	struct usart_config config_right_usart;
	struct usart_config config_bottom_usart;
	struct usart_config config_left_usart;
	struct usart_config config_usba_usart;	
	//! [setup_config_defaults]
	usart_get_config_defaults(&config_top_usart);
	usart_get_config_defaults(&config_right_usart);
	usart_get_config_defaults(&config_bottom_usart);
	usart_get_config_defaults(&config_left_usart);
	usart_get_config_defaults(&config_usba_usart);
	//usart_get_config_defaults(&config_usbc_usart);
	//! [setup_config_defaults]

struct port_config pin_conf;
port_get_config_defaults(&pin_conf);
pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	
		//Top Connector (TX5/RX5/SERCOM5)
		config_top_usart.baudrate    = 38400; //This should be 9600, but there's some weird clock thing going on and I didn't have time to fix it. The port was going exactly 4 times too slow, so I sped it up by 4x and it works. *shrug*
		config_top_usart.mux_setting = USART_RX_3_TX_2_XCK_3;
		config_top_usart.pinmux_pad0 = PINMUX_PB22D_SERCOM5_PAD2;
		config_top_usart.pinmux_pad1 = PINMUX_PB23D_SERCOM5_PAD3;
		config_top_usart.pinmux_pad2 = PINMUX_UNUSED;
		config_top_usart.pinmux_pad3 = PINMUX_UNUSED;
		config_top_usart.run_in_standby = true;
		config_top_usart.generator_source = GCLK_GENERATOR_3;
		//! [setup_change_config]

		//! [setup_set_config]
		while (usart_init(&usart_top_instance, SERCOM5, &config_top_usart) != STATUS_OK) {
		}
		//! [setup_set_config]
		PORT->Group[1].PINCFG[23].bit.PULLEN = 1;
		PORT->Group[1].OUTCLR.reg = 1 << 23;
		//! [setup_enable]
		usart_enable(&usart_top_instance);
		//! [setup_enable]
		
		//Right Connector (TX1/RX1/SERCOM1)
		config_right_usart.baudrate    = 38400;
		config_right_usart.mux_setting = USART_RX_1_TX_0_XCK_1;
		config_right_usart.pinmux_pad0 = PINMUX_PA16C_SERCOM1_PAD0;
		config_right_usart.pinmux_pad1 = PINMUX_PA17C_SERCOM1_PAD1;
		config_right_usart.pinmux_pad2 = PINMUX_UNUSED;
		config_right_usart.pinmux_pad3 = PINMUX_UNUSED;
		config_right_usart.run_in_standby = true;
		config_right_usart.generator_source = GCLK_GENERATOR_3;
		//! [setup_change_config]

		//! [setup_set_config]
		while (usart_init(&usart_right_instance, SERCOM1, &config_right_usart) != STATUS_OK) {
		}
		//! [setup_set_config]
		PORT->Group[0].PINCFG[17].bit.PULLEN = 1;
		PORT->Group[0].OUTCLR.reg = 1 << 17;
		//! [setup_enable]
		usart_enable(&usart_right_instance);
		//! [setup_enable]
		
		//Bottom Connector (TX2/RX2/SERCOM2)
		config_bottom_usart.baudrate    = 38400;
		config_bottom_usart.mux_setting = USART_RX_1_TX_0_XCK_1;
		config_bottom_usart.pinmux_pad0 = PINMUX_PA12C_SERCOM2_PAD0;
		config_bottom_usart.pinmux_pad1 = PINMUX_PA13C_SERCOM2_PAD1;
		config_bottom_usart.pinmux_pad2 = PINMUX_UNUSED;
		config_bottom_usart.pinmux_pad3 = PINMUX_UNUSED;
		config_bottom_usart.run_in_standby = true;
		config_bottom_usart.generator_source = GCLK_GENERATOR_3;
		//! [setup_change_config]

		//! [setup_set_config]
		while (usart_init(&usart_bottom_instance, SERCOM2, &config_bottom_usart) != STATUS_OK) {
		}
		//! [setup_set_config]
		PORT->Group[0].PINCFG[13].bit.PULLEN = 1;
		PORT->Group[0].OUTCLR.reg = 1 << 13;
		//! [setup_enable]
		usart_enable(&usart_bottom_instance);
		//! [setup_enable]
		
		//Left Connector (TX0/RX0/SERCOM0)
		config_left_usart.baudrate    = 38400;
		config_left_usart.mux_setting = USART_RX_1_TX_0_XCK_1;
		config_left_usart.pinmux_pad0 = PINMUX_PA08C_SERCOM0_PAD0;
		config_left_usart.pinmux_pad1 = PINMUX_PA09C_SERCOM0_PAD1;
		config_left_usart.pinmux_pad2 = PINMUX_UNUSED;
		config_left_usart.pinmux_pad3 = PINMUX_UNUSED;
		config_left_usart.run_in_standby = true;
		config_left_usart.generator_source = GCLK_GENERATOR_3;
		//! [setup_change_config]

		//! [setup_set_config]
		while (usart_init(&usart_left_instance, SERCOM0, &config_left_usart) != STATUS_OK) {
		}
		//! [setup_set_config]
		PORT->Group[0].PINCFG[9].bit.PULLEN = 1;
		PORT->Group[0].OUTCLR.reg = 1 << 9;
		//! [setup_enable]
		usart_enable(&usart_left_instance);
		//! [setup_enable]
		
		
		//USB-A Connector (TX4/RX4/SERCOM4)
		config_usba_usart.baudrate    = 38400;
		config_usba_usart.mux_setting = USART_RX_3_TX_2_XCK_3;
		config_usba_usart.pinmux_pad0 = PINMUX_PA14D_SERCOM4_PAD2;
		config_usba_usart.pinmux_pad1 = PINMUX_PA15D_SERCOM4_PAD3;
		config_usba_usart.pinmux_pad2 = PINMUX_UNUSED;
		config_usba_usart.pinmux_pad3 = PINMUX_UNUSED;
		config_usba_usart.run_in_standby = true;
		config_usba_usart.generator_source = GCLK_GENERATOR_3;
		//! [setup_change_config]

		//! [setup_set_config]
		while (usart_init(&usart_usba_instance, SERCOM4, &config_usba_usart) != STATUS_OK) {
		}
		//! [setup_set_config]
		PORT->Group[0].PINCFG[15].bit.PULLEN = 1;
		PORT->Group[0].OUTCLR.reg = 1 << 15;
		//! [setup_enable]
		usart_enable(&usart_usba_instance);
		//! [setup_enable]
}

void configure_usart_top_default(void)
{
	
}

void configure_usart_top_usb(void)
{
	struct usart_config config_usbc_usart;
	usart_get_config_defaults(&config_usbc_usart);
	
		//Top Connector (TX3/RX3/SERCOM3)
		config_usbc_usart.baudrate    = 38400;
		config_usbc_usart.mux_setting = USART_RX_3_TX_2_XCK_3;
		config_usbc_usart.pinmux_pad0 = PINMUX_PA24C_SERCOM3_PAD2;
		config_usbc_usart.pinmux_pad1 = PINMUX_PA25C_SERCOM3_PAD3;
		config_usbc_usart.pinmux_pad2 = PINMUX_UNUSED;
		config_usbc_usart.pinmux_pad3 = PINMUX_UNUSED;
		config_usbc_usart.run_in_standby = true;
		config_usbc_usart.generator_source = GCLK_GENERATOR_3;
		//! [setup_change_config]

		//! [setup_set_config]
		while (usart_init(&usart_usbc_instance, SERCOM3, &config_usbc_usart) != STATUS_OK) {
		}
		//! [setup_set_config]
		PORT->Group[0].PINCFG[25].bit.PULLEN = 1;
		PORT->Group[0].OUTCLR.reg = 1 << 25;
		//! [setup_enable]
		usart_enable(&usart_usbc_instance);
		//! [setup_enable]
		usart_register_callback(&usart_usbc_instance, usart_usbc_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
		usart_register_callback(&usart_usbc_instance, usart_usbc_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
		usart_enable_callback(&usart_usbc_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
		usart_enable_callback(&usart_usbc_instance, USART_CALLBACK_BUFFER_RECEIVED);
		usart_read_buffer_job(&usart_usbc_instance,(uint8_t *)rx_usbc_buffer,1);
		usart_register_callback(&usart_usbc_instance, usart_usbc_error_callback, USART_CALLBACK_ERROR);
		usart_enable_callback(&usart_usbc_instance, USART_CALLBACK_ERROR);
}

void disable_usart_top(void){
	usart_disable(&usart_usbc_instance);
}

void configure_usart_callbacks(void)
{
	//! [setup_register_callbacks]
	usart_register_callback(&usart_top_instance, usart_top_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_top_instance, usart_top_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_register_callback(&usart_right_instance, usart_right_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_right_instance, usart_right_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_register_callback(&usart_bottom_instance, usart_bottom_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_bottom_instance, usart_bottom_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_register_callback(&usart_left_instance, usart_left_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_left_instance, usart_left_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_register_callback(&usart_usba_instance, usart_usba_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_usba_instance, usart_usba_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	
	usart_register_callback(&usart_top_instance, usart_usbc_error_callback, USART_CALLBACK_ERROR);
	usart_register_callback(&usart_right_instance, usart_usbc_error_callback, USART_CALLBACK_ERROR);
	usart_register_callback(&usart_bottom_instance, usart_usbc_error_callback, USART_CALLBACK_ERROR);
	usart_register_callback(&usart_left_instance, usart_usbc_error_callback, USART_CALLBACK_ERROR);
	usart_register_callback(&usart_usba_instance, usart_usbc_error_callback, USART_CALLBACK_ERROR);
	
	//! [setup_register_callbacks]

	//! [setup_enable_callbacks]
	usart_enable_callback(&usart_top_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_top_instance, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_right_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_right_instance, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_bottom_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_bottom_instance, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_left_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_left_instance, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_usba_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_usba_instance, USART_CALLBACK_BUFFER_RECEIVED);
	
	usart_enable_callback(&usart_top_instance, USART_CALLBACK_ERROR);
	usart_enable_callback(&usart_right_instance, USART_CALLBACK_ERROR);
	usart_enable_callback(&usart_bottom_instance, USART_CALLBACK_ERROR);
	usart_enable_callback(&usart_left_instance, USART_CALLBACK_ERROR);
	usart_enable_callback(&usart_usba_instance, USART_CALLBACK_ERROR);
	//! [setup_enable_callbacks]
	
	usart_read_buffer_job(&usart_top_instance,(uint8_t *)rx_top_buffer,1);
	usart_read_buffer_job(&usart_right_instance,(uint8_t *)rx_right_buffer,1);
	usart_read_buffer_job(&usart_bottom_instance,(uint8_t *)rx_bottom_buffer,1);
	usart_read_buffer_job(&usart_left_instance,(uint8_t *)rx_left_buffer,1);
	usart_read_buffer_job(&usart_usba_instance,(uint8_t *)rx_usba_buffer,1);
}

void check_comms(void){
	if(rx_top_buffer_length > 0){
		uint8_t data = rx_top_buffer[rx_top_buffer_read_index];
		rx_top_buffer_read_index ++;
		if(rx_top_buffer_read_index == RX_BUFFER_LENGTH) rx_top_buffer_read_index = 0;
		rx_top_buffer_length --;
		usart_rx_handler(0,data);
	}
	if(rx_right_buffer_length > 0){
		uint8_t data = rx_right_buffer[rx_right_buffer_read_index];
		rx_right_buffer_read_index ++;
		if(rx_right_buffer_read_index == RX_BUFFER_LENGTH) rx_right_buffer_read_index = 0;
		rx_right_buffer_length --;
		usart_rx_handler(1,data);
	}
	if(rx_bottom_buffer_length > 0){
		uint8_t data = rx_bottom_buffer[rx_bottom_buffer_read_index];
		rx_bottom_buffer_read_index ++;
		if(rx_bottom_buffer_read_index == RX_BUFFER_LENGTH) rx_bottom_buffer_read_index = 0;
		rx_bottom_buffer_length --;
		usart_rx_handler(2,data);
	}
	if(rx_left_buffer_length > 0){
		uint8_t data = rx_left_buffer[rx_left_buffer_read_index];
		rx_left_buffer_read_index ++;
		if(rx_left_buffer_read_index == RX_BUFFER_LENGTH) rx_left_buffer_read_index = 0;
		rx_left_buffer_length --;
		usart_rx_handler(3,data);
	}
	if(rx_usbc_buffer_length > 0){
		uint8_t data = rx_usbc_buffer[rx_usbc_buffer_read_index];
		rx_usbc_buffer_read_index ++;
		if(rx_usbc_buffer_read_index == RX_BUFFER_LENGTH) rx_usbc_buffer_read_index = 0;
		rx_usbc_buffer_length --;
		usart_rx_handler(4,data);
	}
	if(rx_usba_buffer_length > 0){
		uint8_t data = rx_usba_buffer[rx_usba_buffer_read_index];
		rx_usba_buffer_read_index ++;
		if(rx_usba_buffer_read_index == RX_BUFFER_LENGTH) rx_usba_buffer_read_index = 0;
		rx_usba_buffer_length --;
		usart_rx_handler(5,data);
	}
	
	
	if((millis - last_heartbeat_message_time[0]) > RX_HEARTBEAT_INTERVAL){
		usart_write_buffer_job(&usart_top_instance, heartbeat_message, sizeof(heartbeat_message));
		last_heartbeat_message_time[0] = millis;
		missed_heartbeat_counter[0] ++;
	}
	if((millis - last_heartbeat_message_time[1]) > RX_HEARTBEAT_INTERVAL){
		usart_write_buffer_job(&usart_right_instance, heartbeat_message, sizeof(heartbeat_message));
		last_heartbeat_message_time[1] = millis;
		missed_heartbeat_counter[1] ++;
	}
	if((millis - last_heartbeat_message_time[2]) > RX_HEARTBEAT_INTERVAL){
		usart_write_buffer_job(&usart_bottom_instance, heartbeat_message, sizeof(heartbeat_message));
		last_heartbeat_message_time[2] = millis;
		missed_heartbeat_counter[2] ++;
	}
	if((millis - last_heartbeat_message_time[3]) > RX_HEARTBEAT_INTERVAL){
		usart_write_buffer_job(&usart_left_instance, heartbeat_message, sizeof(heartbeat_message));
		last_heartbeat_message_time[3] = millis;
		missed_heartbeat_counter[3] ++;
	}
	if((millis - last_heartbeat_message_time[4]) > RX_HEARTBEAT_INTERVAL){
		usart_write_buffer_job(&usart_usbc_instance, heartbeat_message, sizeof(heartbeat_message));
		last_heartbeat_message_time[4] = millis;  
		missed_heartbeat_counter[4] ++;
	}
	if((millis - last_heartbeat_message_time[5]) > RX_HEARTBEAT_INTERVAL){
		usart_write_buffer_job(&usart_usba_instance, heartbeat_message, sizeof(heartbeat_message));
		last_heartbeat_message_time[5] = millis;
		missed_heartbeat_counter[5] ++;
	}
	
	if(missed_heartbeat_counter[0] > 2){
		isconnected[0] = false;
		hello_sent[0] = false;
	}
	if(missed_heartbeat_counter[1] > 2){
		isconnected[1] = false;
		hello_sent[1] = false;
	}
	if(missed_heartbeat_counter[2] > 2){
		isconnected[2] = false;
		hello_sent[2] = false;
	}
	if(missed_heartbeat_counter[3] > 2){
		isconnected[3] = false;
		hello_sent[3] = false;
	}
	if(missed_heartbeat_counter[4] > 2){
		isconnected[4] = false;
		hello_sent[4] = false;
	}
	if(missed_heartbeat_counter[5] > 2){
		isconnected[5] = false;
		hello_sent[5] = false;
	}
	
	if(isconnected[0] == true && hello_sent[0] == false) send_hello(0);
	if(isconnected[1] == true && hello_sent[1] == false) send_hello(1);
	if(isconnected[2] == true && hello_sent[2] == false) send_hello(2);
	if(isconnected[3] == true && hello_sent[3] == false) send_hello(3);
	if(isconnected[4] == true && hello_sent[4] == false) send_hello(4);
	if(isconnected[5] == true && hello_sent[5] == false) send_hello(5);
	
	if(isconnected[4] == false && isconnected[5] == false && gamemode != IDLE && gamemode != SIMON_SOLO){
		gamemode = IDLE;
	}
	
	if(isconnected[4] == true && isconnected[5] == false && gamemode == IDLE){ //We're the first(left badge) and connected via USBC port
		led_set_color(1,LED_COLOR_OFF);
		led_set_color(2,LED_COLOR_OFF);
		led_set_color(3,LED_COLOR_OFF);
		led_set_color(4,LED_COLOR_OFF);
		gamemode = SELECT_GAME;
	}
	
	if(isconnected[5] && gamemode == IDLE){
		gamemode = WAIT_FOR_START;
	}
	
	if(isconnected[4] == false && isconnected[5] == true && gamemode == SELECT_GAME){
		gamemode = WAIT_FOR_START;
	}
	
	if(isconnected[4] == true && isconnected[5] == true && gamemode == SELECT_GAME){
		gamemode = WAIT_FOR_START;
	}
	
	if(isconnected[4] == true && isconnected[5] == false && gamemode == WAIT_FOR_START){
		gamemode = SELECT_GAME;
	}
	
	if(gamemode == IDLE){
		if(isconnected[0] | isconnected[1] | isconnected[2] | isconnected[3] | isconnected[4] | isconnected[5]){
			led_set_color(1,led1color);
			led_set_color(2,led2color);
			led_set_color(3,led3color);
			led_set_color(4,led4color);
		} else if(USBPower == false){
			led_set_color(1,LED_COLOR_OFF);
			led_set_color(2,LED_COLOR_OFF);
			led_set_color(3,LED_COLOR_OFF);
			led_set_color(4,LED_COLOR_OFF);
		}
	}
}

void usart_rx_handler(uint8_t port, uint8_t data){
	//RX State Machine
	if(data == 29){ //Sync Byte
		synccounter[port] ++;
		if(synccounter[port] == 4){
			synccounter[port] = 0;
			newbadgetype[port] = false;
			newserialnum[port] = false;
			serialnumcounter[port] = 0;
			rxstate[port] = 1;
			last_heartbeat_message_time[port] = millis;
			missed_heartbeat_counter[port] = 0;
		}
	} else {
		synccounter[port] = 0;
	}
	switch(rxstate[port]){
		case 0: //Start
			break;
		case 1: //Message Type
			if(data == 0){ //Heartbeat
				last_heartbeat_message_time[port] = millis;
				missed_heartbeat_counter[port] = 0;
				isconnected[port] = true;
				if(port == 0) usart_write_buffer_job(&usart_top_instance, ack_message, sizeof(ack_message));
				if(port == 1) usart_write_buffer_job(&usart_right_instance, ack_message, sizeof(ack_message));
				if(port == 2) usart_write_buffer_job(&usart_bottom_instance, ack_message, sizeof(ack_message));
				if(port == 3) usart_write_buffer_job(&usart_left_instance, ack_message, sizeof(ack_message));
				if(port == 4) usart_write_buffer_job(&usart_usbc_instance, ack_message, sizeof(ack_message));
				if(port == 5) usart_write_buffer_job(&usart_usba_instance, ack_message, sizeof(ack_message));
			}
			if(data == 1){ //Hello Message
				rxstate[port] = 2;
				serialnumcounter[port] = 0;
			}
			if(port > 3){ //Side connectors don't do games
				if(data == 6){ //Get Badge Count
					incoming_byte_counter[port] = 0;
					badge_count_ready = false;
					rxstate[port] = 6;
				}
				if(data == 7){ //Badge Count Response
					incoming_byte_counter[port] = 0;
					rxstate[port] = 7;
				}
				if(data == 8){ //Simon Sequence Message
					incoming_byte_counter[port] = 0;
					rxstate[port] = 8;
				}
				if(data == 9){ //Simon Button Pressed Message
					incoming_byte_counter[port] = 0;
					rxstate[port] = 9;
				}
				if(data == 10){ //Simon Game Over Message
					incoming_byte_counter[port] = 0;
					rxstate[port] = 10;
				}
			}
			if(data == 200){ //Challenge Data Request
				rxstate[port] = 0;
				send_challenge_status(port);
			}
			if(data == 255){ //Ack Message
				last_heartbeat_message_time[port] = millis;
				missed_heartbeat_counter[port] = 0;
				isconnected[port] = true;
			}
			break;
		case 2: //Serial Number
			incmoingserialnum[port] |= data << (serialnumcounter[port]*8);
			serialnumcounter[port] ++;
			if(serialnumcounter[port] > 3){
				newserialnum[port] = true;
				for(int x=0; x<NUM_BADGE_SERIALS; x++){
					if(incmoingserialnum[port] == serialnumlist.numbers[x]) newserialnum[port] = false;
				}
				if(incmoingserialnum[port] == serialnum[3]) newserialnum[port] = false;
				rxstate[port] = 3;
			}
			break;
		case 3: //Badge Type
			if(newserialnum[port]){
				if((data & challengedata.badgetypes) == 0){ //Haven't connected to this badge type before. Add it!
					if(data > 0 && (data & (data-1))== 0){ //Make sure it's not 0, and make sure only one bit is set
						newbadgetype[port] = true;
						challengedata.badgetypes |= data;
						#if DEBUG > 0
						if(main_b_cdc_enable){
							udi_cdc_write_buf("connected new badge type: ",26);
							uint8_t bytes[8];
							for(int x=0; x<8; x++){
								bytes[x] = ((challengedata.badgetypes >> x) & 1) + 48;//convert to ascii 0s and 1s
							}
							udi_cdc_write_buf(bytes,8);
							udi_cdc_putc(13);//CR
							udi_cdc_putc(10);//LF
						}
						#endif
						//Play tune
						if(challengedata.badgetypes > 126){
							challenge_section_finish = true;
						} else {
							new_connection = true;
						}
					}
				}
				challengedata.numconnected ++;
				uint8_t bytes[3];
				bytes[0] = challengedata.badgetypes;
				bytes[1] = challengedata.numconnected;
				bytes[2] = challengedata.numconnected >> 8;
				rww_eeprom_emulator_write_buffer(EEP_CHALLENGE_CONNECTED_TYPES, bytes,3); //Write connected types and num connected
				rww_eeprom_emulator_commit_page_buffer();
				//Add serial number to list
				for(int x=0; x<NUM_BADGE_SERIALS-1; x++){
					serialnumlist.numbers[x] = serialnumlist.numbers[x+1];
				}
				serialnumlist.numbers[NUM_BADGE_SERIALS-1] = incmoingserialnum[port];
				rww_eeprom_emulator_write_buffer(EEP_CHALLENGE_BADGE_SERIALS, serialnumlist.bytes, 80);
				rww_eeprom_emulator_commit_page_buffer();
				#if DEBUG > 0
				if(main_b_cdc_enable){
					udi_cdc_write_buf("new serial number: ",19);
					char bytes[32];
					itoa(incmoingserialnum[port],bytes,10);
					uint8_t x;
					for(x=0; x<sizeof(bytes); x++){
						if(bytes[x] == 0) break;
					}
					udi_cdc_write_buf(bytes,x);
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
				} 
				#endif
			}
			rxstate[port] = 4;
			break;
		case 4: //Badges Collected
			if(newserialnum[port] == true && newbadgetype[port] == false){ //If this isn't a new badge type, check to see if they're sharing the signal.
				//NOTE: This is where the code breaks if an UBER badge is presented. Since challengedata.badgetypes would be above 127, it would never collect the other badge types :(
				if(data >= 127 && challengedata.badgetypes < 127){ //Collected all badge types. Grab one that we need!
					if((challengedata.badgetypes & 0x1) == 0){
						challengedata.badgetypes |= 1;
					} else if((challengedata.badgetypes & 2) == 0){
						challengedata.badgetypes |= 2;
					} else if((challengedata.badgetypes & 4) == 0){
						challengedata.badgetypes |= 4;
					} else if((challengedata.badgetypes & 8) == 0){
						challengedata.badgetypes |= 8;
					} else if((challengedata.badgetypes & 16) == 0){
						challengedata.badgetypes |= 16;
					} else if((challengedata.badgetypes & 32) == 0){
						challengedata.badgetypes |= 32;
					} else if((challengedata.badgetypes & 64) == 0){
						challengedata.badgetypes |= 64;
					}
					uint8_t bytes[1];
					bytes[0] = challengedata.badgetypes;
					rww_eeprom_emulator_write_buffer(EEP_CHALLENGE_CONNECTED_TYPES, bytes,1);
					rww_eeprom_emulator_commit_page_buffer();
					//Play tune
					if(challengedata.badgetypes > 126){
						challenge_section_finish = true;
					} else {
						new_connection = true;
					}
				} 
			} else {
				old_connection = true;
			}
			newbadgetype[port] = false;
			if(challengedata.badgetypes >= 127){ //Have we collected all the parts of the signal?
				if(newserialnum[port]){ //Is this a new badge?
					if(data < 127){ //Did we connect to someone that needs the Signal?
						challengedata.numshared ++;
						uint8_t bytes[2];
						bytes[0] = challengedata.numshared;
						bytes[1] = challengedata.numshared >> 8;
						rww_eeprom_emulator_write_buffer(EEP_CHALLENGE_NUM_SHARED, bytes,2);
						rww_eeprom_emulator_commit_page_buffer();
						new_connection = false;
						if(challengedata.numshared > 19){
							challenge_section_finish = true;
						} else {
							new_signal_share = true;
						}
					}
				}
			}
			rxstate[port] = 5;
			break;
		case 5: //Badge Connections
			//Do something with this data?
			send_hello(port);
			rxstate[port] = 0; //This is the last byte in this message
			break;
		case 6: //Get Badge Count
			if(incoming_byte_counter[port] == 0){
				badge_counter = data << 8;
			}
			if(incoming_byte_counter[port] == 1){
				badge_counter |= data;
				badge_counter ++;
				if(isconnected[4] == true){ //If there's a badge connected to the USBC port
					get_badge_count_message[5] = badge_counter >> 8;
					get_badge_count_message[6] = badge_counter;
					usart_write_buffer_job(&usart_usbc_instance, get_badge_count_message, sizeof(get_badge_count_message));
					rxstate[port] = 0;
				} else { //We're the last badge, so send the response message back
					badge_count_response_message[5] = badge_counter >> 8;
					badge_count_response_message[6] = badge_counter;
					usart_write_buffer_job(&usart_usba_instance, badge_count_response_message, sizeof(badge_count_response_message));
					uint8_t bytes[2];
					gamedata.simon_multi_connections = badge_counter;
					bytes[0] = gamedata.simon_multi_connections;
					bytes[1] = gamedata.simon_multi_connections >> 8;
					rww_eeprom_emulator_write_buffer(EEP_GAME_SIMON_MULTI_CONNECTIONS, bytes,2);
					rww_eeprom_emulator_commit_page_buffer();
					gamemode = SIMON_MULTI_SECONDARY;
					gamestate = 0;
				}
			}
			incoming_byte_counter[port] ++;
			break;
		case 7: //Badge Count Response
			if(incoming_byte_counter[port] == 0){
				badge_counter_response = data << 8;
			}
			if(incoming_byte_counter[port] == 1){
				badge_counter_response |= data;
				if(isconnected[5]){ //If there's a badge connected to the USBA port
					badge_count_response_message[5] = badge_counter_response >> 8;
					badge_count_response_message[6] = badge_counter_response;
					usart_write_buffer_job(&usart_usba_instance,badge_count_response_message,sizeof(badge_count_response_message));
					uint8_t bytes[2];
					gamedata.simon_multi_connections = badge_counter_response;
					bytes[0] = gamedata.simon_multi_connections;
					bytes[1] = gamedata.simon_multi_connections >> 8;
					rww_eeprom_emulator_write_buffer(EEP_GAME_SIMON_MULTI_CONNECTIONS, bytes,2);
					rww_eeprom_emulator_commit_page_buffer();
					gamemode = SIMON_MULTI_SECONDARY;
					gamestate = 0;
				} else { //We're the first (far left) badge
					badge_count_ready = true;
					uint8_t bytes[2];
					gamedata.simon_multi_connections = badge_counter_response;
					bytes[0] = gamedata.simon_multi_connections;
					bytes[1] = gamedata.simon_multi_connections >> 8;
					rww_eeprom_emulator_write_buffer(EEP_GAME_SIMON_MULTI_CONNECTIONS, bytes,2);
					rww_eeprom_emulator_commit_page_buffer();
				}
				rxstate[port] = 0; //last byte of this message
			}
			incoming_byte_counter[port] ++;
			break;
		case 8: //Simon Sequence Message
			if(incoming_byte_counter[port] == 0){
				sequence_badge_number = data << 8;
			}
			if(incoming_byte_counter[port] == 1){
				sequence_badge_number |= data;
				
			}
			if(incoming_byte_counter[port] == 2){
				if(sequence_badge_number == badge_counter){ //This sequence is for us
					//Turn on LED/play tune
					incoming_badge_number = badge_counter;
					incoming_badge_button = data;
					incoming_sequence_packet = true;
				} else { //Pass message down the chain
					if(isconnected[4]){ //If we're not the last badge in the chain
						incoming_simon_sequence_message[5] = sequence_badge_number >> 8;
						incoming_simon_sequence_message[6] = sequence_badge_number;
						incoming_simon_sequence_message[7] = data;
						while(usart_write_buffer_job(&usart_usbc_instance,incoming_simon_sequence_message,sizeof(incoming_simon_sequence_message)));
					}
				}
				rxstate[port] = 0;
			}
			incoming_byte_counter[port] ++;
			break;
		case 9: //Simon Button Pressed Message
			if(incoming_byte_counter[port] == 0){
				sequence_badge_number = data << 8;
			}
			if(incoming_byte_counter[port] == 1){
				sequence_badge_number |= data;
			}
			if(incoming_byte_counter[port] == 2){
				//if(isconnected[5] == false){ //We're the first badge
					incoming_badge_number = sequence_badge_number;
					incoming_badge_button = data;
					incoming_button_press_ready = true;
				//} else { //Pass message up the chain
					incoming_simon_button_message[5] = sequence_badge_number >> 8;
					incoming_simon_button_message[6] = sequence_badge_number;
					incoming_simon_button_message[7] = data;
					while(usart_write_buffer_job(&usart_usba_instance,incoming_simon_button_message,sizeof(incoming_simon_button_message)));
				//}
				rxstate[port] = 0;
			}
			incoming_byte_counter[port] ++;
			break;
		case 10: //Simon Game Over Message
			if(incoming_byte_counter[port] == 0){
				simon_multi_score = data << 8;
			}
			if(incoming_byte_counter[port] == 1){
				simon_multi_score |= data;
				simon_game_over(simon_multi_score);
				//send_simon_game_over(simon_multi_score);
				gamemode = WAIT_FOR_START;
				rxstate[port] = 0; //last byte of this message
			}
			incoming_byte_counter[port] ++;
			break;
		default:
			rxstate[port] = 0;//Don't know what to do with this data... reset back to 0
			break;
	}
}


void send_hello(uint8_t port){
	hello_message[0] = 29;
	hello_message[1] = 29;
	hello_message[2] = 29;
	hello_message[3] = 29;
	hello_message[4] = 1; //Hello Message
	hello_message[5] = serialnum[3];
	hello_message[6] = serialnum[3] >> 8;
	hello_message[7] = serialnum[3] >> 16;
	hello_message[8] = serialnum[3] >> 24;
	hello_message[9] = BADGE_TYPE;
	hello_message[10] = challengedata.badgetypes;
	hello_message[11] = 0; //Badge Connections (not implemented)
	//Reply with Hello Message
	if(port == 0 && hello_sent[0] == false){
		while(usart_write_buffer_job(&usart_top_instance, hello_message, sizeof(hello_message)));
		hello_sent[0] = true;
	}
	if(port == 1 && hello_sent[1] == false){
		while(usart_write_buffer_job(&usart_right_instance, hello_message, sizeof(hello_message)));
		hello_sent[1] = true;
	}
	if(port == 2 && hello_sent[2] == false){
		while(usart_write_buffer_job(&usart_bottom_instance, hello_message, sizeof(hello_message)));
		hello_sent[2] = true;
	}
	if(port == 3 && hello_sent[3] == false){
		while(usart_write_buffer_job(&usart_left_instance, hello_message, sizeof(hello_message)));
		hello_sent[3] = true;
	}
	if(port == 4 && hello_sent[4] == false){
		while(usart_write_buffer_job(&usart_usbc_instance, hello_message, sizeof(hello_message)));
		hello_sent[4] = true;
	}
	if(port == 5 && hello_sent[5] == false){
		while(usart_write_buffer_job(&usart_usba_instance, hello_message, sizeof(hello_message)));
		hello_sent[5] = true;
	}
}

void send_challenge_status(uint8_t port){
	isconnected[port] = true;
	challenge_status_message[0] = 29;
	challenge_status_message[1] = 29;
	challenge_status_message[2] = 29;
	challenge_status_message[3] = 29;
	challenge_status_message[4] = 201; //Challenge Status Response Message
	challenge_status_message[5] = challengedata.badgetypes;
	challenge_status_message[6] = challengedata.numconnected;
	challenge_status_message[7] = challengedata.numconnected >> 8;
	challenge_status_message[8] = challengedata.numshared;
	challenge_status_message[9] = challengedata.numshared >> 8;
	challenge_status_message[10] = gamedata.simon_solo_high_score;
	challenge_status_message[11] = gamedata.simon_solo_high_score >> 8;
	challenge_status_message[12] = gamedata.simon_multi_high_score;
	challenge_status_message[13] = gamedata.simon_multi_high_score >> 8;
	challenge_status_message[14] = gamedata.simon_multi_connections;
	challenge_status_message[15] = gamedata.simon_multi_connections >> 8;
	challenge_status_message[16] = gamedata.simon_multi_games_played;
	challenge_status_message[17] = gamedata.simon_multi_games_played >> 8;
	challenge_status_message[18] = gamedata.wam_solo_high_score;
	challenge_status_message[19] = gamedata.wam_solo_high_score >> 8;
	challenge_status_message[20] = gamedata.wam_multi_high_score;
	challenge_status_message[21] = gamedata.wam_multi_high_score >> 8;
	challenge_status_message[22] = gamedata.wam_multi_connections;
	challenge_status_message[23] = gamedata.wam_multi_connections >> 8;
	challenge_status_message[24] = gamedata.wam_multi_games_played;
	challenge_status_message[25] = gamedata.wam_multi_games_played >> 8;
	challenge_status_message[26] = gamedata.wam_multi_wins;
	challenge_status_message[27] = gamedata.wam_multi_wins >> 8;
	

	if(port == 0){
		while(usart_write_buffer_job(&usart_top_instance, challenge_status_message, sizeof(challenge_status_message)));
	}
	if(port == 1){
		while(usart_write_buffer_job(&usart_right_instance, challenge_status_message, sizeof(challenge_status_message)));
	}
	if(port == 2){
		while(usart_write_buffer_job(&usart_bottom_instance, challenge_status_message, sizeof(challenge_status_message)));
	}
	if(port == 3){
		while(usart_write_buffer_job(&usart_left_instance, challenge_status_message, sizeof(challenge_status_message)));
	}
	if(port == 4){
		while(usart_write_buffer_job(&usart_usbc_instance, challenge_status_message, sizeof(challenge_status_message)));
	}
	if(port == 5){
		while(usart_write_buffer_job(&usart_usba_instance, challenge_status_message, sizeof(challenge_status_message)));
	}
	
	if((millis - last_challenge_status_request) > 1000){
		new_connection = true;
	}
	last_challenge_status_request = millis;
}

void get_badge_count(void){
	get_badge_count_message[5] = 0;
	get_badge_count_message[6] = 1;
	while(usart_write_buffer_job(&usart_usbc_instance,get_badge_count_message,sizeof(get_badge_count_message)));
}

void send_simon_game_packet(uint16_t badge, uint8_t button){
	if(isconnected[4]){ //If there's another badge in the chain, send message along
		simon_sequence_message[5] = badge >> 8;
		simon_sequence_message[6] = badge;
		simon_sequence_message[7] = button;
		while(usart_write_buffer_job(&usart_usbc_instance, simon_sequence_message, sizeof(simon_sequence_message)));
	}
}

void send_simon_button_packet(uint16_t badge, uint8_t button){

	simon_button_message[5] = badge >> 8;
	simon_button_message[6] = badge;
	simon_button_message[7] = button;
	while(usart_write_buffer_job(&usart_usba_instance, simon_button_message, sizeof(simon_button_message)));
}

void send_simon_game_over(uint16_t score){
	simon_game_over_message[5] = score >> 8;
	simon_game_over_message[6] = score;
	if(isconnected[4]) while(usart_write_buffer_job(&usart_usbc_instance,simon_game_over_message,sizeof(simon_game_over_message)));
}

void send_heartbeats(void){
	usart_write_buffer_job(&usart_top_instance, heartbeat_message, sizeof(heartbeat_message));
	usart_write_buffer_job(&usart_right_instance, heartbeat_message, sizeof(heartbeat_message));
	usart_write_buffer_job(&usart_bottom_instance, heartbeat_message, sizeof(heartbeat_message));
	usart_write_buffer_job(&usart_left_instance, heartbeat_message, sizeof(heartbeat_message));
	usart_write_buffer_job(&usart_usbc_instance, heartbeat_message, sizeof(heartbeat_message));
	usart_write_buffer_job(&usart_usba_instance, heartbeat_message, sizeof(heartbeat_message));
}

void disable_usarts(void){
	usart_disable(&usart_usba_instance);
}

