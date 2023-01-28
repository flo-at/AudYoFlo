/* ======================================================================== *
 * MATHLIB -- TI Floating-Point Math Function Library                       *
 *                                                                          *
 *                                                                          *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/   *
 *                                                                          *
 *                                                                          *
 *  Redistribution and use in source and binary forms, with or without      *
 *  modification, are permitted provided that the following conditions      *
 *  are met:                                                                *
 *                                                                          *
 *    Redistributions of source code must retain the above copyright        *
 *    notice, this list of conditions and the following disclaimer.         *
 *                                                                          *
 *    Redistributions in binary form must reproduce the above copyright     *
 *    notice, this list of conditions and the following disclaimer in the   *
 *    documentation and/or other materials provided with the                *
 *    distribution.                                                         *
 *                                                                          *
 *    Neither the name of Texas Instruments Incorporated nor the names of   *
 *    its contributors may be used to endorse or promote products derived   *
 *    from this software without specific prior written permission.         *
 *                                                                          *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     *
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       *
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   *
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT    *
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   *
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        *
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   *
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   *
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     *
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   *
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    *
 * ======================================================================== */

/* ======================================================================= */
/* sinsp_i.h - single precision floating point sine                        */
/*              optimized inlined C implementation (w/ intrinsics)         */
/* ======================================================================= */

#ifndef SINSP_I_
#define SINSP_I_ 1

#include <ti/mathlib/src/common/common.h>

#ifndef __cplusplus         /* FOR PROTECTION PURPOSE - C++ NOT SUPPORTED. */
#pragma CODE_SECTION(sinsp_i, ".text:optci");
#endif

static inline float sinsp_i (float a)
{
  float InvPI =  0.318309886183791;
  float One   =  1.0;
  float MAX   =  1048576.0;
  float Zero  =  0.0;
  float s1    = -1.666665668e-1;
  float s2    =  8.333025139e-3; 
  float s3    = -1.980741872e-4;
  float s4    =  2.601903036e-6; 
  float C1    =  3.140625;
  float C2    =  9.67653589793e-4;
  float Sign, X, Y, Z, F, G, R;
  int   N;

  Sign = One;
  Y    = a;

  if (_fabsf(Y) > MAX) {
    Y = Zero;
  }

  X = Y * InvPI;            /* X = Y * (1/PI)  */
  N = _spint(X);            /* N = integer part of X  */
  Z = (float) N;                         
  
  if ((N & 1) != 0.0f) {
    Sign = -Sign;           /* Quadrant 3 or 4 */
  }

  F = (Y - Z*C1) - Z*C2;      
  G = F * F;
  R = (((s4*G + s3)*G + s2)*G + s1)*G;
        
  return ((F + F*R) * Sign);
}

#endif

/* ======================================================================== */
/*  End of file: sinsp_i.h                                                  */
/* ======================================================================== */

