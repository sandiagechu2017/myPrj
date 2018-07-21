#pragma MEDIUM
#pragma RENAMECLASS (FCODE=BCODE)

#include <absacc.h>               // Absolute Memory Access
#include <intrins.h>

#define FSR_BUSY      0xFFFF06    // Flash Busy Register Address
#define FSR_OP        0xFFFF08    // Flash Operation Register Address
#define FSR_PRO       0xFFFF0A    // Flash Protection Register Address



union IMB_FSR_BUSY {              // Flash Busy Register
  struct b  {
    unsigned short busyfm1  : 1;
    unsigned short busyfm2  : 1;
    unsigned short busyfm3  : 1;
    unsigned short res3     : 1;
    unsigned short res4     : 1;
    unsigned short res5     : 1;
    unsigned short res6     : 1;
    unsigned short res7     : 1;
    unsigned short pagem1   : 1;
    unsigned short pagem2   : 1;
    unsigned short pagem3   : 1;
  } b;
  unsigned short v;
};


union IMB_FSR_OP {                // Flash State Operations
  struct o  {
    unsigned short prog   : 1;
    unsigned short erase  : 1;
    unsigned short power  : 1;
    unsigned short mar    : 1;
    unsigned short sqer   : 1;
    unsigned short oper   : 1;
  } b;
  unsigned short v;
};

union IMB_FSR_PRO {                // Flash State Protection
  struct p  {
    unsigned short proin  : 1;
    unsigned short proiner: 1;
    unsigned short rprodis: 1;
    unsigned short wprodis: 1;
    unsigned short proer  : 1;
  } b;
  unsigned short v;
};


unsigned long base = 0xC00000;         // Device Base Address for the first Flash module [C00000H - C3FFFF] 256k Flash
                                       // Change the base address to 0xC40000 if the second Flash module [C40000-C4FFFF] is used.
                                          


void delay()
{
  int counter;
  for (counter =0; counter <0xFFFF; counter++) {_nop_ ();} // delay for hardware
}



/*
 *  Check Status of Flash Device
 *    Return Value:   0 - OK,  1 - Failed
 */

int Check (void) {
  union IMB_FSR_BUSY fbr;
  union IMB_FSR_OP fop;
  union IMB_FSR_PRO fpr;

  do {
     fbr.v = HVAR(unsigned short, FSR_BUSY);    // Read Busy Register

  } while (fbr.b.busyfm1 || fbr.b.busyfm2 || fbr.b.busyfm3); // Wait as long as Flash is Busy


  fop.v = HVAR(unsigned short, FSR_OP);        //Read Operation Register
  fpr.v = HVAR(unsigned short, FSR_PRO);       //Read Protection Register

  if (fop.b.prog || fop.b.erase) {
      HVAR(unsigned short, base | 0xAA) = 0xF5;  // Clear Status
  }

  if (fop.b.oper || fop.b.sqer || fpr.b.proer) {
      HVAR(unsigned short, base | 0xAA) = 0xF0;  // Reset to Read Mode
      return (1);                                // Failed or the Flash is in protection.
  }


  return (0);                                  // Done
}



/*
 *  Erase Block in Flash Memory
 *    Parameter:      adr:  Block Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int PFlash_Erase (unsigned long adr) {

  if ((adr >= 0xC0F000) && (adr < 0xC0FFFF))   // don't erase 0xC0F000-0xC0FFFF, this 4-kbyte is reserved for internal use
    return (0);



  HVAR(unsigned short, base | 0xAA) = 0x80;    // Erase Sector (1. Cycle)
  HVAR(unsigned short, base | 0x54) = 0xAA;    // Erase Sector (2. Cycle)
  HVAR(unsigned short, adr)         = 0x33;    // Erase Sector (3. Cycle)
  return (Check());                            // Check until Device Ready
}


/*
 *  Program Block in Flash Memory
 *    Parameter:      adr:  Block Address
 *                    buf:  Block Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int PFlash_Write (unsigned long adr, void near *buf) {
  unsigned int cnt;

  // Enter Page Mode
  HVAR(unsigned short, base | 0xAA) = 0x50;
  HVAR(unsigned short, adr) = 0xAA;
  delay();

  // Load Page
  for (cnt = 0; cnt < 64; cnt++)  {
      HVAR(unsigned short, base | 0xF2) = *((unsigned short near *) buf);
      buf = (unsigned short near *) buf+1;
  }
  
  // Write Page
  HVAR(unsigned short, base | 0xAA) = 0xA0;
  HVAR(unsigned short, base | 0x5A) = 0xAA;

  return (Check());                            // Check until Device Ready
}
