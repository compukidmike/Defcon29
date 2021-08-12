/*
 * serialconsole.h
 *
 *  Author: compukidmike
 */ 


#ifndef SERIALCONSOLE_H_
#define SERIALCONSOLE_H_



#include "main.h"

static const uint8_t SERIAL_SPLASH[] = "Welcome to DEF CON 29!";
static const uint8_t MKFACTOR1[] = "         ________";
static const uint8_t MKFACTOR2[] = "Badge   / |\\/|_/ \\";
static const uint8_t MKFACTOR3[] = "Design  | |  | \\ |";
static const uint8_t MKFACTOR4[] = "By      \\_FACTOR_/";
#if BADGE_TYPE == 1 //Human
static const uint8_t THANK_YOU_TYPE[] = "";
#elif BADGE_TYPE == 2 //Goon
static const uint8_t THANK_YOU_TYPE[] = "Thank you for being a DEF CON Goon!";
#elif BADGE_TYPE == 4 //Creator
static const uint8_t THANK_YOU_TYPE[] = "Thank you for being a DEF CON Creator!";
#elif BADGE_TYPE == 8 //Speaker
static const uint8_t THANK_YOU_TYPE[] = "Thank you for being a DEF CON Speaker!";
#elif BADGE_TYPE == 16 //Artist
static const uint8_t THANK_YOU_TYPE[] = "Thank you for being a DEF CON Artist!";
#elif BADGE_TYPE == 32 //Vendor
static const uint8_t THANK_YOU_TYPE[] = "Thank you for being a DEF CON Vendor!";
#elif BADGE_TYPE == 64 //Press
static const uint8_t THANK_YOU_TYPE[] = "Thank you for being DEF CON Press!";
#elif BADGE_TYPE == 128 //UBER
static const uint8_t THANK_YOU_TYPE[] = "Congrats for earning an UBER badge!";
#endif
static const uint8_t CHALLENGE_STATUS[] = "***Challenge Status***";
static const uint8_t CHALLENGE_BADGE_TYPES[] = "Badge Types Collected: ";
static const uint8_t CHALLENGE_BADGES_CONNECTED[] = "Number of Badges Connected: ";
static const uint8_t CHALLENGE_SIGNAL_SHARES[] = "Times You've Shared the Signal: ";
static const uint8_t WEBSITE[] = "defcon.org/signal";
static const uint8_t WEBSITE_COLLECTED_SIGNAL[] = {248,162,131,151,155,140,166,193,128,140,200,144,173,163,205,130,163,162};//"/GottaCatchThemAll"
static const uint8_t WEBSITE_SHARED_SIGNAL[] = {248,178,132,130,155,164,131,233,160,128,204,160,156,169,213};//"/WhatIfIToldYou"
static const uint8_t WEBSITE_MATRIX[] = {248,188,131,150,157,167,138,213,134,129,197,189,135,163,199,170,161,189};//"/YourJourneyBegins"
static const uint8_t WEBSITE_MAX[] = {248,178,141,151,140,133,188,207,129,157,232,161,164,162};//"/WatchYourHead"
static const uint8_t WEBSITE_ALIENS[] = {248,172,152,180,142,158,177,207,128,142,204,168,188,135,204,170,170,160,211};//"/ItWasTotallyAliens"
static const uint8_t MENU[] = "Badge Menu:";
static const uint8_t OPTION1[] = "[1]: Change LED Colors";
static const uint8_t OPTION2[] = "[2]: Change Keymap";
static const uint8_t OPTION3[] = "[3]: Reset EEPROM";
static const uint8_t OPTION4[] = "[4]: Generate Virtual Badge Connection Request";
static const uint8_t OPTION5[] = "[5]: Enter Virtual Badge Connection Request or Reply";
static const uint8_t RESET[] = "Are you sure to want to reset the EEPROM? This will erase all your settings, but will keep your game/challenge data!";
static const uint8_t INPUT[] = "Choose an option:";
static const uint8_t INVALID[] = "Invalid Input. Please try again:";
static const uint8_t KEYS1[] = " _____ _____   _____";
static const uint8_t KEYS2[] = "|     |     | |     |";
static const uint8_t KEYS3[] = "|  1  |  2  | |  5  |";
static const uint8_t KEYS4[] = "|_____|_____| |_____|";
static const uint8_t KEYS5[] = "|  3  |  4  | |  6  |";
static const uint8_t LEDSELECT[] = "Which LED would you like to change?";
static const uint8_t ENTERCOLORS[] = "Enter color values from 0-255 for LED ";
static const uint8_t RED[] = "Red value:";
static const uint8_t GREEN[] = "Green value:";
static const uint8_t BLUE[] = "Blue value:";
static const uint8_t COLORSAVE[] = "Would you like to save the color for LED ";
static const uint8_t YESNO[] = "Yes(y) or No(n):";
static const uint8_t KEYSELECT[] = "Which Key would you like to change?";
static const uint8_t KEYENTRY[] = "Enter new keystroke for key ";
static const uint8_t KEYSTROKE[] = "New keystroke:";
static const uint8_t KEYOPTIONS1[] = "You may enter a single character or string of characters.";
static const uint8_t KEYOPTIONS2[] = "Modifiers go before the character they affect";
static const uint8_t KEYOPTIONS3[] = "Available modifiers: [ctrl][alt][shift][gui]";
static const uint8_t KEYOPTIONS4[] = "Media keys: [play][next][back][stop][eject][mute][vol+][vol-]";
static const uint8_t KEYOPTIONS5[] = "Other keys: [F1]-[F24] [none] (to disable key)";
static const uint8_t TOOLONG[] = "ERROR: Too long. Please try again:";
static const uint8_t GENERATE_STRING[] = "Copy this string and send it to fellow attendees. Their badge will generate a reply string for your badge.";
static const uint8_t GENERATE_REPLY[] = "Send this string back to the person who gave you the request.";
static const uint8_t REPLY_STRING[] = "Enter the request or reply string:";
static const uint8_t ENTER_CONTINUE[] = "Press ENTER to continue...";
static const uint8_t NEW_BADGE[] = "Badge successfully connected!";
static const uint8_t NEW_BADGE_TYPE[] = "New badge type collected!!";
static const uint8_t SIGNAL_SHARED[] = "Signal shared!!!";


static const uint8_t GAME[] = "*** Game Stats ***";
static const uint8_t GAME1[] = "Simon High Score: ";
static const uint8_t GAME2[] = "Multiplayer Simon High Score: ";
static const uint8_t GAME3[] = "Multiplayer Simon Games Played: ";
static const uint8_t GAME4[] = "Multiplayer Simon Longest Badge Chain: ";


static const uint8_t CHALLENGE1[] = "***Badge Challenge Status***";
static const uint8_t CHALLENGE2[] = "Signal Parts Collected: ";
static const uint8_t CHALLENGE3[] = "Badges Connected: ";
static const uint8_t CHALLENGE4[] = "Signal Shares: ";
static const uint8_t CHALLENGE5[] = "You have collected all the signal parts! Now that you have the signal, you must share it with others. Connect to at least 20 people who have not yet collected all the badge types.";
static const uint8_t CHALLENGE6[] = "Thank you for doing your part to keep the signal going! To continue your journey, go to:";

#if DEBUG > 0
static const uint8_t DEBUG1[] = "Which parameter do you want to change?";
static const uint8_t DEBUGOPTION1[] = "[1]: Badge Types Collected";
static const uint8_t DEBUGOPTION2[] = "[2]: Number of Badges Connected";
static const uint8_t DEBUGOPTION3[] = "[3]: Number of Signal Shares";
static const uint8_t DEBUGOPTION4[] = "[4]: Simon Solo High Score";
static const uint8_t DEBUGOPTION5[] = "[5]: Simon Multiplayer High Score";
static const uint8_t DEBUGOPTION6[] = "[6]: Simon Multiplayer Number of Badges";
static const uint8_t DEBUGOPTION7[] = "[7]: Simon Multiplayer Number of Games Played";
static const uint8_t DEBUGOPTION8[] = "[8]: WAM Solo High Score";
static const uint8_t DEBUGOPTION9[] = "[9]: WAM Multiplayer High Score";
static const uint8_t DEBUGOPTIONA[] = "[a]: WAM Multiplayer Number of Badges";
static const uint8_t DEBUGOPTIONB[] = "[b]: WAM Multiplayer Number of Games Played";
static const uint8_t DEBUGOPTIONC[] = "[c]: WAM Multiplayer Number of Wins";
static const uint8_t BINARY[] = "Enter 8 bit binary bitmask. Badge types are: Human,Goon,Creator,Speaker,Artist,Vendor,Press,UBER: ";
static const uint8_t UINT8[] = "Enter number from 0-255: ";
static const uint8_t UINT16[] = "Enter number from 0-65535: ";
#endif


static const uint8_t ascii_to_hid[128] = { //Convert from ascii to HID codes. Larger than 128 requires shift modifier.
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	44,30+128,52+128,50,33+128,34+128,36+128,52,38+128,39+128,37+128,46+128,54,45,55,56,
	39,30,31,32,33,34,35,36,37,38,51+128,51,54+128,46,55+128,56+128,
	31+128,4+128,5+128,6+128,7+128,8+128,9+128,10+128,11+128,12+128,13+128,14+128,15+128,16+128,17+128,18+128,
	19+128,20+128,21+128,22+128,23+128,24+128,25+128,26+128,27+128,28+128,29+128,47,49,48,35+128,45+128,
	53,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
	20,21,22,23,24,25,26,27,28,29,47+128,49+128,48+128,53+128,0
};

void updateSerialConsole(void);

void showSplashScreen(void);

void decrypt(const uint8_t *encoded_string, uint8_t length);

void my_itoa(uint32_t sn, char* outbuf);

#endif /* SERIALCONSOLE_H_ */ 