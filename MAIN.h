//****************************************************************************
// @Module        Project Settings
// @Filename      MAIN.h
// @Project       Boot_EPS.dav
//----------------------------------------------------------------------------
// @Controller    Infineon XC2365B-40F80
//
// @Compiler      Keil
//
// @Codegenerator 2.0
//
// @Description   This file contains all function prototypes and macros for 
//                the MAIN module.
//
//----------------------------------------------------------------------------
// @Date          2011-8-15 15:48:23
//
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,1)

// USER CODE END



#ifndef _MAIN_H_
#define _MAIN_H_

//****************************************************************************
// @Project Includes
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,2)

// USER CODE END


//****************************************************************************
// @Macros
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,3)

// USER CODE END


//****************************************************************************
// @Defines
//****************************************************************************
#define KEIL

#define SEG(func) (unsigned int)(((unsigned long)((void (far*)(void))func) >> 16))
#define SOF(func) (unsigned int)(((void (far *) (void))func))

// USER CODE BEGIN (MAIN_Header,4)

// USER CODE END


//****************************************************************************
// @Typedefs
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,5)
#define JMP(func) ((void (far*) (void)) func)()
// USER CODE END


//****************************************************************************
// @Imported Global Variables
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,6)

// USER CODE END


//****************************************************************************
// @Global Variables
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,7)

// USER CODE END


//****************************************************************************
// @Prototypes Of Global Functions
//****************************************************************************


void MAIN_vUnlockProtecReg(void);

void MAIN_vLockProtecReg(void);

void MAIN_vChangeFreq(void);


// USER CODE BEGIN (MAIN_Header,8)

// USER CODE END


//****************************************************************************
// @Interrupt Vectors
//****************************************************************************

// USER CODE BEGIN (MAIN_Header,9)

// USER CODE END


//****************************************************************************
// @Project Includes
//****************************************************************************

#include <Intrins.h>

#include  "XC23xxBREGS.H"
#include  "SCS.h"

#include  "IO.h"


// USER CODE BEGIN (MAIN_Header,10)
//#include "Adc_Cfg.h"
#include "Bootroom.h"

// USER CODE END


#endif  // ifndef _MAIN_H_
