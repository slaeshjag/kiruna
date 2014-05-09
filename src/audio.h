#ifndef __AUDIO_H__
#define	__AUDIO_H__


void audio_init();
void speaker_prebuffer();
void speaker_fill(unsigned char *data, int data_len);
void speaker_output();
unsigned char microphone_sample();
int microphone_send();
void audio_loop();


#endif
