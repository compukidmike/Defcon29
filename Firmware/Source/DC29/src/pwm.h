/*
 * pwm.h
 *
 *  Author: compukidmike
 */ 


#ifndef PWM_H_
#define PWM_H_

typedef enum {
	LED1R,
	LED1G,
	LED1B,
	LED2R,
	LED2G,
	LED2B,
	LED3R,
	LED3G,
	LED3B,
	LED4R,
	LED4G,
	LED4B,
	LEDCOUNT
}
leds;

struct RGB {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	};

void pwm_init(void);

void led_set_brightness(leds led, uint8_t brightness);

void led_on(leds led);

void led_off(leds led);

void led_toggle(leds led);

void led_set_color(uint8_t led, uint8_t color[3]);

void buzzer_on(void);

void buzzer_off(void);

void buzzer_set_value(uint8_t value);

#endif /* PWM_H_ */