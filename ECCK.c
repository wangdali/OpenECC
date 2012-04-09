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

#include "stdio.h"
#include "OpenECC.h"
#include "GF2n.h"
#include "ECCK.h"

//Base point G
ec_point_aff G[2] = {{{0x5C94EEE8UL,0xDE4E6D5EUL,0xAA07D793UL,0x7BBC11ACUL,0xFE13C053UL,0x2UL,0x0UL,0x0UL},
                      {0xCCDAA3D9UL,0x0536D538UL,0x321F2E80UL,0x5D38FF58UL,0x89070FB0UL,0x2UL,0x0UL,0x0UL}},
                     {{0xEFAD6126UL,0x0A4C9D6EUL,0x19C26BF5UL,0x149563A4UL,0x29F22FF4UL,0x7E731AF1UL,0x32BA853AUL,0x172UL},
                      {0x56FAE6A3UL,0x56E0C110UL,0xF18AEB9BUL,0x27A8CD9BUL,0x555A67C4UL,0x19B7F70FUL,0x537DECE8UL,0x1DBUL}}};
//Precomputation points
ec_point_aff G3[2]  = {{{0xCA10CE8CUL,0xB3F21EBEUL,0x9DED1294UL,0x07BD695DUL,0xB169733EUL,0x7UL,0x0UL,0x0UL},
                        {0x4A46E566UL,0x40CF0940UL,0x5596317AUL,0xB3FA22D8UL,0xEA6C3F7AUL,0x2UL,0x0UL,0x0UL}},
                       {{0x35B25BE6UL,0x89382387UL,0x731F5E3EUL,0x6B7069F0UL,0x195489BBUL,0xF5A0FFE9UL,0x310F0F79UL,0x12CUL},
                        {0xE628C820UL,0x64B993B6UL,0x28632097UL,0x236B0F48UL,0x2C3A6BE1UL,0x6BBD39C3UL,0x5FF84152UL,0x183UL}}};
ec_point_aff G5[2]  = {{{0xC68A1031UL,0x8BC0046DUL,0x536B5A45UL,0x8FE46728UL,0xC1428F6AUL,0x4UL,0x0UL,0x0UL},
                        {0xB8914834UL,0xF440E165UL,0xDD5562F1UL,0x7DD8530CUL,0x6187D5FFUL,0x4UL,0x0UL,0x0UL}},
                       {{0x32077AC0UL,0xA21C78A5UL,0x00A4D4A1UL,0x43B18CDAUL,0x4C7796F2UL,0x854EF6B6UL,0x8B9DF3EAUL,0x09EUL},
                        {0x7EDFB6FDUL,0x39AF846FUL,0xDE0568EBUL,0xA3DDD6BFUL,0x45F163A8UL,0xE1760EE5UL,0x7ABB1ED2UL,0x18CUL}}};
ec_point_aff G7[2]  = {{{0x29CE13DFUL,0x92A33027UL,0x4A538869UL,0x6B74501DUL,0x8343C0D1UL,0x3UL,0x0UL,0x0UL},
                        {0xCB2EC5AAUL,0xCADD67F0UL,0xB9A997F6UL,0xF90D1502UL,0x0D1B30B0UL,0x3UL,0x0UL,0x0UL}},
                       {{0x68CA9EF4UL,0x32051179UL,0x22EDA6B8UL,0xDB9391ECUL,0x17F88440UL,0x3A514032UL,0xC1A39AB0UL,0x061UL},
                        {0x49314A3EUL,0x983B92C2UL,0x737B8FABUL,0xBF314919UL,0x5A01A0BCUL,0xD2F10F5BUL,0x4D4FDF13UL,0x0BEUL}}};
ec_point_aff G9[2]  = {{{0x57394F5CUL,0x23D642E3UL,0x02FF0DDFUL,0x1993B9C9UL,0xFC3309ABUL,0x5UL,0x0UL,0x0UL},
                        {0x07DD78C9UL,0x8544EE5BUL,0x89E8C5C9UL,0x8B0253B6UL,0xA6C19961UL,0x4UL,0x0UL,0x0UL}},
                       {{0xA2046507UL,0x2D75F8EBUL,0xECC6183EUL,0x02374E1DUL,0x9C93CEEFUL,0x860B4C66UL,0x07861FE6UL,0x179UL},
                        {0x76EFC4A5UL,0xDFCDCCC1UL,0x52F6CA04UL,0x1AA40CEAUL,0xED2B1ACCUL,0xFE93521FUL,0x00254BDDUL,0x021UL}}};
ec_point_aff G11[2] = {{{0x5C534041UL,0x86217FDEUL,0xCE11A2B9UL,0x5727F0B7UL,0xC1722273UL,0x1UL,0x0UL,0x0UL},
                        {0x71A22148UL,0x7B870045UL,0x68F71342UL,0x41B0C99DUL,0x9BCB6731UL,0x4UL,0x0UL,0x0UL}},
                       {{0xF6090701UL,0x2C8A5469UL,0x48905717UL,0xDA1B1809UL,0xE45A3F40UL,0x7901EB9DUL,0x1D2E7C27UL,0x177UL},
                        {0xF2CF2426UL,0x501870BEUL,0x3985CC0AUL,0xF9EF9CDDUL,0x67D57436UL,0x71E708A8UL,0x10570BFDUL,0x1F5UL}}};
ec_point_aff G13[2] = {{{0x151346BBUL,0xEC1470B5UL,0xDAB99B8AUL,0x180B46D0UL,0xD51336A7UL,0x6UL,0x0UL,0x0UL},
                        {0xDDFB23FEUL,0x2A08EDA0UL,0x2A0721B1UL,0x6A21A61AUL,0x24037BDAUL,0x7UL,0x0UL,0x0UL}},
                       {{0x671660EAUL,0x4DFE234DUL,0xD54523CAUL,0x691C1256UL,0x02F90B6CUL,0xB29699FFUL,0x44924BD3UL,0x06DUL},
                        {0x7DD20370UL,0xDDCAF165UL,0x4A3F2621UL,0x99C5A435UL,0x43E06E7DUL,0x9B0D9641UL,0xF45D7BDEUL,0x02BUL}}};
ec_point_aff G15[2] = {{{0x5FDE2A56UL,0xC7EEF084UL,0x7764A39AUL,0xEEF24A99UL,0xA595788AUL,0x1UL,0x0UL,0x0UL},
                        {0xC41F467AUL,0x08922A85UL,0x514F8790UL,0xBF784D56UL,0x95D86326UL,0x5UL,0x0UL,0x0UL}},
                       {{0x94DD6F83UL,0x0199617BUL,0x55134A2CUL,0x4C74E5F6UL,0xFC6493FAUL,0x7DB601D9UL,0x3B30EE91UL,0x175UL},
                        {0x345CBEAFUL,0x20024B93UL,0xF705982CUL,0xF9197A73UL,0x7669FFE3UL,0x7600877FUL,0x89C093A4UL,0x1FCUL}}};
//Constants for partial reduction 
uint32 ec_s0[2][4] = {{0x73BDCB51UL,0xD1AD2426UL,0x22234UL,0x0UL},{0x10C103ABUL,0x3C775348UL,0xFFAFD49CUL,0x55D96UL}};         //s0
uint32 ec_s1[2][4] = {{0x40112ADAUL,0x26B17BFCUL,0x09FF4UL,0x0UL},{0xCB36BEE6UL,0x16AA143CUL,0x2D7AE36EUL,0x882D7UL}};         //s1
uint32 ec_s2[2][4] = {{0x802255B4UL,0x4D62F7F8UL,0x13FE8UL,0x0UL},{0x966D7DCCUL,0x2D542879UL,0x5AF5C6DCUL,0x1105AEUL}};        //s2=2*s1 
uint32 ec_s[2][4]  = {{0x33ACA077UL,0xAAFBA82AUL,0x18240UL,0x0UL},{0xBA75BB3BUL,0xDA32C0F4UL,0x2DCB0ED1UL,0x32540UL}};         //s=s1-s0
uint32 ec_Vm[2][4] = {{0x33F14BDDUL,0x45C1981BUL,0x40211UL,0x0UL},{0xC5CEAF7BUL,0xBBEC6B57UL,0xE456F351UL,0x1A756EUL}};        //Vm=2^m+1-#E_a(F_2^m)

/********************************************************************************************
  Point compression 
 ********************************************************************************************/
 void point_compression(ec_point_aff *P, uint32 *Pc, uint32 curve_id)
 {
	 uint32 z[N], Num;

	 switch(curve_id){
	 case sect163k1:
		 if(IF0_163(P->x))
		 {
			 if(IF0_163(P->y))      //P is infinity point
			 {
				 XIS0(Pc, 6);
				 return;
			 }
			 *z = 0;
		 }
		 Num = 6;
		 break;
	 case sect233k1:
		 if(IF0_233(P->x))
		 {
			 if(IF0_233(P->y))      //P is infinity point
			 {
				 XIS0(Pc, 8);
				 return;
			 }
			 *z = 0;
		 }
		 Num = 8;
		 break;
	 default: return;
	 }

	 modinv(P->x, z, curve_id);
	 modmul(P->y, z, z, curve_id);

	 XTOY(Pc, P->x, Num);
		 
     if((*z & 0x1) == 0x0)
	 {
		 switch(curve_id){
	     case sect163k1:
			  *(Pc + 5) |= 0x200;
			 break;
	     case sect233k1: 
			 *(Pc + 7) |= 0x20000;
		     break;
	     default: return;
		 }	
	 }
	 else
	 {
		 switch(curve_id){
	     case sect163k1:
			 *(Pc + 5) |= 0x300;
		     break;
	     case sect233k1: 
			 *(Pc + 7) |= 0x30000;
		     break;
	     default: return;
		 }	
	 }
 }

 /********************************************************************************************
  Point decompression 
  ********************************************************************************************/
 uint32 point_decompression(uint32 *Pc, ec_point_aff *P, uint32 curve_id)
 {
	 uint32 yp, Num, t;
	 uint32 beta[N];
	 uint32 z[N];

	 switch(curve_id){
	 case sect163k1:
		 Num = 6;
		 XTOY(z, Pc, 6);
		 if((*(z + 5) >> 8) == 0x2) yp = 0;
		 else if((*(z + 5) >> 8) == 0x3) yp = 1;
		 else return 0;
		 *(z + 5) &= 0xff;
		 t = IF0_163(Pc);
		 break;
	case sect233k1:
		Num = 8;
		XTOY(z, Pc, 8);
		if((*(z + 7) >> 16) == 0x2) yp = 0;
		else if((*(z + 7) >> 16) == 0x3) yp = 1;
		else return 0;
		*(z + 7) &= 0xffff;
		t = IF0_233(Pc);
		break;
	 default: return 0;
	 }

	 if(t)
	 {
		 XIS0(P->x, Num);
		 XIS1(P->y, Num);
		 return 1;
	 }
	 else
	 {
		 modinv(z,beta, curve_id);
		 modsq(beta, beta, curve_id);
		 modadd(z, beta, beta, curve_id);

		 switch(curve_id){
	     case sect163k1:
			 *beta ^= 0x1;
			 if((*beta & 0x1) == ((*(beta + 4) >> 29) & 0x1))     //trace(c) = c0 + c157
			 {
				 halftrace(beta, beta, curve_id);
				 if((*beta & 0x1) == yp)
					 modmul(z, beta, P->y, curve_id);
				 else
				 {
					 *beta ^= 0x1;
					 modmul(z, beta, P->y, curve_id);
				 }
				 XTOY(P->x, z, Num);
				 return 1;
			 }
			 else return 0;
			 break;
	     case sect233k1:
			 if((*beta & 0x1) == (*(beta + 4) >> 31))             //trace(c) = c0 + c159
			 {
				 halftrace(beta, beta, curve_id);
				 if((*beta & 0x1) == yp)
					 modmul(z, beta, P->y, curve_id);
				 else
				 {
					 *beta ^= 0x1;
					 modmul(z, beta, P->y, curve_id);
				 }
				 XTOY(P->x, z, Num);
				 return 1;
			 }
			 else return 0;
			 break;
	     default: return 0;
		 }
	 }
 }

/********************************************************************************************
  Point negation 
 ********************************************************************************************/
  void point_negation(ec_point_aff *P, ec_point_aff *negP, uint32 curve_id)
  {
	  uint32 i, Num, t;

	  switch(curve_id){
	  case sect163k1:
		  Num = 6;
		  t = (IF0_163(P->x) && IF0_163(P->y));
		  break;
	  case sect233k1:
		  Num = 8;
		  t = (IF0_233(P->x) && IF0_233(P->y));
		  break;
	  default: return;
	  }

	  if(t)
	  {
		  XIS0(negP->x, Num);
		  XIS0(negP->y, Num);
		  return;		  
	  }
	  
	  XTOY(negP->x, P->x, Num);

	  for(i = Num - 1; i != 0; i--)
		   negP->y[i] = P->x[i]^ P->y[i];
      negP->y[0] = P->x[0]^ P->y[0];
  }

 /********************************************************************************************
  Coordinate Transformation: LD to Affine
  ********************************************************************************************/
  void project_to_affine(ec_point_pro *Qp, ec_point_aff *Qa, uint32 curve_id)
  {
	  uint32 Zinv[N], Num, t;

	  switch(curve_id){
	  case sect163k1:
		  Num = 6;
		  t = IF0_163(Qp->Z);
		  break;
	  case sect233k1: 
		  Num = 8;
		  t = IF0_233(Qp->Z);
		  break;
	  default: return;
	  }

	  if(t)
	  {
		  XIS0(Qa->x, Num);
		  XIS0(Qa->y, Num);
		  return;
	  }
	  
	  modinv(Qp->Z, Zinv, curve_id);
	  modmul(Qp->X, Zinv, Qa->x, curve_id);
	  modsq(Zinv, Zinv, curve_id);
	  modmul(Qp->Y, Zinv, Qa->y, curve_id);
  }

  /*******************************************************************************************
   Coordinate Transformation: Affine to LD
   *******************************************************************************************/
  void affine_to_project(ec_point_aff *Qa, ec_point_pro *Qp, uint32 curve_id)
  {
	  uint32 Num;

	  switch(curve_id){
	  case sect163k1: Num = 6;
		  break;
	  case sect233k1: Num = 8;
		  break;
	  default: return;
	  }

	  XTOY(Qp->X, Qa->x, Num);
	  XTOY(Qp->Y, Qa->y, Num);
	  XIS1(Qp->Z, Num);
  }

 /********************************************************************************************
  Mixed-addition on Koblitz curve with LD coordinates
  *********************************************************************************************/
 void mixed_addition(ec_point_pro *P, ec_point_aff *Q, ec_point_pro *R, uint32 curve_id)
 {
	 uint32 T1[N], T2[N], T3[N];
	 uint32 Num, t1, t2;

	 switch(curve_id){
	 case sect163k1:
		 Num = 6;
		 t1 = (IF0_163(Q->x) && IF0_163(Q->y));
		 t2 = IF0_163(P->Z);
		 break;
	 case sect233k1:
		 Num = 8;
		 t1 = (IF0_233(Q->x) && IF0_233(Q->y));
		 t2 = IF0_233(P->Z);
		 break;
	 default: return;
	 }

	 // If Q = infinity then return P
	 if(t1)
	 {
		 XTOY(R->X, P->X, Num);
         XTOY(R->Y, P->Y, Num);
         XTOY(R->Z, P->Z, Num);
		 return;
	 }

	 // If P = infinity then return (x2 : y2 : 1)
	 if(t2)
	 {
		 XTOY(R->X, Q->x, Num);
		 XTOY(R->Y, Q->y, Num);
         XIS1(R->Z, Num);
		 return;
	 }

     modmul(P->Z, Q->x, T1, curve_id);
	 modsq(P->Z, T2, curve_id);
	 modadd(P->X, T1, R->X, curve_id);
	 modmul(P->Z, R->X, T1, curve_id);
	 modmul(T2, Q->y, T3, curve_id);
	 modadd(P->Y, T3, R->Y, curve_id);

	 switch(curve_id){
	 case sect163k1:
		 t1 = IF0_163(R->X);
		 t2 = IF0_163(R->Y);
		 break;
	 case sect233k1:
		 t1 = IF0_233(R->X);
		 t2 = IF0_233(R->Y);
		 break;
	 default: return;
	 }

	 if(t1)
	 {
		 if(t2)
		 {
			 affine_to_project(Q, P, curve_id);
			 point_doubling(P, R, curve_id);
			 return;
		 }
	     else
         {
			 XIS1(R->X, Num);
			 XIS0(R->Y, Num);
			 XIS0(R->Z, Num);
			 return;
         }		 
	 }
	 modsq(T1, R->Z, curve_id);
	 modmul(T1, R->Y, T3, curve_id);
	 if(curve_id == sect163k1)
		 modadd(T1, T2, T1, curve_id);
	 modsq(R->X, T2, curve_id);
	 modmul(T2, T1, R->X, curve_id);
	 modsq(R->Y, T2, curve_id);
	 modadd(R->X, T2, R->X, curve_id);
	 modadd(R->X, T3, R->X, curve_id);
	 modmul(Q->x, R->Z, T2,curve_id);
	 modadd(T2, R->X, T2, curve_id);
	 modsq(R->Z, T1, curve_id);
	 modadd(T3, R->Z, T3, curve_id);
	 modmul(T3, T2, R->Y, curve_id);
	 modadd(Q->x, Q->y, T2, curve_id);
	 modmul(T1, T2, T3, curve_id);
	 modadd(R->Y, T3, R->Y, curve_id);
 }

 /*********************************************************************************************
  Doubling on Koblitz curves with LD coordinates
  *********************************************************************************************/
 void point_doubling(ec_point_pro *P, ec_point_pro *Q, uint32 curve_id)
 {
	 uint32 T1[N], T2[N];
	 uint32 Num, t;

     switch(curve_id){
	 case sect163k1:
		 Num = 6;
		 t = IF0_163(P->Z);
		 break;
	 case sect233k1:
		 Num = 8;
		 t = IF0_233(P->Z);
		 break;
	 default: return;
	 }

	 // If P = infinity then return infinity
	 if(t)
	 {
		 XIS1(Q->X, Num);
		 XIS0(Q->Y, Num);
		 XIS0(Q->Z, Num);
		 return;
	 }

     modsq(P->Z, T1, curve_id);
	 modsq(P->X, T2, curve_id);
	 modmul(T1, T2, Q->Z, curve_id);
	 modsq(T2, Q->X, curve_id);
	 modsq(T1, T1, curve_id);
	 modadd(Q->X, T1, Q->X, curve_id);
	 modsq(P->Y, T2, curve_id);
	 if(curve_id == sect163k1)
		 modadd(T2, Q->Z, T2, curve_id);
	 modadd(T1, T2, T2, curve_id);
	 modmul(Q->X, T2, Q->Y, curve_id);
	 modmul(T1, Q->Z, T2, curve_id);
	 modadd(Q->Y, T2, Q->Y, curve_id);
 }

 /********************************************************************************************
  Frobenius endomorphism on Koblitz curves
  ********************************************************************************************/
 void point_frobenius(ec_point_pro *P, uint32 curve_id)
 {
	 uint32 Num, t;

	 switch(curve_id){
	 case sect163k1:
		 Num = 6;
		 t = IF0_163(P->Z);
		 break;
	 case sect233k1:
		 Num = 8;
		 t = IF0_233(P->Z);
		 break;
	 default: return;
	 }

	 if(t)
	 {
		 XIS1(P->X, Num);
	 	 XIS0(P->Y, Num);
		 return;
	 }
	 modsq(P->X, P->X, curve_id);
	 modsq(P->Y, P->Y, curve_id);
	 modsq(P->Z, P->Z, curve_id);
  }

 /********************************************************************************************
  Compute the partial modular reduction of an element in Z[t]
  ********************************************************************************************/
void partial_mod(uint32 *k, uint32 *r0, uint32 *r1, uint32 curve_id)
{
	uint32 kp[5] = {0x0}; 
	uint32 gp[8] = {0x0}; 
	uint32 jp[5] = {0x0};
	uint32 fp[5] = {0x0};
	uint64 T1, T2, T3;
	uint32 lamda0i[4], lamda1i[4];     //integer part of lamda0 and lamda1
	sint32 lamda0f, lamda1f;               //fraction part of lamda0 and lamda1
	sint32 h0, h1, lamda;
    uint32 i, j, t, u, v, w, Num;

	//k' = [k/2^(a - C + (m - 9)/2)]
	switch(curve_id){
	case sect163k1:
		Num = 6;
		t = 3;        //t = Num/2
		u = 3;
		v = 0x80000;    // + 2^83
		w = 0x0;   
		break;
	case sect233k1:
		Num = 8;
		t = 4;        //t = Num/2
		u = 9;
		v = 0x0;
		w = 0x400000;   // + 2^118
		break;
	default: return;
	}
	for(i = t; i != 0; i--)
		*(kp + i) = *(k + i + (t - 1));
	*kp = *(k + (t - 1));

	//g' = s0 * k'
	T1 = (uint64)(*(ec_s0[curve_id])) * (*kp);
	*gp = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < Num - 1; i++)
	{
		T3 = 0;
		for(j = (i <= t ? 0 : (i - t)); j <= (i < t ? i : (t - 1)); j++)
		{
			T2 = (uint64)(*(ec_s0[curve_id] + j)) * (*(kp + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(gp + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	*(gp + (Num - 1)) = (uint32)T1 + (*(ec_s0[curve_id] + (t - 1))) * (*(kp + t));

	//j'=Vm*[g'/2^m]
	T1 = (uint64)(*(gp + (Num - 1)) >> u);

	if(T1 != 0)
	{
		T2 = T1 * (*(ec_Vm[curve_id]));
		*jp = (uint32)T2;
		T2 = T2 >> 32;

		T3 = T1 * (*(ec_Vm[curve_id] + 1));
		T2 += (uint32)T3;
		*(jp + 1) = (uint32)T2;
		T2 = (T2 >> 32) + (T3 >> 32);

		T3 = T1 * (*(ec_Vm[curve_id] + 2));
		T2 += (uint32)T3;
		*(jp + 2) = (uint32)T2;

		switch(curve_id){
	    case sect163k1:
			*(jp + 3) = (uint32)(T2 >> 32) + (uint32)(T3 >> 32);
			break;
	    case sect233k1:
			T2 = (T2 >> 32) + (T3 >> 32);

			T3 = T1 * (*(ec_Vm[curve_id] + 3));
			T2 += (uint32)T3;
			*(jp + 3) = (uint32)T2;
			*(jp + 4) = (uint32)(T2 >> 32) + (uint32)(T3 >> 32);
		    break;
	    default: return;
		}	
	}

	//Rounding off in Z[t]
	T1 = (uint64)(*gp) + ~(*jp) + 0x1;
	T2 = T1 >> 32;
	*gp = (uint32)T1;

	T1 = T2 + (*(gp + 1)) + ~(*(jp + 1));
	T2 = T1 >> 32;
	*(gp + 1) = (uint32)T1;
	
	T1 = T2 + (*(gp + 2)) + ~(*(jp + 2)) + v;  
	T2 = T1 >> 32;
	*(gp + 2) = (uint32)T1;

	T1 = T2 + (*(gp + 3)) + ~(*(jp + 3)) + w;
	T2 = T1 >> 32;
	*(gp + 3) = (uint32)T1;

	switch(curve_id){
	case sect163k1:
		T1 = T2 + (*(gp + 4)) + 0xffffffff;   
		T2 = T1 >> 32;	  
		*(gp + 4) = (uint32)T1;
		*(gp + 5) += (uint32)T2 + 0xffffffff;

		//Obtain the fraction part of lamda0
		lamda0f = (sint32)((*(gp + 2) >> 20) | ((*(gp + 3) & 0x3) << 12)); //the faction of lamda0 is 14 bits
		v = 2;
		w = 30;
		break;
	case sect233k1:
		T1 = T2 + (*(gp + 4)) + ~(*(jp + 4));  
		T2 = T1 >> 32;
		*(gp + 4) = (uint32)T1;

		T1 = T2 + (*(gp + 5)) + 0xffffffff;   
		T2 = T1 >> 32;	  
		*(gp + 5) = (uint32)T1;

		T1 = T2 + (*(gp + 6)) + 0xffffffff;   
		T2 = T1 >> 32;	  
		*(gp + 6) = (uint32)T1;
		*(gp + 7) += (uint32)T2 + 0xffffffff;

		//Obtain the fraction part of lamda0
		lamda0f = (sint32)((*(gp + 3) >> 23) | ((*(gp + 4) & 0x7f) << 9));  //the faction of lamda0 is 16 bits 
		v = 7;
		w = 25;
		break;
	default: return;
	}
	
	//Obtain the integer part of lamda0
	*(lamda0i + (t - 1)) =  *(gp + (Num - 1)) >> v;
	for(i = t - 2; i != 0; i--)
		*(lamda0i + i) = (*(gp + i + t) >> v) | (*(gp + i + t + 1) << w);
	*(lamda0i) = (*(gp + t) >> v) | (*(gp + t + 1) << w);

	//round(lamda0)
	switch(curve_id){
	case sect163k1:
		if((lamda0f & 0x2000) == 0x2000)
		{
			for(j = 0; j < 3; j++)
			{
				if(*(lamda0i + j) == 0xffffffff) *(lamda0i + j) = 0;
				else
				{
					lamda0i[j]++;
					break;
				}
			}
			lamda0f = lamda0f - 0x4000; 		  
		}
		v = 0x80000;    // + 2^83
		w = 0x0;   
		break;
	case sect233k1:
		if((lamda0f & 0x8000) == 0x8000)
		{
			for(j = 0; j < t; j++)
			{
				if(*(lamda0i + j) == 0xffffffff) *(lamda0i + j) = 0;
				else
				{
					lamda0i[j]++;
					break;
				}
			}
			lamda0f = 0x10000 - lamda0f; 		  
		}
		else
		{
			lamda0f = -lamda0f;
		}
		v = 0x0;
		w = 0x400000;   // + 2^118
		break;
	default: return;
	}
	
	//g' = s1 * k'
	T1 = (uint64)(*(ec_s1[curve_id])) * (*kp);
	*gp = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < Num - 1; i++)
	{
		T3 = 0;
		for(j = (i <= t ? 0 : (i - t)); j <= (i < t ? i : (t - 1)); j++)
		{
			T2 = (uint64)(*(ec_s1[curve_id] + j)) * (*(kp + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(gp + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	*(gp + (Num - 1)) = (uint32)T1 + (*(ec_s1[curve_id] + (t - 1))) * (*(kp + t));

	//j'=Vm*[g'/2^m], sign: +, 136 bits
	T1 = (uint64)(*(gp + (Num - 1)) >> u);

	if(T1 != 0)
	{
		T2 = T1 * (*(ec_Vm[curve_id]));
		*jp = (uint32)T2;
		T2 = T2 >> 32;

		T3 = T1 * (*(ec_Vm[curve_id] + 1));
		T2 += (uint32)T3;
		*(jp + 1) = (uint32)T2;
		T2 = (T2 >> 32) + (T3 >> 32);

		T3 = T1 * (*(ec_Vm[curve_id] + 2));
		T2 += (uint32)T3;
		*(jp + 2) = (uint32)T2;

		switch(curve_id){
	    case sect163k1:
			*(jp + 3) = (uint32)(T2 >> 32) + (uint32)(T3 >> 32);
			break;
	    case sect233k1:
			T2 = (T2 >> 32) + (T3 >> 32);

			T3 = T1 * (*(ec_Vm[0] + curve_id * 4 + 3));
			T2 += (uint32)T3;
			*(jp + 3) = (uint32)T2;
			*(jp + 4) = (uint32)(T2 >> 32) + (uint32)(T3 >> 32);
		    break;
	    default: return;
		}
	}

	//Rounding off in Z[t]
	T1 = (uint64)(*gp) + ~(*jp) + 0x1;
	T2 = T1 >> 32;
	*gp = (uint32)T1;

	T1 = T2 + (*(gp + 1)) + ~(*(jp + 1));
	T2 = T1 >> 32;
	*(gp + 1) = (uint32)T1;

	T1 = T2 + (*(gp + 2)) + ~(*(jp + 2)) + v;  
	T2 = T1 >> 32;
	*(gp + 2) = (uint32)T1;

	T1 = T2 + (*(gp + 3)) + ~(*(jp + 3)) + w;
	T2 = T1 >> 32;
	*(gp + 3) = (uint32)T1;

	switch(curve_id){
	case sect163k1:
		T1 = T2 + (*(gp + 4)) + 0xffffffff;   
		T2 = T1 >> 32;	  
		*(gp + 4) = (uint32)T1;
		*(gp + 5) += (uint32)T2 + 0xffffffff;

		//Obtain the fraction part of lamda1
		lamda1f = (sint32)((*(gp + 2) >> 20) | ((*(gp + 3) & 0x3) << 12)); //the faction of lamda0 is 14 bits
		v = 2;
		w = 30;
		break;
	case sect233k1:
		T1 = T2 + (*(gp + 4)) + ~(*(jp + 4));  
		T2 = T1 >> 32;
		*(gp + 4) = (uint32)T1;

		T1 = T2 + (*(gp + 5)) + 0xffffffff;   
		T2 = T1 >> 32;	  
		*(gp + 5) = (uint32)T1;

		T1 = T2 + (*(gp + 6)) + 0xffffffff;   
		T2 = T1 >> 32;	  
		*(gp + 6) = (uint32)T1;
		*(gp + 7) += (uint32)T2 + 0xffffffff;

		//Obtain the fraction part of lamda1
		lamda1f = (sint32)((*(gp + 3) >> 23) | ((*(gp + 4) & 0x7f) << 9));  //the faction of lamda0 is 16 bits 
		v = 7;
		w = 25;
		break;
	default: return;
	}

	//Obtain the integer part of lamda1
	*(lamda1i + (t - 1)) =  *(gp + (Num - 1)) >> v;
	for(i = t - 2; i != 0; i--)
		*(lamda1i + i) = (*(gp + i + t) >> v) | (*(gp + i + t + 1) << w);
	*(lamda1i) = (*(gp + t) >> v) | (*(gp + t + 1) << w);

	h0 = 0; 
	h1 = 0;

	//round(lamda1)
	switch(curve_id){
	case sect163k1: 
		if((lamda1f & 0x2000) == 0x2000)
		{
			for(j = 0; j < 3; j++)
			{
				if(*(lamda1i + j) == 0xffffffff) *(lamda1i + j) = 0;
				else
				{
					lamda1i[j]++;
					break;
				}
			}
			lamda1f = 0x4000 - lamda1f; 		  
		}
		else
		{
			lamda1f = -lamda1f;
		}

		//eta=2*eta0+eta1
		lamda = 2*lamda0f + lamda1f;

		if(lamda >= 0x4000)
		{
			if((lamda0f - 3*lamda1f) < -0x4000) h1 = 1;
			else h0 = 1;
		}
		else
		{
			if((lamda0f + 4*lamda1f) >= 0x8000) h1 = 1;
		}

		if(lamda < -0x4000)
		{
			if((lamda0f - 3*lamda1f) >= 0x4000) h1 = -1;
			else h0 = -1;
		}
		else
		{
			if((lamda0f + 4*lamda1f) < -0x8000) h1 = -1;
		}
		break;
	case sect233k1:
		if((lamda1f & 0x8000) == 0x8000)
		{
			for(j = 0; j < 4; j++)
			{
				if(*(lamda1i + j) == 0xffffffff) *(lamda1i + j) = 0;
				else
				{
					lamda1i[j]++;
					break;
				}
			}
			lamda1f = 0x10000 - lamda1f; 		  
		}
		else
		{
			lamda1f = -lamda1f;
		}

		//eta = 2*eta0 + eta1
		lamda = 2*lamda0f - lamda1f;

		if(lamda >= 0x10000)
		{
			if((lamda0f + 3*lamda1f) < -0x10000) h1 = -1;
			else h0 = 1;
		}
		else
		{
			if((lamda0f - 4*lamda1f) >= 0x20000) h1 = -1;
		}

		if(lamda < -0x10000)
		{
			if((lamda0f + 3*lamda1f) >= 0x10000) h1 = 1;
			else h0 = -1;
		}
		else
		{
			if((lamda0f - 4*lamda1f) < -0x20000) h1 = 1;
		}
		break;
	default: return;
	}

	//q0 = f0 + h0
	if(h0 != 0)
	{
		if(h0 == 1)
		{
			switch(curve_id){
	        case sect163k1:
				for(j = 0; j < 3; j++)
				{
					if(*(lamda0i + j) == 0xffffffff) *(lamda0i + j) = 0;
					else
					{
						lamda0i[j]++;
						break;
					}
				}
				break;
	        case sect233k1:
				T1 = (uint64)(*lamda0i) + 0xffffffff;
				T2 = T1 >> 32;
				*lamda0i = (uint32)T1;

				T1 = T2 + (*(lamda0i + 1)) + 0xffffffff;
				T2 = T1 >> 32;
				*(lamda0i + 1) = (uint32)T1;

				T1 = T2 + (*(lamda0i + 2)) + 0xffffffff;
				T2 = T1 >> 32;
				*(lamda0i + 2) = (uint32)T1;

				*(lamda0i + 3) += (uint32)T2 + 0xffffffff;
		        break;
	        default: return;
			}
		}
		else
		{
			switch(curve_id){
	        case sect163k1:
				T1 = (uint64)(*lamda0i) + 0xffffffff;
				T2 = T1 >> 32;
				*lamda0i = (uint32)T1;

				T1 = T2 + (*(lamda0i + 1)) + 0xffffffff;
				T2 = T1 >> 32;
				*(lamda0i + 1) = (uint32)T1;

				*(lamda0i + 2) += (uint32)T2 + 0xffffffff;
				break;
	        case sect233k1:
				for(j = 0; j < 4; j++)
				{
					if(*(lamda0i + j) == 0xffffffff) *(lamda0i + j) = 0;
					else
					{
						lamda0i[j]++;
						break;
					}
				}
		        break;
	        default: return;
			}
		}
	}

	//q1 = f1 + h1
	if(h1 != 0)
	{
		if(h1 == 1)
		{
			switch(curve_id){
	        case sect163k1:
				T1 = (uint64)(*lamda1i) + 0xffffffff;
				T2 = T1 >> 32;
				*lamda1i = (uint32)T1;

				T1 = T2 + (*(lamda1i + 1)) + 0xffffffff;
				T2 = T1 >> 32;
				*(lamda1i + 1) = (uint32)T1;

				*(lamda1i + 2) += (uint32)T2 + 0xffffffff;
				break;
	        case sect233k1:
				for(j = 0; j < 4; j++)
				{
					if(*(lamda1i + j) == 0xffffffff) *(lamda1i + j) = 0;
					else
					{
						lamda1i[j]++;
						break;
					}
				}
		        break;
	        default: return;
			}
		}
		else
		{
			switch(curve_id){
	        case sect163k1:
				for(j = 0; j < 3; j++)
				{
					if(*(lamda1i + j) == 0xffffffff) *(lamda1i + j) = 0;
					else
					{
						lamda1i[j]++;
						break;
					}
				}
		        break;
	        case sect233k1:
				T1 = (uint64)(*lamda1i) + 0xffffffff;
				T2 = T1 >> 32;
				*lamda1i = (uint32)T1;

				T1 = T2 + (*(lamda1i + 1)) + 0xffffffff;
				T2 = T1 >> 32;
				*(lamda1i + 1) = (uint32)T1;

				T1 = T2 + (*(lamda1i + 2)) + 0xffffffff;
				T2 = T1 >> 32;
				*(lamda1i + 2) = (uint32)T1;

				*(lamda1i + 3) += (uint32)T2 + 0xffffffff;
		        break;
	        default: return;
			}
		}
	}

	//s * f0
	T1 = (uint64)(*(ec_s[curve_id])) * (*lamda0i);
	*fp = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < t; i++)
	{
		T3 = 0;
		for(j = 0; j <= i; j++)
		{
			T2 = (uint64)(*(ec_s[curve_id] + j)) * (*(lamda0i + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(fp + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	switch(curve_id){
	case sect163k1: 
		T2 = (uint64)(*(ec_s[curve_id] + 1)) * (*(lamda0i + 2));
		T1 = T1 + (uint32)T2;
		*(fp + 3) = (uint32)T1 + (*(ec_s[curve_id] + 2)) * (*(lamda0i + 1));
		break;
	case sect233k1:
		T2 = (uint64)(*(ec_s[curve_id] + 1)) * (*(lamda0i + 3));
		T1 = T1 + (uint32)T2;
		T2 = (uint64)(*(ec_s[curve_id] + 2)) * (*(lamda0i + 2));
		T1 = T1 + (uint32)T2;
		*(fp + 4) = (uint32)T1 + (*(ec_s[curve_id] + 3)) * (*(lamda0i + 1));
		break;
	default: return;
	}

	//s2 * f1
	T1 = (uint64)(*(ec_s2[curve_id])) * (*lamda1i);
	*gp = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < t; i++)
	{
		T3 = 0;
		for(j = 0; j <= i; j++)
		{
			T2 = (uint64)(*(ec_s2[curve_id] + j)) * (*(lamda1i + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(gp + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	switch(curve_id){
	case sect163k1:
		T2 = (uint64)(*(ec_s2[curve_id] + 1)) * (*(lamda1i + 2));
		T1 = T1 + (uint32)T2;
		*(gp + 3) = (uint32)T1 + (*(ec_s2[curve_id] + 2)) * (*(lamda1i + 1));

		//r0 = k - (|fp| + |gp|)
		T1 = (uint64)(*fp) + (*gp);
		T2 = T1 >> 32;
		*fp = (uint32)T1;

		T1 = T2 + (*(fp + 1)) + (*(gp + 1));
		T2 = T1 >> 32;
		*(fp + 1) = (uint32)T1;

		T1 = T2 + (*(fp + 2)) + (*(gp + 2));
		T2 = T1 >> 32;
		*(fp + 2) = (uint32)T1;

		*(fp + 3) = (uint32)T2 + (*(fp + 3)) + (*(gp + 3));

		if(compare(k, fp, 4) == 1)
		{
			T1 = (uint64)(*k) + ~(*fp) + 0x1;
			T2 = T1 >> 32;
			*r0 = (uint32)T1;

			T1 = T2 + (*(k + 1)) + ~(*(fp + 1));
			j = (uint32)(T1 >> 32);
			*(r0 + 1) = (uint32)T1;
			*(r0 + 2) = (*(k + 2)) + ~(*(fp + 2)) + j;
			*(r0 + 3) = 1;
		}
		else if(compare(k, fp, 4) == -1)
		{
			T1 = (uint64)(*fp) + ~(*k) + 0x1;
			T2 = T1 >> 32;
			*r0 = (uint32)T1;

			T1 = T2 + (*(fp + 1)) + ~(*(k + 1));
			j = (uint32)(T1 >> 32);
			*(r0 + 1) = (uint32)T1;
			*(r0 + 2) = (*(fp + 2)) + ~(*(k + 2)) + j;
			*(r0 + 3) = 2;
		}
		else
		{
			for(i = 0; i < 4; i++)
				*(r0 + i) = 0;
		}
		break;
	case sect233k1:
		T2 = (uint64)(*(ec_s2[curve_id] + 1)) * (*(lamda1i + 3));
		T1 = T1 + (uint32)T2;
		T2 = (uint64)(*(ec_s2[curve_id] + 2)) * (*(lamda1i + 2));
		T1 = T1 + (uint32)T2;
		*(gp + 4) = (uint32)T1 + (*(ec_s2[curve_id] + 3)) * (*(lamda1i + 1));

		//r0 = (k + |fp|) - |gp|
		T1 = (uint64)(*k) + (*fp);
		T2 = T1 >> 32;
		*fp = (uint32)T1;

		for(i = 1; i < 4; i++)
		{
			T1 = T2 + (*(k + i)) + (*(fp + i));
			T2 = T1 >> 32;
			*(fp + i) = (uint32)T1;
		}

		*(fp + 4) = (uint32)T2 + *(k + 4) + *(fp + 4);

		if(compare(fp, gp, 5) == 1)
		{
			T1 = (uint64)(*fp) + ~(*gp) + 0x1;
			T2 = T1 >> 32;
			*r0 = (uint32)T1;

			T1 = T2 + (*(fp + 1)) + ~(*(gp + 1));
			T2 = T1 >> 32;
			*(r0 + 1) = (uint32)T1;

			T1 = T2 + (*(fp + 2)) + ~(*(gp + 2));
			j = (uint32)(T1 >> 32);
			*(r0 + 2) = (uint32)T1;
			*(r0 + 3) = (*(fp + 3)) + ~(*(gp + 3)) + j;
			*(r0 + 4) = 1;
		}
		else if(compare(fp, gp, 5) == -1)
		{
			T1 = (uint64)(*gp) + ~(*fp) + 0x1;
			T2 = T1 >> 32;
			*r0 = (uint32)T1;

			T1 = T2 + (*(gp + 1)) + ~(*(fp + 1));
			T2 = T1 >> 32;
			*(r0 + 1) = (uint32)T1;

			T1 = T2 + (*(gp + 2)) + ~(*(fp + 2));
			j = (uint32)(T1 >> 32);
			*(r0 + 2) = (uint32)T1;
			*(r0 + 3) = (*(gp + 3)) + ~(*(fp + 3)) + j;
			*(r0 + 4) = 2;
		}
		else
		{
			for(i = 0; i < 5; i++)
				*(r0 + i) = 0;
		}
		break;
	default: return;
	}

	//|s0|*f1
	T1 = (uint64)(*(ec_s0[curve_id])) * (*lamda1i);
	*fp = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < t; i++)
	{
		T3 = 0;
		for(j = 0; j <= i; j++)
		{
			T2 = (uint64)(*(ec_s0[curve_id] + j)) * (*(lamda1i + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(fp + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	switch(curve_id){
	case sect163k1:
		T2 = (uint64)(*(ec_s0[curve_id] + 1)) * (*(lamda1i + 2));
		T1 = T1 + (uint32)T2;
		*(fp + 3) = (uint32)T1 + (*(ec_s0[curve_id] + 2)) * (*(lamda1i + 1));
		break;
	case sect233k1:
		T2 = (uint64)(*(ec_s0[curve_id] + 1)) * (*(lamda1i + 3));
		T1 = T1 + (uint32)T2;
		T2 = (uint64)(*(ec_s0[curve_id] + 2)) * (*(lamda1i + 2));
		T1 = T1 + (uint32)T2;
		*(fp + 4) = (uint32)T1 + (*(ec_s0[curve_id] + 3)) * (*(lamda1i + 1));
		break;
	default: return;
	}

	//|s1|*f0
	T1 = (uint64)(*(ec_s1[curve_id])) * (*lamda0i);
	*gp = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < t; i++)
	{
		T3 = 0;
		for(j = 0; j <= i; j++)
		{
			T2 = (uint64)(*(ec_s1[curve_id] + j)) * (*(lamda0i + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(gp + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	switch(curve_id){
	case sect163k1:
		T2 = (uint64)(*(ec_s1[0] + curve_id * 4 + 1)) * (*(lamda0i + 2));
		T1 = T1 + (uint32)T2;
		*(gp + 3) = (uint32)T1 + (*(ec_s1[0] + curve_id * 4 + 2)) * (*(lamda0i + 1));

		//r1 = fp - gp
		if(compare(fp, gp, 4) == 1)
		{
			T1 = (uint64)(*fp) + ~(*gp) + 0x1;
			T2 = T1 >> 32;
			*r1 = (uint32)T1;

			T1 = T2 + (*(fp + 1)) + ~(*(gp + 1));
			j = (uint32)(T1 >> 32);
			*(r1 + 1) = (uint32)T1;
			*(r1 + 2) = (*(fp + 2)) + ~(*(gp + 2)) + j;
			*(r1 + 3) = 1;
		}
		else if(compare(fp, gp, 4) == -1)
		{
			T1 = (uint64)(*gp) + ~(*fp) + 0x1;
			T2 = T1 >> 32;
			*r1 = (uint32)T1;

			T1 = T2 + (*(gp + 1)) + ~(*(fp + 1));
			j = (uint32)(T1 >> 32);
			*(r1 + 1) = (uint32)T1;
			*(r1 + 2) = (*(gp + 2)) + ~(*(fp + 2)) + j;
			*(r1 + 3) = 2;
		}
		else
		{
			for(i = 0; i < 4; i++)
				*(r1 + i) = 0;
		}
		break;
	case sect233k1:
		T2 = (uint64)(*(ec_s1[curve_id] + 1)) * (*(lamda0i + 3));
		T1 = T1 + (uint32)T2;
		T2 = (uint64)(*(ec_s1[curve_id] + 2)) * (*(lamda0i + 2));
		T1 = T1 + (uint32)T2;
		*(gp + 4) = (uint32)T1 + (*(ec_s1[curve_id] + 3)) * (*(lamda0i + 1));

		//r1 = gp - fp
		if(compare(gp, fp, 5) == 1)
		{
			T1 = (uint64)(*gp) + ~(*fp) + 0x1;
			T2 = T1 >> 32;
			*r1 = (uint32)T1;

			T1 = T2 + (*(gp + 1)) + ~(*(fp + 1));
			T2 = T1 >> 32;
			*(r1 + 1) = (uint32)T1;

			T1 = T2 + (*(gp + 2)) + ~(*(fp + 2));
			j = (uint32)(T1 >> 32);
			*(r1 + 2) = (uint32)T1;
			*(r1 + 3) = (*(gp + 3)) + ~(*(fp + 3)) + j;
			*(r1 + 4) = 1;
		}
		else if(compare(gp, fp, 5) == -1)
		{
			T1 = (uint64)(*fp) + ~(*gp) + 0x1;
			T2 = T1 >> 32;
			*r1 = (uint32)T1;

			T1 = T2 + (*(fp + 1)) + ~(*(gp + 1));
			T2 = T1 >> 32;
			*(r1 + 1) = (uint32)T1;

			T1 = T2 + (*(fp + 2)) + ~(*(gp + 2));
			j = (uint32)(T1 >> 32);
			*(r1 + 2) = (uint32)T1;
			*(r1 + 3) = (*(fp + 3)) + ~(*(gp + 3)) + j;
			*(r1 + 4) = 2;
		}
		else
		{
			for(i = 0; i < 5; i++)
				*(r1 + i) = 0;
		}
		break;
	default: return;
	}
}

/*******************************************************************************************
 Compute a width-5 TNAF of an element in Z[t]
 ********************************************************************************************/
void TNAF5_expansion(uint32 *r0, uint32 *r1, char *u, uint32 curve_id)
{	
	char t;
	uint32 i, j, r, c, v, Num;
	uint64 T;
	uint32 s0[4], s1[4], s2[4];
	sint32 flags0, flags0p, flags1, s, sbeta, sgama;
	sint32 beta[8] = {1, -3, -1, 1, -3, -1, 1, 1};
	sint32 gama[2][8] = {{0, 1, 1, 1, 2, 2, 2, -3},{0, -1, -1, -1, -2, -2, -2, 3}};
    
	switch(curve_id){
	case sect163k1:
		Num = 6;
		v = 3;       //j = Num/2
		break;
	case sect233k1:
		Num = 8;
		v = 4;       //j = Num/2
		break;
	default: return;
	}

	for(i = v - 1; i != 0; i--)
	{
		*(s0 + i) = *(r0 + i);
		*(s1 + i) = *(r1 + i);
	}
	*s0 = *r0;
	*s1 = *r1;

	if(*(r0 + v) == 2) flags0 = -1;
	else if(*(r0 + v) == 1) flags0 = 1;
	else flags0 = 0;

	if(*(r1 + v) == 2) flags1 = -1;
	else if(*(r1 + v) == 1) flags1 = 1;
	else flags1 = 0;

	i = 0;

	while(!(flags0 == 0 && flags1 == 0))
	{
		if((*s0 & 0x1) == 0x1)            //r0 is not equal to 0
		{
			//compute r0+tw*r1
		    if(flags0 > 0 && flags1 >= 0)   
			{
				//compute the lowest 32 bits of r0+tw*r1
				switch(curve_id){
	            case sect163k1:
					r = *s0 + 6*(*s1);
					break;
	            case sect233k1:
					r = *s0 + 26*(*s1);
		            break;
	            default: return;
				}

				//get the lowest 5 bits of r0+tw*r1 
				r = r & 0x1f;

				if(r >= 0 && r <= 16) t = (char)r;
				else t = (char)r - 32;

				*(u + i) = t;
			}
			else if(flags0 < 0 && flags1 <= 0)
			{
				//compute the lowest 32 bits of r0+tw*r1
                switch(curve_id){
	            case sect163k1:
					r = *s0 + 6*(*s1);
					break;
	            case sect233k1:
					r = *s0 + 26*(*s1);
		            break;
	            default: return;
				}

				//get the lowest 5 bits of r0+tw*r1 
				r = r & 0x1f;

				if(r >= 0 && r <= 16) t = -(char)r;
				else t = 32 - (char)r;

				*(u + i) = t;
			}
			else if (flags0 >= 0 && flags1 < 0)
			{
				//compute the lowest 32 bits of r0-tw*r1=r0+(32+tw)*|r1|
				switch(curve_id){
	            case sect163k1:
					r = *s0 + 26*(*s1);
		            break;
	            case sect233k1:
		            r = *s0 + 6*(*s1);
		            break;
	            default: return;
				}

				//take out the lowest 5 bits of r0+(32+tw)*|r1| 
				r = r & 0x1f;

				if(r >= 0 && r <= 16) t = (char)r;
				else t = (char)r - 32;

				*(u + i) = t;
			}
			else  // curve_ids0 <= 0 && curve_ids1 > 0
			{
				//compute the lowest 32 bits of r0-tw*r1=-(|r0|+(32+tw)*r1) 
				switch(curve_id){
				case sect163k1:
					r = *s0 + 26*(*s1);
					break;
				case sect233k1:
					r = *s0 + 6*(*s1);
					break;
				default: return;
				}

				//take out the lowest 5 bits of -(|r0|+(32+tw)*r1) 
				r = r & 0x1f;

				if(r >= 0 && r <= 16) t = -(char)r;
				else t = 32 - (char)r;

				*(u + i) = t;
			}

			if(t > 0) s = 1;
			else
			{
				s = -1;
				t = -t;
			}

			//compute r0 = r0 - s*beta_u
			sbeta = s * (*(beta + ((t - 1) >> 1)));
			if(sbeta > 0) r = (uint32)(sbeta);
			else r = (uint32)(-sbeta);

			if(flags0 == 0)
			{
				*s0 = r;
				if(sbeta > 0) flags0 = -1;
				else flags0 = 1;
			}
			else
			{
				if((flags0 < 0 && sbeta > 0) || (flags0 > 0 && sbeta < 0)) //addition
				{
					T = (uint64)(*s0) + r;
					*s0 = (uint32)T;
					c = (uint32)(T >> 32);
					j = 1;
					while(c != 0 && j < v)
					{
						T = (uint64)(*(s0 + j)) + c;
						*(s0 + j) = (uint32)T;
						c = (uint32)(T >> 32);
						j++;
					}
				}
				else //subtraction
				{
					switch(curve_id){
	                case sect163k1:
						if(*(s0 + 2) == 0 && *(s0 + 1) == 0 && *s0 == r)
						{
							*s0 = 0;
							flags0 = 0;
						}
						else if(*(s0 + 2) == 0 && *(s0 + 1) == 0 && *s0 < r)
						{
							*s0 = r - (*s0);
							flags0 = -flags0;
						}
						else
						{
							T = (uint64)(*s0) + ~r + 0x1;
							*s0 = (uint32)T;
							c = (uint32)(T >> 32);
							for(j = 1; j < 3; j++)
							{
								T = (uint64)(*(s0 + j)) + 0xffffffff + c;
								*(s0 + j) = (uint32)T;
								c = (uint32)(T >> 32);
							}
						}
		                break;
	                case sect233k1:
						if(*(s0 + 3) == 0 && *(s0 + 2) == 0 && *(s0 + 1) == 0 && *s0 == r)
						{
							*s0 = 0;
							flags0 = 0;
						}
						else if(*(s0 + 3) == 0 && *(s0 + 2) == 0 && *(s0 + 1) == 0 && *s0 < r)
						{
							*s0 = r - (*s0);
							flags0 = -flags0;
						}
						else
						{
							T = (uint64)(*s0) + ~r + 0x1;
							*s0 = (uint32)T;
							c = (uint32)(T >> 32);
							for(j = 1; j < 4; j++)
							{
								T = (uint64)(*(s0 + j)) + 0xffffffff + c;
								*(s0 + j) = (uint32)T;
								c = (uint32)(T >> 32);
							}
						}
		                break;
	                default: return;
					}
				}
			}
			
			//compute r1 = r1 - s*gama_u
			sgama = s * (*(gama[0] + curve_id* 8 + ((t - 1) >> 1)));
			if(sgama >= 0) r = (uint32)(sgama);
			else r = (uint32)(-sgama);
			
			if(flags1 == 0)
			{
				*s1 = r;
				if(sgama > 0) flags1 = -1;
				if(sgama < 0) flags1 = 1;
			}
			else
			{
				if((flags1 < 0 && sgama > 0) || (flags1 > 0 && sgama < 0)) //addition
				{
					T = (uint64)(*s1) + r;
					*s1 = (uint32)T;
					c = (uint32)(T >> 32);
					j = 1;
					while(c != 0 && j < v)
					{
						T = (uint64)(*(s1 + j)) + c;
						*(s1 + j) = (uint32)T;
						c = (uint32)(T >> 32);
						j++;
					}
				}
				else //subtraction
				{
					switch(curve_id){
	                case sect163k1:
						if(*(s1 + 2) == 0 && *(s1 + 1) == 0 && *s1 == r)
						{
							*s1 = 0;
							flags1 = 0;
						}
						else if(*(s1 + 2) == 0 && *(s1 + 1) == 0 && *s1 < r)
						{
							*s1 = r - (*s1);
							flags1 = -flags1;
						}
						else
						{
							T = (uint64)(*s1) + ~r + 0x1;
							*s1 = (uint32)T;
							c = (uint32)(T >> 32);
							for(j = 1; j < 3; j++)
							{
								T = (uint64)(*(s1 + j)) + 0xffffffff + c;
								*(s1 + j) = (uint32)T;
								c = (uint32)(T >> 32);
							}
						}
						break;
	                case sect233k1:
						if(*(s1 + 3) == 0 && *(s1 + 2) == 0 && *(s1 + 1) == 0 && *s1 == r)
						{
							*s1 = 0;
							flags1 = 0;
						}
						else if(*(s1 + 3) == 0 && *(s1 + 2) == 0 && *(s1 + 1) == 0 && *s1 < r)
						{
							*s1 = r - (*s1);
							flags1 = -flags1;
						}
						else
						{
							T = (uint64)(*s1) + ~r + 0x1;
							*s1 = (uint32)T;
							c = (uint32)(T >> 32);
							for(j = 1; j < 4; j++)
							{
								T = (uint64)(*(s1 + j)) + 0xffffffff + c;
								*(s1 + j) = (uint32)T;
								c = (uint32)(T >> 32);
							}
						}
		                break;
	                default: return;
					}
				}
			}
		}
		else *(u + i) = 0;

		//compute r0/2
		for(j = 0; j < v - 1; j++)
			*(s2 + j) = (*(s0 + j) >> 1) | (*(s0 + j + 1) << 31);
		*(s2 + (v - 1)) = *(s0 + (v - 1)) >> 1;
	    flags0p = flags0;

		switch(curve_id){
	    case sect163k1:
			//r0 = r1 + r0/2
			if(flags0 == 0 || flags1 == 0)
			{
				if(flags0 == 0)
				{
					*s0 = *s1;
					*(s0 + 1) = *(s1 + 1);
					*(s0 + 2) = *(s1 + 2);
					flags0 = flags1;
				}
				else
				{
					*s0 = *s2;
					*(s0 + 1) = *(s2 + 1);
					*(s0 + 2) = *(s2 + 2);
				}		  
			}
			else if(flags0 == flags1) //addition (curve_ids0 > 0 && curve_ids1 > 0) || (curve_ids0 < 0 && curve_ids1 < 0)
			{
				T = (uint64)(*s1) + (*s2);
				*s0 = (uint32)T;
				c = (uint32)(T >> 32);
				T = (uint64)(*(s1 + 1)) + (*(s2 + 1)) + c;
				*(s0 + 1) = (uint32)T;
				c = (uint32)(T >> 32);
				*(s0 + 2) = (*(s1 + 2)) + (*(s2 + 2)) + c;
			}
			else
			{
				if(compare(s1, s2, 3) == 1)  //r0 = |r1| - |r0|/2
				{
					T = (uint64)(*s1) + ~(*s2) + 0x1;
					*s0 = (uint32)T;
					c = (uint32)(T >> 32);
					T = (uint64)(*(s1 + 1)) + ~(*(s2 + 1)) + c;
					*(s0 + 1) = (uint32)T;
					c = (uint32)(T >> 32);
					*(s0 + 2) = (*(s1 + 2)) + ~(*(s2 + 2)) + c;
					flags0 = flags1;
				}
				else if(compare(s1, s2, 3) == -1) //r0 = |r0|/2 - |r1|
				{
					T = (uint64)(*s2) + ~(*s1) + 0x1;
					*s0 = (uint32)T;
					c = (uint32)(T >> 32);
					T = (uint64)(*(s2 + 1)) + ~(*(s1 + 1)) + c;
					*(s0 + 1) = (uint32)T;
					c = (uint32)(T >> 32);
					*(s0 + 2) = (*(s2 + 2)) + ~(*(s1 + 2)) + c;
				}
				else //r0 = 0
				{
					*s0 = 0;
					*(s0 + 1) = 0;
					*(s0 + 2) = 0;
					flags0 = 0;
				}
			}
			break;
	    case sect233k1:
			//r0 = r1 - r0/2
			if(flags0 == 0 || flags1 == 0)
			{
				if(flags0 == 0)
				{
					for(j = 3; j != 0; j--)
						*(s0 + j) = *(s1 + j);
					*s0 = *s1;
					flags0 = flags1;
				}
				else
				{
					for(j = 3; j != 0; j--)
						*(s0 + j) = *(s2 + j);
					*s0 = *s2;
					flags0 = -flags0;
				}		  
			}
			else if(flags0 != flags1) //addition (curve_ids0 < 0 && curve_ids1 > 0) || (curve_ids0 > 0 && curve_ids1 < 0)
			{
				T = (uint64)(*s1) + (*s2);
				*s0 = (uint32)T;
				c = (uint32)(T >> 32);
				T = (uint64)(*(s1 + 1)) + (*(s2 + 1)) + c;
				*(s0 + 1) = (uint32)T;
				c = (uint32)(T >> 32);
				T = (uint64)(*(s1 + 2)) + (*(s2 + 2)) + c;
				*(s0 + 2) = (uint32)T;
				c = (uint32)(T >> 32);
				*(s0 + 3) = (*(s1 + 3)) + (*(s2 + 3)) + c;
				flags0 = flags1;
			}
			else
			{
				if(compare(s1, s2, 4) == 1)  //r0 = |r1| - |r0|/2
				{
					T = (uint64)(*s1) + ~(*s2) + 0x1;
					*s0 = (uint32)T;
					c = (uint32)(T >> 32);
					T = (uint64)(*(s1 + 1)) + ~(*(s2 + 1)) + c;
					*(s0 + 1) = (uint32)T;
					c = (uint32)(T >> 32);
					T = (uint64)(*(s1 + 2)) + ~(*(s2 + 2)) + c;
					*(s0 + 2) = (uint32)T;
					c = (uint32)(T >> 32);
					*(s0 + 3) = (*(s1 + 3)) + ~(*(s2 + 3)) + c;
				}
				else if(compare(s1, s2, 4) == -1) //r0 = |r0|/2 - |r1|
				{
					T = (uint64)(*s2) + ~(*s1) + 0x1;
					*s0 = (uint32)T;
					c = (uint32)(T >> 32);
					T = (uint64)(*(s2 + 1)) + ~(*(s1 + 1)) + c;
					*(s0 + 1) = (uint32)T;
					c = (uint32)(T >> 32);
					T = (uint64)(*(s2 + 2)) + ~(*(s1 + 2)) + c;
					*(s0 + 2) = (uint32)T;
					c = (uint32)(T >> 32);
					*(s0 + 3) = (*(s2 + 3)) + ~(*(s1 + 3)) + c;
					flags0 = -flags0;
				}
				else //r0 = 0
				{
					for(j = 3; j != 0; j--)
						*(s0 + j) = 0;
					*s0 = 0;
					flags0 = 0;
				}
			}
		    break;
	    default: return;
		}
		
		//r1 = -r0/2
		for(j = v - 1; j != 0; j--)
			*(s1 + j) = *(s2 + j);
		*s1 = *s2;
		flags1 = -flags0p;

		i++;
	}
}

 /**************************************************************************************************
  Compute the scalar multiplication with the TNAF5 method (fixed point)
  *************************************************************************************************/
  void TNAF5_fixed_scalarmul(uint32 *k, ec_point_aff *Q, uint32 curve_id)
  {
	  char a[236] = {0x0};
	  uint32 i, Num, BitLen;
	  uint32 lamda0[5];
	  uint32 lamda1[5];
	  ec_point_aff T;
	  ec_point_pro R;

	  switch(curve_id){
	  case sect163k1:
		  Num = 6;
		  BitLen = 163;
		  break;
	  case sect233k1:
		  Num = 8;
		  BitLen = 233;
		break;
	  default: return;
	  }

	  partial_mod(k, lamda0, lamda1, curve_id);
      TNAF5_expansion(lamda0, lamda1, a, curve_id);
	  
	  //R = infinity
	  XIS1(R.X, Num);
	  XIS0(R.Y, Num);
	  XIS0(R.Z, Num);

	  for(i = BitLen + 3; i != 0; i--)
	  {
		  point_frobenius(&R, curve_id);

		  switch(a[i - 1]){
		  case 0:   break;
		  case 1:   mixed_addition(&R, &G[curve_id], &R, curve_id);
			        break;
		  case -1:  point_negation(&G[curve_id], &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 3:   mixed_addition(&R, &G3[curve_id], &R, curve_id);
			        break;
		  case -3:  point_negation(&G3[curve_id], &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 5:   mixed_addition(&R, &G5[curve_id], &R, curve_id);
			        break;
		  case -5:  point_negation(&G5[curve_id], &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 7:   mixed_addition(&R, &G7[curve_id], &R, curve_id);
			        break;
		  case -7:  point_negation(&G7[curve_id], &T, curve_id);
	         		mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 9:   mixed_addition(&R, &G9[curve_id], &R, curve_id);
			        break;
		  case -9:  point_negation(&G9[curve_id], &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 11:  mixed_addition(&R, &G11[curve_id], &R, curve_id);
			        break;
		  case -11: point_negation(&G11[curve_id], &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 13:  mixed_addition(&R, &G13[curve_id], &R, curve_id);
			        break;
		  case -13: point_negation(&G13[curve_id], &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 15:  mixed_addition(&R, &G15[curve_id], &R, curve_id);
			        break;
		  case -15: point_negation(&G15[curve_id], &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  default:  return;                    //Error
		  }
	  }
	  project_to_affine(&R, Q, curve_id);
  }

 /**************************************************************************************************
  Compute the scalar multiplication with the TNAF5 method (random point)
  *************************************************************************************************/
  void TNAF5_random_scalarmul(uint32 *k, ec_point_aff *P, ec_point_aff *Q, uint32 curve_id)
  {
	  char a[236] = {0x0};
	  uint32 i, Num, BitLen;
	  uint32 lamda0[5];
	  uint32 lamda1[5];
	  ec_point_aff P3, P5, P7, P9, P11, P13, P15, T;
	  ec_point_pro R, S;

	  switch(curve_id){
	  case sect163k1:
		  Num = 6;
		  BitLen = 163;

		  //pre-computation
		  affine_to_project(P, &R, sect163k1);
		  point_frobenius(&R, sect163k1);
		  point_negation(P, &P3, sect163k1);
		  mixed_addition(&R, &P3, &S, sect163k1);
		  project_to_affine(&S, &P5, sect163k1);
		  mixed_addition(&R, P, &S, sect163k1);
		  project_to_affine(&S, &P7, sect163k1);
		  point_frobenius(&R, sect163k1);
		  mixed_addition(&R, &P3, &S, sect163k1);
		  project_to_affine(&S, &P3, sect163k1);
		  point_frobenius(&R, sect163k1);
		  point_negation(&P3, &P11, sect163k1);
		  mixed_addition(&R, &P11, &S, sect163k1);
		  project_to_affine(&S, &P11, sect163k1);
		  point_negation(&P11, &P11, sect163k1);
		  point_negation(&P5, &P13, sect163k1);
		  mixed_addition(&R, &P13, &S, sect163k1);
		  project_to_affine(&S, &P13, sect163k1);
		  point_negation(&P13, &P13, sect163k1);
		  point_frobenius(&R, sect163k1);
		  mixed_addition(&R, &P7, &S, sect163k1);
		  project_to_affine(&S, &P9, sect163k1);
		  point_negation(&P9, &P9, sect163k1);
		  affine_to_project(P, &R, sect163k1);
		  point_frobenius(&R, sect163k1);
		  mixed_addition(&R, &P11, &S, sect163k1);
		  project_to_affine(&S, &P15, sect163k1);
		  point_negation(&P15, &P15, sect163k1);
		  break;
	  case sect233k1:
		  Num = 8;
		  BitLen = 233;

		  //pre-computation
		  affine_to_project(P, &R, sect233k1);
		  point_frobenius(&R, sect233k1);         
		  mixed_addition(&R, P, &S, sect233k1);
		  project_to_affine(&S, &P5, sect233k1);
		  point_negation(&P5, &P5, sect233k1);
		  point_negation(P, &P3, sect233k1);
		  mixed_addition(&R, &P3, &S, sect233k1);
		  project_to_affine(&S, &P7, sect233k1);
		  point_negation(&P7, &P7, sect233k1);
		  point_frobenius(&R, sect233k1);
		  mixed_addition(&R, &P3, &S, sect233k1);
		  project_to_affine(&S, &P3, sect233k1);
		  point_frobenius(&R, sect233k1);
		  mixed_addition(&R, &P3, &S, sect233k1);
		  project_to_affine(&S, &P11, sect233k1);
		  mixed_addition(&R, &P5, &S, sect233k1);
		  project_to_affine(&S, &P13, sect233k1);
		  point_frobenius(&R, sect233k1);
		  mixed_addition(&R, &P7, &S, sect233k1);
		  project_to_affine(&S, &P9, sect233k1);
		  point_negation(&P9, &P9, sect233k1);
		  affine_to_project(P, &R, sect233k1);
		  point_frobenius(&R, sect233k1);
		  point_negation(&P11,&P15, sect233k1);
		  mixed_addition(&R, &P15, &S, sect233k1);
		  project_to_affine(&S, &P15, sect233k1);
		  break;
	  default: return;
	  }

	  //tau-adic expansion
	  partial_mod(k, lamda0, lamda1, curve_id);
	  TNAF5_expansion(lamda0, lamda1, a, curve_id);
	  
      //R = infinity
      XIS1(R.X, Num);
	  XIS0(R.Y, Num);
	  XIS0(R.Z, Num);

      for(i = BitLen + 3; i != 0; i--)
	  {
		  point_frobenius(&R, curve_id);

		  switch(a[i - 1]){
		  case 0:   break;
		  case 1:   mixed_addition(&R, P, &R, curve_id);
		  	        break;
		  case -1:  point_negation(P, &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 3:   mixed_addition(&R, &P3, &R, curve_id);
			        break;
		  case -3:  point_negation(&P3, &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 5:   mixed_addition(&R, &P5, &R, curve_id);
			        break;
		  case -5:  point_negation(&P5, &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 7:   mixed_addition(&R, &P7, &R, curve_id);
			        break;
		  case -7:  point_negation(&P7, &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 9:   mixed_addition(&R, &P9, &R, curve_id);
			        break;
		  case -9:  point_negation(&P9, &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 11:  mixed_addition(&R, &P11, &R, curve_id);
			        break;
		  case -11: point_negation(&P11, &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 13:  mixed_addition(&R, &P13, &R, curve_id);
			        break;
		  case -13: point_negation(&P13, &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  case 15:  mixed_addition(&R, &P15, &R, curve_id);
			        break;
		  case -15: point_negation(&P15, &T, curve_id);
			        mixed_addition(&R, &T, &R, curve_id);
			        break;
		  default:  return;                           //Error
		  }
	  }
	  project_to_affine(&R, Q, curve_id);
  }



