#include "PlayerData.h"

static int PlayerCoin = 0;

void PlayerData_Initialize()
{
}

void PlayerData_Finitialize()
{
}

void PlayerData_Update(double elapsed_time)
{
}

void PlayerData_AddCoin(int amount)
{
	PlayerCoin += amount;
}

int PlayerData_GetCoin()
{
	return PlayerCoin;
}
