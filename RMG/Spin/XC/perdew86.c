

#include "main.h"
#include <float.h>
#include <math.h> 

void perdew86 ( rmg_double_t rho, rmg_double_t grho, rmg_double_t * sc, rmg_double_t * v1c, rmg_double_t * v2c )
{
	/* Perdew gradient correction on correlation: PRB 33, 8822 (1986) */
	
	rmg_double_t p1=0.023266, p2=7.389e-6, p3=8.723, p4=0.472;
	rmg_double_t pc1=0.001667, pc2=0.002568, pci=pc1+pc2;
	rmg_double_t third=1.0/3.0, pi34=0.6203504908994;   /* (3/(4*pi))^(1/3) */
	rmg_double_t rho13, rho43, rs, rs2, rs3, cna, cnb, cn, drs, dcna, dcnb, dcn, phi, ephi;

	rho13 = pow ( rho, third );
	rho43 = pow ( rho13, 4 );
	rs = pi34 / rho13;
	rs2 = rs * rs;
	rs3 = rs2 * rs;
	cna = pc2 + p1 * rs + p2 * rs2;
	cnb = 1.0 + p3 * rs + p4 * rs2 + (1.e4) * p2 * rs3;
	cn = pc1 + cna / cnb;
	drs = - third * pi34 / rho43;
	dcna = ( p1 + 2.0 * p2 * rs ) * drs;
	dcnb = ( p3 + 2.0 * p4 * rs + (3.e4) * p2 * rs2 ) * drs;
	dcn = dcna / cnb - cna / (cnb * cnb) * dcnb;
	phi = 0.19195 * pci / cn * grho * pow ( rho, (-7.0 / 6.0) );

	ephi = exp ( -phi );
	*sc = grho * grho / rho43 * cn * ephi;
	*v1c = (*sc) * ( ( 1.0 + phi ) * dcn / cn - ( ( 4.0 / 3.0 ) - ( 7.0 / 6.0 ) * phi ) / rho );
	*v2c = cn * ephi / rho43 * ( 2.0 - phi );
	*sc = (*sc) / rho;

}


void becke88 ( rmg_double_t rho, rmg_double_t grho, rmg_double_t * sx, rmg_double_t * v1x, rmg_double_t * v2x )
{
	/* Becke exchange: A.D. Becke, PRA 38, 3098 (1988) 
	 * only gradient-corrected part, no Slater term included*/

	rmg_double_t beta=0.0042, third=1.0/3.0, two13=1.259921049894873; /* two13 = 2^(1/3) */
	rmg_double_t rho13, rho43, xs, xs2, sa2b8, shm1, dd, dd2, ee;

	rho13 = pow ( rho, third );
	rho43 = pow ( rho13, 4 );
	xs = two13 * grho / rho43; 
	xs2 = xs * xs;
	sa2b8 = sqrt ( 1.0 + xs2 );
	shm1 = log ( xs + sa2b8 );
	dd = 1.0 + 6.0 * beta * xs * shm1;
	dd2 = dd * dd;
	ee = 6.0 * beta * xs2 / sa2b8 - 1.0;
	*sx = two13 * grho * grho / rho43 * ( -beta / dd );
	*v1x = - ( 4.0 / 3.0 ) / two13 * xs2 * beta * rho13 * ee / dd2;
	*v2x = two13 * beta * ( ee - dd ) / ( rho43 * dd2 );
	*sx = (*sx) / rho;
}


void becke88_spin ( rmg_double_t rho, rmg_double_t grho, rmg_double_t * sx, rmg_double_t * v1x, rmg_double_t * v2x )
{
	/* Becke exchange: A.D. Becke, PRA 38, 3098 (1988)  - Spin polarized case */
	
	rmg_double_t beta=0.0042, third=1.0/3.0;
	rmg_double_t rho13, rho43, xs, xs2, sa2b8, shm1, dd, dd2, ee;
	
	rho13 = pow ( rho, third );
	rho43 = pow ( rho13, 4 );
	xs =  grho / rho43; 
	xs2 = xs * xs;
	
	sa2b8 = sqrt ( 1.0 + xs2 );
	shm1 = log ( xs + sa2b8 );
	dd = 1.0 + 6.0 * beta * xs * shm1;
	dd2 = dd * dd;
	ee = 6.0 * beta * xs2 / sa2b8 - 1.0;
	*sx = grho * grho / rho43 * ( -beta / dd );
	*v1x = - ( 4.0 / 3.0 ) * xs2 * beta * rho13 * ee / dd2;
	*v2x = beta * ( ee - dd ) / ( rho43 * dd2 );
	*sx = (*sx) / rho;
}



void perdew86_spin ( rmg_double_t rho, rmg_double_t zeta, rmg_double_t grho, rmg_double_t * sc, rmg_double_t * v1cup, rmg_double_t * v1cdw, rmg_double_t * v2c )
{
	/* Perdew gradient correction on correlation: PRB 33, 8822 (1986)  - Spin polarized case*/
	
	rmg_double_t p1=0.023266, p2=7.389e-6, p3=8.723, p4=0.472;
	rmg_double_t pc1=0.001667, pc2=0.002568, pci=pc1+pc2;
	rmg_double_t third=1.0/3.0, third2=2.0/3.0, third5=5.0/3.0, pi34=0.6203504908994;   /* (3/(4*pi))^(1/3) */
	rmg_double_t rho13, rho43, rs, rs2, rs3, cna, cnb, cn, drs, dcna, dcnb, dcn, phi, ephi, dd, ddd;
	rmg_double_t g1, g2, gg;
		

	rho13 = pow ( rho, third );
	rho43 = pow ( rho13, 4 );
	rs = pi34 / rho13;
	rs2 = rs * rs;
	rs3 = rs2 * rs;
	cna = pc2 + p1 * rs + p2 * rs2;
	cnb = 1.0 + p3 * rs + p4 * rs2 + (1.e4) * p2 * rs3;
	cn = pc1 + cna / cnb;
	drs = - third * pi34 / rho43;
	dcna = ( p1 + 2.0 * p2 * rs ) * drs;
	dcnb = ( p3 + 2.0 * p4 * rs + (3.e4) * p2 * rs2 ) * drs;
	dcn = dcna / cnb - cna / (cnb * cnb) * dcnb;
	phi = 0.19195 * pci / cn * grho * pow ( rho, (-7.0 / 6.0) );

	g1 = 0.5 * (1.0 + zeta);
	g2 = 0.5 * (1.0 - zeta);
	gg = pow (g1, third5) + pow (g2, third5);
	dd = pow (2.0, third) * sqrt ( gg );

	gg = pow (g1, third2) - pow (g2, third2);
	ddd = pow (2.0, -4.0/3.0) * 5.0 * gg / ( 3.0 * dd );
	
	ephi = exp ( -phi );
	*sc = grho * grho / rho43 * cn * ephi / dd;
	*v1cup = (*sc) * ( ( 1.0 + phi ) * dcn / cn - ( ( 4.0 / 3.0 ) - ( 7.0 / 6.0 ) * phi ) / rho ) - (*sc) * ddd / dd * (1.0 - zeta) / rho;
	*v1cdw = (*sc) * ( ( 1.0 + phi ) * dcn / cn - ( ( 4.0 / 3.0 ) - ( 7.0 / 6.0 ) * phi ) / rho ) + (*sc) * ddd / dd * (1.0 + zeta) / rho;
	*v2c = cn * ephi / rho43 * ( 2.0 - phi ) / dd;
	*sc = (*sc) / rho;

}





































