/*
 * games.h
 *
 *  Author: compukidmike
 */ 


#ifndef GAMES_H_
#define GAMES_H_

#include "main.h"

#define SEQUENCE_TIME_1 420
#define SEQUENCE_TIME_2 320
#define SEQUENCE_TIME_3 220

#define SIMON_BUTTON_TIMEOUT 3000

/*
Simon Colors
 ______ ______
|      |      |
|Green | Red  |
|______|______|
|      |      |
|Yellow| Blue |
|______|______|

*/

bool new_connection;
bool old_connection;
bool simon_start_tune;
bool game_over_tune;
bool challenge_section_finish;
bool new_signal_share;

void run_games(void);
void simon_game_over(uint16_t score);
void new_connection_tune(void);
void play_sounds(void);

#endif /* GAMES_H_ */