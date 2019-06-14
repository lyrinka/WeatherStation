#include <stm32f10x.h> 
#include <OS.h> 
#include <Serial.h> 
#include <Networking_RxStateMachine.h> 

// Internals 
typedef struct _buffer{ 
	char * p; 
	int i; 
	int s; 
}buffer; 

buffer Buf_New(u32 size){ 
	buffer buf; 
	buf.p = Lin_MemAlloc(size); 
	buf.s = size; 
	buf.i = -1; 
	return buf; 
}

void Buf_Push(buffer * buf, char ch){ 
	if(buf->i++ >= buf->s) 
		buf->i = 0; 
	buf->p[buf->i] = ch; 
}

char * Buf_Retrieve(buffer * buf){ 
	int i; 
	for(i = buf->i; i >= 0; i--){ 
		char ch = buf->p[i]; 
		if(ch != ' ' && ch != '\t') break; 
	}
	i++; 
	char * ptr = Lin_MemAlloc(i + 1); 
//memcpy(ptr, buf->p, i); 
	for(int j = 0; j < i; j++) 
		ptr[j] = buf->p[j]; 
	ptr[i] ='\0'; 
	buf->i = -1; 
	return ptr; 
}

int String_Cmp(const char * str1, const char * str2, int len){ 
	for(int i = 0; i < len; i++){ 
		int a = str1[i] - str2[i]; 
		if(a > 0) return 1; 
		if(a < 0) return -1; 
	}
	return 0; 
}

int String_toUint(const char * str, int size, int * offset, int * value){ 
	int i = *offset; 
	int num = 0; 
	int state = 0; 
	while(state >= 0){ 
		if(i >= size) break; 
		char ch = str[i++]; 
		switch(state){ 
			case 0: { 
				if(ch >= '0' && ch <= '9'){ 
					num = ch - '0'; 
					state = 1; 
				}
				break; 
			}
			case 1: { 
				if(ch >= '0' && ch <= '9'){ 
					num = num * 10 + (ch - '0'); 
					break; 
				}
				state = -1; 
				break; 
			default: 
				state = 0; 
				break; 
			}
		}
	}
	*offset = i; 
	if(state == 0) return -1; 
	*value = num; 
	return 0; 
}

// Externals 
void Net_RxStateMachine(void){ 
	int state = 0; 
	int startup = 0; 
	buffer buf = Buf_New(32); 
	
	while(state >= 0){ 
		switch(state){ 
			
			case 0: { 
				char ch = Serial_Getch(); 
				switch(ch){ 
					case '\t': 
					case ' ' : 
					case '\n': 
					case '\r': 
						break; 
					case '+': 
						Buf_Push(&buf, ch); 
						state = 3; 
						break; 
					case '>': 
						Buf_Push(&buf, ch); 
						state = 2; 
						break; 
					default: 
						Buf_Push(&buf, ch); 
						state = 1; 
						break; 
				}
				break; 
			}
			
			case 1: { 
				char ch = Serial_Getch(); 
				switch(ch){ 
					case '\n': 
					case '\r': 
						state = 2; 
						break; 
					default: 
						Buf_Push(&buf, ch); 
						break; 
				}
				break; 
			}
			
			case 2: { 
				state = 0; 
				char * str = Buf_Retrieve(&buf); 
				if(startup == 0){ 
					if(String_Cmp(str, "ready", 5)){ 
						Lin_MemFree(str); 
						break; 
					}
					startup = 1; 
				}
				Net_RxRply_cbFunc(str); 
				break; 
			}
			
			case 3: { 
				char ch = Serial_Getch(); 
				switch(ch){ 
					case '\n': 
					case '\r': 
						state = 2; 
						break; 
					case ':': 
						Buf_Push(&buf, ch); 
						state = 4; 
						break; 
					default: 
						Buf_Push(&buf, ch); 
						break; 
				}
				break; 
			}
			
			case 4:{ 
//				const char IPD[4] = "IPD"; 
//				if(memcmp(buf.p + 1, IPD, 3) != 0){ 
					if(buf.p[1] == 'I' && buf.p[2] == 'P' && buf.p[3] == 'D'){ 
					state = 5; 
					break; 
				}
				state = 1; 
				break; 
				
			}
			
			case 5: { 
				int id, size; 
				int offset = 1; 
				if(String_toUint(buf.p, buf.s, &offset, &id)){ 
					state = 1; 
					break; 
				}
				if(String_toUint(buf.p, buf.s, &offset, &size)){ 
					state = 1; 
					break; 
				}
				char * ptr = Lin_MemAlloc(size + 1); 
				ptr[size] = '\0'; 
				for(int i = 0; i < size; i++)
					ptr[i] = Serial_Getch(); 
				Net_RxData_cbFunc(id, ptr); 
				buf.i = -1; 
				state = 0; 
				break; 
			}
			
			default: {
				state = 0; 
				break; 
			}
		}
	}
	Lin_MemFree(buf.p); 
}

void Net_RxStateMachine_Init(void){ 
	TASK Task; 
	Task = OS_New(512, Net_RxStateMachine); 
	OS_ChgPri(Task, -1); 
	OS_GenEvent(Task, 0); 
	OS_Preempt(); 
}

// End of file. 
