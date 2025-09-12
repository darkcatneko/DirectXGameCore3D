#ifndef AUDIO_H
#define AUDIO_H


void InitAudio();
void UninitAudio();


int LoadAudio(const char* FileName);
void UnloadAudio(int Index);
void PlayAudio(int Index, bool Loop = false);

void SetAudioVolume(int index, float volume);
#endif

