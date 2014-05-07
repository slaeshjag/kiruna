#ifndef __AUDIO_H__
#define	__AUDIO_H__


void audio_init();
void speaker_prebuffer();
void speaker_output();
unsigned char microphone_sample();
void microphone_send();
void audio_loop();


#endif
