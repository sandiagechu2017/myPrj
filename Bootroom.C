#include "main.h"
#include "Pflash.h"								
#include <absacc.h> 
#pragma MEDIUM
#pragma RENAMECLASS (FCODE=BCODE)

/*
The upper 256 Bytes of the PSRAM may be altered during the initialization phase
after a reset. This area, therefore, should not store data to be preserved beyond a
reset.
so, Boot_PSRAM_Start Addr:0xE00100
*/


extern void Main_App(void);

unsigned char sdata buf[128] = "Code execute out of PSRAM,Erase and Write Flash Completely";
unsigned char data_read;

#define FLASH_ADR 0xC20000
unsigned long Boot_Write_Addr_32 = FLASH_ADR;

unsigned char Boot_App_Switch_u08=0x00;	/*�л�Boot ��App ����*/

/*Debug Mode ���޸� Boot_Write_Addr_32 = 0xC40000(Bootrom),���Կ���
PSRAM�������еĳ�����Խ�Boot�����ĳ����޸ĵ�
*/


static void Delay(void)	;

unsigned char PFlash_Read_Chr (unsigned long adr) 
{

   return (HVAR(unsigned char, adr))	;

}


void Boot_Function(void)
{

	while (1)
	{
	 Delay();
	 TOGGLEIO_P10_6;
  	  #if 0
	  data_read = PFlash_Read_Chr (Boot_Write_Addr_32);  //��Flash�е�����

	  PFlash_Erase (Boot_Write_Addr_32);        // Erase sector starting at address 0xC08000 (FLASH_ADR);

	  data_read = PFlash_Read_Chr (Boot_Write_Addr_32);  //��Flash�е�����
	  
	  PFlash_Write (Boot_Write_Addr_32, buf);   // Program 64 bytes to address 0xC08000 (FLASH_ADR);

	  data_read = PFlash_Read_Chr (Boot_Write_Addr_32);  //��Flash�е�����
	  #endif 
		NOP();
	  if(Boot_App_Switch_u08 == 0xA5)
	  {
	 	 JMP(&Main_App);									/*���� APP FUNCTION*/
	  }


	}
	
}

static void Delay(void)
{
	uword i;
	uword j;
	for (i=0x0210;i!=0;i--)
	{
		for (j=0xFF00;j!=0;j--)
		{
			;
		}
	}

}




