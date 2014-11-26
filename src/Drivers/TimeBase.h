#ifndef _TIMEBASE_H_
#define _TIMEBASE_H_


void Delay(unsigned int delay);
void TimeBase_Init(void);
void StartDelay(unsigned short nTime);
unsigned char DelayIsEnd(void);
void TIM2_UpdateISRHandler(void);

#endif