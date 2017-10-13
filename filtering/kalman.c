#include "kalman.h"
#include "../in4073.h"

#define P2HPHI 1
#define P2HTHETA 1
#define P2HPSI 1

int16_t p_b, q_b, r_b;
void kalman_filter() {
	//update angular rate estimation
	p = sp - p_b;
	q = sq - q_b;
	r = sr - r_b;

	//first estimate of attitude
	phi = phi + p * P2HPHI;
	theta = theta + q * P2HTHETA;
	psi = psi + r * P2HPSI;

	//adjust for bias
	phi = phi - (sphi - phi) / C1;
	theta = theta - (stheta - theta) / C1;
	psi = psi - (spsi - psi) / C1;

	//update bias
	p_b = p_b + (phi - sphi) / C2;
	q_b = q_b + (theta - stheta) / C2;
	r_b = r_b + (psi - spsi) / C2;
}
