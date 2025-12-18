#ifndef PlayerData_H
#define PlayerData_H

void PlayerData_Initialize();
void PlayerData_Finitialize();
void PlayerData_Update(double elapsed_time);
void PlayerData_AddCoin(int amount); 
int PlayerData_GetCoin();
//void PlayerData_Draw();

#endif