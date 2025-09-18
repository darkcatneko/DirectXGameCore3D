#ifndef KEY_LOGGER_H
#define KEY_LOGGER_H

#include "keyboard.h"
enum Mouse_Inputstate
{
	DOWN,
	TRIGGER,
	RELEASE,
	NONE,
};
struct MouseScrollerOutPut
{
	bool trigger = false;
	float value = 0;;
};
void KeyLogger_Initialize();
void KeyLogger_Update();
void MouseLogger_Update();

bool KeyLogger_IsPressed(Keyboard_Keys key);
bool KeyLogger_IsTrigger(Keyboard_Keys key);
bool KeyLogger_IsRelease(Keyboard_Keys key);
MouseScrollerOutPut MouseLogger_IsScroll();
bool MouseLogger_IsDown(int mouseButton);
bool MouseLogger_IsRelease(int mouseButton);
bool MouseLogger_IsTrigger(int mouseButton);

#endif // !KEY_LOGGER_H

