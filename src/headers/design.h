#include <allegro5/allegro_font.h>

extern ALLEGRO_COLOR myColors[];

//Characters Forms
/**
 * draw filled circle
 * @param cx center x
 * @param cy center y
 * @param rad radious
 * @param color color of circle
 * @see myColors
 */
void circle(int cx,int cy,int rad,int color);
/**
 * Draw player' sword, used for draw hilt and blade
 * @param x1 start x
 * @param y1 start y
 * @param x2 end x
 * @param y2 end y
 * @param color color
 * @param width width
 * @see myColors
 */
void sword(int x1,int y1,int x2,int y2,int color,int width);
/**
 * draw eq triangle
 * @param x left x
 * @param tip y of triangle tip
 * @param side width of side
 */
void triangle(int x,int tip,int side);
/**
 * draw wall(vertical line)
 * @param x left x
 * @param height wall height(height as display)
 */
void wall(int x,int height);
/**
 * draw flag
 * @param x x left
 * @param y y of flag
 */
void dflag(int x,int y);

/**
 * crate group of button
 * @param button group of buttons
 * @param size number of buttons
 */
void drawButtons(const Button button[],int size);
/**
 * draw levels buttons
 * @param button buttons of levels
 * @param num_levels size of group buttons
 */
void drawLevels(const Button button[],int num_levels);

/**
 * Draw loading written
 * @param font font of written
 * @param color color of written
 */
void loading(ALLEGRO_FONT *font,int color);
