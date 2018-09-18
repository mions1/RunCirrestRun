/*! \mainpage Run Cirrest, Run
 *
 * \section intro_sec What is?
 *
 * Run Cirrest, Run is a horizontal scroll game created by Mions Technologies.
 *
 * \section install_sec Installation
 * 		1. Uncompress archive file with command: "tar -zxvf filename.tar.gz"
 * 		2. Open "Run Cirrest, Run"->bin directory with terminal
 * 		3. Compile game with command: "Make"
 * \section run_sec Run Game
 * 		1. Open "Run Cirrest, Run"->bin directory
 * 		2. Run run.out
 *  \section doc_sec Getting start with docs
 *  	see \ref main.cc doc
 *
 */

#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <cstdlib>
#include <cstdio>

#include "headers/objects.h"
#include "headers/io.h"
#include "headers/engine.h"

//DEBUG
#ifdef DEB
static unsigned int MASK = 1 ;
#define DBG(A, B) {if ((A) & MASK) {B; } }
#else
#define DBG(A, B)
#endif
#define D1(a) DBG(1, a)
//

//Variables used as extern
int config[4]; /*!< Configs saved of game (Fullscreen,mute,score etc) */
ALLEGRO_FONT *titleFont; /*!< Font of title */
int WIDTH = 780;/*!< Actual WIDTH of screen */
int HEIGHT = 480;/*!< Actual HEIGHT of screen */
char fileScore[] = "../res/config/score.dat"; /*!< File to save best score */
char fileLevel[] = "../res/config/level.dat"; /*!< File to save level reached */
char fileMute[] = "../res/config/mute.dat"; /*!< File to save if mute or not */
char fileFullScreen[] = "../res/config/fullscreen.dat"; /*!< File to save if fullscreen or not */
char DEFAULT_TTF[] = "../res/font/arial.ttf"; /*!< Theme TTF */
char TITLE_TTF[] = "../res/font/chambre.ttf"; /*!< Title TTF */
ALLEGRO_COLOR myColors[8]; /*!< My Colors @see colors */

//Extern function
/**
 * Declared in \ref anim.cc
 */
extern int myBoot(ALLEGRO_DISPLAY *display);
/**
 * Declared in \ref menu.cc
 */
extern int menu(ALLEGRO_DISPLAY *display);

/**
 * Main do: load all configs from files,creates display, creates colors,creates titlefont, start boot and then start menu
 * @see myBoot()
 * @see menu()
 */
int main(){

	//Allegro Variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_MONITOR_INFO monitorInfo;

	//Check init of allegro
		if(!al_init()) {
			std::cerr<<"Errore init allegro";
			return -1;
		}
		//Get saved fullscreen config
		loadFromFile(config[FULLSCREENOFF],fileFullScreen);
		if (config[FULLSCREENOFF]) {
			al_set_new_display_flags(ALLEGRO_RESIZABLE);
		}
		else {
			//Get monitor resolution for right display size
			al_get_monitor_info(0,&monitorInfo);
			WIDTH = monitorInfo.x2 - monitorInfo.x1 + 1;
			HEIGHT = monitorInfo.y2 - monitorInfo.x1 + 1;
			al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
		}
		display = al_create_display(WIDTH,HEIGHT);
		al_set_window_title(display,"Run Cirrest, Run");
		al_init_image_addon();
		al_set_display_icon(display,loadBitmap("../res/img/icon.png",100,100));
		if (!display) {
			std::cerr<<"Errore init display";
			return -1;
		}
	//

	al_init_font_addon();
	al_init_ttf_addon();

	//inits best score and last level passed from file or default if doesn't exist
	loadFromFile(config[SCORE],fileScore);
	loadFromFile(config[LASTLEVELPASSED],fileLevel);
	loadFromFile(config[MUSICOFF],fileMute);

	//Colors, Fonts
	myColors[RED] = al_map_rgb(255,0,0);
	myColors[GREEN] = al_map_rgb(0,255,0);
	myColors[BLUE] = al_map_rgb(0,0,255);
	myColors[WHITE] = al_map_rgb(255,255,255);
	myColors[BLACK] = al_map_rgb(0,0,0);
	myColors[GREY] = al_map_rgb(30,30,30);
	myColors[DEFAULT_FOCUS_COLOR] = myColors[WHITE]; //Default theme foucs color, cyano
	myColors[DEFAULT_COLOR] = al_map_rgb(0,128,128); //Default theme color, cyano
	titleFont = al_load_font(TITLE_TTF,80,0);

	//Start boot and then menu
	myBoot(display);
	menu(display);

}
