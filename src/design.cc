#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "headers/objects.h"
#include "headers/design.h"

//Extern
extern ALLEGRO_COLOR myColors[];
extern const int DEFAULT_Y;
extern const int WIDTH;
extern const int HEIGHT;

//Forms
void circle(int cx,int cy,int rad,int color){
	al_init_primitives_addon();
	al_draw_filled_circle(cx,cy,rad,myColors[color]);
}
void sword(int x1,int y1,int x2,int y2,int color,int width){
	al_draw_line(x1,y1,x2,y2,myColors[color],width);
}
void triangle(int x,int tip,int side){
	/*
	 * Draw a eq triangle start by x and floor(Default_y)
	 * So, base start by x and finish on x+side (so it's long side)
	 * y tip is as param tip and x tip is half base
	 */
	al_draw_triangle(x,DEFAULT_Y,x+side,DEFAULT_Y,
					 x + (side/2),tip,myColors[RED],2);
}
void wall(int x,int height){
	/*
	 * It's a vertical line tall as display height and width 3
	 */
	al_draw_filled_rectangle(x,DEFAULT_Y,x+3,height,myColors[GREEN]);
}
void dflag(int x,int y){
	//Flag' staff
	al_draw_filled_rectangle(x,y,x+5,y - 45, myColors[WHITE]);
	//Flag
	al_draw_filled_triangle(x,y-25,x,y-45,x+20,y-35,myColors[RED]);
}

//Buttons
void drawButtons(const Button button[],int size){
	//Default menu button
	/*
	 * Use font->height for draw buttons big as text
	 */
	for (int i=0;i<size;i++) {
		al_draw_rounded_rectangle(button[i].x,button[i].y,
				button[i].x2,
				button[i].y2,2,2,
				myColors[button[i].buttonColor],1);

		al_draw_text(button[i].font,myColors[button[i].textColor],
					button[i].x + (button[i].font->height/2),button[i].y,0,button[i].text);
	}
}
void drawLevels(const Button button[],int num_levels){
	for (int i=0;i<num_levels;i++){
		al_draw_rectangle(button[i].x,button[i].y,button[i].x2,button[i].y2,myColors[button[i].buttonColor],2);
		al_draw_textf(button[i].font,myColors[button[i].textColor],button[i].x+25,40,0,"%i",i+1);
	}
}
void loading(ALLEGRO_FONT *font,int color){
	al_draw_text(font,myColors[color],WIDTH/2,HEIGHT/2,0,"Loading...");
}

