/*******************************************************************************
**                                                                            **
** Copyright (C) Infineon Technologies (2009)                                 **
**                                                                            **
** All rights reserved.                                                       **
**                                                                            **
********************************************************************************
**                                                                            **
**  FILENAME  : Scu.h                                                         **
**                                                                            **
**  VERSION   : 1.0.0                                                         **
**                                                                            **
**  DATE      : 2009.09.11                                                    **
**                                                                            **
**  PLATFORM  : Infineon XC2000, XE166                                        **
**                                                                            **
**  COMPILER  : Tasking VX 2.3r3, Tasking Classic 8.7r3, Keil 6.17            **
**                                                                            **
**  PROJECT   : SCU Driver                                                    **
**                                                                            **
**  AUTHOR    : Infineon Technologies                                         **
**                                                                            **
**  DESCRIPTION : This file exports the functionality of the SCU driver.      **
**                                                                            **
**  SPECIFICATION(S) :                                                        **
**                                                                            **
**  MAY BE CHANGED BY USER [yes/no]: no                                       **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Author(s) Identity                                    **
********************************************************************************
**                                                                            **
** Initials     Name                                                          **
** ---------------------------------------------------------------------------**
** SS           Steffen Storandt                                              **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/*
 * V1.0.0: 2009.09.11, RW:  Final Version
 * V0.5.4: 2009.08.07, SS: Sporadic power saving problem solved
 * V0.5.3: 2009.08.03, SS: Modified controller selection, flash on/off by MODEN,
 *                         new features (osc. low gain, ULPEVR, fast VCO clock
 *                         in stop-over, user K2 divider change, user delay
 *                         function, add. port for clock output), some new
 *                         tool chain settings
 * V0.5.1: 2009.03.31, SS: Wake-up via system timer added, additional tool
 *                         chains and older devices supported, minor changes
 * V0.5.0: 2009.02.16, SS: Major optimization regarding speed, new features
 *                         and configurations
 * V0.4.1: 2009.01.14, SS: Minor changes and additions for CLKIN1, DIDIS, timer
 *                         restoring
 * V0.4.0: 2009.01.08, SS: Major optimization regarding speed/PSRAM,
 *                         additional clock and stop-over mode features
 * V0.3.2: 2008.11.12, SS: Minor changes (no SBRAM ECC; timeout measurement)
 * V0.3.1: 2008.11.11, SS: Changes in fast startup mode
 * V0.3.0: 2008.11.10, SS: Power saving features added
 * V0.2.0: 2008.08.05, SS: Support for MR+ added, some test functions removed,
 *                         minor formal changes
 * V0.0.1: 2008.07.14, SS: Initial version
*/

#ifndef _SCU_H_
#define _SCU_H_

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

#include "Scu_Cfg.h"

#if(SCU_COMPILER == SCU_COMPILER_KEIL)
#  include <intrins.h>
#  if(SCU_CONTROLLER >= SCU_CONTROLLER_BASE_LINE)
#    include <XC2xxx_M_A_X.h>
#  else
#    include <XC2xxx.h>
#  endif
#endif /* (SCU_COMPILER == SCU_COMPILER_KEIL) */


#if(SCU_COMPILER == SCU_COMPILER_TASKING_VX) && \
(SCU_STANDBY_FSM_USED || SCU_STANDBY_NORMAL_USED || \
SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED)
#  pragma optimize -compact /* disable code compaction (reverse in-lining) */
#  pragma optimize -inline /* disable automatic in-lining */
#  pragma linear_switch /* disable jump table generation for switch  */
#endif

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/
/*
 * compiler related values *****************************************************
 */
#if(SCU_COMPILER == SCU_COMPILER_TASKING_VX)
#  define DISWDT __diswdt
#  define FAR __far
#  define INLINE inline
#  define MKSP __mksp
#  define NOP __nop
#  define SHUGE __shuge
#  if(SCU_CONTROLLER >= SCU_CONTROLLER_BASE_LINE)
#    ifndef SCU_PSCSTAT /* workaround for V2.3r2: */
#      define SCU_PSCSTAT (*(volatile __near unsigned int *)0xFFE8)
#      define SCU_PSCSTAT_LSTSEQ (SCU_PSCSTAT & (1U << 12U))
#    endif
#  endif /* (SCU_CONTROLLER >= SCU_CONTROLLER_BASE_LINE) */
#endif /* (SCU_COMPILER == SCU_COMPILER_TASKING_VX) */


#if(SCU_COMPILER == SCU_COMPILER_TASKING_CLASSIC)
#  define DISWDT _diswdt
#  define FAR _far
#  define INLINE _inline
#  define MKSP _mksp
#  define NOP _nop
#  define SHUGE _shuge
#  ifndef SCU_PSCSTAT /* workaround for V8.7r3: */
#    define SCU_PSCSTAT (*(volatile _near unsigned int *)0xFFE8)
#    define SCU_PSCSTAT_LSTSEQ (SCU_PSCSTAT & (1U << 12U))
#  endif
#  if(SCU_CONTROLLER >= SCU_CONTROLLER_BASE_LINE)
#    ifndef SCU_STMCON /* workaround for V8.7r3: */
#      define SCU_STMCON (*(volatile _near unsigned int *)0xF1AA)
#    endif
#    ifndef SCU_STMREL /* workaround for V8.7r3: */
#      define SCU_STMREL (*(volatile _near unsigned int *)0xF1A8)
#    endif
#    ifndef SCU_WUTREL /* workaround for V8.7r3: */
#      define SCU_WUTREL (*(volatile _near unsigned int *)0xF0B0)
#    endif
#  endif /* (SCU_CONTROLLER >= SCU_CONTROLLER_BASE_LINE) */
#endif /* (SCU_COMPILER == SCU_COMPILER_TASKING_CLASSIC) */

#if(SCU_COMPILER == SCU_COMPILER_KEIL)
#  define DISWDT _diswdt_
#  define FAR far
#  define INLINE static __inline
#  define MKSP(Offset,Segment)\
   ((unsigned int huge *)((((unsigned long)(Segment)) <<16U) + (Offset)))
#  define NOP _nop_
#  define SHUGE huge
#  if(SCU_CONTROLLER >= SCU_CONTROLLER_BASE_LINE)
#    ifndef SCU_PSCSTAT
#      define SCU_PSCSTAT (*((unsigned int volatile sdata *)0xFFE8))
#      define SCU_PSCSTAT_LSTSEQ (SCU_PSCSTAT & (1U << 12U))
#    endif
#  endif /* (SCU_CONTROLLER >= SCU_CONTROLLER_BASE_LINE) */
   sbit SCU_PLLCON0_VCOSEL = SCU_PLLCON0^2;
#endif /* (SCU_COMPILER == SCU_COMPILER_KEIL) */

/*
 * values for calculation ******************************************************
 */
/* if not user defined:
 * target VCO frequency fVCO in [Hz] for normal operation mode */
#ifndef SCU_F_VCO_TARGET
#  define SCU_F_VCO_TARGET 160000000
#endif

/* if not user defined:
 * target frequency after P divider fP in [Hz] for normal operation mode */
#ifndef SCU_F_P_TARGET
#  define SCU_F_P_TARGET 16000000
#endif

/* if not user defined:
 * calculate P divider for normal operation mode, round up */
#ifndef SCU_P
#  define SCU_P ((SCU_F_R + SCU_F_P_TARGET - 1)/SCU_F_P_TARGET)
#endif

/* calculate frequency after P divider fP in [Hz] for normal operation mode,
 * round down */
#define SCU_F_P (SCU_F_R/SCU_P)

/* if not user defined: calculate final K2 divider for normal operation mode,
 * round down to avoid Scu_F_VCO > SCU_F_VCO_TARGET */
#ifndef SCU_K2
#  define SCU_K2 (SCU_F_VCO_TARGET/SCU_F_PLL_TARGET)
#endif

/* if not user defined: calculate N divider for normal operation mode,
 * round to nearest; avoid Scu_F_VCO > SCU_F_VCO_TARGET */
#ifndef SCU_N
#  define SCU_N_TEMP ((SCU_F_PLL_TARGET * SCU_K2 + SCU_F_P/2)/SCU_F_P)
#  if SCU_F_P * SCU_N_TEMP > SCU_F_VCO_TARGET
#    define SCU_N (SCU_N_TEMP - 1)
#  else
#    define SCU_N SCU_N_TEMP
#  endif
#endif

/* calculate actual VCO frequency fVCO in [Hz] for normal operation mode */
#define SCU_F_VCO (SCU_F_P * SCU_N)

/* calculate K2 for 5 MHz, round to nearest */
#define SCU_K2_5MHZ ((SCU_F_VCO + 2500000)/5000000)

/* calculate K2 for 10 MHz, round to nearest */
#define SCU_K2_10MHZ ((SCU_F_VCO + 5000000)/10000000)

/* calculate actual 5 MHz frequency  */
#define SCU_F_5MHZ (SCU_F_VCO/SCU_K2_5MHZ)

/* calculate actual PLL frequency fPLL in [Hz] for normal operation mode */
#define SCU_F_PLL (SCU_F_VCO/SCU_K2)

/* if not user defined and if necessary:
 * calculate additional K2 step and max. frequency
 * for transition from 10 MHz up to normal mode */
#ifndef SCU_K2_UP_1
#  if(SCU_K2 >= 16) || (SCU_K2_10MHZ <= 16)
#    define SCU_K2_UP_1 0
#  else
#    define SCU_K2_UP_1 16
#  endif
#endif
#if SCU_K2_UP_1 == 0
#  define SCU_F_K2_UP_1_MAX 0
#else
#  define SCU_F_K2_UP_1_MAX (SCU_F_VCO/SCU_K2_UP_1)
#endif

/* if not user defined and if necessary:
 * calculate additional K2 step and max. frequency
 * for transition from 10 MHz up to normal mode */
#ifndef SCU_K2_UP_2
#  if(SCU_K2 >= 5) || (SCU_K2_10MHZ <= 5)
#    define SCU_K2_UP_2 0
#  else
#    define SCU_K2_UP_2 5
#  endif
#endif
#if SCU_K2_UP_2 == 0
#  define SCU_F_K2_UP_2_MAX 0
#else
#  define SCU_F_K2_UP_2_MAX (SCU_F_VCO/SCU_K2_UP_2)
#endif

/* if not user defined and if necessary:
 * calculate additional K2 step and max. frequency
 * for transition from 10 MHz up to normal mode */
#ifndef SCU_K2_UP_3
#  if(SCU_K2 >= 3) || (SCU_K2_10MHZ <= 3)
#    define SCU_K2_UP_3 0
#  else
#    define SCU_K2_UP_3 3
#  endif
#endif
#if SCU_K2_UP_3 == 0
#  define SCU_F_K2_UP_3_MAX 0
#else
#  define SCU_F_K2_UP_3_MAX (SCU_F_VCO/SCU_K2_UP_3)
#endif


/* if not user defined and if necessary:
* calculate additional K2 step and max. frequency
* for transition from normal mode down to 5 MHz */
#ifndef SCU_K2_DOWN_1
#  if(SCU_K2_5MHZ <= 3) || (SCU_K2 >= 3)
#    define SCU_K2_DOWN_1 0
#  else
#    define SCU_K2_DOWN_1 3
#  endif
#endif
#if SCU_K2_DOWN_1 == 0
#  define SCU_F_K2_DOWN_1_MAX (SCU_F_VCO/SCU_K2)
#else
#  define SCU_F_K2_DOWN_1_MAX (SCU_F_VCO/SCU_K2_DOWN_1)
#endif

/* if not user defined and if necessary:
* calculate additional K2 step and max. frequency
* for transition from normal mode down to 5 MHz */
#ifndef SCU_K2_DOWN_2
#  if(SCU_K2_5MHZ <= 5) || (SCU_K2 >= 5)
#    define SCU_K2_DOWN_2 0
#  else
#    define SCU_K2_DOWN_2 5
#  endif
#endif
#if SCU_K2_DOWN_2 == 0
#  define SCU_F_K2_DOWN_2_MAX SCU_F_K2_DOWN_1_MAX
#else
#  define SCU_F_K2_DOWN_2_MAX (SCU_F_VCO/SCU_K2_DOWN_2)
#endif

/* if not user defined and if necessary:
* calculate additional K2 step and max. frequency
* for transition from normal mode down to 5 MHz */
#ifndef SCU_K2_DOWN_3
#  if(SCU_K2_5MHZ <= 16) || (SCU_K2 >= 16)
#    define SCU_K2_DOWN_3 0
#  else
#    define SCU_K2_DOWN_3 16
#  endif
#endif
#if SCU_K2_DOWN_3 == 0
#  define SCU_F_K2_DOWN_3_MAX SCU_F_K2_DOWN_2_MAX
#else
#  define SCU_F_K2_DOWN_3_MAX (SCU_F_VCO/SCU_K2_DOWN_3)
#endif


/*
 * constants for SCU registers *************************************************
 */
 /* constants for EXTCON register */
#define SCU_MASK_EXTCON_EN \
(1U   <<0U)   /* EN External Clock Enable */

#define SCU_POS_EXTCON_SEL (1U)


/* constants for HPOSCCON register */
#define SCU_MASK_HPOSCCON_MODE \
(3U   <<2U)   /* MODE Oscillator Mode, 0: external crystal/clock, */
              /* 1,2: reserved, 3: disabled, power-saving mode */

#define SCU_MASK_HPOSCCON_SHBY \
(1U   <<8U)   /* SHBY Shaper Bypass, 0: not bypassed, 1: bypassed */


/* constants for interrupt/trap related registers DMPMIT, DMPMITCLR, INTCLR,
 * INTDIS, INTSTAT */
#define SCU_POS_INT_WUI (7U)
#define SCU_POS_INT_STM1 (10U)
#define SCU_POS_INT_ESR0T (11U)


/* constants for RSTSTAT1 register */
#define SCU_MASK_RSTSTAT1_STM \
(3U   <<12U)  /* STM Power-on for DMPM Reset Status */ \
              /* 0,1,2: no DMPM power-on, 3: DMPM power-on */

#define SCU_MASK_RSTSTAT1_ST1 \
(3U   <<14U)  /* ST1 Power-on for DMP1 Reset Status */ \
              /* 0,1,2: no DMP1 power-on, 3: DMP1 power-on */


/* constants for SYSCON0 register */
#define SCU_MASK_SYSCON0_CLKSEL \
(3U   <<0U)   /* CLKSEL Clock Select, 0: fWU, 1: fOSC, 2: fPLL, 3: fCLKIN1 */

/*
 * constants for non-SCU registers *********************************************
*/
/* constants for PX_IOCRY registers */
#define SCU_MASK_IOCR_OUT_ALT1_PUSH_PULL \
(9U   <<4U)  /* PX Push-pull output, ALT1 */
#define SCU_MASK_IOCR_OUT_ALT2_PUSH_PULL \
(10U   <<4U)  /* PX Push-pull output, ALT2 */

   /* constants for P2_POCON register */
#define SCU_MASK_P2_POCON_P28_STRONG \
(1U    <<12U) /* mask for strong driver at P2.8 */


/*
 * other constants *************************************************************
 */

/* power-saving modes */
#define SCU_POWER_SAVING_STOPOVER_NORMAL     0U
#define SCU_POWER_SAVING_STOPOVER_CLOCK_ON   1U
#define SCU_POWER_SAVING_STANDBY_NORMAL      2U
#define SCU_POWER_SAVING_STANDBY_FSM         3U

/* wake-up sources, do not change! */
#define SCU_WAKEUP_WUT       (1U  <<0U) /* wake-up timer */
#define SCU_WAKEUP_ESR0      (1U  <<1U) /* ESR0 */
#define SCU_WAKEUP_ESR1      (1U  <<2U) /* ESR1 */
#define SCU_WAKEUP_ESR2      (1U  <<3U) /* ESR2 */
#define SCU_WAKEUP_ESR       (SCU_WAKEUP_ESR0|SCU_WAKEUP_ESR1|SCU_WAKEUP_ESR2)
#define SCU_WAKEUP_STM       (1U  <<4U) /* system timer */

#ifndef NULL_PTR
# define NULL_PTR (0)
#endif

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/

/* type for SCU error */
typedef enum
{
  SCU_ERROR_NO_ERROR = 0,        /* no error */
  SCU_ERROR_TO_OSC_HP_PLLV,      /* timeout for high precision oscillator
                                  * PLLV */
  SCU_ERROR_OFW_ATTEMPTS_OSC_HP, /* overflow of attempts for high precision
                                  * oscillator */
  SCU_ERROR_TO_OSCSELST,         /* timeout for PLLSTAT_OSCSELST */
  SCU_ERROR_TO_K1DIV,            /* timeout for K1 divider setting */
  SCU_ERROR_TO_K2DIV,            /* timeout for K2 divider setting */
  SCU_ERROR_TO_PDIV,             /* timeout for P divider setting */
  SCU_ERROR_TO_NDIV,             /* timeout for N divider setting */
  SCU_ERROR_TO_VCOBYST,          /* timeout for VCO bypass switching */
  SCU_ERROR_TO_VCO_LOCK,         /* timeout for VCO lock */
  SCU_ERROR_VCO_UNLOCKED,        /* VCO is unlocked */
  SCU_ERROR_TO_FSR_BUSY,         /* timeout for FSR busy */
  SCU_ERROR_TO_GSCSTAT,          /* timeout for GSCSTAT */
  SCU_ERROR_TO_SEQAEN,           /* timeout for SEQAEN */
  SCU_ERROR_TO_SEQBEN,           /* timeout for SEQBEN */
  SCU_ERROR_TO_PSMSTAT,          /* timeout for PSMSTAT */
  SCU_ERROR_PVC,                 /* error PVC */
  SCU_ERROR_TO_LSTSEQ,           /* timeout for LSTSEQ */
  SCU_ERROR_NO_STANDBY           /* standby mode not entered */
} Scu_ErrorType;

/* type for EXTCLK multiplexer source setting */
typedef enum
{
  SCU_EXTCLK_F_SYS    = 0, /* system frequency fSYS */
  SCU_EXTCLK_F_OUT    = 1, /* programmable output frequency fOUT */
  SCU_EXTCLK_F_PLL    = 2, /* PLL output frequency fPLL */
  SCU_EXTCLK_F_OSC    = 3, /* high precision oscillator frequency fOSC */
  SCU_EXTCLK_F_WU     = 4, /* wake-up oscillator frequency fWU */
  SCU_EXTCLK_F_CLKIN1 = 5, /* direct input frequency fCLKIN1 */
  SCU_EXTCLK_F_RTC    = 8  /* real-time frequency fRTC */
} Scu_ExtclkSrcType;

/* type for EXTCLK output pin setting */
typedef enum
{
  SCU_EXTCLK_P28      = 0, /* P2.8 */
  SCU_EXTCLK_P71      = 1  /* P7.1 */
} Scu_ExtclkPinType;

#if(SCU_COMPILER != SCU_COMPILER_KEIL)
#if(SCU_STANDBY_FSM_USED || SCU_STANDBY_NORMAL_USED \
|| SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED)

#if((SCU_STOPOVER_NORMAL_USED || SCU_STOPOVER_CLOCK_ON_USED) \
&& SCU_STOPOVER_FAST_CLOCK_USED)
/* type for fast clock in stop-over mode */
typedef enum
{
  SCU_FAST_CLOCK_INTERNAL_5MHZ = 0, /* internal PLL clock 5 MHz*/
  SCU_FAST_CLOCK_INTERNAL_VCO  = 1, /* internal VCO clock */
  SCU_FAST_CLOCK_EXTERNAL      = 2  /* configured external crystal/clock */
} Scu_FastClockType;
#endif /* ((SCU_STOPOVER_NORMAL_USED || SCU_STOPOVER_CLOCK_ON_USED) \
&& SCU_STOPOVER_FAST_CLOCK_USED) */

/* type for system mode */
typedef enum
{
  SCU_SYSTEM_MODE_NORMAL    = 0, /* normal mode */
  SCU_SYSTEM_MODE_CLOCK_OFF = 1  /* clock-off mode */
} Scu_SystemModeType;

/* type for wake-up timer configuration */
typedef struct
{
  unsigned int  Interval; /* Timer interval, 0...65535 */
  unsigned char Autostop; /* Timer behavior on trigger:
                           * 0 = continue (Interval = period for wake-up events,
                           * only for base line controllers or higher)
                           * 1 = stop (Interval = power-down time) */
  unsigned char Divider;  /* Divider 0...7, only for base line controllers or higher */
} Scu_TimerType;

/* type for power-saving configuration */
typedef struct
{
  unsigned char Mode;     /* power-saving mode: 0 = normal stop-over,
                             1 = stop-over with crystal on, 2 = normal standby,
                             3 = standby with FSM */
  unsigned char Channels; /* combination of WUT and ESR wake-up channels:
                             Bit0 = WUT, Bit1...3 = ESR0...2, Bit 4 = STM */
  Scu_TimerType Timer;    /* WUT configuration, only relevant if timer is
                             specified in Channels */
} Scu_PowerSavingCfgType;

#endif /* (SCU_STANDBY_FSM_USED || SCU_STANDBY_NORMAL_USED \
|| SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED) */
#endif /* (SCU_COMPILER != SCU_COMPILER_KEIL) */

/*******************************************************************************
**                      Global Constant Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/
/*
 * basic and clock control functions *******************************************
 */
/* INLINE functions */
INLINE void Scu_DelayByLoop(unsigned int Cycles);

#if(SCU_CLOCK == SCU_CLOCK_CRYSTAL)
INLINE void Scu_EnableHighPrecOsc(unsigned int Enable);
#endif

INLINE void Scu_OutputTestClock(Scu_ExtclkSrcType Src, Scu_ExtclkPinType Pin);

/* functions with standard location */
Scu_ErrorType Scu_ApplyNewK2Div(unsigned int K2Div);

Scu_ErrorType Scu_GoFromBaseModeToNormalMode(void);

void Scu_InitTimer(void);

#if(SCU_RESTORE_TIMER_USED)
void Scu_RestoreTimer(void);
#endif

#if(SCU_COMPILER != SCU_COMPILER_KEIL)
#if(SCU_STANDBY_FSM_USED || SCU_STANDBY_NORMAL_USED \
|| SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED)
/*
 * power-saving functions ******************************************************
 */
/* INLINE functions */
INLINE void Scu_ClearWakeupSrc(unsigned int WakeupSrc);

INLINE void Scu_CopyWords(unsigned int SHUGE *DestPtr,
const unsigned int SHUGE *SrcPtr, unsigned int Words);

INLINE unsigned int Scu_GetWakeupSrc(void);

#if(SCU_STANDBY_FSM_USED)
INLINE void Scu_UseWakeupOscInStandbyFsm(void);
#endif

#if(SCU_STANDBY_FSM_USED)
INLINE void Scu_WriteToSbram(const unsigned int SHUGE *SrcPtr,
unsigned int SbramOffset, unsigned int Words);
#endif


/* functions with standard location */
void Scu_CopyVectorToPsram(unsigned int Vector);

Scu_ErrorType Scu_GoFromNormalModeToPowerSavingMode
(const Scu_PowerSavingCfgType *PowerSavingCfgPtr);

Scu_ErrorType Scu_RequestSystemMode(Scu_SystemModeType SystemMode);


/* functions with specific location */
#if((SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED) \
&& SCU_STOPOVER_FAST_CLOCK_USED)
Scu_ErrorType Scu_ApplyNewK2Div_Ps(unsigned int K2Div);
#endif

#if((SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED) \
&& SCU_STOPOVER_FAST_CLOCK_USED)
Scu_ErrorType Scu_UseWakeupOscInStopover_Ps(void);
#endif

Scu_ErrorType Scu_RequestSystemMode_Ps(Scu_SystemModeType SystemMode);

#if((SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED) \
&& SCU_STOPOVER_FAST_CLOCK_USED)
Scu_ErrorType Scu_UseFastClockInStopover_Ps(Scu_FastClockType FastClock);
#endif

void SCU_HANDLE_ERROR_PS(Scu_ErrorType Error);

#if(SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED)
unsigned int SCU_HANDLE_STOPOVER_PS(void);
#endif

#if(SCU_STANDBY_FSM_USED)
unsigned int SCU_HANDLE_STANDBY_FSM_PS_SB(void);
#endif

#endif /* (SCU_STANDBY_FSM_USED || SCU_STANDBY_NORMAL_USED \
|| SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED) */
#endif /* (SCU_COMPILER != SCU_COMPILER_KEIL) */

/*******************************************************************************
**                      Global Inline Function Definitions                    **
*******************************************************************************/

/*******************************************************************************
** Syntax         : INLINE void Scu_DelayByLoop(unsigned int Cycles)          **
**                                                                            **
** Sync/Async     : Synchronous                                               **
**                                                                            **
** Reentrancy     : Non-reentrant                                             **
**                                                                            **
** Parameters(in) : Cycles: Delay in [2 system clock cycles], 1...65535       **
**                                                                            **
** Parameters(out): None                                                      **
**                                                                            **
** Return value   : None                                                      **
**                                                                            **
** Description    :                                                           **
** This function generates a programmable delay by a program loop.            **
** Notes:                                                                     **
** - Depending on HW settings (e.g. zero-cycle jump execution), compiler      **
**   optimization and program alignment in flash, execution time may be       **
**   longer than expected.                                                    **
**                                                                            **
*******************************************************************************/
INLINE void Scu_DelayByLoop(unsigned int Cycles)
{
  do
  {
    /* avoid removal of loop by optimization */
    NOP();

    /* decrement Cycles */
    Cycles--;
  }
  /* until Cycles = 0 */
  while(Cycles != 0U);
} /* end of function Scu_DelayByLoop */


#if(SCU_CLOCK == SCU_CLOCK_CRYSTAL)
/*******************************************************************************
** Syntax         : INLINE void Scu_EnableHighPrecOsc(unsigned int Enable)    **
**                                                                            **
** Sync/Async     : Synchronous                                               **
**                                                                            **
** Reentrancy     : Non reentrant                                             **
**                                                                            **
** Parameters(in) : Enable: 0 = Disable, 1 = Enable                           **
**                                                                            **
** Parameters(out): None                                                      **
**                                                                            **
** Return value   : None                                                      **
**                                                                            **
** Description    :                                                           **
** This use case function enables or disables the high precision oscillator   **
** for external crystal / clock at crystal input; may be called by the user   **
** to reduce waiting time.                                                    **
** Notes:                                                                     **
** - The user is responsible for disabling the register protection.           **
** - System frequency must not depend on VCO bypass during execution.         **
**                                                                            **
*******************************************************************************/
INLINE void Scu_EnableHighPrecOsc(unsigned int Enable)
{
  if(Enable)
  {
    /* clear MODE bits to enable external crystal/clock, clear SHBY to disable
     * shaper bypass, leave other bits unchanged */
    SCU_HPOSCCON &= (~(SCU_MASK_HPOSCCON_MODE | SCU_MASK_HPOSCCON_SHBY));
  }
  else
  {
    /* set MODE bits to disable external crystal/clock, set SHBY to enable
     * shaper bypass, leave other bits unchanged */
    SCU_HPOSCCON |= (SCU_MASK_HPOSCCON_MODE | SCU_MASK_HPOSCCON_SHBY);
  }
} /* end of function Scu_EnableHighPrecOsc */
#endif /* (SCU_CLOCK == SCU_CLOCK_CRYSTAL) */


/*******************************************************************************
** Syntax         : INLINE void Scu_OutputTestClock(Scu_ExtclkSrcType Src,    **
**                  Scu_ExtclkPinType Pin)                                    **
**                                                                            **
** Sync/Async     : Synchronous                                               **
**                                                                            **
** Reentrancy     : Non reentrant                                             **
**                                                                            **
** Parameters(in) : Src: Clock source                                         **
**                  Pin: Clock output pin                                     **
**                                                                            **
** Parameters(out): None                                                      **
**                                                                            **
** Return value   : None                                                      **
**                                                                            **
** Description    :                                                           **
** This test function outputs a selectable clock at an available output pin.  **
** Notes:                                                                     **
** - The function will overwrite previous EXTCLK clock multiplexer settings   **
**   and P2.8/P7.1 output settings.                                           **
** - The user is responsible for disabling the register protection.           **
**                                                                            **
*******************************************************************************/
INLINE void Scu_OutputTestClock(Scu_ExtclkSrcType Src, Scu_ExtclkPinType Pin)
{
  /* enable clock output, switch multiplexer in EXTCON */
  SCU_EXTCON = SCU_MASK_EXTCON_EN | (((unsigned int)Src) << SCU_POS_EXTCON_SEL);

  if(Pin == SCU_EXTCLK_P28)
  {
    /* initialize P2.8 as push/pull output for EXTCLK (ALT2) */
    P2_IOCR08 = SCU_MASK_IOCR_OUT_ALT2_PUSH_PULL;

    /* select strong driver for P2.8 as clock output */
    P2_POCON |= SCU_MASK_P2_POCON_P28_STRONG;
  }
  else if(Pin == SCU_EXTCLK_P71)
  {
    /* initialize P7.1 as push/pull output for EXTCLK (ALT2) */
    P7_IOCR01 = SCU_MASK_IOCR_OUT_ALT1_PUSH_PULL;
  }
  else
  {
    /* wrong pin */
  }
} /* end of function Scu_OutputTestClock */


#if(SCU_COMPILER != SCU_COMPILER_KEIL)
#if(SCU_STANDBY_FSM_USED || SCU_STANDBY_NORMAL_USED \
|| SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED)
/*
 * power-saving functions ******************************************************
 */

/*******************************************************************************
** Syntax         : INLINE void Scu_ClearWakeupSrc(unsigned int WakeupSrc)    **
**                                                                            **
** Sync/Async     : Synchronous                                               **
**                                                                            **
** Reentrancy     : Non reentrant                                             **
**                                                                            **
** Parameters(in) : WakeupSrc: Combination of WUT and ESR wake-up sources:    **
**                             Bit0 = WUT, Bit1...3 = ESR0...2, Bit 4 = STM   **
**                                                                            **
** Parameters(out): None                                                      **
**                                                                            **
** Return value   : None                                                      **
**                                                                            **
** Description    :                                                           **
** This use case function clears the specified wake-up source request(s).     **
** Notes:                                                                     **
** - If necessary, wake-up source should be read before by Scu_GetWakeupSrc.  **
**                                                                            **
*******************************************************************************/
INLINE void Scu_ClearWakeupSrc(unsigned int WakeupSrc)
{
  if(WakeupSrc & SCU_WAKEUP_WUT)
  {
    /* wake-up timer is source: clear trigger */
    SCU_WUCR_CLRTRG = 1U;
  }
  /* clear DMPMIT bit(s) */
  SCU_DMPMITCLR = ((WakeupSrc & SCU_WAKEUP_WUT) << SCU_POS_INT_WUI) |
  ((WakeupSrc & SCU_WAKEUP_ESR) << (SCU_POS_INT_ESR0T - 1U)) |
  ((WakeupSrc & SCU_WAKEUP_STM) << (SCU_POS_INT_STM1 - 4U));

} /* end of function Scu_ClearWakeupSrc */


/*******************************************************************************
** Syntax         : INLINE void Scu_CopyWords(unsigned int SHUGE *DestPtr,    **
**                  const unsigned int SHUGE *SrcPtr, unsigned int Words)     **
**                                                                            **
** Sync/Async     : Synchronous                                               **
**                                                                            **
** Reentrancy     : Non reentrant                                             **
**                                                                            **
** Parameters(in) : DestPtr: Destination pointer                              **
**                : SrcPtr:  Source pointer                                   **
**                : Words:   Number of words to be copied (> 0)               **
** Parameters(out): None                                                      **
**                                                                            **
** Return value   : None                                                      **
**                                                                            **
** Description    :                                                           **
** This global function copies words from source to destination.              **
** Notes:                                                                     **
** - There is no check for overlapping source and destination.                **
**                                                                            **
*******************************************************************************/
INLINE void Scu_CopyWords(unsigned int SHUGE *DestPtr,
const unsigned int SHUGE *SrcPtr, unsigned int Words)
{
  do
  {
    /* copy word and increment pointers */
    *DestPtr = *SrcPtr++;
    DestPtr++;

    /* decrement Words */
    Words--;
  }
  /* until Words = 0 */
  while (Words != 0U);

} /* end of function Scu_CopyWords */


/*******************************************************************************
** Syntax         : INLINE unsigned int Scu_GetWakeupSrc(void)                **
**                                                                            **
** Sync/Async     : Synchronous                                               **
**                                                                            **
** Reentrancy     : Non reentrant                                             **
**                                                                            **
** Parameters(in) : None                                                      **
**                                                                            **
** Parameters(out): None                                                      **
**                                                                            **
** Return value   : Combination of WUT and ESR wake-up sources:               **
**                  Bit0 = WUT, Bit1...3 = ESR0...2, Bit 4 = STM              **
**                                                                            **
** Description    :                                                           **
** This global function returns the activated wake-up source request(s).      **
** Notes:                                                                     **
** - Wake-up bits that are currently unused may be set; they can be masked    **
**   off by ANDing a mask of the used bits.                                   **
** - Wake-up source should be cleared by Scu_ClrWakeupSrc.                    **
**                                                                            **
*******************************************************************************/
INLINE unsigned int Scu_GetWakeupSrc(void)
{
  unsigned int WakeupSrc;

  /* read register */
  WakeupSrc = SCU_DMPMIT;

  /* shift bits */
  WakeupSrc = ((WakeupSrc >> SCU_POS_INT_WUI) & SCU_WAKEUP_WUT) |
  ((WakeupSrc >> (SCU_POS_INT_ESR0T - 1U)) & SCU_WAKEUP_ESR) |
  ((WakeupSrc >> (SCU_POS_INT_STM1 - 4U)) & SCU_WAKEUP_STM);

  return WakeupSrc;

} /* end of function Scu_GetWakeupSrc */

#if(SCU_STANDBY_FSM_USED)
/*******************************************************************************
** Syntax         : INLINE void Scu_UseWakeupOscInStandbyFsm(void)            **
**                                                                            **
** Sync/Async     : Synchronous                                               **
**                                                                            **
** Reentrancy     : Non reentrant                                             **
**                                                                            **
** Parameters(in) : None                                                      **
**                                                                            **
** Parameters(out): None                                                      **
**                                                                            **
** Return value   : None                                                      **
**                                                                            **
** Description    :                                                           **
** This global function uses wake-up oscillator clock in FSM standby mode     **
** wake-up.                                                                   **
** Notes:                                                                     **
** - This function must be called by the FSM standby user function.           **
** - Wake-up oscillator must be running already.                              **
**                                                                            **
*******************************************************************************/
INLINE void Scu_UseWakeupOscInStandbyFsm(void)
{
  /* select wake-up oscillator as system clock */
  SCU_SYSCON0 &= (~SCU_MASK_SYSCON0_CLKSEL);

  /* disable PLL */
  SCU_PLLCON1_PLLPWD = 1U;

  /* disable low power oscillator */
  SCU_PLLOSCCON_OSCPD = 1U;

  /* disable VCO */
  SCU_PLLCON0_VCOPWD = 1U;

  /* disable PLL power regulator */
  SCU_PLLCON0_REGENCLR = 1U;

} /* end of function Scu_UseWakeupOscInStandbyFsm */
#endif /* (SCU_STANDBY_FSM_USED) */

#if(SCU_STANDBY_FSM_USED)
/*******************************************************************************
** Syntax         : INLINE void Scu_WriteToSbram                              **
**                  (const unsigned int SHUGE *SrcPtr,                        **
**                  unsigned int SbramOffset, unsigned int Words)             **
**                                                                            **
** Sync/Async     : Synchronous                                               **
**                                                                            **
** Reentrancy     : Non reentrant                                             **
**                                                                            **
** Parameters(in) : SrcPtr: Source pointer                                    **
**                : SbramOffset: Offset to SBRAM start (even)                 **
**                : Words: Number of words to be copied (> 0)                 **
** Parameters(out): None                                                      **
**                                                                            **
** Return value   : None                                                      **
**                                                                            **
** Description    :                                                           **
** This global function writes words to SBRAM.                                **
**                                                                            **
*******************************************************************************/
INLINE void Scu_WriteToSbram(const unsigned int SHUGE *SrcPtr,
unsigned int SbramOffset, unsigned int Words)
{
  /* write address register = offset */
  SBRAM_WADD = SbramOffset;
  do
  {
    /* write word to SBRAM and increment source pointer */
    SBRAM_DATA1 = *SrcPtr++;

    /* decrement Words */
    Words--;
  }
  /* until Words = 0 */
  while (Words != 0U);

} /* end of function Scu_CopyWordsToSbram */
#endif /* (SCU_STANDBY_FSM_USED) */

#endif /* (SCU_STANDBY_FSM_USED || SCU_STANDBY_NORMAL_USED \
|| SCU_STOPOVER_CLOCK_ON_USED || SCU_STOPOVER_NORMAL_USED) */
#endif /*(SCU_COMPILER != SCU_COMPILER_KEIL) */

#endif /* ifndef _SCU_H_ */
