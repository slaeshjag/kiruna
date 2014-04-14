#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <darnit/darnit.h>

#define SERIAL_PORT 0xabcdef

void do_stuff(DARNIT_TEXT_SURFACE *textplace);
void click(int x, int y, DARNIT_TEXT_SURFACE *textplace);
void initiate_buttons(void);

struct LOOKED{
	
	int frames_looked;
	int x;
	int y;
};

DARNIT_BBOX *buttonlist;

int up_button, right_button, left_button, meny;

struct LOOKED looked;

int main(int argc, char **argv) {
	
	d_init_custom("lek", 800, 600, 0, "lek", NULL);
	
	//Här initierar vi allting. Vi får oss ett fönster
	//på 800 x 600 pixlar, som Inte är fullscreen. 
	
	DARNIT_FONT *kgfont = d_font_load("KGRayofSunshine.ttf", 60, 256, 256);
	
	//Skapar oss en font.
	
	DARNIT_TEXT_SURFACE *papper = d_text_surface_new(kgfont, 16 , 800, 10, 0);
	
	//Skapar oss en textyta som vi kan rendera text med.
	
	d_cursor_show(1);
	
	//Visa muspekaren.
	
	if (!papper)
		fprintf(stderr, "Paper didn't load\n");
		
		
	for(;;) {
		
		//Programloop.
		
		d_render_begin();
		
		//Börja rendera.
		
		d_render_blend_enable();
		
		//Enable blend behövs tydligen.
		
		d_render_tint(255,255,255,255);
		
		//Sätter "genomskinlighet" till noll.
		
		d_text_surface_draw(papper);
		
		//Renderar textytan papper.
		
		do_stuff(papper);
		
		//Här gör vi saker.
		
		d_render_end();
		
		//Slutar rendera.
		
		d_loop();
		
		//Skall göras mellan varje frame.
		
		d_text_surface_reset(papper);
		
		//Här tömmer vi textytan papper på text.
		
	}
	return 0;
	
	//Programmet är nu slut.
}

void initiate_buttons(void){
	
	buttonlist = d_bbox_new(8);
	
	//Gör plats för 8 knappar i våran globala lista.
	
	up_button = d_bbox_add(buttonlist, 100, 100, 400, 150);
	
	//Här gör vi en liten knapp.
	
	left_button = d_bbox_add(buttonlist, 100, 100, 150, 400);
	
	//Här gör vi en till knapp.
	
	right_button = d_bbox_add(buttonlist, 550, 100, 150, 400);
	
	//Här gör vi ännu en knapp.
	
	meny = d_bbox_add(buttonlist, 400, 400, 400, 150);

	//Och så till sist en knapp.

}

void do_stuff(DARNIT_TEXT_SURFACE *textplace){
	
	DARNIT_MOUSE mus = d_mouse_get();
	
	//Här hämtas musdata.
	
	d_text_surface_string_append(textplace, " X: ");
	
	char femton[256]; 
	
	sprintf(femton, "%i", mus.x);
	
	d_text_surface_string_append(textplace, femton);
	
	sprintf(femton, "%i", mus.y);
	
	d_text_surface_string_append(textplace, " Y: ");
	
	d_text_surface_string_append(textplace, femton);
	
	//Här skriver vi ut lite saker.
	
	if((((looked.x - 75) < mus.x) 
	&& (mus.x < (looked.x + 75))) 
	&& (((looked.y - 75) < mus.y) 
	&& (mus.y < (looked.y + 75)))){
	
		//Vi kollar om musens koordinater befinner sig
		//inom en 300x300 ruta ifrån fästpunkten.
	
		//Om den gör detta räknar vi fram en extra frame.
		
		looked.frames_looked = looked.frames_looked + 1;
		
		if(100 < looked.frames_looked){
		
		//Om vi tittat inom samma område i 100 frames...
		
			d_text_surface_string_append(textplace, "   Nu! :D");
			
			//skriv ut på skrämen.
			
			click(looked.x, looked.y, textplace);
			
			//Och så klickar vi.
		
		}
	}
	else{
		
		//Annars fäster vi den tittade possitionen till
		//musens possition, och sätter antalet frames som
		//vi tittat där till 0.
		
		looked.frames_looked = 0;
		looked.x = mus.x;
		looked.y = mus.y;
	}
	
	d_text_surface_draw(textplace);
	
	//Sen anropar vi draw för att rendera textobjektet.
}

void click(int x, int y, DARNIT_TEXT_SURFACE *textplace){
	
	unsigned int hits[4];
	d_bbox_test(buttonlist, x, y, 1, 1, hits, 4);
	
	//Vi kollar vilka knappar som överlappar punken som klickats.
	
	if(hits[0] == right_button || hits[1] == right_button ||
	   hits[2] == right_button || hits[3] == right_button ){
		   
		d_text_surface_string_append(textplace, "\n Högerknappen klickades.");
		   
	}
	
	//Vi kollar om högerlnappen klickades.
	
	if(hits[0] == left_button || hits[1] == left_button ||
	   hits[2] == left_button || hits[3] == left_button ){
		   
		d_text_surface_string_append(textplace, "\n Vänsterknappen klickades.");
		   
	}
	
	//Vi kollar om vänsterknappen klickades.
	
}

