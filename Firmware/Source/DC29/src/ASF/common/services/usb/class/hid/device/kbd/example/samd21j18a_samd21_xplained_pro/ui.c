/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
#include "ui.h"

//! Sequence process running each \c SEQUENCE_PERIOD ms
#define SEQUENCE_PERIOD 150

//! Wakeup, ignore button change until button is back to default state
//static bool btn_wakeup = false;
/*
static struct {
	bool b_modifier;
	bool b_down;
	uint8_t value;
} ui_sequence[] = {
	// Display windows menu
	{true,true,HID_MODIFIER_LEFT_UI},
	{true,false,HID_MODIFIER_LEFT_UI},

};*/

/* Interrupt on "pin change" from push button to do wakeup on USB
 * Note:
 * This interrupt is enable when the USB host enable remote wakeup feature
 * This interrupt wakeup the CPU if this one is in idle mode
 */
static void ui_wakeup_handler(void)
{
	/* It is a wakeup then send wakeup USB */
	udc_remotewakeup();
	LED_On(LED_0_PIN);
}


void ui_init(void)
{
	struct extint_chan_conf config_extint_chan;

	extint_chan_get_config_defaults(&config_extint_chan);

	config_extint_chan.gpio_pin            = BUTTON_0_EIC_PIN;
	config_extint_chan.gpio_pin_mux        = BUTTON_0_EIC_MUX;
	config_extint_chan.gpio_pin_pull       = EXTINT_PULL_UP;
	config_extint_chan.filter_input_signal = true;
	config_extint_chan.detection_criteria  = EXTINT_DETECT_FALLING;
	extint_chan_set_config(BUTTON_0_EIC_LINE, &config_extint_chan);
	extint_register_callback(ui_wakeup_handler, BUTTON_0_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(BUTTON_0_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);

	/* Initialize LEDs */
	LED_Off(LED_0_PIN);
}

void ui_powerdown(void)
{
	LED_Off(LED_0_PIN);
}


void ui_wakeup_enable(void)
{
	extint_chan_enable_callback(BUTTON_0_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
}

void ui_wakeup_disable(void)
{
	extint_chan_disable_callback(BUTTON_0_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
}

void ui_wakeup(void)
{
	LED_On(LED_0_PIN);
}

void ui_process(uint16_t framenumber)
{
	
}

void ui_kbd_led(uint8_t value)
{
	UNUSED(value);
}

 /**
 * \defgroup UI User Interface
 *
 * Human interface on SAMD21-XPlain:
 * - LED0 blinks when USB host has checked and enabled HID Keyboard interface
 * - The SW0 opens a notepad application on Windows O.S.
 *   and sends key sequence "Atmel ARM"
 * - Only SW0 will generate a wakeup to USB Host in remote wakeup mode.
 *
 */
