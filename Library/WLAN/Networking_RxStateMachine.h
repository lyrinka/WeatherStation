
#ifndef __Networking_RxStateMachine_H__ 
#define __Networking_RxStateMachine_H__ 

extern void Net_RxRply_cbFunc(char * str); 
extern void Net_RxData_cbFunc(int id, char * str); 

void Net_RxStateMachine_Init(void); 

int String_Cmp(const char * str1, const char * str2, int len); 

#endif 
