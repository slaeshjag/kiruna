#include <asm/termios.h>
#include <darnit/darnit.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
//Includifications.

#define BAUDRATE 115200
#define MODEMDEVICE "/dev/ttyUSB0"
#define PACKAGE_SIZE 8
//Serial port definitions.

#define HD_IMAGE_WIDTH 640
#define HD_IMAGE_HEIGHT 480
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
//Image numbers.

#define COLOR_X 5
#define COLOR_Y 444
#define MENU_X (WINDOW_WIDTH - COLOR_X - BUTTON_WIDTH)
#define MENU_Y COLOR_Y
#define BUTTON_WIDTH 172
#define BUTTON_HEIGHT 150
//Button numbers.

#define DISCO 0x10
#define TALK 2
#define SOUND 1
#define UI 0
//States.

#define PIXEL_SIZE 8
#define BUF_LEN 4000
//Sound numbers.

void draw_interface();
void do_stuff(DARNIT_TEXT_SURFACE *textplace);
void click(int x, int y, DARNIT_TEXT_SURFACE *textplace);
void initiate_buttons(void);
void initiate_serial_port(void);
void audio_buff_fill();
void image_buff_fill();
void send_message();
void * transfer();
void * musikka();
void * listen_to_sound();
void* get_image();
int read_sound_test(signed short *buff, int buff_len, int pos, void *data);
int make_binary(int repeat_size);
int check_true(unsigned int* hits, unsigned int value);

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

struct SOUND_BUFFER{

	unsigned char buffer[BUF_LEN];
	unsigned int location;
	unsigned int buffer_stop;
};

//Structure that is our sound buffer.

typedef struct{

	char* data;
	int offset;
	struct DATORS byte;

}STRUCT_RETURN_BYTE;

typedef enum {
	
	MOTORS_FORWARD = 0x700,
	MOTORS_BACKWARDS = 0x400,
	TURN_LEFT = 0x500,
	TURN_RIGHT = 0x600,
	STOP = 0x0,
	SYNC = 0xFFFF,
	SEND_MIC = 0x0,
	SEND_SPEAKER = 0x1,
	SEND_CAMERA = 0x2,
	NEW_FRAME = 0x3,

} ACTION;

//Defining some actions we can send to the robot.

DARNIT_SOUND *robot_sound;
DARNIT_BBOX *buttonlist;
unsigned int button_up, button_right, button_left, button_down, button_menu, button_color;
struct LOOKED looked;

DARNIT_TILESHEET * ui1;
DARNIT_TILESHEET * ui2;
DARNIT_TILESHEET * graphic_button_menu;
DARNIT_TILESHEET * graphic_button_back;
DARNIT_TILESHEET * graphic_button_color;
DARNIT_IMAGE_DATA imgdat;
DARNIT_TILESHEET *ts;
DARNIT_TILE *t;

int serial_port, package_count, package_in_buff, image_i = 0, audio_i = 0, hello;
unsigned short robot_state = 0, robot_packages = 0, robot_motors = 0, send_audio = 0;
int state = 0, stickyness = 20;
int* package_buff, image_data;

struct termios2 config;
struct SOUND_BUFFER sound_buf;

int mic_listening;

//Global stuff \o/ Integers, home-made structures, text objects, and a serial port.

int main(int argc, char **argv) {

	d_init_custom("AutoKorg", WINDOW_WIDTH, WINDOW_HEIGHT, 0, "robot", NULL);
	//Calling an initiating function. Said function creates a window
	//with resolution 800x600, that's now full-screen and with the title AutoKorg.
	
	DARNIT_FONT *kgfont = d_font_load("KGRayofSunshine.ttf", 60, 256, 256);
	//We create a font handle.
	
	DARNIT_TEXT_SURFACE *papper = d_text_surface_new(kgfont, 100 , 800, 10, 0);
	//Here we create a handle to a text surface, using the font previously handled.
	
	/*pthread_t threads[1];
	hello = pthread_create(&threads[0], NULL, musikka, (void *) NULL);*/
	
	d_cursor_show(1); //Call a funciton which shows the cursor in the window.
	
	initiate_buttons();
	initiate_serial_port();
	
	for(;;) {
		
		//Program loop.
		
		d_render_begin(); 
		//Call to start rendering.
		
		d_render_blend_enable(); 
		//Enable blend, a call to a function apparently needed.
		
		if(state < DISCO){
			d_render_tint(255,255,255,255);
		} 
		else {
			d_render_tint(rand() & 0xF0, rand() & 0xF0, rand() & 0xF0, 255);
		}
		//Another call to a needed function. Don't question the libdarnit.
		
		//d_text_surface_string_append(papper, svar);
		
		do_stuff(papper); 
		//Call to a function which does stuff.
		
		draw_interface();
		//Draws us some graphics.
		
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

void draw_interface(){
	
	switch(state & 0xF){
	case SOUND:
		
		//Draw the menu.

		d_render_tile_blit(graphic_button_back, 0, COLOR_X, COLOR_Y);
		
		break;
		
	case UI:
		
		//Draw the UI
		
		d_render_tile_blit(ui1, 0, 50, 300);
		d_render_tile_blit(ui1, 0, (760 - 50), 300);
		d_render_tile_blit(ui1, 0, 100, 300);
		d_render_tile_blit(ui1, 0, (760 - 100), 300);
		d_render_tile_blit(ui1, 0, 150, 300);
		d_render_tile_blit(ui1, 0, (760 - 150), 300);
		d_render_tile_blit(ui2, 0, 390, 20);
		d_render_tile_blit(ui2, 0, 390, 70);
		d_render_tile_blit(ui2, 0, 390, 130);
		d_render_tile_blit(ui2, 0, 390, 430);
		d_render_tile_blit(ui2, 0, 390, 490);
		d_render_tile_blit(ui2, 0, 390, 550);
		//Draw lines upon the screen.
		
		d_render_tile_blit(graphic_button_menu, 0, MENU_X, MENU_Y);
		d_render_tile_blit(graphic_button_color, 0, COLOR_X, COLOR_Y);
		//Draw our buttons.
		
		break;
		
	case TALK:
	
		//Render UI for the colour image.
	
		d_render_tile_blit(graphic_button_back, 0, MENU_X, MENU_Y);
		
		break;
	}
}

void initiate_buttons(void){
	
	buttonlist = d_bbox_new(8);
	//We make space for a list containing 8 boxes.
	
	button_up = d_bbox_add(buttonlist, 0, 0, 1000, 180);
	//Then we add a box (button) to the button list.
	//The placement is: X = 50, Y = 100 relative to the program window.
	//The size is X = 400, Y = 150, reaching out from the placement point.
	
	button_left = d_bbox_add(buttonlist, 0, 0, 240, 400);
	button_right = d_bbox_add(buttonlist, 560, 0, 260, 400);
	button_down = d_bbox_add(buttonlist, 150, 400, 500, 200);
	button_menu = d_bbox_add(buttonlist, MENU_X, MENU_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	button_color = d_bbox_add(buttonlist, COLOR_X, COLOR_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	//And here we add not three, not four, but five more buttons! Didn't see that coming, did you?
	
	ui1 = d_render_tilesheet_load("UI thing.gif", 28, 10, 1);
	ui2 = d_render_tilesheet_load("UI thing 2.gif", 12, 29, 1);
	//Here we also load up our graphic sprites. These are just some lines.

	graphic_button_menu  = d_render_tilesheet_load("button_menu.gif", BUTTON_WIDTH, BUTTON_HEIGHT, 1);
	graphic_button_back  = d_render_tilesheet_load("button_back.gif", BUTTON_WIDTH, BUTTON_HEIGHT, 1);
	graphic_button_color = d_render_tilesheet_load("button_color.gif", BUTTON_WIDTH, BUTTON_HEIGHT, 1);
	//Loads button graphics.
	
	imgdat.w = IMAGE_WIDTH;
	imgdat.h = IMAGE_HEIGHT;
	imgdat.data = malloc(sizeof(int) * imgdat.w * imgdat.h);
	ts = d_render_tilesheet_new(1, 1, imgdat.w, imgdat.h, DARNIT_PFORMAT_RGBA8);
	d_render_tilesheet_scale_algorithm(ts, DARNIT_SCALE_LINEAR);
	//Initiate some image graphics. 
}

void initiate_serial_port(void){
	
	if((serial_port = open(MODEMDEVICE, O_RDWR | O_NOCTTY)) < 0){
		
		printf("AAAAAAH!! Serial port not working.\n");
		return;
	}
	//Open serial port as ReadWrite.
	
	ioctl(serial_port, TCGETS2, &config);
	
	config.c_cflag &= ~CBAUD;
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= BOTHER;
	config.c_cflag |= CS8;
	//These options will let us set our own baudrate,
	//no parity and a bytesize of 8 bits.
	
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
	
	robot_sound = d_sound_callback_load(read_sound_test, NULL, 1);
	mic_listening = open("/tmp/microphone_data", O_RDONLY);
	//Open the file to which we pipe our microphone_data,
	//and make us able to play a sound from the robot.
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
		
		if(stickyness < looked.frames_looked){
		
		//Then, if we've looked at the same area for 100 frames...
			
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
	
	unsigned int hits[8] = {110, 110, 110, 110, 110, 110, 110, 110};
	d_bbox_test(buttonlist, x, y, 1, 1, hits, 8);
	//Now we check if the clicked point is overlapping any of
	//our four buttons.
	
	looked.frames_looked = 0;
	//Reset the frames looked.
	
	if(check_true(hits, button_up)){
		
		if(state == UI){
			robot_motors = MOTORS_FORWARD;
			robot_state = NEW_FRAME;  
			send_message(); 
			
			//Tell the robot to drive forwards, and send us camera data.
		}
	}
	
	else if(check_true(hits, button_down)){
		   
		//Down button click.
		
		if(state == UI){
			robot_motors = MOTORS_BACKWARDS;
			robot_state = NEW_FRAME;  
			send_message(); 
			
			//Tell the robot to drive backwards, and send us camera data.
		}
	}
	
	else if(check_true(hits, button_left)){
		
		//Left button click.

		if(state == UI){
			robot_motors = TURN_LEFT;
			robot_state = NEW_FRAME;  
			send_message(); 
			
			//Tell the robot to turn left, and send us camera data.	
		}	   
	}
	
	else if(check_true(hits, button_right)){
		   
		//Right button click.
		
		if(state == UI){
			robot_motors = TURN_RIGHT;
			robot_state = NEW_FRAME;  
			send_message(); 
			
			//Tell the robot to turn right, and send us camera data.	
		}
	}
	
	else if(check_true(hits, button_menu)){
		   
		//Menu button click.
			
		if((state & 0xF) == UI){
			
			state &= 0xF0;
			state &= 0x0;
			state |= SOUND;
		}
		
		else if((state & 0xF) == TALK){
			
			state &= 0xF0;
			state |= UI;
		}
	}
	
	else if(check_true(hits, button_color)){
		
		if((state & 0xF) == UI){
			
			state &= 0xF0;
			state |= TALK;
		}
		else if ((state & 0xF) == SOUND){
			
			state &= 0xF0;
			state |= UI;
		}
		
	}
	
	else {
		//And if we *haven't* pressed a button, this is where we go.
		
		if(state == UI){
			robot_motors = STOP;
			robot_state = NEW_FRAME;  
			send_message(); 
			
			//Tell the robot to shut down its motors, and send us camera data.	
		}
	}
}

int check_true(unsigned int* hits, unsigned int value){
	
	if(hits[0] == value || hits[1] == value ||
	   hits[2] == value || hits[3] == value ||
	   hits[4] == value || hits[5] == value ||
	   hits[6] == value || hits[7] == value   ){
		   
		//If the number that we're looking for is in
		//one of the first 8 blocks of the array
		//we're looking in, return 1.
		   
		return 1;
	}
	
	else{
		return 0;
	}
}

void send_message(){
	
	unsigned short message = robot_motors | robot_state; //Make the message we want to send the robot.
	
	if(!send_audio){
		
		write(serial_port, &message, sizeof(message));		     //Send the robot our message.
	}
	else{
		unsigned short microphone_data[7];   //Make space for a few (6) packets to send the robot.
		read(mic_listening, microphone_data, sizeof(microphone_data));     //Read microphone data.
		
		message |= (7 << 3); 		//Add to the message that we're sending 7 packets of data.
		write(serial_port, &message, sizeof(message)); 			      //Write the message.
		write(serial_port, microphone_data, sizeof(microphone_data)); //Write the microphone data.
	}
	
}

void * musikka(){
	
	DARNIT_SOUND *music = d_sound_streamed_load("TowerTown.ogg", 1, 1); 
	//Load an audiophile.
	
	d_sound_play(music, 1, 128, 128, 0); 
	//Play la musica.
	
	pthread_exit(NULL);
}

void* transfer(){
	
	unsigned char buf[22600];
	int music_test = open("test.wav", O_RDONLY);
	int i;
	//Make some variables we need. Amongst these we open
	//a stream to the soundfile we want to send.
	
	if (read(music_test, buf, 22600)){
		
		//We then read 22600 bytes from the file stream,
		//and send them eight at a time to the serial_port, 
		//waiting inbetween each batch.
		
		for(i = 0; i < (22600 / 8); i++){
			
			if(write(serial_port, &buf[(8*i)], 8)){
				
				sleep(2);
			}
		}
	}
	
	pthread_exit(NULL);
}

int read_sound_test(signed short *buff, int buff_len, int pos, void *data){
	
	read(serial_port, data, buff_len);
	
	return(buff_len);
}

void* get_image(){

	while(7){
		//Infinite looping.
		
		send_message();
		//Calls a function which might send a message to our robot.
		
		read(serial_port, package_buff, PACKAGE_SIZE);
		//We then read a package worth of data from our radio link.
		
		switch(state & 0xF){
		case UI:
		
			//We then proceed to look at which state we are in.
			//When we're in the UI mode, where we steer our robot,
			//the robot will send us image data. We handle this here.
			
			send_audio = 0;
			//Stop sending audio.
			
			image_buff_fill();
			//Handle the data we read from the radio link.
		
			break;
	
		case TALK:
		
			//If we're in the "send audio to robot"-state,
			//we simply set our little flag which tells the
			//send function we want to send audio data.
		
			send_audio = 1;
		
			break;
	
		case SOUND:
		
			//If we're in our "listen to robot microphone" state
			//we're going to get audio sent to us from the robot.
			//We handle this here.
			
			send_audio = 0;
			//We stop sending audio.
			
			audio_buff_fill();
			//And then we transfer the audio from the read package
			//to the audio buffer.
		
			break;
		}
	}
	
	pthread_exit(NULL);
}

void image_buff_fill(){
	
	int temp, temp2, offset;
	//Make us some variables.
}

void audio_buff_fill(){
	
	
	
}
