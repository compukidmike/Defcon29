/*
 * keys.c
 *
 *  Author: compukidmike
 */ 

#include "keys.h"
#include "udi_hid_kbd.h"

extern uint8_t keymap[231];
extern uint8_t keymaplength;
extern uint8_t keymapstarts[6];

extern bool wait_for_sof;

extern volatile uint32_t millis;

extern bool udi_hid_kbd_b_report_trans_ongoing;

uint32_t lastUSBSendTime = 0;

void get_keymap(void){
	rww_eeprom_emulator_read_buffer(EEP_KEY_MAP, keymap, 231);

	keymaplength = keymap[0];
	keymapstarts[0] = 1;
	for(int x=1; x<keymaplength; x++){
		switch (keymap[x]){
			case 251:
				keymapstarts[1] = x;
				break;
			case 252:
				keymapstarts[2] = x;
				break;
			case 253:
				keymapstarts[3] = x;
				break;
			case 254:
				keymapstarts[4] = x;
				break;
			case 255:
				keymapstarts[5] = x;
				break;
			
		}
	}
}

void send_keys(uint8_t key){
	if(key < 6){
		for(int x=keymapstarts[key-1]+1; x<keymapstarts[key]; x+=2){
			if(keymap[x] == 240){ //Media key
				wait_for_sof = true; //Needed?? I think the code that tests for this is gone
				udi_hid_media_down(keymap[x+1]);
				lastUSBSendTime = millis;
				while(millis - lastUSBSendTime < 10);
				wait_for_sof = true;
				udi_hid_media_up();
				lastUSBSendTime = millis;
				while(millis - lastUSBSendTime < 10);
				//while(wait_for_sof);
			} else {
				if(keymap[x+1] != 0){ //Don't send key
					wait_for_sof = true;
					udi_hid_kbd_modifier_down(keymap[x]);
					lastUSBSendTime = millis;
					while(millis - lastUSBSendTime < 10);
					wait_for_sof = true;
					udi_hid_kbd_down(keymap[x+1]);
					lastUSBSendTime = millis;
					while(millis - lastUSBSendTime < 10);
					wait_for_sof = true;
					udi_hid_kbd_up(keymap[x+1]);
					lastUSBSendTime = millis;
					while(millis - lastUSBSendTime < 10);
					wait_for_sof = true;
					udi_hid_kbd_modifier_up(keymap[x]);
					lastUSBSendTime = millis;
					while(millis - lastUSBSendTime < 10);
					wait_for_sof = true;
				}
			}
		}
	} else {
		for(int x=keymapstarts[key-1]+1; x<keymaplength; x+=2){
			if(keymap[x] == 240){ //Media key
				wait_for_sof = true;
				udi_hid_media_down(keymap[x+1]);
				lastUSBSendTime = millis;
				while(millis - lastUSBSendTime < 10);
				wait_for_sof = true;
				udi_hid_media_up();
				lastUSBSendTime = millis;
				while(millis - lastUSBSendTime < 10);
			} else {
				if(keymap[x+1] != 0){ //Don't send key
					wait_for_sof = true;
					udi_hid_kbd_modifier_down(keymap[x]);
					lastUSBSendTime = millis;
					while(millis - lastUSBSendTime < 10);
					wait_for_sof = true;
					udi_hid_kbd_down(keymap[x+1]);
					lastUSBSendTime = millis;
					while(millis - lastUSBSendTime < 10);
					wait_for_sof = true;
					udi_hid_kbd_up(keymap[x+1]);
					lastUSBSendTime = millis;
					while(millis - lastUSBSendTime < 10);
					wait_for_sof = true;
					udi_hid_kbd_modifier_up(keymap[x]);
					lastUSBSendTime = millis;
					while(millis - lastUSBSendTime < 10);
				}
			}
		}
	}
}