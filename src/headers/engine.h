#include <allegro5/allegro.h>

//Init objects
/**
 * Init Player with default values
 * @param player player to setting up
 */
void InitDefaultPlayer(Player &player);
/**
 * Init Enemy with default values
 * @param enemy enemy to setting up
 */
void InitDefaultEnemy(Enemy &enemy);
/**
 * Init Level with default values
 * @param level level to setting up
 */
void InitDefaultLevel(Level &level,int nlevel);
/**
 * Init Flag with default values
 * @param flag flag to setting up
 */
void InitDefaultFlag(Flag &flag);

//Set objects params
/**
 * Set image params
 * @param image image to setting up
 * @param text path of image
 * @param x x of image
 * @param y y of image
 * @param y_dispacement same image can have different height with this phase displacement
 * @param width width of image
 * @param height height of image
 * @param speed movement speed of image
 */
void setImage(Image &image,char *text,int x,int y,int y_displacement,int width,int height,
		int speed,bool isShow);
/**
 * Set button params
 * @param button button to setting up
 * @param x x of button
 * @param y y of button
 * @param x2 destination x of button
 * @param y2 destination y of button
 * @param text text of button
 * @param font font of text
 * @param textColor color of text
 * @param buttonColor color of button
 */
void setButton(Button &button,int x,int y,int x2,int y2,
		char *text,ALLEGRO_FONT *font,int textColor,
		int buttonColor);

//Update game, move object at his speed
/**
 * Rise enemy speed and image speed too when need and move enemy alive
 * @param level relative level
 */
void updateEnvironment(Level &level);
/**
 * update player jump
 * @param player player to update
 */
void updatePlayer(Player &player);
/**
 * update flag if exist
 * @param flag flag to update
 */
void updateFlag(Flag &flag);
/**
 * update only image showed in groups
 * @param images group of images to update
 */
void updateImage(Images images);
/**
 * Rise point
 * @param player player to set point
 */
void updatePoint(Player &player);
void updateBackground(Image &image);

/**
 * Resize display
 * @param width new display width
 * @param height new display height
 */
void resizeDisplay(int width,int height);

//Draw objects
/**
 * Draw player in his position
 * @param player player to draw
 */
void drawPlayer(Player &player);
/**
 * Draw live enemies in level
 * @param level level
 */
void drawEnemy(Level &level);
/**
 * Draw background
 * @param image background image
 */
void drawBackground(Image image);
/**
 * Draw showed images in group
 * @param images images group
 */
void drawImage(Images &images);
/**
 * Draw flag if exist
 * @param flag flag to draw
 */
void drawFlag(Flag &flag);

//Create
/**
 * If some conditions are true, create enemy.
 * Create enemy if last enemy is enough far, if number of enemies
 * alive is lower than NUM_ENEMIES and if randIfCreate return true
 * @param level level
 * @see randIfCreate()
 */
void createEnemy(Level &level);
/**
 * If some conditions are true, create image
 * @see createEnemy()
 * @see randIfCreate()
 * @param images group of images
 */
void createImage(Images &images);

/**
 * Load bitmap whit declared size
 * @param filename path of image
 * @param width width of image
 * @param height height of image
 */
ALLEGRO_BITMAP * loadBitmap(const char *filename,int width,int height);

//Collision detect
/**
 * Verify if an enemy is out of bound
 * @param level level
 * @return return true if an enemy is out of bound
 * @see outOfBoundImage()
 * @see outOfBoundBackground()
 */
bool outOfBoundEnemy(Level &level);
/**
 * Verify if an image is out of bound
 * @param images group of image
 * @return true if an image is out of bound
 * @see outOfBoundEnemy()
 * @see outOfBoundBackground()
 */
bool outOfBoundImage(Images &images);
/**
 * Verify if backgroud is out of bound
 * @param image background
 * @return true if background is out of bound
 * @see outOfBoundEnemy()
 * @see outOfBoundImage()
 */
bool outOfBoundBackground(Image &image);
/**
 * Verify if player collide with an enemy
 * @param level level
 * @param player player
 */
void collision(Level &level,Player &player);

/**
 * check if level must to finish (so if enemies in level all died)
 * @param level level
 * @see endLevel()
 * @return true if all enemies died
 * @see endLevel()
 */
bool checkForEnd(Level &level);
/**
 * check if flag level finished (so if flag exits from level)
 * @param flag flag
 * @return true if flag exits from level
 * @see checkForEnd()
 */
bool endLevel(Flag &flag);
