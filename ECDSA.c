/*
 * Copyright 2012 Xinxin Fan <fan@openecc.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright 
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright   
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "OpenECC.h"
#include "ECDSA.h"
#include "GF2n.h"
#include "ECCK.h"
#include "GFp.h"
#include "AES128.h"
#include "AESMMO.h"

/**************************************************************************************
 modpmul(.): Modular multiplication over GF(p)
 **************************************************************************************/
void modpmul(uint32 *a, uint32 *b, uint32 *amulb, uint32 curve_id)
{
	uint32 i, j, Num;
	uint32 c[2*N-1], p[N+1], q[N+1];
	uint64 T1, T2, T3;

	switch(curve_id){
	case sect163k1: Num = 6;
		break;
	case sect233k1: Num = 8;
		break;
	default: return;
	}

	//c = a * b
	T1 = (uint64)(*a) * (*b);
	*c = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < 2*(Num - 1); i++)
	{
		T3 = 0;
		for(j = (i < Num ? 0 : (i - Num + 1)); j <= (i < Num ? i : (Num - 1)); j++)
		{
			T2 = (uint64)(*(a + j)) * (*(b + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(c + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	*(c + 2*(Num - 1)) = (uint32)T1 + (*(a + (Num - 1))) * (*(b + (Num - 1)));

	//Barrett Modular Reduction 
	T1 = (uint64)(*(c + Num - 1)) * (*(eccu[curve_id]));
	T1 = T1 >> 32;

	for(i = Num; i < Num * 2; i++)
	{
		T3 = 0;
		for(j = Num - 1; j <= (i < (2*Num - 1) ? i : 2*(Num - 1)); j++)
		{
			T2 = (uint64)(*(c + j)) * (*(eccu[curve_id] + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		T1 = (T1 >> 32) + T3;
	}

	for(i = 0; i < Num - 1; i++)
	{
		T3 = 0;
		for(j = i + Num; j <= 2*(Num - 1); j++)
		{
			T2 = (uint64)(*(c + j)) * (*(eccu[curve_id] + (Num * 2 + i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(p + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	*(p + (Num - 1)) = (uint32)T1;

	//q = p * eccn
	T1 = (uint64)(*p) * (*(eccn[curve_id]));
	*q = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < Num - 1; i++)
	{
		T3 = 0;
		for(j = (i < (Num/2) ? 0 : (i - Num/2 + 1)); j <= i; j++)
		{
			T2 = (uint64)(*(p + j)) * (*(eccn[curve_id] + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(q + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	T2 = (uint64)(*p) * (*(eccn[curve_id] + (Num - 1)));
	T1 += (uint32)T2;
	T3 = T2 >> 32;
	for(j = Num/2; j <= Num - 1; j++)
	{
		T2 = (uint64)(*(p + j)) * (*(eccn[curve_id] + (Num - 1 - j)));
		T1 += (uint32)T2;
		T3 += (T2 >> 32);
	}
	*(q + Num - 1) = (uint32)T1;
	T1 = (T1 >> 32) + T3;

	T2 = (uint64)*(p + 1) * (*(eccn[curve_id] + (Num - 1)));
	T1 += (uint32)T2;
	for(j = Num/2 + 1; j <= Num - 1; j++)
	{
		T2 = (uint64)(*(p + j)) * (*(eccn[curve_id] + (Num - j)));
		T1 += (uint32)T2;
	}
	*(q + Num) = (uint32)T1;

	for(i = Num - 1; i != 0; i--)  
		*(p + i) = *(eccn[curve_id] + i);
	*p = *(eccn[curve_id]);
	*(p + Num) = 0;

	if(sub(c, q, q, Num + 1) == 1)                 
	{
		for(i = Num - 1; i != 0; i--)  
			*(amulb + i) = *(q + i);
        *amulb = *q;
		return;
	}

	while(compare(q, p, Num + 1) != -1)  
		sub(q, p, q, Num + 1);
	for(i = Num - 1; i != 0; i--)  
		*(amulb + i) = *(q + i);
	*amulb = *q;
}

/**************************************************************************************
 modpinv(.): Modular inversion over GF(p)
 **************************************************************************************/
 void modpinv(uint32 *a, uint32 *b, uint32 curve_id)
 {
	 uint32 g1[N] = {0x1};
	 uint32 g2[N] = {0x0};
	 uint32 u[N];
	 uint32 v[N];
	 uint32 p[N] = {0x0};
	 uint32 i, Num, uv, t;

	 switch(curve_id){
	 case sect163k1:
		 Num = 6;
		 XTOY(u, a, 6);
		 XTOY(v, eccn[0], 6);
		 uv = (IFN1_163(u)) && (IFN1_163(v));
		 break;
	 case sect233k1:
		 Num = 8;
		 XTOY(u, a, 8);
		 XTOY(v, eccn[1], 8);
		 uv = (IFN1_233(u)) && (IFN1_233(v));
		 break;
	 default: return;
	 }

	 sub(p, eccn[curve_id], p, Num);

	 while(uv)
	 {
		 while((u[0] & 0x1) == 0) 
		 {
			 for(i = 0; i < Num - 1; i++)
				 *(u + i) = (*(u + i) >> 1) | (*(u + i + 1) << 31);
			 *(u + Num - 1) = *(u + Num - 1) >> 1;

			 if((g1[0] & 0x1) != 0)
				 add(g1, eccn[curve_id], g1, curve_id);

			 for(i = 0; i < Num - 1; i++)
				 *(g1 + i) = (*(g1 + i) >> 1) | (*(g1 + i + 1) << 31);
			 *(g1 + Num - 1) = *(g1 + Num - 1) >> 1;
		 }

		 while((v[0] & 0x1) == 0) 
		 {	
			 for(i = 0; i < Num - 1; i++)
				 *(v + i) = (*(v + i) >> 1) | (*(v + i + 1) << 31);
			 *(v + Num - 1) = *(v + Num - 1) >> 1;

			 if((g2[0] & 0x1) != 0)
				 add(g2, eccn[curve_id], g2, curve_id);
	
			 for(i = 0; i < Num - 1; i++)
				 *(g2 + i) = (*(g2 + i) >> 1) | (*(g2 + i + 1) << 31);
			 *(g2 + Num - 1) = *(g2 + Num - 1) >> 1;
		 }

		 if(compare(u, v, Num) != -1)
		 {
			 sub(u, v, u, Num);
			 if(sub(g1, g2, g1, Num) == 1)
				 sub(g1, p, g1, Num); 
		 }
		 else
		 {
			 sub(v, u, v, Num);
			 if(sub(g2, g1, g2, Num) == 1)
				 sub(g2, p, g2, Num);
		 }

		 switch(curve_id){
	     case sect163k1:
			 uv = (IFN1_163(u)) && (IFN1_163(v));
		     break;
	     case sect233k1:
		     uv = (IFN1_233(u)) && (IFN1_233(v));
		     break;
	     default: return;
		 }
	 }

	 switch(curve_id){
	 case sect163k1:
		 t = IF1_163(u);
		 break;
	 case sect233k1:
		 t = IF1_233(u);
		 break;
	 default: return;
	 }

	 if(t)
	 {
		 for(i = Num - 1; i != 0; i--)
			 *(b + i) = *(g1 + i);
		 *b = *g1;
	 }
	 else 
	 {
		 for(i = Num - 1; i != 0; i--)
			 *(b + i) = *(g2 + i);
		 *b = *g2;
	 } 
 }

/***************************************************************************************************
  ECDSA signature generation
  **************************************************************************************************/
uint32 ECDSA_sign(uint32 *d, uint8 *rnd, uint8 *msg, uint32 mlen, uint32 *r, uint32 *s, uint32 curve_id)
{
	uint8 t[16];
	uint32 k[N], e[N];
	uint32 i, Num, u;
	ec_point_aff R;

	switch(curve_id){
	case sect163k1:
		Num = 6;

		*(k + 5) = (uint32)(*(rnd + 20)) & 0x7;
		break;
	case sect233k1:
		Num = 8;

		*(k + 7) = (*(k + 7) << 8) | (uint32)(*(rnd + 29));
		*(k + 7) = (*(k + 7) << 8) | (uint32)(*(rnd + 28));
		*(k + 7) = *(k + 7) & 0xff;
		break;
	default: return 0;
	}

	for(i = 0; i < Num - 1; i++)
	{
		*(k + i) = (uint32)(*(rnd + 4*i + 3));
		*(k + i) = (*(k + i) << 8) | (uint32)(*(rnd + 4*i + 2));
		*(k + i) = (*(k + i) << 8) | (uint32)(*(rnd + 4*i + 1));
		*(k + i) = (*(k + i) << 8) | (uint32)(*(rnd + 4*i));
	}
	
	if(compare(k, eccn[curve_id], Num) != -1)
		sub(k, eccn[curve_id], k, Num);

	TNAF5_fixed_scalarmul(k, &R, curve_id);
	XTOY(r, R.x, Num);
	while(compare(r, eccn[curve_id], Num) != -1)
		sub(r, eccn[curve_id], r, Num);

	AES_MMO(msg, mlen*4, t);

	for(i = 0; i < 4; i++)
	{
		*(e + i) = ((uint32)*(t+4*i) << 24) | ((uint32)*(t+4*i+1) << 16) | ((uint32)*(t+4*i+1) << 8) | ((uint32)*(t+4*i+3));
		*(e + i + 4) = 0x0;
	}

	modpmul(r, d, s, curve_id);
	modpadd(s, e, s, curve_id);
	modpinv(k, e, curve_id);
	modpmul(s, e, s, curve_id);

	switch(curve_id){
	case sect163k1:
		u = IF0_163(s);
		break;
	case sect233k1:
		u = IF0_233(s);
		break;
	default: return 0;
	}

	if(u) return 0;

	return 1;
}

/***************************************************************************************************
  ECDSA signature generation
  **************************************************************************************************/
uint32 ECDSA_verify(uint32 *Qx, uint8 *msg, uint32 mlen, uint32 *r, uint32 *s, uint32 curve_id)
{
	uint8 t[16];
	uint32 si[8], u[8], e[8];
	uint32 i, Num, v;
	ec_point_aff R, S, Q;
	ec_point_pro Rp;

	switch(curve_id){
	case sect163k1: Num = 6;
		break;
	case sect233k1: Num = 8;
		break;
	default: return 0;
	}

	if(compare(r, eccn[curve_id], Num) != -1 || compare(s, eccn[curve_id], Num) != -1)
		return 0;

	AES_MMO(msg, mlen*4, t);

	for(i = 0; i < 4; i++)
	{
		*(e + i) = ((uint32)*(t+4*i) << 24) | ((uint32)*(t+4*i+1) << 16) | ((uint32)*(t+4*i+1) << 8) | ((uint32)*(t+4*i+3));
		*(e + i + 4) = 0x0;
	}

	modpinv(s, si, curve_id);
	modpmul(e, si, e, curve_id);
	modpmul(r, si, u, curve_id);
	TNAF5_fixed_scalarmul(e, &R, curve_id);
	if(point_decompression(Qx, &Q, curve_id) == 0)
		return 0;
	TNAF5_random_scalarmul(u, &Q, &S, curve_id);
	affine_to_project(&R, &Rp, curve_id);
	mixed_addition(&Rp, &S, &Rp, curve_id);
	project_to_affine(&Rp, &R, curve_id);

	switch(curve_id){
	case sect163k1:
		v = (IF0_163(R.x)) && (IF0_163(R.y));
		break;
	case sect233k1: 
		v = (IF0_233(R.x)) && (IF0_233(R.y));
		break;
	default: return 0;
	}

	if(v) return 0;
	
	XTOY(e, R.x, Num);
	while(compare(e, eccn[curve_id], Num) != -1)
		sub(e, eccn[curve_id], e, Num);

	if(compare(e, r, Num) == 0) 
		return 1;
	else return 0;
}



