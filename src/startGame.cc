#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <cstring>

#include "headers/objects.h"
#include "headers/engine.h"
#include "headers/design.h"
#include "headers/io.h"

//DEBUG
#ifdef DEB
static unsigned int MASK = 1;

#define DBG(A, B) {if ((A) & MASK) {B; } }
#else
#define DBG(A, B)
#endif
//
#define D1(a) DBG(1, a)
#define D2(a) DBG(2, a)

//Extern
extern const int WIDTH;
extern const int HEIGHT;
extern const char fileScore[];
extern const char fileLevel[];
extern const char fileMute[];
extern ALLEGRO_COLOR myColors[];
extern const char DEFAULT_TTF[];
extern const int DEFAULT_Y;
extern int config[MUSICOFF];
extern int IMAGE_SPEED;
extern const int NUM_LEVELS;

// Static consts and variables for this file
static bool paused; /*!< If game paused */
static bool over; /*!< Check end of game, so return to menu */
static const int NUM_KEY = 3; /*!< Number of key on keyboard that do something when pressed */
static const int NUM_SKY_OBJECTS = 2; /*!< Number of sky images */
/*!< Path of images for draw sky objects */
static char skyObjectsPath[NUM_SKY_OBJECTS][50] =
		{"../res/img/skyObjects/plane.png","../res/img/skyObjects/cloud.png"};
static bool KEY[NUM_KEY]; /*!< What key pressed */

/**
 * Name Sky Objects
 */
enum skyObject{
	PLANE, /*!< Plane */
	CLOUD /*!< Cloud */
};
/**
 * Buttons in dialog box
 */
enum optionsButton{
	CLOSE, /*!< Close Dialog */
	MUTE, /*!< Mute Music */
	EXIT /*!< Exit game */
};
/**
 * Key pressed
 */
enum key{
	UP, /*!< UP key */
	DOWN, /*!< DOWN key */
	ESC /*!< ESCAPE key */
};

//-----------------------PROTOTYPES------------------------------------------
/**
 * Check if countDown finished, so start game
 * @param countDown countdown to check
 * @return true if countdown finished
 */
static bool checkCountDown(int countDown);
/**
 * Set new score if it's greater than old
 * @param new_score New score done
 * @param old_score Actual best score
 * @return true if new_score if greater than old
 */
static bool setScore(int new_score,int &old_score);
/**
 * Manage dialog button
 * @param button button pressed \ref optionsButton
 * @return optionsButton enum
 * @see optionsButton
 */
static void optionsButtonManage(int button);
/*
 * Manage event
 * -Jump if up/down pressed
 * -pause if escape pressed
 * @param ev event
 * @return false if anything happened
 */
static bool eventManage(ALLEGRO_EVENT &ev);
/**
 * All false every record on array
 * @param v array
 * @param size size of array
 */
static void allFalse(bool v[],int size);
/**
 * Init all those objects
 * @param player player
 * @param level level
 * @param num level number (if i'm not in arcade)
 * @param enemy enemy
 * @param flag flag
 */
static void initAll(Player &player,Level &level,int num,Enemy &enemy,Flag &flag);
/**
 * Init sky images
 * @param images group of images
 */
void initSkyObjectsImages(Images images);

/**
 * Start the real game.
 * If I'm in arcade mode, game haven't end, when player die check if
 * defeat best score, in case save it.
 * If I'm in level mode, when i finish flag is created, and when it go out of
 * screen start next level and save level reached.
 * Going on with game, enemies' speed and probability that a enemy is create increases.
 * @param display display to draw
 * @param config used for check mute,fullscreen and score
 * @param num num of level (0 if arcade)
 * @param context if arcade or level \ref startGameContext
 */
int startGame(ALLEGRO_DISPLAY *display,int config[],int num,int context){

	ALLEGRO_FONT *fontLoading = al_load_font(DEFAULT_TTF,20,0);
	loading(fontLoading,DEFAULT_COLOR);
	al_flip_display();
	al_clear_to_color(myColors[BLACK]);

	//Variables and consts
	bool redraw = true; /*!< If display need to redraw */
	const int FPS = 60; /*!< Frame for seconds */
	const int SECS = 2; /*!< Seconds of count down */
	int countDown = (FPS * SECS)-1; /*!< Every frame decrease count down so secs must be moltiplied by fps */
	paused = false;
	over = false;
	const char gameOgg[] = "../res/music/arcade.ogg"; /*!< Background music file */
	const char bipOgg[] = "../res/music/bip.ogg"; /*!< Count down bip music file */

	//Objects Variables
	Player player; /*!< Player (Hero) @see Player */
	Enemy enemy; /*!< Enemy (obstacle) @see Enemy */
	Level level; /*!< Level @see Level */
	Flag flag; /*!< Victory flag @see Flag */
	Images skyObjects; /*!< Group of sky images @see Images */
	Image background; /*!< Background image @see Image */

	//Allegro Variables
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *fontScore = NULL;
	ALLEGRO_FONT *fontCountDown = NULL;
	ALLEGRO_SAMPLE *gameSample = NULL;
	ALLEGRO_SAMPLE *bipSample = NULL;
	ALLEGRO_SAMPLE_INSTANCE *gameSampleInstance = NULL;
	ALLEGRO_SAMPLE_INSTANCE *bipSampleInstance = NULL;

	/*Init Allegro*/
		if(!al_init()) {
			std::cerr<<"Errore init allegro";
			return -1;
		}
		//in game mode can't resize display and
		//whit no frame is more immersive
		if (config[FULLSCREENOFF])
			al_toggle_display_flag(display,ALLEGRO_NOFRAME,true);
		if (!display) {
			std::cerr<<"Errore init display";
			return -1;
		}
	/**/

	//Install and init addon
	al_init_native_dialog_addon();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_keyboard();
	al_install_audio();
	al_init_acodec_addon();
	al_init_image_addon();
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0/FPS);

	srand(time(NULL));
	initAll(player,level,num,enemy,flag);

	//Fonts, musics and images
		//Sky objects
	skyObjects.image = new Image[NUM_SKY_OBJECTS];
	skyObjects.name = SKY;
	skyObjects.size = NUM_SKY_OBJECTS;
	skyObjects.howShowed = 0;
	initSkyObjectsImages(skyObjects);
		//Background
	setImage(background,"../res/img/bg.jpg",0,0,0,WIDTH,DEFAULT_Y,4,true);
	al_reserve_samples(3);
		//Font
	fontScore = al_load_ttf_font(DEFAULT_TTF,20,0);
	fontCountDown = al_load_ttf_font(DEFAULT_TTF,30,0);
		//Music
	gameSample = al_load_sample(gameOgg);
	bipSample = al_load_sample(bipOgg);
	gameSampleInstance = al_create_sample_instance(gameSample);
	bipSampleInstance = al_create_sample_instance(bipSample);
	al_set_sample_instance_playmode(gameSampleInstance,ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_playmode(bipSampleInstance,ALLEGRO_PLAYMODE_ONCE);
	al_attach_sample_instance_to_mixer(gameSampleInstance,al_get_default_mixer());
	al_attach_sample_instance_to_mixer(bipSampleInstance,al_get_default_mixer());

	//Listener
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_start_timer(timer);

	//Starting of main cycle
	while (!over) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		 if (paused) {
			al_stop_sample_instance(gameSampleInstance);
			char buttonsOptions[20];
			if (config[MUSICOFF])
				strcpy(buttonsOptions,"Close|Music On|Exit");
			else
				strcpy(buttonsOptions,"Close|Mute Off|Exit");
			int buttonOption = al_show_native_message_box(
					display,
					"Options",
					"Options:",
					"You can reset all scores and "
					"reached level or turn off music",
					buttonsOptions,
					0);
			optionsButtonManage(buttonOption-1);
			}
		 //If not paused
		 else {
			 //Every Frame
			 eventManage(ev);

			if(ev.type == ALLEGRO_EVENT_TIMER) {
				redraw = true;
				//Update and check object
				//N.B. check if create image and enemy only if player is alive
				updateBackground(background); //move bg
				updateImage(skyObjects); //move images
				updatePlayer(player); //check jump
				updateEnvironment(level);//go on (move enemies)
				outOfBoundBackground(background);
				outOfBoundImage(skyObjects);
				drawBackground(background);
				al_draw_line(0,DEFAULT_Y,WIDTH,DEFAULT_Y,myColors[DEFAULT_COLOR],2);//Floor
				drawImage(skyObjects);//Draw showed image
				drawPlayer(player);
				drawEnemy(level);//Draw lived enemy
				collision(level,player);//Check for death
				/*
				 * If game began then check if player is alive
				 * else continue with count down
				 */
				if (checkCountDown(countDown)) {
					/*
					 * if player is still alive continue with game
					 * else restart level and countdown
					 * (if restart immediately the gamer continue to play simplest)
					 */
					if (player.isLive) {
						if (config[MUSICOFF])
							al_stop_sample_instance(gameSampleInstance);
						else
							al_play_sample_instance(gameSampleInstance);

						createEnemy(level);//check if create an enemy
						createImage(skyObjects);

						if (outOfBoundEnemy(level))//check if enemy is out
							updatePoint(player);
						/*
						 * If level finished create flag.
						 * When flag is out of bound start next level.
						 * If the game is over return to menu.
						 * If this level is higher than last level passed so save new level reached
						 */
						if (context == STARTLEVEL && checkForEnd(level)) {
							updateFlag(flag);
							drawFlag(flag);
							if (endLevel(flag)) {
								if (num > config[LASTLEVELPASSED]) {
									saveToFile(num,fileLevel);
									config[LASTLEVELPASSED] = num;
								}
								num++;
								if (num > NUM_LEVELS) {
									al_show_native_message_box(display,
											"Wins...for now",
											"Congratulations",
											"You won the game, but only for now, stay tuned for new levels",
											"Ok",
											0);
									over = true;
								}
								player.isLive = false;
							}
						}
					}
					/*
					 * If player not alive reinit objects,
					 * save best score(if need) and restart game
					 */
					else {
						//If this score is higher than lower score
						if (context == STARTARCADE && setScore(player.score,config[SCORE]))
							saveToFile(config[SCORE],fileScore);
						//Restart game for don't lost time uselessly
						initAll(player,level,num,enemy,flag);
						countDown = (FPS*SECS) - 1;
					}
				}
				/*
				 * Else if !checkCountDown(so game not start)
				 * stop music, write countdown or number of level,
				 * and going on with countdown
				 */
				else {
					al_stop_sample_instance(gameSampleInstance);
					//In case i'm on a level
					if (num != 0)
						al_draw_textf(fontCountDown,myColors[DEFAULT_COLOR],WIDTH/2,HEIGHT/2-40,0,"Level %i",num);
					//In case i'm on arcade
					else
						al_draw_textf(fontCountDown,myColors[DEFAULT_COLOR],WIDTH/2,HEIGHT/2,0,"%i",(countDown/60)+1);
					//Every second play a bip
					if ((countDown+1) % 60 == 0 || countDown == 1)
						al_play_sample_instance(bipSampleInstance);
					countDown--;
					IMAGE_SPEED = 30; //Rise image speed for effect
				}
			}
			//If click X button or ESC key
			else if (KEY[ESC])
				paused = true;
			//If press a key
			if((KEY[UP] || KEY[DOWN]) && checkCountDown(countDown)){
				if (KEY[UP]) {
					//check for no double jump
					if (player.cy == DEFAULT_Y)
						player.isJumping = true;
				}
				else if (KEY[DOWN]) {
					if (player.cy == DEFAULT_Y)
						player.isReverseJumping = true;
					}
			}

			al_flush_event_queue(event_queue);
			if (redraw && al_is_event_queue_empty(event_queue)){
				redraw = false;
				allFalse(KEY,NUM_KEY);
				D1(al_draw_textf(fontScore,myColors[DEFAULT_COLOR],0,50,0,"Living:%i and Died:%i.. Player X:%i Player Y:%i",level.howLive,level.howDied,player.x,player.y));
				D1(al_draw_textf(fontScore,myColors[DEFAULT_COLOR],0,200,0,"Is Jumping:%i isReverse:%i",player.isJumping,player.isReverseJumping));
				//Depending by context, draw countdown or level number
				if (context == STARTARCADE)
					al_draw_textf(fontScore,myColors[DEFAULT_COLOR],0,0,0,"Score %i",level.howDied);
				else if (checkCountDown(countDown))
					al_draw_textf(fontScore,myColors[DEFAULT_COLOR],WIDTH/2,0,0,"Level %i",num);

				al_flip_display();
				al_clear_to_color(al_map_rgb(0,0,0));
			}
		}
	}

	delete [] skyObjects.image;
	saveToFile(config[MUSICOFF],fileMute);
	al_toggle_display_flag(display,ALLEGRO_NOFRAME,false);
	al_destroy_sample_instance(gameSampleInstance);
	al_destroy_sample_instance(bipSampleInstance);
	al_destroy_sample(gameSample);
	al_destroy_sample(bipSample);
	al_destroy_event_queue(event_queue);
	al_destroy_font(fontCountDown);
	al_destroy_font(fontScore);
	al_destroy_timer(timer);
	return 0;
}

//FUNCTIONS------------------------------------------------
bool checkCountDown(int countDown){
	if (countDown > 0)
		return false;
	return true;
}
void initAll(Player &player,Level &level,int num,Enemy &enemy,Flag &flag) {
	InitDefaultPlayer(player);
	InitDefaultLevel(level,num);
	InitDefaultEnemy(enemy);
	InitDefaultFlag(flag);
}

bool setScore(int this_score,int &score){
	if (this_score > score){
		score = this_score;
		return true;
	}
	return false;
}

static void optionsButtonManage(int button){
	switch (button) {
	case CLOSE:
		paused = false;
		break;
	case MUTE:
		config[MUSICOFF] = !config[MUSICOFF];
		paused = false;
		break;
	case EXIT:
		paused = false;
		over = true;
		break;
	}
}
static bool eventManage(ALLEGRO_EVENT &ev){
	if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		switch(ev.keyboard.keycode) {
			case ALLEGRO_KEY_ESCAPE:
				KEY[ESC] = true;
				return true;
			case ALLEGRO_KEY_UP:
				KEY[UP] = true;
				return true;
			case ALLEGRO_KEY_DOWN:
				KEY[DOWN] = true;
				return true;
		}
	}
	if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		KEY[ESC] = true;
		return true;
	}
	if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
		resizeDisplay(ev.display.width,ev.display.height);
		al_acknowledge_resize(ev.display.source);
		al_resize_display(ev.display.source,WIDTH,HEIGHT);
		al_flip_display();
		al_clear_to_color(myColors[BLACK]);
		return true;
	}
	return false;
}
static void allFalse(bool v[],int size){
	for (int i=0;i<size;i++)
		v[i] = false;
}

void initSkyObjectsImages(Images images) {
	setImage(images.image[PLANE],skyObjectsPath[PLANE],WIDTH,20,30,50,20,9,false);
	setImage(images.image[CLOUD],skyObjectsPath[CLOUD],WIDTH,10,20,70,30,4,false);
}

