#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

/**
 * My default colors
 */
enum colors{
	RED, /*!< rgb(255,0,0) */
	GREEN, /*!< rgb(0,255,0) */
	BLUE, /*!< rgb(0,0,255) */
	WHITE, /*!< rgb(255,255,255) */
	BLACK, /*!< rgb(0,0,0) */
	GREY, /*!< rgb(30,30,30) */
	DEFAULT_FOCUS_COLOR, /*!< DEFAULT THEME FOCUS COLOR */
	DEFAULT_COLOR /*!< DEFAULT THEME COLOR */
};
/**
 * Enemy Types
 */
enum enemyTypes{
	TRIANGLE,/*!< Triangle enemy */
	WALL /*!< Wall enemy */
};
/**
 * Name of gruop images
 */
enum nameImages{
	SKY /*!< SKY */
};
/**
 * Context which start game
 */
enum startGameContext{
	STARTARCADE,  /*!< If start game in Arcade mode */
	STARTLEVEL  /*!< If start game in Level mode */
};
/**
 * Config file
 */
enum config{
	SCORE, /*!< Arcade Score */
	LASTLEVELPASSED, /*!< Level reached */
	FULLSCREENOFF, /*!< Fullscreen mode */
	MUSICOFF /*!< Music mode */
};

const int NUM_ENEMIES = 3; /*!< Max enemies that can live together */
const int NUM_IMAGES = 5; /*!< Max images that can showed together */

/**
 * Player Game's hero
 */
struct Player {
	int cx; /*!< center x */
	int cy; /*!< center y */
	int rad; /*!< radius of player */
	int x; /*!< x left */
	int y; /*!< y top */
	int x2; /*!< x right*/
	int y2; /*!< y bottom */
	bool isLive; /*!< if player is still alive */
	int jumpHeight; /*!< max height of player jump */
	bool isJumping; /*!< if player is jumping */
	bool isReverseJumping; /*!< if player is reverse jumping */
	int jumpSpeed; /*!< speed of player jump */
	int score; /*!< best score of arcade mode */
};
/**
 * Enemy Game's obstacle
 */
struct Enemy {
	float x; /*!< x position */
	float y; /*!< y position */
	int rad; /*!< radius of enemy */
	bool isLive; /*!< if enemy is still alive */
	bool reverse; /*!< if enemy is reverse */
	int enemyType; /*!< what enemy is */
};
/**
 * Level Attribute of this level
 */
struct Level {
	Enemy enemy[NUM_ENEMIES]; /*!< Enemies of level that can be alive together */
	int enemies; /*!< size of enemy array */
	int howLive; /*!< how enemy is living now */
	int howDied; /*!< how enemy died in this session */
	float speed; /*!< movement speed of enemy */
};
/**
 * Flag Victory flag
 */
struct Flag {
	int x; /*!< x position */
	int y; /*!< y position */
	int speed; /*!< movement speed */
};
/**
 * Button
 */
struct Button {
	int x; /*!< x begin */
	int y; /*!< y begin */
	int x2; /*!< x destination */
	int y2; /*!< y destination */
	char *text; /*!< text inside button */
	int buttonColor; /*!< button color */
	int textColor; /*!< text color */
	ALLEGRO_FONT *font; /*!< font text */
};
/**
 * Buttons Group of buttons
 */
struct Buttons {
	Button *button; /*!< buttons of the group */
	int size; /*!< number of buttons */
	int context; /*!< name of groups */
	int activeButton; /*!< what button is clicked (-1 if no one) */
};
/**
 * Image image
 */
struct Image{
	ALLEGRO_BITMAP *image; /*!< allegro bitmap image */
	int width; /*!< width of image */
	int height; /*!< height of image */
	int x; /*!< position x */
	int y; /*!< position y */
	int y_displacement; /*!< y displacement for same image */
	int speed; /*!< movement speed of image */
	bool isShow; /*!< if image is showed */
};
/**
 * Images group of images
 */
struct Images {
	Image *image; /*!< images of the group */
	int size; /*!< number of images */
	int name; /*!< name of group */
	int howShowed; /*!< how images are showed */
};
