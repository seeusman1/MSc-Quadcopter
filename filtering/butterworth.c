/* http://www-users.cs.york.ac.uk/~fisher/cgi-bin/mkfscript */
/* Butterworth Lowpass filter */
/* 2nd order */
/* sample rate - 300hz define in in4073.h*/
/* corner freq.  10 Hz */

/*
*  D.Patoukas
*/ 


#include "butterworth.h"
#include "../in4073.h"
#include <stdint.h>

/*source https://sourceforge.net/projects/fixedptc/*/
#include "fixedptc.h"

#define NZEROS 2
#define NPOLES 2

#define GAIN 1.049784742e+02
#define D1 2
#define D2 -0.7436551950
#define D3 1.7055521455

#ifdef FIXEDPT_BITS
static fixedpt xv[NZEROS+1], yv[NPOLES+1];
#else
static int32_t xv[NZEROS+1], yv[NPOLES+1];
#endif


#ifdef FIXEDPT_BITS
fixedpt d1 = fixedpt_rconst(D1);
fixedpt d2 = fixedpt_rconst(D2);
fixedpt d3 = fixedpt_rconst(D3);
fixedpt gain = fixedpt_rconst(GAIN);
#else
d1 = D1;
d2 = D2;
d3 = D3;
gain = GAIN;
#endif

int32_t bw_filter (int32_t input)
{ 
    #ifdef FIXEDPT_BITS
    fixedpt in = fixedpt_rconst(input);
    #else
    int32_t in = input;
    #endif

    xv[0] = xv[1]; 
    xv[1] = xv[2]; 
    #ifdef FIXEDPT_BITS
    xv[2] = fixedpt_div(in,gain);
    #else
    xv[2] = in/gain;
    #endif
    yv[0] = yv[1]; 
    yv[1] = yv[2];
    #ifdef FIXEDPT_BITS 
    yv[2] =   (xv[0] + xv[2]) + fixedpt_mul(d1,xv[1])
                 + fixedpt_mul(d2,yv[0]) + fixedpt_mul(d3 ,yv[1]);
    #else
    yv[2] =   (xv[0] + xv[2]) + d1 * xv[1]
                 + (d2 * yv[0]) + (  d3 * yv[1]);
    #endif

    #ifdef FIXEDPT_BITS
        return fixedpt_toint(yv[2]);
    #else       
        return (yv[2]);
    #endif
}