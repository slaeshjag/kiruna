#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <darnit/darnit.h>
#include <asm/termios.h>
#include <stdbool.h>
#include <stropts.h>       
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//No parity -- !PARENB (c_cflag)
//startbit
//One stopbit -- !CSTOPB (c_cflag)
//8 databitar -- CS8 (c_cflag)
//No flödeskontroll -- 
//Baudrate.

#define BAUDRATE 115200
#define MODEMDEVICE "/dev/ttyUSB0"

void do_stuff(DARNIT_TEXT_SURFACE *textplace);
void click(int x, int y, DARNIT_TEXT_SURFACE *textplace);
void initiate_buttons(void);
void initiate_serial_port(void);


int make_binary(int repeat_size);

struct LOOKED{

	int frames_looked;
	int x;
	int y;
};

//Structure to help with keeping track of where we are.

struct DATORS{

	int nr;
	char data;

};

typedef struct{

	char* data;
	int offset;
	struct DATORS byte;	

}STRUCT_RETURN_BYTE;

typedef enum {
        GO_UP = 0,
        GO_LEFT = 1,
        GO_RIGHT = 2,
        JUST_GO = 3,
} ACTION;

//Defining some actions we can send to the robot.

typedef enum {
	STOP = 0,
        SLOWEST = 1,
        SLOW = 2,
        SLOW_MID = 3,
        MID = 4,
        MID_FAST = 5,
        FAST = 6,
        FASTEST = 7,
} SPEEDNESS;

//Defining some speeds we can command the robots to travel at.

STRUCT_RETURN_BYTE* next_bit(char* data, int offset, int pixel_size, int repeat_size);

DARNIT_BBOX *buttonlist;
unsigned int button_up, button_right, button_left, button_menu;
struct LOOKED looked;

int serial_port;
struct termios2 config;

//Global stuff \o/ Integers, home-made structure, text object, serial port. 

int main(int argc, char **argv) {
	
	d_init_custom("AutoKorg", 800, 600, 0, "robot", NULL);
	//Calling an initiating function. Said function creates a window
	//with resolution 800x600, that's now full-screen and with the title AutoKorg.
	
	DARNIT_FONT *kgfont = d_font_load("KGRayofSunshine.ttf", 60, 256, 256);
	//We create a font handle.
	
	DARNIT_TEXT_SURFACE *papper = d_text_surface_new(kgfont, 100 , 800, 10, 0);
	//Here we create a handle to a text surface, using the font previously handled.
	
	d_cursor_show(1); //Call a funciton which shows the cursor in the window.
	
	/*if (!papper)
		fprintf(stderr, "Paper didn't load\n");*/
		
	//DARNIT_SOUND *music = d_sound_streamed_load("TowerTown.ogg", 1, 1); 
	//Load an audiophile.
	
	//d_sound_play(music, 1, 128, 128, 0); 
	//Play la musica.
	
	FILE * music = fopen("Rick Astley - Never Gonna Give You Up.wav", "r");
	
	unsigned char data_collection[1024];
	
	initiate_buttons(); 
	initiate_serial_port();
	//Initiate our buttons and serial port \o/
	
	sleep(5);
	
	while(1024 == fread(data_collection, 512, 512, music)){
		
		write(serial_port, data_collection, 1024);
		usleep(100);
		
	}
	
	/*int fil = open("Robot.c", O_RDONLY);
	
	char svar[64];
	
	write(serial_port, "Arne", sizeof("Arne"));
	
	//wait(5);
	
	read(serial_port, svar, sizeof("Arne"));
	
	printf("%s", svar);*/
	
	DARNIT_TILESHEET * ui1 = d_render_tilesheet_load("UI thing.gif", 28, 10, 1);
	DARNIT_TILESHEET * ui2 = d_render_tilesheet_load("UI thing 2.gif", 12, 29, 1);
	
	for(;;) {
		
		//Program loop.
		
		d_render_begin(); 
		//Call to start rendering.
		
		d_render_blend_enable(); 
		//Enable blend, a call to a function apparently needed.
		
		d_render_tint(255,255,255,255); 
		//Another call to a needed function. Don't question the libdarnit.
		
		//d_text_surface_string_append(papper, svar);
		
		d_render_tile_blit(ui1, 0, 50, 300);
		d_render_tile_blit(ui1, 0, (760 - 50), 300);
		d_render_tile_blit(ui1, 0, 100, 300);
		d_render_tile_blit(ui1, 0, (760 - 100), 300);
		d_render_tile_blit(ui1, 0, 150, 300);
		d_render_tile_blit(ui1, 0, (760 - 150), 300);
		d_render_tile_blit(ui2, 0, 390, 20);
		d_render_tile_blit(ui2, 0, 390, 70);
		d_render_tile_blit(ui2, 0, 390, 130);
		//Draws us some graphics.
		
		do_stuff(papper); 
		//Call to a function which does stuff.
		
		d_render_end(); 
		//Call to a function which you do at the stop of rendering.
		
		d_loop(); 
		//Call to a function that is to be called inbetween frames.
		
		d_text_surface_reset(papper); 
		//Lastly we clear the textsurface.
	}
	return 0;

	//End of show.
}

void initiate_buttons(void){
	
	buttonlist = d_bbox_new(8); //We make space for a list containing 8 boxes.
	
	button_up = d_bbox_add(buttonlist, 0, 0, 800, 200);
	//Then we add a box (button) to the button list.
	//The placement is: X = 50, Y = 100 relative to the program window.
	//The size is X = 400, Y = 150, reaching out from the placement point.
	
	button_left = d_bbox_add(buttonlist, 0, 0, 240, 400);
	button_right = d_bbox_add(buttonlist, 560, 0, 260, 400);
	button_menu = d_bbox_add(buttonlist, 300, 400, 200, 200);
	//And here we add three more buttons. Didn't see that coming, did you?

}

void initiate_serial_port(void){
	
	serial_port = open(MODEMDEVICE, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
	//Open serial port as ReadWrite.
	
	ioctl(serial_port, TCGETS2, &config);
	
	config.c_cflag &= ~CBAUD;
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= BOTHER;
	config.c_cflag |= CS8;
	//These options will let us set our own baudrate,
	//, no parity and a bytesize of 8 bits.
	
	config.c_iflag = 0;
	//No input processing.
	
	config.c_oflag = 0;
	//No output processing.
	
	config.c_lflag = 0;
	//No echoing and stuff.
	
	config.c_ispeed = BAUDRATE;
	config.c_ospeed = BAUDRATE;
	//Set our baudrates.
	
	ioctl(serial_port, TCSETS2, &config);
	//Sets the serial_port to work as we want it to,
	//with the custom baudrate BAUDRATE. 
	
}

void do_stuff(DARNIT_TEXT_SURFACE *textplace){
	
	DARNIT_MOUSE mus = d_mouse_get();
	//Calling a function which gives us various mouse data.
	
	d_text_surface_string_append(textplace, " X: ");
	//"Printing" stuff. (Adds text to the text object.)
	
	
	char femton[256]; 
	sprintf(femton, "%i", mus.x);
	//Converts the mouse x-coordinate to printable text.
	
	d_text_surface_string_append(textplace, femton);
	//Prints said x-coordinate.

	sprintf(femton, "%i", mus.y);
	
	d_text_surface_string_append(textplace, " Y: ");
	d_text_surface_string_append(textplace, femton);
	//Converts and prints Y-coordinate.
	
	if((((looked.x - 75) < mus.x) 
	&& (mus.x < (looked.x + 75))) 
	&& (((looked.y - 75) < mus.y) 
	&& (mus.y < (looked.y + 75)))){
	
		//We check to see if the mouse is within distance
		//of the saved point.
	
		//If it passes this check we count another frame.
		
		looked.frames_looked = looked.frames_looked + 1;
		
		if(100 < looked.frames_looked){
		
		//Then, if we've looked at the same area for 100 frames...
		
			d_text_surface_string_append(textplace, "   Nu! :D");
			//We "print" more text.
			
			click(looked.x, looked.y, textplace);
			//And call the click function, clicking the point
			//from which we defined the area.
		}
	}
	else{
		
		//If we are outside of the area we first looked at,
		//we'll set the new area of looking to the point where 
		//the mouse is, and set the frames looked to 0.
		
		looked.frames_looked = 0;
		looked.x = mus.x;
		looked.y = mus.y;
	}
	
	d_text_surface_draw(textplace);
	//Then we call the function which draws the textobject we've
	//appended our text to.
}

void click(int x, int y, DARNIT_TEXT_SURFACE *textplace){
	
	unsigned int hits[4] = {110, 110, 110, 110};
	d_bbox_test(buttonlist, x, y, 1, 1, hits, 4);
	
	//Now we check if the clicked point is overlapping any of
	//our four buttons.
	
	if(hits[0] == button_up || hits[1] == button_up ||
	   hits[2] == button_up || hits[3] == button_up ){
		
		d_text_surface_string_append(textplace, "\n Uppknappen klickades.");	   
	}
	
	else if(hits[0] == button_left || hits[1] == button_left ||
	   hits[2] == button_left || hits[3] == button_left ){
		
		//Left button click.
		
		d_text_surface_string_append(textplace, "\n Vänsterknappen klickades.");		   
	}
	
	else if(hits[0] == button_right || hits[1] == button_right ||
	   hits[2] == button_right || hits[3] == button_right ){
		   
		//Right button click.
		   
		d_text_surface_string_append(textplace, "\n Högerknappen klickades.");	
	}
	
	else if(hits[0] == button_menu || hits[1] == button_menu ||
	   hits[2] == button_menu || hits[3] == button_menu ){
		   
		//Menu button click.
		
		d_text_surface_string_append(textplace, "\n Menyknappen klickades.");
	}
	
	else {
		//And if we *haven't* pressed a button, this is where we go.
		
		d_text_surface_string_append(textplace, "\n Non-click click thing.");
	}
}

void write_to_serial(ACTION action, SPEEDNESS speed){
	

	
}












STRUCT_RETURN_BYTE* next_bit(char* data, int offset, int pixel_size, int repeat_size){

	char temp = data[0], temp2;
	int new_offset = pixel_size;

	static int max_val = 6;

	if (offset != 0){
	
		//If we dont start at the first byte.
	
		temp = temp >> offset;
		
		//Shuffle down the data to avoid the already used bits.

		temp2 = data[1] << offset;

		temp = temp + temp2;

		//Now we fetch enough data to fill the rest of the char,
		//and add those bits to the byte.

	}

	STRUCT_RETURN_BYTE answer;

	if(max_val < temp){

		int temp_offset = offset+3;

		//If we've encountered an RLE, we move our offset forwards 3 bits.

		int re_size = (make_binary(repeat_size));

		//And we convert the number of bits to a mask which will
		//be (repeat_size) number of 1 's.

		if(8 <= temp_offset){

			temp_offset = temp_offset - 8;
			data++;
			temp = data[0];

			//We move our pointer forwards a byte, and change offset accordingly.

		}

		if (temp_offset != 0){
		
			//Om vi inte ska börja vid första biten...
	
			temp = temp >> temp_offset;
		
			//Shuffle down the data to avoid the already used bits.

			temp2 = data[1] << offset;
	
			temp = temp + temp2;

			//Now we fetch enough data to fill the rest of the char,
			//and add those bits to the byte.

		}

		char read_number[4];

		//We make some space for some calculations.
	
		read_number[0] = (data[0] >> temp_offset) + (data[1] << temp_offset);
		read_number[1] = (data[1] >> temp_offset) + (data[2] << temp_offset);

		//We then read the following 2 bytes with offset in mind. 

		answer.byte.data = temp;
		answer.byte.nr = ((int)read_number[0] + ((int)read_number[1] * 0x100)) & re_size;

		//And then we store the data as we want it; the pixel read on its own,
		//and the number is the number of bits.

	}
	else{
		
		temp = temp << (8 - pixel_size);

		//Otherwise we simply format our pixel,

		answer.byte.data = temp;
		answer.byte.nr = 1;

		//and add it to our answer structure.

		new_offset = new_offset + offset;

		//We also calculate our new offset.

	}



	if(new_offset < 8){
		
		answer.offset = new_offset;
		answer.data = data;

		//If our offset isnt bigger than a byte
		//we return our answer structure.

	}
	
	else{

		char *tempdata = data;

		while (8 <= new_offset){
			
			tempdata++;
			new_offset = new_offset - 8;

		}

		answer.offset = new_offset;
		answer.data = data;

		//Else, if our offset is more than a byte
		//we move our pointer forward a byte,
		//subtract 8 from our offset and loop untill
		//offset is less than 8.

		//When all is said and done we return stuff.
	}

	return &answer;

}

int make_binary(int repeat_size){

	int temp = 0;

	while(0 < repeat_size){

		temp = (temp * 2) + 1;
		repeat_size--;
	} 

	return temp;

}
