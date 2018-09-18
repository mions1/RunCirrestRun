#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <cstring>

#include "headers/objects.h"
#include "headers/engine.h"

//DEBUG
#ifdef DEB
extern unsigned int MASK;

#define DBG(A, B) {if ((A) & MASK) {B; } }
#else
#define DBG(A, B)
#endif
//
#define D1(a) DBG(1, a)
#define D2(a) DBG(2, a)

//Extern consts
extern const int config[];
extern const int WIDTH;
extern const int HEIGHT;
extern char DEFAULT_TTF[];
extern const ALLEGRO_FONT* titleFont;

//Gloabal Variables (not used as extern)
static int titleY = HEIGHT/2; /*!< height of title */
static int boot; /*!< State of boot */

enum boot{PRESENTS,TITLE,END,FINISH};

/**
 * Manage events
 * Any keys on keyboard or on mouse is pressed,
 * boot jumps a phase.
 * If display is resizing manage dependencies
 */
static bool eventManager(ALLEGRO_EVENT &ev);

/**
 * Start boot anim
 * Show author and title
 * @param display display to draw
 */
int myBoot(ALLEGRO_DISPLAY *display){

	//Varibales and consts
	const char bootOgg[] = "../res/music/boot.ogg"; //boot song
	int fps = 60; /*!< FPS of anim */
	int secsTitle = (fps*3); /*!< in boot==TITLE, title is stopped for secsTitle seconds  */

	//Allegro Variables
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_SAMPLE *bootSample = NULL;
	ALLEGRO_SAMPLE_INSTANCE *bootSampleInstance = NULL;

	/*Init Allegro*/
		if(!al_init()) {
			std::cerr<<"Errore init allegro";
			return -1;
		}
		if (!display) {
			std::cerr<<"Errore init display";
			return -1;
		}
	/**/

	//Install and init addon
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_keyboard();
	al_install_audio();
	al_init_acodec_addon();
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0/fps);

	srand(time(NULL));

	//Fonts and Musics
	ALLEGRO_FONT *presentsFont = al_load_font(DEFAULT_TTF,30,0);
		//Music
	al_reserve_samples(1);
	bootSample = al_load_sample(bootOgg);
	bootSampleInstance = al_create_sample_instance(bootSample);
	al_set_sample_instance_playmode(bootSampleInstance,ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(bootSampleInstance,al_get_default_mixer());
	if (!config[MUSICOFF])
		al_play_sample_instance(bootSampleInstance);

	//Listener
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_start_timer(timer);
	int i=255; //For title's alpha (from transparent to opaque)
	boot = PRESENTS;
	//Starting of main cycle
	/*
	 * First show mionstec written, for second show title for secsTitle
	 * then move title up until height(speed depends to display size)
	 * final return
	 * NB every key user press, jump a boot phase
	 */
	while (true) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		eventManager(ev);
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			if (boot == PRESENTS) {
				if (i>0) {
					i-=1;
					al_draw_text(presentsFont,al_map_rgb(255*i,255*i,255*i),
							WIDTH/2,HEIGHT/2,ALLEGRO_ALIGN_CENTRE,"MionsTec presents");
				}
				else {
					boot = TITLE;
				}
			}
			else if (boot == TITLE) {
				if (secsTitle > 0) {
					secsTitle--;
					al_draw_text(titleFont,al_map_rgb(255,255,255),
							WIDTH/2,HEIGHT/2,ALLEGRO_ALIGN_CENTRE,"RUN CIRREST RUN");
				}
				else {
					boot = END;
				}
			}
			else if (boot == END) {
				if (titleY > 0) {
					titleY -= HEIGHT/100; //for any size of display, speed is the same
					al_draw_text(titleFont,al_map_rgb(255,255,255),
							WIDTH/2,titleY,ALLEGRO_ALIGN_CENTRE,"RUN CIRREST RUN");
				}
				else
					boot = FINISH;
			}
		al_flush_event_queue(event_queue);
		al_flip_display();
		al_clear_to_color(al_map_rgb(0,0,0));
		if (boot == FINISH)
			break;
		}
	}
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_sample(bootSample);
	al_destroy_sample_instance(bootSampleInstance);
	return 0;
}

static bool eventManager(ALLEGRO_EVENT &ev){
	//Any key or mouse press, boot jumps a phase
	if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		boot++;
		return true;
	}
	else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
		boot++;
		return true;
	}
	else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE){
		if (HEIGHT > ev.display.height)
			titleY = titleY - ((HEIGHT - ev.display.height)/2);
		else
			titleY = titleY + ((ev.display.height-HEIGHT)/2);
		resizeDisplay(ev.display.width,ev.display.height);
		al_acknowledge_resize(ev.display.source);
		al_resize_display(ev.display.source,WIDTH,HEIGHT);
		return true;
	}
	return false;
}
