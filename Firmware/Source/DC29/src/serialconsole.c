/*
 * serialconsole.c
 *
 *  Author: compukidmike
 */ 

#include "serialconsole.h"
#include "rww_eeprom.h"
#include "pwm.h"
#include "keys.h"

static int serialConsoleState = 0;
extern bool main_b_cdc_enable;
static int ledChangeNum;
static int ledChangeColor;
static int keyChangeNum;
static uint8_t newKeystroke[230];
static int newKeystrokeCounter;
static uint8_t newKeymap[230];

extern uint8_t led1color[3];
extern uint8_t led2color[3];
extern uint8_t led3color[3];
extern uint8_t led4color[3];

extern uint8_t keymaplength;
extern uint8_t keymap[];
extern uint8_t keymapstarts[];
extern ChallengeStruct challengedata;
extern serial serialnumlist;
extern GameStruct gamedata;

extern volatile uint32_t millis;
extern uint32_t serialnum[4];

uint8_t decoded_string[20];
uint8_t connection_request[32];
uint8_t connection_reply[32];
uint8_t connection_reply_counter;

#if DEBUG > 0
static uint8_t newBadgeTypes = 0;
static uint8_t newBadgeTypesCounter = 0;
static uint16_t newParameterData = 0;
static uint8_t eepromOffset = 0;
#endif

void updateSerialConsole(void){
	if(main_b_cdc_enable){
		if(udi_cdc_get_nb_received_data()){
			int data = udi_cdc_getc();
			switch (serialConsoleState){
				case 0: //not started
					//print out splash screen and starting info
					if(data == 13){
						showSplashScreen();
						serialConsoleState = 1;
					}
					break;
				case 1: //at main menu
					if(data == '1'){
						//display LED change screen
						udi_cdc_putc(12);//new page
						udi_cdc_write_buf(LEDSELECT,sizeof(LEDSELECT));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS1,13);
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS2,13);
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS3,13);
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS4,13);
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS2,13);
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS5,13);
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS4,13);
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INPUT,sizeof(INPUT));
						serialConsoleState = 4;
					} else if(data == '2'){
						//display keymap change screen
						udi_cdc_putc(12);//new page
						udi_cdc_write_buf(KEYSELECT,sizeof(KEYSELECT));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS1,sizeof(KEYS1));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS2,sizeof(KEYS2));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS3,sizeof(KEYS3));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS4,sizeof(KEYS4));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS2,sizeof(KEYS2));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS5,sizeof(KEYS5));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYS4,sizeof(KEYS4));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INPUT,sizeof(INPUT));
						serialConsoleState = 9;
					} else if(data == '3'){
						udi_cdc_putc(12);//new page
						udi_cdc_write_buf(RESET,sizeof(RESET));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(YESNO,sizeof(YESNO));
						serialConsoleState = 2;
			#if DEBUG > 0
					} else if(data == '9'){
						//display challenge parameter change screen
						udi_cdc_putc(12);//new page
						udi_cdc_write_buf(DEBUG1,sizeof(DEBUG1));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTION1,sizeof(DEBUGOPTION1));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTION2,sizeof(DEBUGOPTION2));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTION3,sizeof(DEBUGOPTION3));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTION4,sizeof(DEBUGOPTION4));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTION5,sizeof(DEBUGOPTION5));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTION6,sizeof(DEBUGOPTION6));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTION7,sizeof(DEBUGOPTION7));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTION8,sizeof(DEBUGOPTION8));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTION9,sizeof(DEBUGOPTION9));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTIONA,sizeof(DEBUGOPTIONA));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTIONB,sizeof(DEBUGOPTIONB));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(DEBUGOPTIONC,sizeof(DEBUGOPTIONC));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INPUT,sizeof(INPUT));
						serialConsoleState = 100;
			#endif
					} else if(data == '4'){
						//Generate Badge Connection Request
						srand(millis);
						char serial_string[8];
						my_itoa(serialnum[3],serial_string);//convert serial to hex
						for(int x=0; x<32; x++){
							connection_request[x] = (rand() % 16) + 48;
							if(connection_request[x] > 57) connection_request[x] += 7;
						}
						connection_request[2] = serial_string[0];
						if(connection_request[2] > 96 && connection_request[2] < 103) connection_request[2] -= 32; //Convert to capital letters
						connection_request[3] = serial_string[1];
						if(connection_request[3] > 96 && connection_request[3] < 103) connection_request[3] -= 32; //Convert to capital letters
						connection_request[8] = serial_string[2];
						if(connection_request[8] > 96 && connection_request[8] < 103) connection_request[8] -= 32; //Convert to capital letters
						connection_request[9] = serial_string[3];
						if(connection_request[9] > 96 && connection_request[9] < 103) connection_request[9] -= 32; //Convert to capital letters
						connection_request[16] = serial_string[4];
						if(connection_request[16] > 96 && connection_request[16] < 103) connection_request[16] -= 32; //Convert to capital letters
						connection_request[17] = serial_string[5];
						if(connection_request[17] > 96 && connection_request[17] < 103) connection_request[17] -= 32; //Convert to capital letters
						connection_request[18] = serial_string[6];
						if(connection_request[18] > 96 && connection_request[18] < 103) connection_request[18] -= 32; //Convert to capital letters
						connection_request[19] = serial_string[7];
						if(connection_request[19] > 96 && connection_request[19] < 103) connection_request[19] -= 32; //Convert to capital letters
						connection_request[20] = '1'; //Request message type
						connection_request[21] = '2';
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(GENERATE_STRING,sizeof(GENERATE_STRING));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(connection_request,sizeof(connection_request));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INPUT,sizeof(INPUT)); //Choose another menu option
						serialConsoleState = 1;
					} else if(data == '5'){
						//Enter Badge Connection Reply
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(REPLY_STRING,sizeof(REPLY_STRING));
						connection_reply_counter = 0;
						serialConsoleState = 11;
					} else {
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INVALID,sizeof(INVALID));
						//invalid option, go back to start
						serialConsoleState = 1;
					}
					break;
				case 2: //Reset EEPROM
					if(data == 'y'){
						//reset EEPROM and exit
						reset_user_eeprom();
						//get_keymap();
						read_eeprom();
						serialConsoleState = 1;
						showSplashScreen();
					} else if(data == 'n'){
						//exit
						serialConsoleState = 1;
						showSplashScreen();
					} else {
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INVALID,sizeof(INVALID));
					}
					break;
				case 3: //Keymap change
					//display keymap change options
					udi_cdc_putc(12);//new page
					udi_cdc_write_buf(KEYSELECT,sizeof(KEYSELECT));
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
					udi_cdc_write_buf(KEYS1,sizeof(KEYS1));
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
					udi_cdc_write_buf(KEYS2,sizeof(KEYS2));
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
					udi_cdc_write_buf(KEYS3,sizeof(KEYS3));
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
					udi_cdc_write_buf(KEYS4,sizeof(KEYS4));
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
					udi_cdc_write_buf(KEYS2,sizeof(KEYS2));
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
					udi_cdc_write_buf(KEYS5,sizeof(KEYS5));
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
					udi_cdc_write_buf(KEYS4,sizeof(KEYS4));
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
					udi_cdc_write_buf(INPUT,sizeof(INPUT));
					serialConsoleState = 9;
					break;
				case 4: //LED change entry
					udi_cdc_putc(data);//echo input
					ledChangeNum = 0;
					if(data == '1') ledChangeNum = 1;
					if(data == '2') ledChangeNum = 2;
					if(data == '3') ledChangeNum = 3;
					if(data == '4') ledChangeNum = 4;
					if(ledChangeNum > 0){
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(ENTERCOLORS,sizeof(ENTERCOLORS));
						udi_cdc_putc(ledChangeNum + 48);//convert to ascii
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(RED,sizeof(RED));
						ledChangeColor = 0;
						serialConsoleState = 5;
					} else {
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INVALID,sizeof(INVALID));
					}
					break;
				case 5: //Enter RED Color Value
					if(data == 13){
						//parse number
						if(ledChangeColor < 256){
							if(ledChangeNum == 1) led1color[0] = ledChangeColor;
							if(ledChangeNum == 2) led2color[0] = ledChangeColor;
							if(ledChangeNum == 3) led3color[0] = ledChangeColor;
							if(ledChangeNum == 4) led4color[0] = ledChangeColor;
							udi_cdc_putc(13);//CR
							udi_cdc_putc(10);//LF
							udi_cdc_write_buf(GREEN,sizeof(GREEN));
							ledChangeColor = 0;
							serialConsoleState = 6;
						} else {
							udi_cdc_putc(13);//CR
							udi_cdc_putc(10);//LF
							udi_cdc_write_buf(INVALID,sizeof(INVALID));
						}
					} else if(data == 127) {
					if(ledChangeColor > 0){
						ledChangeColor = ledChangeColor/10; //shift decimal place
					}
					} else {
						ledChangeColor = ledChangeColor * 10; //shift decimal place
						ledChangeColor += data-48; //convert from ascii
						udi_cdc_putc(data);//echo input
					}
					break;
				case 6: //Enter GREEN Color Value
					if(data == 13){
						//parse number
						if(ledChangeColor < 256){
							if(ledChangeNum == 1) led1color[1] = ledChangeColor;
							if(ledChangeNum == 2) led2color[1] = ledChangeColor;
							if(ledChangeNum == 3) led3color[1] = ledChangeColor;
							if(ledChangeNum == 4) led4color[1] = ledChangeColor;
							udi_cdc_putc(13);//CR
							udi_cdc_putc(10);//LF
							udi_cdc_write_buf(BLUE,sizeof(BLUE));
							ledChangeColor = 0;
							serialConsoleState = 7;
						} else {
							udi_cdc_putc(13);//CR
							udi_cdc_putc(10);//LF
							udi_cdc_write_buf(INVALID,sizeof(INVALID));
						}
					} else if(data == 127) {
						if(ledChangeColor > 0){
							ledChangeColor = ledChangeColor/10; //shift decimal place
						}
					} else {
						ledChangeColor = ledChangeColor * 10; //shift decimal place
						ledChangeColor += data-48; //convert from ascii
						udi_cdc_putc(data);//echo input
					}
					break;
				case 7: //Enter BLUE Color Value
					if(data == 13){
						//parse number
						if(ledChangeColor < 256){
							if(ledChangeNum == 1) led1color[2] = ledChangeColor;
							if(ledChangeNum == 2) led2color[2] = ledChangeColor;
							if(ledChangeNum == 3) led3color[2] = ledChangeColor;
							if(ledChangeNum == 4) led4color[2] = ledChangeColor;
							if(ledChangeNum == 1) led_set_color(ledChangeNum, led1color);
							if(ledChangeNum == 2) led_set_color(ledChangeNum, led2color);
							if(ledChangeNum == 3) led_set_color(ledChangeNum, led3color);
							if(ledChangeNum == 4) led_set_color(ledChangeNum, led4color);
							udi_cdc_putc(13);//CR
							udi_cdc_putc(10);//LF
							udi_cdc_write_buf(COLORSAVE,sizeof(COLORSAVE));
							udi_cdc_putc(ledChangeNum + 48);
							udi_cdc_putc('?');
							udi_cdc_putc(13);//CR
							udi_cdc_putc(10);//LF
							udi_cdc_write_buf(YESNO,sizeof(YESNO));
							serialConsoleState = 8;
						} else {
							udi_cdc_putc(13);//CR
							udi_cdc_putc(10);//LF
							udi_cdc_write_buf(INVALID,sizeof(INVALID));
						}
					} else if(data == 127) {
					if(ledChangeColor > 0){
						ledChangeColor = ledChangeColor/10; //shift decimal place
					}
					} else {
						ledChangeColor = ledChangeColor * 10; //shift decimal place
						ledChangeColor += data-48; //convert from ascii
						udi_cdc_putc(data);//echo input
					}
					break;
				case 8: //Save color?
					if(data == 'y'){
						//save to eeprom and exit
						if(ledChangeNum == 1) rww_eeprom_emulator_write_buffer(EEP_LED_1_COLOR,led1color,3);
						if(ledChangeNum == 2) rww_eeprom_emulator_write_buffer(EEP_LED_2_COLOR,led2color,3);
						if(ledChangeNum == 3) rww_eeprom_emulator_write_buffer(EEP_LED_3_COLOR,led3color,3);
						if(ledChangeNum == 4) rww_eeprom_emulator_write_buffer(EEP_LED_4_COLOR,led4color,3);
						rww_eeprom_emulator_commit_page_buffer();
						serialConsoleState = 1;
						showSplashScreen();
					} else if(data == 'n'){
						//revert LED color and exit
						if(ledChangeNum == 1) rww_eeprom_emulator_read_buffer(EEP_LED_1_COLOR,led1color,3);
						if(ledChangeNum == 2) rww_eeprom_emulator_read_buffer(EEP_LED_2_COLOR,led2color,3);
						if(ledChangeNum == 3) rww_eeprom_emulator_read_buffer(EEP_LED_3_COLOR,led3color,3);
						if(ledChangeNum == 4) rww_eeprom_emulator_read_buffer(EEP_LED_4_COLOR,led4color,3);
						if(ledChangeNum == 1) led_set_color(ledChangeNum, led1color);
						if(ledChangeNum == 2) led_set_color(ledChangeNum, led2color);
						if(ledChangeNum == 3) led_set_color(ledChangeNum, led3color);
						if(ledChangeNum == 4) led_set_color(ledChangeNum, led4color);
						serialConsoleState = 1;
						showSplashScreen();
					} else {
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INVALID,sizeof(INVALID));
					}
					break;
				case 9: //Key Change
					udi_cdc_putc(data);//echo input
					ledChangeNum = 0;
					if(data == '1') keyChangeNum = 1;
					if(data == '2') keyChangeNum = 2;
					if(data == '3') keyChangeNum = 3;
					if(data == '4') keyChangeNum = 4;
					if(data == '5') keyChangeNum = 5;
					if(data == '6') keyChangeNum = 6;
					if(keyChangeNum > 0){
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYENTRY,sizeof(KEYENTRY));
						udi_cdc_putc(keyChangeNum + 48);//convert to ascii
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYOPTIONS1,sizeof(KEYOPTIONS1));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYOPTIONS2,sizeof(KEYOPTIONS2));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYOPTIONS3,sizeof(KEYOPTIONS3));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYOPTIONS4,sizeof(KEYOPTIONS4));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYOPTIONS5,sizeof(KEYOPTIONS5));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(KEYSTROKE,sizeof(KEYSTROKE));
						newKeystrokeCounter = 0;
						for(int x=0; x<sizeof(newKeystroke); x++){newKeystroke[x] = 0;}
						serialConsoleState = 10;
					} else {
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INVALID,sizeof(INVALID));
					}
					break;
				case 10: //New Keystroke
					udi_cdc_putc(data);//echo input
					if(data == 13){ //enter
						int newKeymapCounter = 0;
						for(int x=0; x<newKeystrokeCounter; x++){
							if(newKeystroke[x] == '['){
								x++;
								if(newKeystroke[x] == 'c' && newKeystroke[x+1] == 't' && newKeystroke[x+2] == 'r' && newKeystroke[x+3] == 'l' && newKeystroke[x+4] == ']' && newKeystroke[x+5] != '['){
									newKeymap[newKeymapCounter] = HID_MODIFIER_LEFT_CTRL;
									newKeymapCounter ++;
									x += 4;
								} else if(newKeystroke[x] == 'a' && newKeystroke[x+1] == 'l' && newKeystroke[x+2] == 't' && newKeystroke[x+3] == ']' && newKeystroke[x+4] != '['){
									newKeymap[newKeymapCounter] = HID_MODIFIER_LEFT_ALT;
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 's' && newKeystroke[x+1] == 'h' && newKeystroke[x+2] == 'i' && newKeystroke[x+3] == 'f' && newKeystroke[x+4] == 't' && newKeystroke[x+5] == ']' && newKeystroke[x+6] != '['){
									newKeymap[newKeymapCounter] = HID_MODIFIER_LEFT_SHIFT;
									newKeymapCounter ++;
									x += 5;
								} else if(newKeystroke[x] == 'g' && newKeystroke[x+1] == 'u' && newKeystroke[x+2] == 'i' && newKeystroke[x+3] == ']' && newKeystroke[x+4] != '['){
									newKeymap[newKeymapCounter] = HID_MODIFIER_LEFT_UI;
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'p' && newKeystroke[x+1] == 'l' && newKeystroke[x+2] == 'a' && newKeystroke[x+3] == 'y' && newKeystroke[x+4] == ']' && newKeystroke[x+5] != '['){
									newKeymap[newKeymapCounter] = 240;//media key identifier
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = HID_MEDIA_PLAY;
									newKeymapCounter ++;
									x += 4;
								} else if(newKeystroke[x] == 'n' && newKeystroke[x+1] == 'e' && newKeystroke[x+2] == 'x' && newKeystroke[x+3] == 't' && newKeystroke[x+4] == ']' && newKeystroke[x+5] != '['){
									newKeymap[newKeymapCounter] = 240;//media key identifier
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = HID_MEDIA_NEXT;
									newKeymapCounter ++;
									x += 4;
								} else if(newKeystroke[x] == 'b' && newKeystroke[x+1] == 'a' && newKeystroke[x+2] == 'c' && newKeystroke[x+3] == 'k' && newKeystroke[x+4] == ']' && newKeystroke[x+5] != '['){
									newKeymap[newKeymapCounter] = 240;//media key identifier
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = HID_MEDIA_BACK;
									newKeymapCounter ++;
									x += 4;
								} else if(newKeystroke[x] == 's' && newKeystroke[x+1] == 't' && newKeystroke[x+2] == 'o' && newKeystroke[x+3] == 'p' && newKeystroke[x+4] == ']' && newKeystroke[x+5] != '['){
									newKeymap[newKeymapCounter] = 240;//media key identifier
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = HID_MEDIA_STOP;
									newKeymapCounter ++;
									x += 4;
								} else if(newKeystroke[x] == 'e' && newKeystroke[x+1] == 'j' && newKeystroke[x+2] == 'e' && newKeystroke[x+3] == 'c' && newKeystroke[x+4] == 't' && newKeystroke[x+5] != ']' && newKeystroke[x+6] != '['){
									newKeymap[newKeymapCounter] = 240;//media key identifier
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = HID_MEDIA_EJECT;
									newKeymapCounter ++;
									x += 5;
								} else if(newKeystroke[x] == 'm' && newKeystroke[x+1] == 'u' && newKeystroke[x+2] == 't' && newKeystroke[x+3] == 'e' && newKeystroke[x+4] == ']' && newKeystroke[x+5] != '['){
									newKeymap[newKeymapCounter] = 240;//media key identifier
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = HID_MEDIA_MUTE;
									newKeymapCounter ++;
									x += 4;
								} else if(newKeystroke[x] == 'v' && newKeystroke[x+1] == 'o' && newKeystroke[x+2] == 'l' && newKeystroke[x+3] == '+' && newKeystroke[x+4] == ']' && newKeystroke[x+5] != '['){
									newKeymap[newKeymapCounter] = 240;//media key identifier
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = HID_MEDIA_VOL_PLUS;
									newKeymapCounter ++;
									x += 4;
								} else if(newKeystroke[x] == 'v' && newKeystroke[x+1] == 'o' && newKeystroke[x+2] == 'l' && newKeystroke[x+3] == '-' && newKeystroke[x+4] == ']' && newKeystroke[x+5] != '['){
									newKeymap[newKeymapCounter] = 240;//media key identifier
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = HID_MEDIA_VOL_MINUS;
									newKeymapCounter ++;
									x += 4;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 58;//F1
									newKeymapCounter ++;
									x += 2;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '2' && newKeystroke[x+2] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 59;//F2
									newKeymapCounter ++;
									x += 2;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '3' && newKeystroke[x+2] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 60;//F3
									newKeymapCounter ++;
									x += 2;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '4' && newKeystroke[x+2] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 61;//F4
									newKeymapCounter ++;
									x += 2;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '5' && newKeystroke[x+2] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 62;//F5
									newKeymapCounter ++;
									x += 2;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '6' && newKeystroke[x+2] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 63;//F6
									newKeymapCounter ++;
									x += 2;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '7' && newKeystroke[x+2] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 64;//F7
									newKeymapCounter ++;
									x += 2;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '8' && newKeystroke[x+2] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 65;//F8
									newKeymapCounter ++;
									x += 2;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '9' && newKeystroke[x+2] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 66;//F9
									newKeymapCounter ++;
									x += 2;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == '0' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 67;//F10
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == '1' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 68;//F11
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == '2' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 69;//F12
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == '3' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 104;//F13
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == '4' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 105;//F14
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == '5' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 106;//F15
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == '6' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 107;//F16
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == '7' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 108;//F17
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == '8' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 109;//F18
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '1' && newKeystroke[x+2] == '9' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 110;//F19
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '2' && newKeystroke[x+2] == '0' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 111;//F20
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '2' && newKeystroke[x+2] == '1' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 112;//F21
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '2' && newKeystroke[x+2] == '2' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 113;//F22
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '2' && newKeystroke[x+2] == '3' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 114;//F23
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'F' && newKeystroke[x+1] == '2' && newKeystroke[x+2] == '4' && newKeystroke[x+3] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 115;//F24
									newKeymapCounter ++;
									x += 3;
								} else if(newKeystroke[x] == 'n' && newKeystroke[x+1] == 'o' && newKeystroke[x+2] == 'n' && newKeystroke[x+3] == 'e' && newKeystroke[x+4] == ']'){
									newKeymap[newKeymapCounter] = 0; //No modifiers
									newKeymapCounter ++;
									newKeymap[newKeymapCounter] = 0; //No key
									newKeymapCounter ++;
									x += 4;
								}
							} else {
								if(ascii_to_hid[newKeystroke[x]] > 127){
									newKeymap[newKeymapCounter] = HID_MODIFIER_LEFT_SHIFT;
									newKeymapCounter++;
									newKeymap[newKeymapCounter] = ascii_to_hid[newKeystroke[x]]-128;
									newKeymapCounter++;
								} else {
									newKeymap[newKeymapCounter] = 0;
									newKeymapCounter++;
									newKeymap[newKeymapCounter] = ascii_to_hid[newKeystroke[x]];
									newKeymapCounter++;
								}
							}
						}
							if(newKeymapCounter + keymaplength < 231){
								int x=0;
								int y=0;
								if(keyChangeNum == 1){
									x=0;y=0;
									newKeystroke[0] = 250;
									for(x=1; x<newKeymapCounter+1; x++){
										newKeystroke[x] = newKeymap[x-1];	
									}
									for(y=keymapstarts[1]; y<keymaplength; y++){
										newKeystroke[x] = keymap[y];
										x++;
									}
								}
								if(keyChangeNum == 2){
									x=0;y=0;
									for(y=keymapstarts[0]; y<keymapstarts[1]; y++){
										newKeystroke[x] = keymap[y];
										x++;
									}
									newKeystroke[x] = 251;
									x++;
									y=0;
									for(x=x; x<newKeymapCounter+keymapstarts[1]; x++){
										newKeystroke[x] = newKeymap[y];
										y++;
									}
									for(y=keymapstarts[2]; y<keymaplength; y++){
										newKeystroke[x] = keymap[y];
										x++;
									}
								}
								if(keyChangeNum == 3){
									x=0;y=0;
									for(y=keymapstarts[0]; y<keymapstarts[2]; y++){
										newKeystroke[x] = keymap[y];
										x++;
									}
									newKeystroke[x] = 252;
									x++;
									y=0;
									for(x=x; x<newKeymapCounter+keymapstarts[2]; x++){
										newKeystroke[x] = newKeymap[y];
										y++;
									}
									for(y=keymapstarts[3]; y<keymaplength; y++){
										newKeystroke[x] = keymap[y];
										x++;
									}
								}
								if(keyChangeNum == 4){
									x=0;y=0;
									for(y=keymapstarts[0]; y<keymapstarts[3]; y++){
										newKeystroke[x] = keymap[y];
										x++;
									}
									newKeystroke[x] = 253;
									x++;
									y=0;
									for(x=x; x<newKeymapCounter+keymapstarts[3]; x++){
										newKeystroke[x] = newKeymap[y];
										y++;
									}
									for(y=keymapstarts[4]; y<keymaplength; y++){
										newKeystroke[x] = keymap[y];
										x++;
									}
								}
								if(keyChangeNum == 5){
									x=0;y=0;
									for(y=keymapstarts[0]; y<keymapstarts[4]; y++){
										newKeystroke[x] = keymap[y];
										x++;
									}
									newKeystroke[x] = 254;
									x++;
									y=0;
									for(x=x; x<newKeymapCounter+keymapstarts[4]; x++){
										newKeystroke[x] = newKeymap[y];
										y++;
									}
									for(y=keymapstarts[5]; y<keymaplength; y++){
										newKeystroke[x] = keymap[y];
										x++;
									}
								}
								if(keyChangeNum == 6){
									x=0;y=0;
									for(y=keymapstarts[0]; y<keymapstarts[5]; y++){
										newKeystroke[x] = keymap[y];
										x++;
									}
									newKeystroke[x] = 255;
									x++;
									y=0;
									for(x=x; x<newKeymapCounter+keymapstarts[5]; x++){
										newKeystroke[x] = newKeymap[y];
										y++;
									}
								}
								uint8_t length[1];
								length[0] = x;
								rww_eeprom_emulator_write_buffer(EEP_KEY_MAP,length,1);
								rww_eeprom_emulator_write_buffer(EEP_KEY_MAP+1,newKeystroke,x);
								rww_eeprom_emulator_commit_page_buffer();
								get_keymap();
								serialConsoleState = 1;
								showSplashScreen();
							} else {
								udi_cdc_putc(13);//CR
								udi_cdc_putc(10);//LF
								udi_cdc_write_buf(TOOLONG,sizeof(TOOLONG));
								newKeystrokeCounter = 0;
							}
						
					} else if(data == 127) { //backspace
						if(newKeystrokeCounter > 0){
							newKeystrokeCounter --;
						}
					} else {
						if(newKeystrokeCounter < sizeof(newKeystroke)){
							newKeystroke[newKeystrokeCounter] = data;
							newKeystrokeCounter ++;
						} else {
							newKeystrokeCounter = 0;
							for(int x=0; x<sizeof(newKeystroke); x++){newKeystroke[x] = 0;}
							udi_cdc_putc(13);//CR
							udi_cdc_putc(10);//LF
							udi_cdc_write_buf(INVALID,sizeof(INVALID));
						}
					}
					break;
				case 11: //Parse Reply String
					if(data != ' '){
						udi_cdc_putc(data);//echo input and ignore spaces
						if(data == 127){ //backspace
							if(connection_reply_counter > 0) connection_reply_counter --;
						} else {
							if(connection_reply_counter < sizeof(connection_request)){
								connection_request[connection_reply_counter] = data;
								connection_reply_counter ++;
							}
						}
					}
					if(data == 13){ //enter
						if(connection_reply_counter == 32){
							uint32_t new_serial_num;
							uint8_t tempnum;
							tempnum = connection_request[4] - 48; //back to decimal
							if(tempnum > 9) tempnum -= 7; //convert A-F
							new_serial_num |= tempnum << 28;
							tempnum = connection_request[5] - 48; //back to decimal
							if(tempnum > 9) tempnum -= 7; //convert A-F
							new_serial_num |= tempnum << 24;
							tempnum = connection_request[6] - 48; //back to decimal
							if(tempnum > 9) tempnum -= 7; //convert A-F
							new_serial_num |= tempnum << 20;
							tempnum = connection_request[7] - 48; //back to decimal
							if(tempnum > 9) tempnum -= 7; //convert A-F
							new_serial_num |= tempnum << 16;
							tempnum = connection_request[10] - 48; //back to decimal
							if(tempnum > 9) tempnum -= 7; //convert A-F
							new_serial_num |= tempnum << 12;
							tempnum = connection_request[11] - 48; //back to decimal
							if(tempnum > 9) tempnum -= 7; //convert A-F
							new_serial_num |= tempnum << 8;
							tempnum = connection_request[14] - 48; //back to decimal
							if(tempnum > 9) tempnum -= 7; //convert A-F
							new_serial_num |= tempnum << 4;
							tempnum = connection_request[15] - 48; //back to decimal
							if(tempnum > 9) tempnum -= 7; //convert A-F
							new_serial_num |= tempnum;
							
							if(new_serial_num == serialnum[3]){ //This is a request from this badge!
								udi_cdc_putc(13);//CR
								udi_cdc_putc(10);//LF
								udi_cdc_write_buf("ERROR: This string is from your badge.",39);
								udi_cdc_putc(13);//CR
								udi_cdc_putc(10);//LF
								udi_cdc_write_buf(ENTER_CONTINUE, sizeof(ENTER_CONTINUE));
								serialConsoleState = 0;
								break; //Exit case early
							}
							
							if(connection_request[20] == '1' && connection_request[21] == '2'){ //Request message type, generate reply message
								//Generate Badge Connection Reply
								srand(millis);
								char serial_string[8];
								my_itoa(serialnum[3],serial_string);//convert serial to hex
								for(int x=0; x<32; x++){
									connection_reply[x] = (rand() % 16) + 48;
									if(connection_reply[x] > 57) connection_reply[x] += 7;
								}
								connection_reply[2] = connection_request[2];
								connection_reply[3] = connection_request[3];
								connection_reply[8] = connection_request[8];
								connection_reply[9] = connection_request[9];
								connection_reply[16] = connection_request[16];
								connection_reply[17] = connection_request[17];
								connection_reply[18] = connection_request[18];
								connection_reply[19] = connection_request[19];
								connection_reply[4] = serial_string[0];
								if(connection_reply[4] > 96 && connection_reply[4] < 103) connection_reply[4] -= 32; //Convert to capital letters
								connection_reply[5] = serial_string[1];
								if(connection_reply[5] > 96 && connection_reply[5] < 103) connection_reply[5] -= 32; //Convert to capital letters
								connection_reply[6] = serial_string[2];
								if(connection_reply[6] > 96 && connection_reply[6] < 103) connection_reply[6] -= 32; //Convert to capital letters
								connection_reply[7] = serial_string[3];
								if(connection_reply[7] > 96 && connection_reply[7] < 103) connection_reply[7] -= 32; //Convert to capital letters
								connection_reply[10] = serial_string[4];
								if(connection_reply[10] > 96 && connection_reply[10] < 103) connection_reply[10] -= 32; //Convert to capital letters
								connection_reply[11] = serial_string[5];
								if(connection_reply[11] > 96 && connection_reply[11] < 103) connection_reply[11] -= 32; //Convert to capital letters
								connection_reply[14] = serial_string[6];
								if(connection_reply[14] > 96 && connection_reply[14] < 103) connection_reply[14] -= 32; //Convert to capital letters
								connection_reply[15] = serial_string[7];
								if(connection_reply[15] > 96 && connection_reply[15] < 103) connection_reply[15] -= 32; //Convert to capital letters
								connection_reply[20] = '1'; //Reply message type
								connection_reply[21] = '3'; //Reply message type
								itoa(BADGE_TYPE,serial_string,16);
								if(BADGE_TYPE < 16){
									connection_reply[24] = '0';
									connection_reply[25] = serial_string[0];
								} else {
									connection_reply[24] = serial_string[0];
									connection_reply[25] = serial_string[1];
								}
								itoa(challengedata.badgetypes,serial_string,16);
								if(challengedata.badgetypes < 16){
									connection_reply[28] = '0';
									connection_reply[29] = serial_string[0];
								} else {
									connection_reply[28] = serial_string[0];
									connection_reply[29] = serial_string[1];
								}
								if(connection_reply[28] > 96 && connection_reply[28] < 103) connection_reply[28] -= 32; //Convert to capital letters
								if(connection_reply[29] > 96 && connection_reply[29] < 103) connection_reply[29] -= 32; //Convert to capital letters
								udi_cdc_putc(13);//CR
								udi_cdc_putc(10);//LF
								udi_cdc_write_buf(GENERATE_REPLY,sizeof(GENERATE_REPLY));
								udi_cdc_putc(13);//CR
								udi_cdc_putc(10);//LF
								udi_cdc_write_buf(connection_reply,sizeof(connection_reply));
								udi_cdc_putc(13);//CR
								udi_cdc_putc(10);//LF
								udi_cdc_write_buf(ENTER_CONTINUE, sizeof(ENTER_CONTINUE));
								serialConsoleState = 0;
								break; //Exit case early
							} else if(connection_request[20] == '1' && connection_request[21] == '3'){ //Reply message type, parse data
								uint32_t original_serial_num;
								connection_request[2] -= 48; //back to decimal
								if(connection_request[2] > 9) connection_request[2] -= 7; //convert A-F
								original_serial_num = connection_request[2] << 28;
								connection_request[3] -= 48; //back to decimal
								if(connection_request[3] > 9) connection_request[3] -= 7; //convert A-F
								original_serial_num |= connection_request[3] << 24;
								connection_request[8] -= 48; //back to decimal
								if(connection_request[8] > 9) connection_request[8] -= 7; //convert A-F
								original_serial_num |= connection_request[8] << 20;
								connection_request[9] -= 48; //back to decimal
								if(connection_request[9] > 9) connection_request[9] -= 7; //convert A-F
								original_serial_num |= connection_request[9] << 16;
								connection_request[16] -= 48; //back to decimal
								if(connection_request[16] > 9) connection_request[16] -= 7; //convert A-F
								original_serial_num |= connection_request[16] << 12;
								connection_request[17] -= 48; //back to decimal
								if(connection_request[17] > 9) connection_request[17] -= 7; //convert A-F
								original_serial_num |= connection_request[17] << 8;
								connection_request[18] -= 48; //back to decimal
								if(connection_request[18] > 9) connection_request[18] -= 7; //convert A-F
								original_serial_num |= connection_request[18] << 4;
								connection_request[19] -= 48; //back to decimal
								if(connection_request[19] > 9) connection_request[19] -= 7; //convert A-F
								original_serial_num |= connection_request[19];
								
								if(original_serial_num != serialnum[3]){ //This reply is NOT from our original request
									udi_cdc_putc(13);//CR
									udi_cdc_putc(10);//LF
									udi_cdc_write_buf("ERROR: This reply is not for your badge.",40);
									udi_cdc_putc(13);//CR
									udi_cdc_putc(10);//LF
									udi_cdc_write_buf(ENTER_CONTINUE, sizeof(ENTER_CONTINUE));
									serialConsoleState = 0;
									break; //Exit case early
								}
								
								bool newserialnum = true;
								for(int x=0; x<NUM_BADGE_SERIALS; x++){
									if(new_serial_num == serialnumlist.numbers[x]) newserialnum = false;
								}
								if(newserialnum == true){ //Save new serial number to EEPROM
									for(int x=0; x<NUM_BADGE_SERIALS-1; x++){
										serialnumlist.numbers[x] = serialnumlist.numbers[x+1];
									}
									serialnumlist.numbers[NUM_BADGE_SERIALS-1] = new_serial_num;
									rww_eeprom_emulator_write_buffer(EEP_CHALLENGE_BADGE_SERIALS, serialnumlist.bytes, 80);
									rww_eeprom_emulator_commit_page_buffer();
									udi_cdc_putc(13);//CR
									udi_cdc_putc(10);//LF
									udi_cdc_write_buf(NEW_BADGE, sizeof(NEW_BADGE));
									
									uint8_t badgetype = 0;
									connection_request[24] -= 48; //back to decimal
									if(connection_request[24] > 9) connection_request[24] -= 7; //convert A-F
									badgetype = connection_request[24] << 4;
									connection_request[25] -= 48; //back to decimal
									if(connection_request[25] > 9) connection_request[25] -= 7; //convert A-F
									badgetype |= connection_request[25];
									
									if((badgetype & challengedata.badgetypes) == 0){ //Haven't connected to this badge type before. Add it!
										if(badgetype > 0 && (badgetype & (badgetype-1))== 0){ //Make sure it's not 0, and make sure only one bit is set
											
											udi_cdc_putc(13);//CR
											udi_cdc_putc(10);//LF
											udi_cdc_write_buf(NEW_BADGE_TYPE, sizeof(NEW_BADGE_TYPE));
										}
									} else { //See if they're sharing the signal
										uint8_t badgescollected = 0;
										connection_request[28] -= 48; //back to decimal
										if(connection_request[28] > 9) connection_request[28] -= 7; //convert A-F
										badgescollected = connection_request[28] << 4;
										connection_request[29] -= 48; //back to decimal
										if(connection_request[29] > 9) connection_request[29] -= 7; //convert A-F
										badgescollected |= connection_request[29];
										//NOTE: This is where the code breaks if an UBER badge is presented. Since challengedata.badgetypes would be above 127, it would never collect the other badge types :(
										if(badgescollected >= 127 && challengedata.badgetypes < 127){ //They collected all badge types. Grab one that we need! 
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
											udi_cdc_putc(13);//CR
											udi_cdc_putc(10);//LF
											udi_cdc_write_buf(NEW_BADGE_TYPE, sizeof(NEW_BADGE_TYPE));
										} else { //They need the signal, so share it if we have it. This is going to be on the honor system since there's no real 2-way comms for virtual.
											if(challengedata.badgetypes >= 127){ //Have we collected all the badge types?
												challengedata.numshared ++;
												uint8_t bytes[2];
												bytes[0] = challengedata.numshared;
												bytes[1] = challengedata.numshared >> 8;
												rww_eeprom_emulator_write_buffer(EEP_CHALLENGE_NUM_SHARED, bytes,2);
												rww_eeprom_emulator_commit_page_buffer();
												udi_cdc_putc(13);//CR
												udi_cdc_putc(10);//LF
												udi_cdc_write_buf(SIGNAL_SHARED, sizeof(SIGNAL_SHARED));
											}
										}
									}
									challengedata.badgetypes |= badgetype;
									challengedata.numconnected ++;
									uint8_t bytes[3];
									bytes[0] = challengedata.badgetypes;
									bytes[1] = challengedata.numconnected;
									bytes[2] = challengedata.numconnected >> 8;
									rww_eeprom_emulator_write_buffer(EEP_CHALLENGE_CONNECTED_TYPES, bytes,3); //Write connected types and num connected
									rww_eeprom_emulator_commit_page_buffer();
								} else {
									udi_cdc_putc(13);//CR
									udi_cdc_putc(10);//LF
									udi_cdc_write_buf("ERROR: Already connected to this badge",38);
									udi_cdc_putc(13);//CR
									udi_cdc_putc(10);//LF
									udi_cdc_write_buf(ENTER_CONTINUE, sizeof(ENTER_CONTINUE));
									serialConsoleState = 0;
									break; //Exit case early
								}
								udi_cdc_putc(13);//CR
								udi_cdc_putc(10);//LF
								udi_cdc_write_buf(ENTER_CONTINUE, sizeof(ENTER_CONTINUE));
								serialConsoleState = 0;
								break; //Exit case early
							}
						}
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INVALID, sizeof(INVALID));
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(ENTER_CONTINUE, sizeof(ENTER_CONTINUE));
						serialConsoleState = 0;
					}
					break;
		#if DEBUG > 0
				case 100:
					newBadgeTypes = 0;
					newBadgeTypesCounter = 0;
					newParameterData = 0;
					eepromOffset = 0;
					udi_cdc_putc(data);//echo input
					if(data == '1'){ //Badge Types
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(BINARY,sizeof(BINARY));
						serialConsoleState = 101;
					} else { //All other inputs are uint16
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(UINT16,sizeof(UINT16));
					}
					if(data == '2'){ //Num Badges Connected
						serialConsoleState = 102;
						eepromOffset = EEP_CHALLENGE_NUM_CONNECTED;
					}
					if(data == '3'){ //Num Signal Shares
						serialConsoleState = 102;
						eepromOffset = EEP_CHALLENGE_NUM_SHARED;
					}
					if(data == '4'){ //Simon Solo High Score
						serialConsoleState = 102;
						eepromOffset = EEP_GAME_SIMON_SOLO_HIGH_SCORE;
					}
					if(data == '5'){ //Simon Multiplayer High Score
						serialConsoleState = 102;
						eepromOffset = EEP_GAME_SIMON_MULTI_HIGH_SCORE;
					}
					if(data == '6'){ //Simon Multi Num Badges
						serialConsoleState = 102;
						eepromOffset = EEP_GAME_SIMON_MULTI_CONNECTIONS;
					}
					if(data == '7'){ //Simon Multi Num Games Played
						serialConsoleState = 102;
						eepromOffset = EEP_GAME_SIMON_MULTI_GAMES_PLAYED;
					}
					if(data == '8'){ //WAM Solo High Score
						serialConsoleState = 102;
						eepromOffset = EEP_GAME_WAM_SOLO_HIGH_SCORE;
					}
					if(data == '9'){ //WAM Multi High Score
						serialConsoleState = 102;
						eepromOffset = EEP_GAME_WAM_MULTI_HIGH_SCORE;
					}
					if(data == 'a'){ //WAM Multi Num Badges
						serialConsoleState = 102;
						eepromOffset = EEP_GAME_WAM_MULTI_CONNECTIONS;
					}
					if(data == 'b'){ //WAM Multi Num Games Played
						serialConsoleState = 102;
						eepromOffset = EEP_GAME_WAM_MULTI_GAMES_PLAYED;
					}
					if(data == 'c'){ //WAM Multi Num Wins
						serialConsoleState = 102;
						eepromOffset = EEP_GAME_WAM_MULTI_WINS;
					}
					if(serialConsoleState == 100){ //if state hasn't changed, invalid input
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						udi_cdc_write_buf(INVALID,sizeof(INVALID));
					}
					
					break;
				case 101:
					//Check if it's 8 binary bits
					if(data == 13){ //Enter
						//parse number
						rww_eeprom_emulator_write_buffer(EEP_CHALLENGE_CONNECTED_TYPES, &newBadgeTypes, 1);
						rww_eeprom_emulator_commit_page_buffer();
						read_eeprom();
						showSplashScreen();
						serialConsoleState = 1;
					} else if(data == 127) { //Backspace
						if(newBadgeTypesCounter > 0){
							newBadgeTypes = newBadgeTypes >> 1; //Drop off last bit
							newBadgeTypesCounter --;
							udi_cdc_putc(data);//echo input
						}
					} else if((data > 47) && (data < 50)){ //Only allow 1 and 0
						if(newBadgeTypesCounter < 8){
							newBadgeTypesCounter ++;
							if(data == 49){ 
								if(newBadgeTypesCounter == 1) newBadgeTypes += 1;
								if(newBadgeTypesCounter == 2) newBadgeTypes += 2;
								if(newBadgeTypesCounter == 3) newBadgeTypes += 4;
								if(newBadgeTypesCounter == 4) newBadgeTypes += 8;
								if(newBadgeTypesCounter == 5) newBadgeTypes += 16;
								if(newBadgeTypesCounter == 6) newBadgeTypes += 32;
								if(newBadgeTypesCounter == 7) newBadgeTypes += 64;
								if(newBadgeTypesCounter == 8) newBadgeTypes += 128;
							}
							udi_cdc_putc(data);//echo input
						}
					}
					break;
				case 102:
					if(data == 13){ //Enter
						//parse number
						uint8_t bytes[2];
						bytes[0] = newParameterData; //Why is the byte order reversed for game data vs challenge data????
						bytes[1] = newParameterData >> 8;
						rww_eeprom_emulator_write_buffer(eepromOffset, bytes, 2); //Why is this messing with both challenge data numbers?????
						rww_eeprom_emulator_commit_page_buffer();
						read_eeprom();
						showSplashScreen();
						serialConsoleState = 1;
					} else if(data == 8) { //Backspace
						if(newParameterData > 0){
							newParameterData = newParameterData/10; //shift decimal place
						}
					} else if((data > 47) && (data < 58)){ //Only allow numbers
						newParameterData = newParameterData * 10; //shift decimal place
						newParameterData += data-48; //convert from ascii
						udi_cdc_putc(data);//echo input
					}
					break;
		#endif
				default:
					break;
			}
		}
	}
}

void showSplashScreen(void){
	char bytes[8];
	uint8_t x = 0;
	udi_cdc_putc(12);//new page
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(SERIAL_SPLASH,sizeof(SERIAL_SPLASH));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(MKFACTOR1,sizeof(MKFACTOR1));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(MKFACTOR2,sizeof(MKFACTOR2));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(MKFACTOR3,sizeof(MKFACTOR3));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(MKFACTOR4,sizeof(MKFACTOR4));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	if(sizeof(THANK_YOU_TYPE) > 1){
		udi_cdc_write_buf(THANK_YOU_TYPE,sizeof(THANK_YOU_TYPE));
		udi_cdc_putc(13);//CR
		udi_cdc_putc(10);//LF
	}
	udi_cdc_write_buf(WEBSITE,sizeof(WEBSITE));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(CHALLENGE_STATUS,sizeof(CHALLENGE_STATUS));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(CHALLENGE_BADGES_CONNECTED,sizeof(CHALLENGE_BADGES_CONNECTED));
	itoa(challengedata.numconnected,bytes,10);
	for(x=0; x<sizeof(bytes); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(bytes,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(CHALLENGE_BADGE_TYPES,sizeof(CHALLENGE_BADGE_TYPES));
	if(challengedata.badgetypes & 1) udi_cdc_write_buf("Human,",6);
	if(challengedata.badgetypes & 2) udi_cdc_write_buf("Goon,",6);
	if(challengedata.badgetypes & 4) udi_cdc_write_buf("Creator,",8);
	if(challengedata.badgetypes & 8) udi_cdc_write_buf("Speaker,",8);
	if(challengedata.badgetypes & 16) udi_cdc_write_buf("Artist,",7);
	if(challengedata.badgetypes & 32) udi_cdc_write_buf("Vendor,",7);
	if(challengedata.badgetypes & 64) udi_cdc_write_buf("Press",6);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	
	if(challengedata.badgetypes >= 127){ //Have we collected all the badge types?
		udi_cdc_write_buf(CHALLENGE5,sizeof(CHALLENGE5));
		udi_cdc_putc(13);//CR
		udi_cdc_putc(10);//LF
		udi_cdc_write_buf(WEBSITE,sizeof(WEBSITE));
		decrypt(WEBSITE_COLLECTED_SIGNAL,sizeof(WEBSITE_COLLECTED_SIGNAL));
		udi_cdc_putc(13);//CR
		udi_cdc_putc(10);//LF
		
		udi_cdc_write_buf(CHALLENGE_SIGNAL_SHARES,sizeof(CHALLENGE_SIGNAL_SHARES));
		itoa(challengedata.numshared,bytes,10);
		for(x=0; x<sizeof(bytes); x++){
			if(bytes[x] == 0) break;
		}
		udi_cdc_write_buf(bytes,x);
		udi_cdc_putc(13);//CR
		udi_cdc_putc(10);//LF
		if(challengedata.numshared >= 20){ //Have we shared the signal at least 20 times?
			udi_cdc_write_buf(CHALLENGE6,sizeof(CHALLENGE6));
			udi_cdc_putc(13);//CR
			udi_cdc_putc(10);//LF
			udi_cdc_write_buf(WEBSITE,sizeof(WEBSITE));
			decrypt(WEBSITE_SHARED_SIGNAL,sizeof(WEBSITE_SHARED_SIGNAL));
			if(port_pin_get_input_level(MATRIX) == true){ //Matrix has been disconnected!
				udi_cdc_putc(13);//CR
				udi_cdc_putc(10);//LF
				udi_cdc_write_buf(WEBSITE,sizeof(WEBSITE));
				decrypt(WEBSITE_MATRIX,sizeof(WEBSITE_MATRIX));
				if(port_pin_get_input_level(MAX) == false){ //Max has been reconnected!
					udi_cdc_putc(13);//CR
					udi_cdc_putc(10);//LF
					decrypt(WEBSITE_MAX,sizeof(WEBSITE_MAX));
					if(port_pin_get_input_level(ALIENS) == false){ //Aliens have made a connection!
						udi_cdc_putc(13);//CR
						udi_cdc_putc(10);//LF
						decrypt(WEBSITE_ALIENS,sizeof(WEBSITE_ALIENS));
					}
				}
			}
			udi_cdc_putc(13);//CR
			udi_cdc_putc(10);//LF
		}
	}
	
	//Game Stats
	if(gamedata.simon_solo_high_score > 0 || gamedata.simon_multi_high_score > 0){
		udi_cdc_putc(13);//CR
		udi_cdc_putc(10);//LF
		udi_cdc_write_buf(GAME,sizeof(GAME));
		udi_cdc_putc(13);//CR
		udi_cdc_putc(10);//LF
		if(gamedata.simon_solo_high_score > 0){
			udi_cdc_write_buf(GAME1,sizeof(GAME1));
			itoa(gamedata.simon_solo_high_score,bytes,10);
			for(x=0; x<sizeof(bytes); x++){
				if(bytes[x] == 0) break;
			}
			udi_cdc_write_buf(bytes,x);
			udi_cdc_putc(13);//CR
			udi_cdc_putc(10);//LF
		}
		if(gamedata.simon_multi_high_score > 0){
			udi_cdc_write_buf(GAME2,sizeof(GAME2));
			itoa(gamedata.simon_multi_high_score,bytes,10);
			for(x=0; x<sizeof(bytes); x++){
				if(bytes[x] == 0) break;
			}
			udi_cdc_write_buf(bytes,x);
			udi_cdc_putc(13);//CR
			udi_cdc_putc(10);//LF
			udi_cdc_write_buf(GAME3,sizeof(GAME3));
			itoa(gamedata.simon_multi_games_played,bytes,10);
			for(x=0; x<sizeof(bytes); x++){
				if(bytes[x] == 0) break;
			}
			udi_cdc_write_buf(bytes,x);
			udi_cdc_putc(13);//CR
			udi_cdc_putc(10);//LF
			udi_cdc_write_buf(GAME4,sizeof(GAME4));
			itoa(gamedata.simon_multi_connections,bytes,10);
			for(x=0; x<sizeof(bytes); x++){
				if(bytes[x] == 0) break;
			}
			udi_cdc_write_buf(bytes,x);
			udi_cdc_putc(13);//CR
			udi_cdc_putc(10);//LF
		}
	}

#if DEBUG > 0
	udi_cdc_write_buf("serial numbers: ", 16);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	for(int y=0; y<20; y++){
		itoa(serialnumlist.numbers[y],bytes,16);
		//for(int y=0; y<32; y++){
		//	bytes[y] = (((serialnumlist.numbers[x] >> y) & 1) + 48);//convert to ascii 0s and 1s
		//}
		for(x=0; x<sizeof(bytes); x++){
			if(bytes[x] == 0) break;
		}
		udi_cdc_write_buf(bytes,x);
		udi_cdc_putc(',');
		//udi_cdc_putc(13);//CR
		//udi_cdc_putc(10);//LF
	}
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
#endif
	
#if DEBUG > 0
	udi_cdc_write_buf("connected types: ", 17);
	for(x=0; x<8; x++){
		bytes[x] = ((challengedata.badgetypes >> x) & 1) + 48;//convert to ascii 0s and 1s
	}
	udi_cdc_write_buf(bytes,8);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("number connected: ", 18);
	itoa(challengedata.numconnected,bytes,10);
	for(x=0; x<sizeof(bytes); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(bytes,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("number shared: ", 15);
	itoa(challengedata.numshared,bytes,10);
	for(x=0; x<sizeof(bytes); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(bytes,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("serial numbers: ", 16);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	for(int y=0; y<20; y++){
		itoa(serialnumlist.numbers[y],bytes,16);
		//for(int y=0; y<32; y++){
		//	bytes[y] = (((serialnumlist.numbers[x] >> y) & 1) + 48);//convert to ascii 0s and 1s
		//}
		for(x=0; x<sizeof(bytes); x++){
			if(bytes[x] == 0) break;
		}
		udi_cdc_write_buf(bytes,x);
		udi_cdc_putc(',');
		//udi_cdc_putc(13);//CR
		//udi_cdc_putc(10);//LF
	}
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("simon solo high score: ", 23);
	char number[5] = {0};
	itoa(gamedata.simon_solo_high_score,number,10);
	for(x=0; x<sizeof(number); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(number,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("simon multiplayer high score: ", 30);
	itoa(gamedata.simon_multi_high_score,number,10);
	for(x=0; x<sizeof(number); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(number,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("simon multiplayer num badges: ", 30);
	itoa(gamedata.simon_multi_connections,number,10);
	for(x=0; x<sizeof(number); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(number,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("simon multiplayer games played: ", 32);
	itoa(gamedata.simon_multi_games_played,number,10);
	for(x=0; x<sizeof(number); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(number,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("WAM solo high score: ", 21);
	itoa(gamedata.wam_solo_high_score,number,10);
	for(x=0; x<sizeof(number); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(number,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("WAM multiplayer high score: ", 28);
	itoa(gamedata.wam_multi_high_score,number,10);
	for(x=0; x<sizeof(number); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(number,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("WAM multiplayer num badges: ", 28);
	itoa(gamedata.wam_multi_connections,number,10);
	for(x=0; x<sizeof(number); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(number,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("WAM multiplayer games played: ", 30);
	itoa(gamedata.wam_multi_games_played,number,10);
	for(x=0; x<sizeof(number); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(number,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf("WAM multiplayer wins: ", 22);
	itoa(gamedata.wam_multi_wins,number,10);
	for(x=0; x<sizeof(number); x++){
		if(bytes[x] == 0) break;
	}
	udi_cdc_write_buf(number,x);
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	
#endif
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(MENU,sizeof(MENU));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(OPTION1,sizeof(OPTION1));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(OPTION2,sizeof(OPTION2));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(OPTION3,sizeof(OPTION3));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(OPTION4,sizeof(OPTION4));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
	udi_cdc_write_buf(OPTION5,sizeof(OPTION5));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
#if DEBUG > 0
	udi_cdc_write_buf(OPTION4,sizeof(OPTION4));
	udi_cdc_putc(13);//CR
	udi_cdc_putc(10);//LF
#endif
	udi_cdc_write_buf(INPUT,sizeof(INPUT));
	
}

void decrypt(const uint8_t *encoded_string, uint8_t length){
	for(int x=0; x<length; x++){
		decoded_string[x] = (encoded_string[x]-128) ^ SERIAL_SPLASH[x];
	}
	udi_cdc_write_buf(decoded_string,length);
}

const char lookup[]="0123456789ABCDEF";

inline void my_itoa(uint32_t sn, char* outbuf) {
	for (int i=0; i<4; ++i) {
		char c = (sn >> ((3-i)*8)) & 0xff;
		outbuf[i*2]=lookup[c >> 4];
		outbuf[i*2+1]=lookup[c & 0xF];
	}
}
