#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "headers/objects.h"
#include "headers/engine.h"
#include "headers/design.h"

//Extern
extern int WIDTH;
extern int HEIGHT;
extern const int DEFAULT_JUMP_SPEED;
extern const int DEFAULT_JUMP_HEIGHT;
extern ALLEGRO_COLOR myColors[];
extern int DEFAULT_Y;

//Consts and variables for this file
int createProb = 1; /*!< probability that an enemy or an image have to be create, start by 1 and then rises  */
const int DEFAULT_ENEMY_SPEED = 8; /*!< Default enemy speed */
int ENEMY_SPEED = DEFAULT_ENEMY_SPEED; /*!< Enemy speed, it rises going on with game */
int IMAGE_SPEED = ENEMY_SPEED/3; /*!< Image speed, it rises as enemy speed */
const int DEFAULT_TRIANGLE_RAD = 15; /*!< Default radius of triangle enemy */
const int DEFAULT_FLAG_SPEED = 5; /*!< Default flag speed*/
const int DEFAULT_PLAYER_RAD = 15; /*!< Default radius of player */
const int DEFAULT_PLAYER_CX = 50;

//Randomize Function
int randEnemyTypes(){
	//Probability it's 50%
	return rand() % 2;
}
bool randIfCreate(){
	//probability it's true is createProb/200
	if (rand() % 200 <= createProb)
		return true;
	return false;
}
bool randIfReverse(){
	//Probability is 50%
	if (rand() % 2 == 0)
		return true;
	return false;
}

//Check Function
bool isBetween(int x,int a,int b){
	//if a > b switch them
	if (a > b) {
		int appo = a;
		a = b;
		b = appo;
	}
	//Collide if x is between a and b
	if (x >= a && x <= b)
		return true;
	return false;
}
bool checkForEnd(Level &level){
	//End if died enemies and living enemies are > num level enemies
	//So if enemies in that level finished
	if (level.enemies != 0 &&
			(level.howDied + level.howLive) >= level.enemies) {
		return true;
	}
	return false;
}
bool endLevel(Flag &flag) {
	//Real end level if flag go out from display
	if (flag.x <= 0)
		return true;
	return false;
}

//Movements
/*
 * Manages player's jump
 * if player is jumping but it still isn't on top jump height it continues to go up
 * else if player isn't jumping but it is higher than his default y it go down
 * if player is on his max high it begin to go down
 */
void jump(Player &player){
	//If still didn't reach jump height continue to rise
	if (player.isJumping &&
			player.cy > (DEFAULT_Y - player.jumpHeight) )
		player.cy -= DEFAULT_JUMP_SPEED;
	//If still didn't reach floor, continue to go down
	else if (!player.isJumping && player.cy < DEFAULT_Y)
			player.cy += DEFAULT_JUMP_SPEED;
	//If it reached max height, it start to go down
	if(player.isJumping &&
			player.cy <= DEFAULT_Y - player.jumpHeight)
		player.isJumping = false;
}
/*
 * Manages player's reverse jump
 * see: jump()
 */
void reverseJump(Player &player){
	//See above
	if (player.isReverseJumping &&
			player.cy < (DEFAULT_Y + player.jumpHeight) )
		player.cy += DEFAULT_JUMP_SPEED;
	else if (!player.isReverseJumping && player.cy > DEFAULT_Y)
		player.cy -= DEFAULT_JUMP_SPEED;
	if(player.isReverseJumping &&
			player.cy >= DEFAULT_Y + player.jumpHeight)
		player.isReverseJumping = false;
}

/*
 * Manage sword attack
 *
 */
 /*
void swordAttack() {
//If still didn't reach end of attack, continue to sink the shot
	if (player.isAttacking &&
			player.cy > (DEFAULT_Y - player.jumpHeight) )
		player.cy -= DEFAULT_JUMP_SPEED;
	//If still didn't reach floor, continue to go down
	else if (!player.isJumping && player.cy < DEFAULT_Y)
			player.cy += DEFAULT_JUMP_SPEED;
	//If it reached max height, it start to go down
	if(player.isJumping &&
			player.cy <= DEFAULT_Y - player.jumpHeight)
		player.isJumping = false;
}
*/

//Init Object with default params
void InitDefaultPlayer(Player &player){
	player.cx = DEFAULT_PLAYER_CX;
	player.cy = DEFAULT_Y;
	player.rad = DEFAULT_PLAYER_RAD;
	player.x = player.cx - player.rad;
	player.y = player.cy - player.rad;
	player.x2 = player.cx + player.rad;
	player.y2 = player.cy + player.rad;
	player.jumpHeight = DEFAULT_JUMP_HEIGHT;
	player.isJumping = false;
	player.isReverseJumping = false;
	player.isLive = true;
	player.jumpSpeed = DEFAULT_JUMP_SPEED;
	player.score = 0;
}
void InitDefaultEnemy(Enemy &enemy){
	enemy.x = WIDTH;
	enemy.y = DEFAULT_Y;
	enemy.rad = DEFAULT_TRIANGLE_RAD;
	enemy.isLive = false;
	enemy.reverse = true;
}
void InitDefaultLevel(Level &level,int nlevel){
	//In case i'm in level mode
	level.speed = DEFAULT_ENEMY_SPEED;
	if (nlevel !=0) {
		//inc level difficult
		level.enemies = (((nlevel+1)*3)+20)/2; //inc num enemies
		level.speed += nlevel; //Inc speed
		createProb = nlevel; //inc probability that a enemy is created
	}
	level.howLive = 0;
	level.howDied = 0;
	for (int i=0; i<NUM_ENEMIES; i++)
		InitDefaultEnemy(level.enemy[i]);
	//In case I'm in arcade mode
	if (nlevel == 0)
		level.enemies = 0;
}
void InitDefaultFlag(Flag &flag){
	flag.x = WIDTH;
	flag.y = DEFAULT_Y;
	flag.speed = DEFAULT_FLAG_SPEED;
}

//Set objects params
void setImage(Image &image,char *path,int x,int y,int y_displacement,int width,int height,
				int speed,bool isShow){
	image.image = loadBitmap(path,width,height);
	image.x = x;
	image.y = y;
	image.y_displacement = y_displacement;
	image.width = width;
	image.height = height;
	image.speed = speed;
	image.isShow = isShow;
}
void setButton(Button &button,int x,int y,int x2,int y2,
				char *text,ALLEGRO_FONT *font,int textColor,
				int buttonColor){
	button.x = x;
	button.y = y;
	button.x2 = x2;
	button.y2 = y2;
	button.text = text;
	button.font = font;
	button.textColor = textColor;
	button.buttonColor = buttonColor;
}

//Draw
void drawPlayer(Player &player){
	//Body
	circle(player.cx,player.cy,player.rad,WHITE);
	//Eyes
	circle(player.x2-player.rad/2,player.cy-4,2,BLACK);

	//LightSaber
	//Hilt and Blade
	sword(player.cx,player.cy,player.cx,player.cy,
			GREY, GREEN,3);
}
void drawEnemy(Level &level){
	/*
	 * check if enemy is live and in case show it
	 */
	for (int i=0; i<NUM_ENEMIES; i++)
		if(level.enemy[i].isLive)
			switch (level.enemy[i].enemyType) {
			case TRIANGLE:
				triangle(level.enemy[i].x,level.enemy[i].y,level.enemy[i].rad);
				break;
			case WALL:
				wall(level.enemy[i].x,level.enemy[i].y);
				break;
			}
}
void drawFlag(Flag &flag){
	dflag(flag.x,flag.y);
}
void drawBackground(Image image){
	al_draw_bitmap(image.image, image.x,image.y,0);
	//Draw another background to append it at the end
	if ((image.width + image.x) < image.width)
		al_draw_bitmap(image.image, image.x+image.width,image.y,0);
}
void drawImage(Images &images){
	/*
	 * check if image must be showed and in case show it
	 */
	for (int i=0;i<images.size;i++) {
		if (images.image[i].isShow)
			al_draw_bitmap(images.image[i].image,
					images.image[i].x,images.image[i].y,
					0);
	}
}
ALLEGRO_BITMAP *loadBitmap(const char *filename, int w, int h) {
  ALLEGRO_BITMAP *tmp;
  ALLEGRO_BITMAP *bitmap;
  ALLEGRO_BITMAP *previousTarget;

  //Create sized bitmap
  tmp = al_create_bitmap(w, h);
  if (!tmp) return NULL;

  //Load my bitmap
  bitmap = al_load_bitmap(filename);
  if (!bitmap)
  {
     al_destroy_bitmap(bitmap);
     return NULL;
  }

  //Save prev target and set tmp as new target
  previousTarget = al_get_target_bitmap();
  al_set_target_bitmap(tmp);

  //Resize my bitmap
  al_draw_scaled_bitmap(bitmap,
     0, 0,                                // source origin
     al_get_bitmap_width(bitmap),     // source width
     al_get_bitmap_height(bitmap),    // source height
     0, 0,                                // target origin
     w, h,                                // target dimensions
     0                                    // flags
  );

  //Restore prev target
  al_set_target_bitmap(previousTarget);

  return tmp;
}

void resizeDisplay(int width,int height){
	//Call when display is resizing
	WIDTH = width;
	HEIGHT = height;
	DEFAULT_Y = height - 100;
}

//Update
void updatePlayer(Player &player){
	jump(player);
	reverseJump(player);
}
void updateEnvironment(Level &level){
	/*
	 * Move enemies and update image speed
	 */
	for (int i=0; i<NUM_ENEMIES; i++)
		if (level.enemy[i].isLive) {
			level.enemy[i].x -= level.speed;
			ENEMY_SPEED = level.speed;
		}
	IMAGE_SPEED = ENEMY_SPEED / 3;
}
void updateFlag(Flag &flag){
	flag.x -= flag.speed;
}
void updateBackground(Image &image){
	image.speed = IMAGE_SPEED;
	image.x -= image.speed;
}
void updateImage(Images images){
	for (int i=0; i<images.size; i++)
		if (images.image[i].isShow) {
			images.image[i].x -= images.image[i].speed;
		}
}
void updatePoint(Player &player) {
	player.score++;
}

//Collision
bool outOfBoundImage(Images &images) {
	/*
	 * If image is showed check if is out of bound or not
	 * and in case delete it
	 */
	for(int i=0; i<NUM_ENEMIES; i++)
			if (images.image[i].isShow &&
					(images.image[i].x + images.image[i].width) < 0) {
				images.image[i].isShow = false;
				images.image[i].x = WIDTH;
				images.howShowed--;
				return true;
			}
		return false;
}
bool outOfBoundBackground(Image &image){
	/*
	 * if backgroud is out, reinit his position
	 */
	if ((image.width + image.x) < 0) {
		image.x = 0;
		return true;
	}
	return false;
}
bool outOfBoundEnemy(Level &level){
	/*
	 * If enemy is live, check if is out and in case
	 * kill him (so reinit him), update howdied and howlive
	 * Every 3 enemies increase level speed
	 */
	for(int i=0; i<NUM_ENEMIES; i++)
		if (level.enemy[i].isLive &&
				level.enemy[i].x < 0) {
			InitDefaultEnemy(level.enemy[i]);
			level.howDied++;
			level.howLive--;
			if (level.howDied % 3 == 0) {
				level.speed += 0.5;
				ENEMY_SPEED = level.speed;
			}
			return true;
		}
	return false;
}
void collision(Level &level,Player &player){
	for (int i=0; i<NUM_ENEMIES; i++)
		//If go in front of enemy
		if (level.enemy[i].isLive){
			if ((isBetween(player.x2,
					level.enemy[i].x,
					level.enemy[i].x+level.enemy[i].rad)) &&
					(isBetween(player.cy,DEFAULT_Y,level.enemy[i].y)) )
				player.isLive = false;
			//If jump on enemy
			else if ((isBetween(player.cx,
					level.enemy[i].x,
					level.enemy[i].x+level.enemy[i].rad)) &&
					(isBetween(player.cy,DEFAULT_Y,level.enemy[i].y)) )
				player.isLive = false;
		}
}

//Create objects
/*
 *  Create enemy if there are some conditions
 *  1. there isn't others enemies or last enemy is
 *     enough longer
 *  2. randIfCreate() function return true
 *  if enemy created, with some functions the program
 *  decides what type is and if it's reverse (see also: randReverse() and randEnemyType())
 */
void createEnemy(Level &level) {
	bool checkForNew = false;

	//If enemies are finished, can't create other
	if(checkForEnd(level))
		return;

	if (level.howLive == 0)
		checkForNew = true;
	else if (level.howLive > 0) {
		/*
		 * Last created enemy
		 */
		Enemy lastEnemy;
		lastEnemy.x = 1;
		for (int i=0; i<NUM_ENEMIES; i++)
			if (level.enemy[i].isLive)
				if (lastEnemy.x < level.enemy[i].x)
					lastEnemy = level.enemy[i];
		if (lastEnemy.isLive &&
				lastEnemy.x < (WIDTH - 150))
			checkForNew = true;
	}

	//Create new enemy
	//Check if a die enemy must be create, and in case init it
	//Depend by its type
	if (checkForNew) {
		for (int i=0; i<NUM_ENEMIES; i++)
			if (!level.enemy[i].isLive &&
					randIfCreate() ) {
				level.enemy[i].isLive = true;
				level.enemy[i].reverse = !randIfReverse();
				level.enemy[i].enemyType = randEnemyTypes();
				switch(level.enemy[i].enemyType){
				case TRIANGLE:
					if (level.enemy[i].reverse)
						level.enemy[i].y = DEFAULT_Y + level.enemy[i].rad;
					else
						level.enemy[i].y = DEFAULT_Y - level.enemy[i].rad;
					break;
				case WALL:
					if (level.enemy[i].reverse)
						level.enemy[i].y = HEIGHT;
					else
						level.enemy[i].y = 0;
					break;
				}
				level.howLive++;
				break;
			}
	}
}
void createImage(Images &images) {
	bool checkForNew = false;

	//If there isn't any image, can create one
	if (images.howShowed == 0)
		checkForNew = true;
	//If there is at least one image, can create another only
	//if it's enough far
	else if (images.howShowed > 0) {

		//Get last image is created
		Image lastImage;
		lastImage.x = 1;
		for (int i=0; i<images.size; i++)
			if (images.image[i].isShow)
				if (lastImage.x < images.image[i].x)
					lastImage = images.image[i];
		//If lastimage is out of buond and it's enough far
		if (lastImage.isShow &&
				lastImage.x+lastImage.width < (WIDTH))
			checkForNew = true;
	}


	if (checkForNew) {
		//rand image
		int randIndex = rand() % images.size;
			//Randomize if create it
			if (!images.image[randIndex].isShow &&
					randIfCreate()) {
				images.image[randIndex].isShow = true;
				//Random height of object
				images.image[randIndex].y = images.image[randIndex].y - (rand() % images.image[randIndex].y_displacement);
				images.howShowed++;
			}
	}
}
