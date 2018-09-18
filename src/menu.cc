#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <cstdlib>
#include <cstdio>

#include "headers/objects.h"
#include "headers/engine.h"
#include "headers/io.h"
#include "headers/design.h"

//DEBUG
#ifdef DEB
static unsigned int MASK = 1 ;
#define DBG(A, B) {if ((A) & MASK) {B; } }
#else
#define DBG(A, B)
#endif
#define D1(a) DBG(1, a)
//

//Extern variables and variables used as extern
extern int WIDTH;
extern int HEIGHT;
extern const char fileScore[];
extern const char fileLevel[];
extern const char fileMute[];
extern const char fileFullScreen[];
extern const char DEFAULT_TTF[];
extern ALLEGRO_FONT *titleFont;
extern ALLEGRO_COLOR myColors[8];
extern int config[];
int DEFAULT_JUMP_SPEED = 8; /*!< Default speed of player jump */
int DEFAULT_JUMP_HEIGHT = 55; /*!< Default height of player jump */
int DEFAULT_Y; /*!< Default height of floor */
int NUM_LEVELS = 5; /*!< Number of levels */ //Add new level in next update

//Global consts for main
static const int NUM_MENU_BUTTONS = 6; /*!< Number of menu buttons (arcade, levels etc) */
static const int NUM_EVENTS = 8; /*!< Number of event to manage */

//Enum for menu buttons,dialog options buttons,context, event type
/**
 * Menu button
 * @see drawMenu()
 * @see setButton();
 */
//UPDATE NUM_MENU_BUTTONS!!!!
enum btn{
	ARCADE,/*!< Start Arcade Game */
	LEVELS,/*!< Level selection */
	OPTIONS,/*!< Game Options */
	INSTRUCTION, /*!<Instructions for user */
	CREDITS, /*!<Credits */
	EXIT/*!< Exit game */
};
/**
 * Dialog options box buttons
 */
enum optionsButton{
	CLOSE,/*!< Close dialog */
	FULLSCREEN,/*!< Set Fullscreen mode */
	MUTE,/*!< Set mute mode */
	RESET/*!< Reset level reached and best score */
};
/**
 * If screen is in menu or in level selection
 */
enum context{
	MENU, /*!< Menu Screen */
	LEVEL /*!< Level Selection Screen */
};
/**
 * What event happened
 */
enum eventType{
	NOTHING, /*!< If nothing happened */
	DISPLAY_CLOSE, /*!< Close display */
	KEYDOWN,/*!< Key down */
	KEYUP,/*!< Release key */
	RESIZE,/*!< Resize display */
	MOVEMOUSE,/*!< Move cursor */
	MOUSEDOWN,/*!< Mouse key down */
	MOUSEUP/*!< Mouse key up */
};

//--------------------------------PROTOTYPES----------------------------
extern int startGame(ALLEGRO_DISPLAY *,int config[],int num,int context);

/**
 * Manage event in relative context
 * -If left buttons on mouse pressed check what button pressed (depend by context)
 * -If mouse is moving, check if it's on a button and focus it
 * -Close display if press X
 * -Manage resize dependencies if display is resizing
 * @param buttons group of buttons that can be pressed
 * @param ev event
 * @param eventType array that contains what event happens
 * @return return what happens
 * @see clickedButtonManager()
 * @see buttonFocus()
 * @see resizeDependencies()
 */
static int eventManager(Buttons &buttons,ALLEGRO_EVENT &ev,bool eventType[]);
/**
 * Says what button clicked, run when mousedown event is true
 * @param buttons group of buttons to check
 * @param x point x of mouse click
 * @param y point y of mouse click
 */
void clickedButtonManager(Buttons &buttons,int x,int y);
/**
 * says what button focused, run when movemouse event is true
 * @param buttons group of buttons to check
 * @param x point x of mouse
 * @param y point y of mouse
 */
void buttonFocus(Buttons buttons,int x,int y);
/**
 * Manage buttons in dialog options
 * @param button dialog options button pressed \ref optionsButton
 * @param display dispay which draw dialog
 */
static void optionsButtonManager(int button,ALLEGRO_DISPLAY *display);

/**
 * Inits menu buttons, they have got same x and different y
 * @param buttons group of buttons
 * @param textFont font of menu buttons
 * @param x x of buttons
 * @param y y of buttons
 * @see initLevelsButtons()
 */
void initMenuButtons(Buttons buttons,ALLEGRO_FONT *textFont,int x,int y);
/**
 * Inits levelsButtons
 * @param buttons group of buttons
 * @param font font of text buttons
 */
void initLevelsButtons(Buttons buttons,ALLEGRO_FONT *font);
/**
 * Set false to all bool variables in array
 * @param v Array of bool
 * @param size array size
 */
static void allFalse(bool v[],int size);

/**
 * there are many variables need to update after resize display
 * @param width new width
 * @param height new height
 * @param menu_x variable to update
 * @param menu_y variable to update
 */
static void resizeLocalDependences(int width,int height,int &menu_x,int &menu_y);
//-------------------------------------------------------------------------

/**
 * Start menu, show menu buttons and levels button and allow to manage resize
 * and start new game.
 * @param display display to draw
 */
int menu(ALLEGRO_DISPLAY *display){

	//Local Variables
	bool done = false; /*!< Checks main cycle */
	int redraw = -1; /*!< Checks if display need to redraw */
	int menu_x; /*!< X Position of menu buttons */
	int menu_y; /*!< Y Position of menu buttons */
	int pos_x; /*!< X Position of mouse */
	int pos_y; /*!< Y Position of mouse */
	Buttons menuButtons; /*!< Group of menu buttons @see Buttons @see Button */
	Buttons levelsButtons; /*!< Group of level selection buttons @see Buttons @see Button*/
	bool eventType[NUM_EVENTS]; /*!< What's event happened @see eventType */
	const char menuOgg[] = "../res/music/menu.ogg";

	//Allegro Variables
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_SAMPLE *menuSample = NULL;
	ALLEGRO_SAMPLE_INSTANCE *menuSampleInstance = NULL;

	//Check init of allegro
		if(!al_init()) {
			std::cerr<<"Errore init allegro";
			return -1;
		}
		if (!display) {
			std::cerr<<"Errore init display";
			return -1;
		}

	//Install and init
	al_init_native_dialog_addon();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_audio();
	al_init_acodec_addon();
	al_install_keyboard();
	al_install_mouse();
	event_queue = al_create_event_queue();
	//inits best score and last level passed from file or default if doesn't exist
	loadFromFile(config[SCORE],fileScore);
	loadFromFile(config[LASTLEVELPASSED],fileLevel);
	loadFromFile(config[MUSICOFF],fileMute);
	allFalse(eventType,NUM_EVENTS);

	//Colors and Fonts and musics
	ALLEGRO_FONT *fontButton = al_load_font(DEFAULT_TTF,20,ALLEGRO_ALIGN_CENTRE);
	ALLEGRO_FONT *fontBest = al_load_font(DEFAULT_TTF,18,0);
	ALLEGRO_FONT *fontLevel = al_load_font(DEFAULT_TTF,20,ALLEGRO_ALIGN_CENTRE);
	al_reserve_samples(1);
	menuSample = al_load_sample(menuOgg);
	menuSampleInstance = al_create_sample_instance(menuSample);
	al_set_sample_instance_playmode(menuSampleInstance,ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(menuSampleInstance,al_get_default_mixer());
	if (!config[MUSICOFF])
		al_play_sample_instance(menuSampleInstance);

	//Init menu and levels buttons
	menuButtons.button = new Button[NUM_MENU_BUTTONS];
	menuButtons.size = NUM_MENU_BUTTONS;
	menuButtons.context = MENU;
	menuButtons.activeButton = -1;
	menuButtons.button[ARCADE].text = "Arcade";
	menuButtons.button[LEVELS].text = "Levels";
	menuButtons.button[OPTIONS].text = "Options";
	menuButtons.button[INSTRUCTION].text = "Instruction";
	menuButtons.button[CREDITS].text = "Credits";
	menuButtons.button[EXIT].text = "Exit";
	levelsButtons.button = new Button[NUM_LEVELS];
	levelsButtons.size = NUM_LEVELS;
	levelsButtons.context = LEVEL;
	levelsButtons.activeButton = -1;

	//set event sources
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_mouse_event_source());

	//In boot can resize screen so set right positions
	resizeDisplay(WIDTH,HEIGHT);
	resizeLocalDependences(WIDTH,HEIGHT,menu_x,menu_y);
	initMenuButtons(menuButtons,fontButton,menu_x,menu_y);

	//main cycle
	while (!done) {
		ALLEGRO_EVENT ev;
		//If need redraw, run relative function
		if(redraw) {

			/*
			 * if display resize, change width and height
			 * with newest values and reinit menu buttons
			 */
			if (eventType[RESIZE]) {
				resizeLocalDependences(ev.display.width,ev.display.height,menu_x,menu_y);
				resizeDisplay(ev.display.width,ev.display.height);
				initMenuButtons(menuButtons,fontButton,menu_x,menu_y);
				al_flip_display();
				al_clear_to_color(myColors[BLACK]);
			}

			/*Start game as arcade
			 * so num_level have to be 0 and context must to be STARTARCADE
			 */
			if (menuButtons.activeButton == ARCADE) {
				al_stop_sample_instance(menuSampleInstance);
				startGame(display,config,0,STARTARCADE);
				al_play_sample_instance(menuSampleInstance);
			}
			//Open window of levels
			else if (menuButtons.activeButton == LEVELS) {
				bool isStarted = false; //if game started at least one time
				al_clear_to_color(myColors[BLACK]);
				loadFromFile(config[LASTLEVELPASSED],fileLevel);
				initLevelsButtons(levelsButtons,fontLevel);
				/*
				 * if game never started wait for level selection or for esc button;
				 * Verify what level pressed and manage if unlocked or not;
				 * In case it unlocked start that level;
				 * In case it locked show warning pop up.
				 */
				while (!isStarted) {
					al_wait_for_event(event_queue,&ev);
					D1(al_draw_textf(fontBest,myColors[DEFAULT_COLOR],0,200,0,"last level: %i",config[LASTLEVELPASSED]));
					redraw = eventManager(levelsButtons,ev,eventType);
					if (redraw == DISPLAY_CLOSE) {
						isStarted = true; //for exit from level select
						done = true;
					}
					if (eventType[KEYDOWN])
						if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
							isStarted = true;
					if (eventType[RESIZE]) {
						resizeLocalDependences(ev.display.width,ev.display.height,menu_x,menu_y);
						resizeDisplay(ev.display.width,ev.display.height);
						initLevelsButtons(levelsButtons,fontButton);
						al_flip_display();
						al_clear_to_color(myColors[BLACK]);
					}
					if (eventType[MOUSEDOWN]){
						for (int i=0;i<NUM_LEVELS;i++) {
							//verify what button pressed
							if (levelsButtons.activeButton == i){
								levelsButtons.activeButton = -1; //reinit to -1
								//If select level is locked or not
								if (i <= config[LASTLEVELPASSED]){
									isStarted = true;
									al_stop_sample_instance(menuSampleInstance);
									startGame(display,config,i+1,STARTLEVEL);
									al_play_sample_instance(menuSampleInstance);
									loadFromFile(config[LASTLEVELPASSED],fileLevel); //reload last passed level in case changed
									break;
								}
								else
									al_show_native_message_box(display,"Locked","Level Locked","Before you must pass previous level",NULL,ALLEGRO_MESSAGEBOX_WARN);
							}
						}
					}
					allFalse(eventType,NUM_EVENTS);
					drawLevels(levelsButtons.button,levelsButtons.size);
					al_flip_display();
					al_clear_to_color(myColors[BLACK]);
				}
			}
			//Options menu
			else if (menuButtons.activeButton == OPTIONS) {
				char buttonsOptions[40];
				/*
				 * buttonOptions changes depending on configs file
				 */
				strcpy(buttonsOptions,"Close|");
				if (config[FULLSCREENOFF])
					strcat(buttonsOptions,"Fullscreen On|");
				else
					strcat(buttonsOptions,"Fullscreen Off|");
				if (config[MUSICOFF])
					strcat(buttonsOptions,"Music On|");
				else
					strcat(buttonsOptions,"Music Off|");
				strcat(buttonsOptions,"Reset");
				int buttonOption = al_show_native_message_box(
						display,
						"Options",
						"Options:",
						"You can reset best score and "
						"reached level, turn off/on music,"
						" or enable/disable fullscreen",
						buttonsOptions,
						0);
				optionsButtonManager(buttonOption-1,display);
			}
			//Instruction
			else if (menuButtons.activeButton == INSTRUCTION) {
				al_show_native_message_box(
					display,
					"Instruction",
					"How to play:",
					"You have to avoid from enemies pressing"
					" up or down buttons",
					NULL,
					NULL);
			}
			//Credits
			else if (menuButtons.activeButton == CREDITS) {
				al_show_native_message_box(
					display,
					"Credits",
					"Special thanks to:",
					"Author: Mione Simone\n"
					"Idea: Mione Simone\n"
					"Songs: Internet\n"
					"Pictures: Internet\n"
					"Design tips: Madeccia Emanuela\n"
					"Icon: Montorsi Enrico ",
					NULL,
					0);
			}
			else if (menuButtons.activeButton == EXIT || redraw == DISPLAY_CLOSE) {
				done = true;
			}

			//Reinit varibles events
			redraw = false;
			menuButtons.activeButton = -1;
			allFalse(eventType,NUM_EVENTS);
			//Final update display
			D1(al_draw_textf(fontBest,myColors[DEFAULT_COLOR],0,0,0,"WIDTH: %i HEIGHT: %i POS_X: %i POS_Y: %i",WIDTH,HEIGHT,ev.mouse.x,ev.mouse.y));
			drawButtons(menuButtons.button,menuButtons.size);
			//Draw Title
			al_draw_text(titleFont,al_map_rgb(255,255,255),
					WIDTH/2,0,ALLEGRO_ALIGN_CENTRE,"RUN CIRREST RUN");
			//Draw best arcade score
			al_draw_textf(fontBest,myColors[DEFAULT_COLOR],0,HEIGHT-fontBest->height,0,"BEST: %i",config[SCORE]);
			al_flip_display();
			al_clear_to_color(myColors[BLACK]);
		}

		al_wait_for_event(event_queue, &ev);
		//redraw if there is an interesting event
		redraw = eventManager(menuButtons,ev,eventType);
		al_flush_event_queue(event_queue);
	}
	saveToFile(config[MUSICOFF],fileMute);
	saveToFile(config[FULLSCREENOFF],fileFullScreen);

	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_font(fontBest);
	al_destroy_font(fontButton);
	al_destroy_font(fontLevel);
	al_destroy_timer(timer);
	delete [] menuButtons.button;
	delete [] levelsButtons.button;
	return 0;
}

//----------------------FUNCTIONS---------------------------------------------
static int eventManager(Buttons &buttons,ALLEGRO_EVENT &ev,bool eventType[]){
	//Check what button i pressed
	if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
		eventType[MOUSEDOWN] = true;
		if (ev.mouse.button & 1) {
			int pos_x = ev.mouse.x;
			int pos_y = ev.mouse.y;
			clickedButtonManager(buttons,pos_x,pos_y);
			return -1;
		}
	}
	//When move mouse check if focus any button
	if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
		eventType[MOVEMOUSE] = true;
		int pos_x = ev.mouse.x;
		int pos_y = ev.mouse.y;
		buttonFocus(buttons,pos_x,pos_y);
		return -1;
	}
	if ((ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)) {
		int exit = al_show_native_message_box(ev.display.source,"Exit","Are you sure?","Are you sure do you want exit?",
				NULL,ALLEGRO_MESSAGEBOX_YES_NO);
		if (exit == 1)
			return DISPLAY_CLOSE;
		else
			return -1;
	}
	if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		eventType[KEYDOWN] = true;
		return -1;
	}
	if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE){
		eventType[RESIZE] = true;
		resizeDisplay(ev.display.width,ev.display.height);
		al_acknowledge_resize(ev.display.source);
		al_resize_display(ev.display.source,WIDTH,HEIGHT);
		return -1;
	}
	return NOTHING;
}
void clickedButtonManager(Buttons &buttons,int x,int y){
	/*
	 * checks where mouse clicked (coordinates) and set
	 * what is the button in that position
	 */
	for (int i=0;i<buttons.size;i++) {
		if ( (x > buttons.button[i].x && x < buttons.button[i].x2)
				&& (y > buttons.button[i].y && y < buttons.button[i].y2 ) ) {
			buttons.activeButton = i;
			return;
		}
	}
}
void buttonFocus(Buttons buttons,int x,int y){
	int i = 0;
	int size = NUM_MENU_BUTTONS;
	if (buttons.context == MENU) {
		for (;i<size;i++) {
				if ((x > buttons.button[0].x && x < buttons.button[0].x2) &&
						(y > buttons.button[i].y && y < buttons.button[i].y2) ) {
					buttons.button[i].buttonColor = DEFAULT_FOCUS_COLOR;
					buttons.button[i].textColor = DEFAULT_FOCUS_COLOR;
					return;
				}
		else {
			buttons.button[i].buttonColor = DEFAULT_COLOR;
			buttons.button[i].textColor = DEFAULT_COLOR;
		}
		}
	}
	else if (buttons.context == LEVELS){
		size = NUM_LEVELS;
		for (;i<size;i++) {
			if ((x > buttons.button[i].x && x < buttons.button[i].x2) &&
					(y > buttons.button[0].y && y < buttons.button[0].y2) ) {
				if (buttons.button[i].buttonColor != GREY) {
					buttons.button[i].buttonColor = DEFAULT_FOCUS_COLOR;
					buttons.button[i].textColor = DEFAULT_FOCUS_COLOR;
					return;
				}
			}
			else {
				if (buttons.button[i].buttonColor != GREY){
					buttons.button[i].buttonColor = DEFAULT_COLOR;
					buttons.button[i].textColor = DEFAULT_COLOR;
				}
			}
		}
	}
}
static void optionsButtonManager(int button,ALLEGRO_DISPLAY *display){
	switch (button) {
	//Remove files and restore them whit default values
	case RESET:
		remove(fileLevel);
		remove(fileScore);
		loadFromFile(config[LASTLEVELPASSED],fileLevel);
		loadFromFile(config[SCORE],fileScore);
		break;
	//Change musicon/musicoff
	case MUTE:
		config[MUSICOFF] = !config[MUSICOFF];
		break;
	//Change fullscreenon/off, will apply at reboot
	case FULLSCREEN:
		config[FULLSCREENOFF] = !config[FULLSCREENOFF];
		al_show_native_message_box(display,"NOTE",
				"Note","This option will apply at reboot",
				NULL,0);
		break;
	}
}

void initMenuButtons(Buttons menuButtons, ALLEGRO_FONT *font,int x, int y){
	//Set all menu button with default values
	for (int i=0; i<menuButtons.size; i++) {
		setButton(menuButtons.button[i],
				x,
				y+i*(font->height+10),
				x + (font->height/2*10), //Change last '10' for buttons bigger
				y+i*(font->height+10) +23,
				menuButtons.button[i].text,
				font,
				DEFAULT_COLOR,
				DEFAULT_COLOR);
	}
}
void initLevelsButtons(Buttons buttons,ALLEGRO_FONT *font){
	//Set all  buttolevelsn with default values
	int x = 20;
	int color = DEFAULT_COLOR;
	for (int i=0;i<NUM_LEVELS;i++){
		if (i > config[LASTLEVELPASSED])
			color = GREY;
		setButton(buttons.button[i],x,20,x+40,60,NULL,font,color,color);
		x +=50;
	}
}
static void allFalse(bool v[],int size){
	for (int i=0;i<size;i++)
		v[i] = false;
}

static void resizeLocalDependences(int width,int height,int &menu_x,int &menu_y){
	menu_x = WIDTH / 2; //Update menu x
	menu_y = HEIGHT / 2; //Update menu y
	return;
}
