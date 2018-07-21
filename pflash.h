#ifndef PFLASH_H
#define PFLASH_H


int PFlash_Write(unsigned long adr, void near *buf);
int PFlash_Erase(unsigned long adr);
//int Check(void);



#define ACKW 0x8AFE
 

#define FSR     0xFFF000      // Flash Status Register Address
#define PROCON  0xFFF004
#define CAN_HWOBJ ((struct stCanObj volatile far *) 0x200300)


#endif 