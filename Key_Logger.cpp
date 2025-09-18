#include "Key_Logger.h"
#include "keyboard.h"
#include "mouse.h"

static Keyboard_State g_PrevState{};
static Keyboard_State g_TriggerState{};
static Keyboard_State g_RealeaseState{};

static Mouse_State g_mouseState{};
static bool g_PrevMouseInput_R = false;
static bool g_PrevMouseInput_L = false;
static bool g_NowMouseInput_R = false;
static bool g_NowMouseInput_L = false;
static float g_MouseScrollerPrevInput = 0;
static Mouse_Inputstate R_state = NONE;
static Mouse_Inputstate L_state = NONE;
//static Mouse_Inputstate Scroller_state = NONE;

static MouseScrollerOutPut g_mouseScrollerOutPut;

void KeyLogger_Initialize()
{
	Keyboard_Initialize();
}

void KeyLogger_Update()
{
	const Keyboard_State* pState = Keyboard_GetState();
	LPBYTE pn = (LPBYTE)pState;
	LPBYTE pp = (LPBYTE)&g_PrevState;
	LPBYTE pt = (LPBYTE)&g_TriggerState;
	LPBYTE pr = (LPBYTE)&g_RealeaseState;

	for (int i = 0; i < sizeof(Keyboard_State); i++)
	{
		pt[i] = (pp[i] ^ pn[i]) & pn[i];
		pr[i] = (pp[i] ^ pn[i]) & pp[i];
	}
	g_PrevState = *pState;
}
void MouseLogger_Update()
{
	Mouse_GetState(&g_mouseState);
	if (g_PrevMouseInput_L == false && g_mouseState.leftButton == false)
	{
		R_state = NONE;
	}
	if (g_PrevMouseInput_L == false && g_mouseState.leftButton == true)
	{
		R_state = TRIGGER;
	}
	if (g_PrevMouseInput_L == true && g_mouseState.leftButton == false)
	{
		R_state = RELEASE;
	}
	if (g_PrevMouseInput_L == true && g_mouseState.leftButton == true)
	{
		R_state = DOWN;
	}
	if (g_PrevMouseInput_R == false && g_mouseState.rightButton == false)
	{
		L_state = NONE;
	}
	if (g_PrevMouseInput_R == false && g_mouseState.rightButton == true)
	{
		L_state = TRIGGER;
	}
	if (g_PrevMouseInput_R == true && g_mouseState.rightButton == false)
	{
		L_state = RELEASE;
	}
	if (g_PrevMouseInput_R == true && g_mouseState.rightButton == true)
	{
		L_state = DOWN;
	}
	if (g_MouseScrollerPrevInput != g_mouseState.scrollWheelValue)
	{
		g_mouseScrollerOutPut.trigger = true;
		g_mouseScrollerOutPut.value = g_mouseState.scrollWheelValue - g_MouseScrollerPrevInput;
	}
	else
	{
		g_mouseScrollerOutPut.trigger = false;
		g_mouseScrollerOutPut.value = 0;
	}
	g_PrevMouseInput_L = g_mouseState.leftButton;
	g_PrevMouseInput_R = g_mouseState.rightButton;
	g_MouseScrollerPrevInput = g_mouseState.scrollWheelValue;
}

bool KeyLogger_IsPressed(Keyboard_Keys key)
{
	return Keyboard_IsKeyDown(key);
}

bool KeyLogger_IsTrigger(Keyboard_Keys key)
{
	return Keyboard_IsKeyDown(key, &g_TriggerState);
}

bool KeyLogger_IsRelease(Keyboard_Keys key)
{
	return Keyboard_IsKeyDown(key, &g_RealeaseState);
}

MouseScrollerOutPut MouseLogger_IsScroll()
{
	return g_mouseScrollerOutPut;
}

bool MouseLogger_IsDown(int mouseButton)
{
	if (mouseButton == 0)
	{
		if (R_state == DOWN)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (mouseButton == 1)
	{
		if (L_state == DOWN)
		{
			return true;
		}
		else
		{
			return false;
		}

	}
}
bool MouseLogger_IsRelease(int mouseButton)
{
	if (mouseButton == 0)
	{
		if (R_state == RELEASE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (mouseButton == 1)
	{
		if (L_state == RELEASE)
		{
			return true;
		}
		else
		{
			return false;
		}

	}
}
bool MouseLogger_IsTrigger(int mouseButton)
{
	if (mouseButton == 0)
	{
		if (R_state == TRIGGER)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	if (mouseButton == 1)
	{
		if (L_state == TRIGGER)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
