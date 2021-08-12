/*
 * games.c
 *
 *  Author: compukidmike
 */ 

#include "games.h"

extern GameStruct gamedata;
extern GameModes gamemode;
extern volatile uint32_t millis;
extern volatile bool button1;
extern volatile bool button2;
extern volatile bool button3;
extern volatile bool button4;
extern uint16_t badge_counter;
extern uint16_t badge_counter_response;
extern uint8_t led1color[3];
extern uint8_t led2color[3];
extern uint8_t led3color[3];
extern uint8_t led4color[3];
extern struct tcc_module tcc2_instance;
extern volatile bool games_buzzer_off;

uint8_t gamestate = 0;
bool badge_count_ready = false;
uint16_t sequence_badges[128] = {0};
uint8_t sequence_buttons[128] = {0};
uint8_t sequence_send_counter = 0;
uint8_t sequence_recv_counter = 0;
uint8_t sequence_progress = 0;
uint16_t simon_score_counter = 0;

uint32_t next_sequence_time = 0;
uint32_t simon_timeout_counter = 0;

uint16_t incoming_badge_number = 0;
uint8_t incoming_badge_button = 0;
bool incoming_button_press_ready = false;
bool incoming_sequence_packet = false;

bool game_select_led = false;
uint32_t select_led_last_time = 0;
uint32_t select_led_delay = 350;

uint32_t led_on_time = 0;
uint32_t pause_time = 50;

bool packet_sent = false;
bool led_or_pause = true;

uint32_t last_led_pattern_time = 0;
uint32_t led_pattern_delay = 350;
bool led_pattern = true;;

uint32_t last_button_press_time = 0;

uint8_t playstate = 0;
uint32_t last_time = 0;

void run_games(void){
	switch(gamemode){
		case IDLE:
			break;
		case SELECT_GAME:
			//Turn on LEDs
			if((millis - select_led_last_time) > select_led_delay){
				select_led_last_time = millis;
				if(game_select_led){
					game_select_led = false;
					led_set_color(1,LED_COLOR_GREEN);
					led_set_color(3,LED_COLOR_OFF);
					led_set_color(2,LED_COLOR_OFF);
					led_set_color(4,LED_COLOR_OFF);
					} else {
					game_select_led = true;
					led_set_color(3,LED_COLOR_OFF);
					led_set_color(1,LED_COLOR_OFF);
					led_set_color(2,LED_COLOR_OFF);
					led_set_color(4,LED_COLOR_OFF);
				}
			}
			if(button1){
				button1 = false;
				get_badge_count();
				led_set_color(1,LED_COLOR_OFF);
				led_set_color(2,LED_COLOR_OFF);
				led_set_color(3,LED_COLOR_OFF);
				led_set_color(4,LED_COLOR_OFF);
				gamestate = 0;
				gamemode = SIMON_MULTI_PRIMARY;
			}
			break;
		case SIMON_SOLO:
			switch(gamestate){
				case 0: //Wait for game start
					if(game_over_tune == false){
						if((millis - select_led_last_time) > select_led_delay){
							select_led_last_time = millis;
							if(game_select_led){
								game_select_led = false;
								led_set_color(1,LED_COLOR_GREEN);
								led_set_color(3,LED_COLOR_OFF);
								led_set_color(2,LED_COLOR_OFF);
								led_set_color(4,LED_COLOR_OFF);
								} else {
								game_select_led = true;
								led_set_color(3,LED_COLOR_OFF);
								led_set_color(1,LED_COLOR_OFF);
								led_set_color(2,LED_COLOR_OFF);
								led_set_color(4,LED_COLOR_OFF);
							}
						}
						if(button1){
							button1 = false;
							get_badge_count();
							led_set_color(1,LED_COLOR_OFF);
							led_set_color(2,LED_COLOR_OFF);
							led_set_color(3,LED_COLOR_OFF);
							led_set_color(4,LED_COLOR_OFF);
							gamestate = 1;
							gamemode = SIMON_SOLO;
						}

					}
				case 1: //Game Setup
						simon_start_tune = true;
						srand(millis);
						for(int x=0; x<sizeof(sequence_buttons); x++){
							sequence_buttons[x] = (rand() % 4)+1;
						}
						sequence_send_counter = 0;
						sequence_progress = 0;
						next_sequence_time = 0;
						led_or_pause = false;
						gamestate = 2;
					break;
				case 2:
					//Start Sequence
					if(simon_start_tune == false){
						if(millis > next_sequence_time){
							if(led_or_pause == true){ //Finished LED on time
								led_or_pause = false;
								next_sequence_time = millis + pause_time;
								led_set_color(1,LED_COLOR_OFF);
								led_set_color(2,LED_COLOR_OFF);
								led_set_color(3,LED_COLOR_OFF);
								led_set_color(4,LED_COLOR_OFF);
								games_buzzer_off = true;
								tcc_stop_counter(&tcc2_instance);
								sequence_send_counter ++;
							} else { //Finished off delay
								led_or_pause = true;
								if(sequence_send_counter > sequence_progress){
									sequence_recv_counter = 0;
									last_button_press_time = millis;
									gamestate = 3; //Wait for button press
									break;
								}
								//Original timing for solo game
								led_on_time = 220;
								if(sequence_progress < 10) led_on_time = 320;
								if(sequence_progress < 5) led_on_time = 420;
								next_sequence_time = millis + led_on_time;
							
								//Turn on next LED in sequence & play tune
								if(sequence_buttons[sequence_send_counter] == 1){
									led_set_color(1,LED_COLOR_GREEN);
									games_buzzer_off = false;
									tcc_restart_counter(&tcc2_instance);
									tcc_set_compare_value(&tcc2_instance,0,219); //415Hz
								}
								if(sequence_buttons[sequence_send_counter] == 2){
									led_set_color(2,LED_COLOR_RED);
									games_buzzer_off = false;
									tcc_restart_counter(&tcc2_instance);
									tcc_set_compare_value(&tcc2_instance,0,293); //310Hz
								}
								if(sequence_buttons[sequence_send_counter] == 3){
									led_set_color(3,LED_COLOR_YELLOW);
									games_buzzer_off = false;
									tcc_restart_counter(&tcc2_instance);
									tcc_set_compare_value(&tcc2_instance,0,360); //252Hz
								}
								if(sequence_buttons[sequence_send_counter] == 4){
									led_set_color(4,LED_COLOR_BLUE);
									games_buzzer_off = false;
									tcc_restart_counter(&tcc2_instance);
									tcc_set_compare_value(&tcc2_instance,0,435); //209Hz
								}
							}
							//gamestate = 2;
						}
					}
					break;
				case 3:
					//Waiting for button press
					if((millis - last_button_press_time) > SIMON_BUTTON_TIMEOUT){
						game_over_tune = true;
						gamemode = SIMON_SOLO;
						gamestate = 0;
						//Save score
						uint8_t bytes[2];
						if(sequence_progress > gamedata.simon_solo_high_score){
							bytes[0] = sequence_progress;
							bytes[1] = sequence_progress >> 8;
							rww_eeprom_emulator_write_buffer(EEP_GAME_SIMON_SOLO_HIGH_SCORE, bytes, 2);
							rww_eeprom_emulator_commit_page_buffer();
						}
						break;
					}
				
					uint8_t button = 0;
					if(button1){
						button1 = false;
						button = 1;
					}
					if(button2){
						button2 = false;
						button = 2;
					}
					if(button3){
						button3 = false;
						button = 3;
					}
					if(button4){
						button4 = false;
						button = 4;
					}
					if(button > 0){
						if(button == sequence_buttons[sequence_recv_counter]){ //Correct Button Pressed!
							last_button_press_time = millis;
							sequence_recv_counter ++;
							if(sequence_recv_counter == sequence_send_counter){ //We made it to the end of the current sequence
								sequence_send_counter = 255;
								//led_or_pause = false;
								next_sequence_time = millis + 1000;
								sequence_progress ++;
								gamestate = 2;
							}
						} else { //Wrong button!!!
							//Game Over
							game_over_tune = true;
							gamemode = SIMON_SOLO;
							gamestate = 0;
							//Save score
							uint8_t bytes[2];
							if(sequence_progress > gamedata.simon_solo_high_score){
								bytes[0] = sequence_progress;
								bytes[1] = sequence_progress >> 8;
								rww_eeprom_emulator_write_buffer(EEP_GAME_SIMON_SOLO_HIGH_SCORE, bytes, 2);
								rww_eeprom_emulator_commit_page_buffer();
							}
						}
					}
				
					break;
			}
			break;
		case SIMON_MULTI_PRIMARY:
			switch(gamestate){
				case 0:
					//Send Badge Count
					if(badge_count_ready == true){ //Start the game!
						badge_count_ready = false;
						srand(millis);
						for(int x=0; x<sizeof(sequence_buttons); x++){
							sequence_badges[x] = (rand() % badge_counter_response)+1;
							sequence_buttons[x] = (rand() % 4)+1;
						}
						sequence_send_counter = 0;
						sequence_progress = 0;
						next_sequence_time = 0;
						led_or_pause = false;
						//Play tune and wait a second
						simon_start_tune = true;
						gamestate = 1;
					}
					break;
				case 1:
					//Send sequence
					if(simon_start_tune == false){
						if(millis > next_sequence_time){
							if(led_or_pause == true){ //Finished LED on time
								led_or_pause = false;
								next_sequence_time = millis + pause_time;
								led_set_color(1,LED_COLOR_OFF);
								led_set_color(2,LED_COLOR_OFF);
								led_set_color(3,LED_COLOR_OFF);
								led_set_color(4,LED_COLOR_OFF);
								games_buzzer_off = true;
								sequence_send_counter ++;
							} else { //Finished off delay
								led_or_pause = true;
								if(sequence_send_counter > sequence_progress){
									sequence_recv_counter = 0;
									last_button_press_time = millis;
									incoming_button_press_ready = false;
									gamestate = 2; //Wait for button press
									break;
								}
								//Slightly longer times for multiplayer :)
								led_on_time = 300;//220;
								if(sequence_progress < 10) led_on_time = 400;//320;
								if(sequence_progress < 5) led_on_time = 500;//420;
								next_sequence_time = millis + led_on_time;
								//Send next sequence packet
								send_simon_game_packet(sequence_badges[sequence_send_counter], sequence_buttons[sequence_send_counter]); //Send packet down badge chain
							
								//Turn on next LED in sequence
								if(sequence_badges[sequence_send_counter] == 1){ //That's this badge
									//Turn on LED/play tune
									if(sequence_buttons[sequence_send_counter] == 1){
										led_set_color(1,LED_COLOR_GREEN);
										games_buzzer_off = false;
										tcc_restart_counter(&tcc2_instance);
										tcc_set_compare_value(&tcc2_instance,0,219); //415Hz
									}
									if(sequence_buttons[sequence_send_counter] == 2){
										led_set_color(2,LED_COLOR_RED);
										games_buzzer_off = false;
										tcc_restart_counter(&tcc2_instance);
										tcc_set_compare_value(&tcc2_instance,0,293); //310Hz
									}
									if(sequence_buttons[sequence_send_counter] == 3){
										led_set_color(3,LED_COLOR_YELLOW);
										games_buzzer_off = false;
										tcc_restart_counter(&tcc2_instance);
										tcc_set_compare_value(&tcc2_instance,0,360); //252Hz
									}
									if(sequence_buttons[sequence_send_counter] == 4){
										led_set_color(4,LED_COLOR_BLUE);
										games_buzzer_off = false;
										tcc_restart_counter(&tcc2_instance);
										tcc_set_compare_value(&tcc2_instance,0,435); //209Hz
									}
								
								}
							
							}
							//gamestate = 2;
						}
					}
					break;
				case 2:
					//Waiting for button press
					if((millis - last_button_press_time) > SIMON_BUTTON_TIMEOUT){
						simon_game_over(sequence_progress);
						gamemode = SELECT_GAME;
						break;
					}
					if(incoming_button_press_ready == true){
						incoming_button_press_ready = false;
						if(incoming_badge_number == sequence_badges[sequence_recv_counter] && incoming_badge_button == sequence_buttons[sequence_recv_counter]){ //Correct button pressed!
							last_button_press_time = millis;
							sequence_recv_counter ++;
							if(sequence_recv_counter == sequence_send_counter){ //We made it to the end of the current sequence
								sequence_send_counter = 255;
								//led_or_pause = false;
								next_sequence_time = millis + 1000;
								sequence_progress ++;
								gamestate = 1;
							}
						} else { //Wrong button!!!
							//Game Over
							simon_game_over(sequence_progress);
							gamemode = SELECT_GAME;
						}
						
					}
					uint8_t button = 0;
					if(button1){
						button1 = false;
						button = 1;
					}
					if(button2){
						button2 = false;
						button = 2;
					}
					if(button3){
						button3 = false;
						button = 3;
					}
					if(button4){
						button4 = false;
						button = 4;
					}
					if(button > 0){
						if(sequence_badges[sequence_recv_counter] == 1 && button == sequence_buttons[sequence_recv_counter]){ //Correct Button Pressed!
							last_button_press_time = millis;
							sequence_recv_counter ++;
							if(sequence_recv_counter == sequence_send_counter){ //We made it to the end of the current sequence
								sequence_send_counter = 255;
								next_sequence_time = millis + 1000;
								sequence_progress ++;
								gamestate = 1;
							}
						} else { //Wrong button!!!
							//Game Over
							//Send game over message
							simon_game_over(sequence_progress);
							gamemode = SELECT_GAME;
						}
					}
					
					break;
			}
			break;
		case SIMON_MULTI_SECONDARY:
			switch(gamestate){
				case 0: //Game Start!	
					simon_start_tune = true;
					gamestate = 1;
				case 1: //Waiting for sequence packet
					if(simon_start_tune == false){
						if(incoming_sequence_packet == true){ //New sequence data
							incoming_sequence_packet = false;
							sequence_recv_counter ++;
							if(incoming_badge_number == badge_counter){ //Sequence packet is for us
								if(incoming_badge_button == 1){
									led_set_color(1,LED_COLOR_GREEN);
									games_buzzer_off = false;
									tcc_restart_counter(&tcc2_instance);
									tcc_set_compare_value(&tcc2_instance,0,219); //415Hz
								}
								if(incoming_badge_button == 2){
									led_set_color(2,LED_COLOR_RED);
									games_buzzer_off = false;
									tcc_restart_counter(&tcc2_instance);
									tcc_set_compare_value(&tcc2_instance,0,293); //310Hz
								}
								if(incoming_badge_button == 3){
									led_set_color(3,LED_COLOR_YELLOW);
									games_buzzer_off = false;
									tcc_restart_counter(&tcc2_instance);
									tcc_set_compare_value(&tcc2_instance,0,360); //252Hz
								}
								if(incoming_badge_button == 4){
									led_set_color(4,LED_COLOR_BLUE);
									games_buzzer_off = false;
									tcc_restart_counter(&tcc2_instance);
									tcc_set_compare_value(&tcc2_instance,0,435); //209Hz
								}
								led_on_time = 220;
								if(sequence_recv_counter < 10) led_on_time = 320;
								if(sequence_recv_counter < 5) led_on_time = 420;
								next_sequence_time = millis + led_on_time;
								gamestate = 2;
							}
						
						} else {
							led_set_color(1,LED_COLOR_OFF);
							led_set_color(2,LED_COLOR_OFF);
							led_set_color(3,LED_COLOR_OFF);
							led_set_color(4,LED_COLOR_OFF);
							games_buzzer_off = true;
						}
					}
					
					break;
				case 2: //LED on
					if(millis > next_sequence_time){
						led_set_color(1,LED_COLOR_OFF);
						led_set_color(2,LED_COLOR_OFF);
						led_set_color(3,LED_COLOR_OFF);
						led_set_color(4,LED_COLOR_OFF);	
						games_buzzer_off = true;
						gamestate = 1;
					}
					break;
			}
			//Check buttons
			uint8_t button = 0;
			if(button1){
				button1 = false;
				button = 1;
			}
			if(button2){
				button2 = false;
				button = 2;
			}
			if(button3){
				button3 = false;
				button = 3;
			}
			if(button4){
				button4 = false;
				button = 4;
			}
			if(button > 0){
				send_simon_button_packet(badge_counter, button);
			}
			break;
		case WAM_SOLO:
			
			break;
		case WAM_MULTI:
			gamemode = SELECT_GAME;
			break;
		case WAIT_FOR_START:
			//LED pattern while waiting
			if((millis - last_led_pattern_time) > led_pattern_delay){
				last_led_pattern_time = millis;
				if(led_pattern){
					led_pattern = false;
					led_set_color(1,LED_COLOR_GREEN);
					led_set_color(2,LED_COLOR_RED);
					led_set_color(3,LED_COLOR_YELLOW);
					led_set_color(4,LED_COLOR_BLUE);
				} else {
					led_pattern = true;
					led_set_color(1,LED_COLOR_OFF);
					led_set_color(2,LED_COLOR_OFF);
					led_set_color(3,LED_COLOR_OFF);
					led_set_color(4,LED_COLOR_OFF);
				}
			}
			break;
		
	}
}


void simon_game_over(uint16_t score){
	//Send game over message
	send_simon_game_over(score);
	//Save score
	uint8_t bytes[2];
	if(score > gamedata.simon_multi_high_score){
		bytes[0] = score;
		bytes[1] = score >> 8;
		rww_eeprom_emulator_write_buffer(EEP_GAME_SIMON_MULTI_HIGH_SCORE, bytes, 2);
		rww_eeprom_emulator_commit_page_buffer();
	}
	if(badge_counter_response > gamedata.simon_multi_connections){
		bytes[0] = badge_counter_response;
		bytes[1] = badge_counter_response >> 8;
		rww_eeprom_emulator_write_buffer(EEP_GAME_SIMON_MULTI_CONNECTIONS, bytes, 2);
		rww_eeprom_emulator_commit_page_buffer();
	}
	gamedata.simon_multi_games_played ++;
	bytes[0] = gamedata.simon_multi_games_played;
	bytes[1] = gamedata.simon_multi_games_played >> 8;
	rww_eeprom_emulator_write_buffer(EEP_GAME_SIMON_MULTI_GAMES_PLAYED, bytes, 2);
	rww_eeprom_emulator_commit_page_buffer();
	
	game_over_tune = true;
	gamemode = WAIT_FOR_START;
}


void play_sounds(void){
	
	if(new_connection){
		switch(playstate){
			case 0:
				games_buzzer_off = false;
				tcc_restart_counter(&tcc2_instance);
				tcc_set_compare_value(&tcc2_instance,0,150); //209Hz
				last_time = millis;
				playstate = 1;
				break;
			case 1:
				if(millis - last_time > 100){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 2;
				}
				break;
			case 2:
				if(millis - last_time > 20){
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,100); //252Hz
					
					last_time = millis;
					games_buzzer_off = false;
					playstate = 3;
				}
				break;
			case 3:
				if(millis - last_time > 350){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 0;
					new_connection = false;
				}
				break;
			default:
				new_connection = false;
				playstate = 0;
				break;
		}
	} else if(old_connection){
		switch(playstate){
			case 0:
				games_buzzer_off = false;
				tcc_restart_counter(&tcc2_instance);
				tcc_set_compare_value(&tcc2_instance,0,150); //209Hz
				
				last_time = millis;
				playstate = 1;
				break;
			case 1:
				if(millis - last_time > 100){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					old_connection = false;
					playstate = 0;
				}
				break;
			default:
				old_connection = false;
				playstate = 0;
				break;
		}
	} else if(new_signal_share){
		switch(playstate){
			case 0:
				games_buzzer_off = false;
				tcc_restart_counter(&tcc2_instance);
				tcc_set_compare_value(&tcc2_instance,0,150); //209Hz
				last_time = millis;
				playstate = 1;
				break;
			case 1:
				if(millis - last_time > 100){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 2;
				}
				break;
			case 2:
				if(millis - last_time > 20){
					games_buzzer_off = false;
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,100); //209Hz
					last_time = millis;
					playstate = 3;
				}
				break;
			case 3:
				if(millis - last_time > 100){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 4;
				}
				break;
			case 4:
				if(millis - last_time > 20){
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,90); //252Hz
				
					last_time = millis;
					games_buzzer_off = false;
					playstate = 5;
				}
				break;
			case 5:
				if(millis - last_time > 350){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 0;
					new_signal_share = false;
				}
				break;
			default:
			new_signal_share = false;
			playstate = 0;
			break;
		}
	} else if(simon_start_tune){
		switch(playstate){
			case 0:
				games_buzzer_off = false;
				tcc_restart_counter(&tcc2_instance);
				tcc_set_compare_value(&tcc2_instance,0,435); //209Hz
				last_time = millis;
				playstate = 1;
				break;
			case 1:
				if(millis - last_time > 100){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 2;
				}
				break;
			case 2:
				if(millis - last_time > 20){
					games_buzzer_off = false;
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,360); //252Hz
					
					last_time = millis;
					
					playstate = 3;
				}
				break;
			case 3:
				if(millis - last_time > 100){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 4;
				}
				break;
			case 4:
				if(millis - last_time > 20){
					games_buzzer_off = false;
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,293); //310Hz
					
					last_time = millis;
					
					playstate = 5;
				}
				break;
			case 5:
				if(millis - last_time > 100){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 6;
				}
				break;
			case 6:
				if(millis - last_time > 20){
					games_buzzer_off = false;
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,219); //415Hz
					
					last_time = millis;
					
					playstate = 7;
				}
				break;
			case 7:
				if(millis - last_time > 200){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 8;
				}
				break;
			case 8:
				if(millis - last_time > 500){
					last_time = millis;
					playstate = 0;
					simon_start_tune = false;
				}
				break;
			default:
				simon_start_tune = false;
				playstate = 0;
				break;
		}
	} else if(game_over_tune){
		switch(playstate){
			case 0:
			games_buzzer_off = false;
			tcc_restart_counter(&tcc2_instance);
			tcc_set_compare_value(&tcc2_instance,0,904); //101Hz
			
			last_time = millis;
			playstate = 1;
			break;
			case 1:
			if(millis - last_time > 1000){
				tcc_set_compare_value(&tcc2_instance,0,0); //off
				tcc_stop_counter(&tcc2_instance);
				last_time = millis;
				games_buzzer_off = true;
				game_over_tune = false;
				playstate = 0;
			}
			break;
			default:
			old_connection = false;
			playstate = 0;
			break;
		}
	} else if(challenge_section_finish){
		switch(playstate){
			case 0:
				games_buzzer_off = false;
				tcc_restart_counter(&tcc2_instance);
				tcc_set_compare_value(&tcc2_instance,0,200); //209Hz
				last_time = millis;
				playstate = 1;
				break;
			case 1:
				if(millis - last_time > 100){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 2;
				}
				break;
			case 2:
				if(millis - last_time > 20){
					games_buzzer_off = false;
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,200); //252Hz
				
					last_time = millis;
				
					playstate = 3;
				}
				break;
			case 3:
				if(millis - last_time > 100){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 4;
				}
				break;
			case 4:
				if(millis - last_time > 20){
					games_buzzer_off = false;
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,200); //310Hz
				
					last_time = millis;
				
					playstate = 5;
				}
				break;
			case 5:
				if(millis - last_time > 100){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 6;
				}
				break;
			case 6:
				if(millis - last_time > 20){
					games_buzzer_off = false;
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,100); //310Hz
				
					last_time = millis;
				
					playstate = 7;
				}
				break;
			case 7:
				if(millis - last_time > 250){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 8;
				}
				break;
			case 8:
				if(millis - last_time > 20){
					games_buzzer_off = false;
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,150); //415Hz
				
					last_time = millis;
				
					playstate = 9;
				}
				break;
			case 9:
				if(millis - last_time > 75){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 10;
				}
				break;
			case 10:
				if(millis - last_time > 20){
					games_buzzer_off = false;
					tcc_restart_counter(&tcc2_instance);
					tcc_set_compare_value(&tcc2_instance,0,100); //415Hz
				
					last_time = millis;
				
					playstate = 11;
				}
				break;
			case 11:
				if(millis - last_time > 400){
					tcc_set_compare_value(&tcc2_instance,0,0); //off
					tcc_stop_counter(&tcc2_instance);
					last_time = millis;
					games_buzzer_off = true;
					playstate = 12;
				}
				break;
			case 12:
				if(millis - last_time > 500){
					last_time = millis;
					playstate = 0;
					challenge_section_finish = false;
				}
				break;
			default:
				challenge_section_finish = false;
				playstate = 0;
				break;
		}
	}
}
