/*
 *  fpu/fpu_uae.h - Extra Definitions for the old UAE FPU core
 *
 *  Basilisk II (C) 1997-2008 Christian Bauer
 *
 *  MC68881/68040 fpu emulation
 *
 *  Original UAE FPU, copyright 1996 Herman ten Brugge
 *  Rewrite for x86, copyright 1999-2000 Lauri Pesonen
 *  New framework, copyright 2000 Gwenole Beauchesne
 *  Adapted for JIT compilation (c) Bernd Meyer, 2000
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef FPU_IEEE_H
#define FPU_IEEE_H
#include <stdint.h>
#include <mpfr.h>
void init_fpu();
void exit_fpu();


void init_fpu_opc();
struct M68881 {
    // FPU
    mpfr_t fp[8];
    struct fpcr_t {
        uint8_t prec;
        bool inex1;
        bool inex2;
        bool dz;
        bool unfl;
        bool ovfl;
        bool operr;
        bool snan;
        bool bsun;
    } FPCR;
    struct fpsr_t {
        bool ac_inex;
        bool ac_dz;
        bool ac_unfl;
        bool ac_ovfl;
        bool ac_iop;
        bool inex1;
        bool inex2;
        bool dz;
        bool unfl;
        bool ovfl;
        bool operr;
        bool snan;
        bool bsun;
        int8_t qutinent;
        bool nan;
        bool i;
        bool z;
        bool n;
    } FPSR;
    uint32_t fpiar;

    
};
enum RND_MODE { ROUND_RN, ROUND_RZ, ROUND_RM, ROUND_RP };
enum RND_PREC { ROUND_EXT, ROUND_SGL, ROUND_DBL, ROUND_ERR };
#endif /* FPU_IEEE_H */
