/*--------------------------------------------------------------------------
SROM.H

Handling for SROM MEMORY SECTIONS for C166 Version 4.
Copyright (c) 1998-1999 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/

#define SROM_PS(n)                                                  \
extern unsigned char huge _PR_##n##_s_;  /* section source start */ \
extern unsigned char huge _PR_##n##_l_;  /* section len          */ \
extern unsigned char huge _PR_##n##_t_;  /* target address       */

#define SROM_PS_TRG(n) ((void *)       &_PR_##n##_t_)
#define SROM_PS_SRC(n) ((void *)       &_PR_##n##_s_)
#define SROM_PS_LEN(n) ((unsigned int) &_PR_##n##_l_)

