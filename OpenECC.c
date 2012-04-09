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
#include "AES128.h"
#include "AESMMO.h"
#include "GFp.h"

/*********************************************************************
  Public Key Validation Primitive
  ********************************************************************/
uint32 pk_validation(ec_point_aff *Q, uint32 curve_id)
{
	uint32 S[N], T[N];

	switch(curve_id){
	case sect163k1: 
		//Q is infinity
		if(IF0_163(Q->x) && IF0_163(Q->y)) return 0;

		//Check the degree of x_Q and y_Q
		if(((Q->x[5] >> 3) != 0x0) || ((Q->y[5] >> 3) != 0x0)) return 0;
		break;
	case sect233k1:
		//Q is infinity
		if(IF0_233(Q->x) && IF0_233(Q->y)) return 0;

		//Check the degree of x_Q and y_Q
		if(((Q->x[7] >> 9) != 0x0) || ((Q->y[7] >> 9) != 0x0)) return 0;
		break;
	default: return 0;
	}

	//Compute x_Q^3 + 1
	modsq(Q->x, S, curve_id);
	modmul(Q->x, S, T, curve_id);
	if(curve_id == sect163k1)
		modadd(T, S, T, curve_id);
	T[0] = T[0] ^ 0x1;

	//Compute y_Q^2 + x_Qy_Q
	modsq(Q->y, S, curve_id);
	modadd(T, S, T, curve_id);
	modmul(Q->x, Q->y, S, curve_id);
	modadd(T, S, T, curve_id);

	switch(curve_id){
	case sect163k1:
		if(IFN0_163(T)) return 0;

		//Check that 2Q is not infinity
		if(IF0_163(Q->x))       // Q is a 2-torsion point
			return 0; 
		break;
	case sect233k1:
		if(IFN0_233(T)) return 0;

		//Check that 2Q or 4Q is not infinity
		if(IF0_233(Q->x) || IF1_233(Q->x))  // Q is a 2- or 4-torsion point
			return 0;  
		break;
	default: return 0;
	}

	return 1;
}

/*********************************************************************
  Key Generation Primitive
  ********************************************************************/
void key_generation(ec_key_pair *key, uint8 *rnd, uint32 curve_id)
{
	uint32 c[N];
	uint32 i, Num;
	ec_point_aff Q;

	switch(curve_id){
	case sect163k1:
		Num = 6;

		*(c + 5) = (uint32)(*(rnd + 20)) & 0x7;
		break;
	case sect233k1: 
		Num = 8;

		*(c + 7) = (*(c + 7) << 8) | (uint32)(*(rnd + 29));
		*(c + 7) = (*(c + 7) << 8) | (uint32)(*(rnd + 28));
		*(c + 7) = *(c + 7) & 0xff;
		break;
	default: return;
	}

	for(i = 0; i < Num - 1; i++)
	{
		*(c + i) = (uint32)(*(rnd + 4*i + 3));
		*(c + i) = (*(c + i) << 8) | (uint32)(*(rnd + 4*i + 2));
		*(c + i) = (*(c + i) << 8) | (uint32)(*(rnd + 4*i + 1));
		*(c + i) = (*(c + i) << 8) | (uint32)(*(rnd + 4*i));
	}

    if(compare(c, eccn[curve_id], Num) != -1)
		sub(c, eccn[curve_id], c, Num);

	XTOY(key->d, c, Num);
	TNAF5_fixed_scalarmul(c, &Q, curve_id);
	point_compression(&Q, key->Qx, curve_id);
}

/*********************************************************************
  ECQV certificate request
  ********************************************************************/
void ECQV_request(ec_key_pair *key, uint8 *rnd, uint32 curve_id)
{
	key_generation(key, rnd, curve_id);
}

/****************************************************************************
  ECQV public key extraction
  ***************************************************************************/
uint32 ECQV_extraction(uint32 *QCAx, uint8 *CertA, uint32 *QAx, uint32 curve_id)
{
	uint8 s[16];
	uint32 i;
	uint32 e[N], BAx[N];
	ec_point_aff BA, QCA, QA;
	ec_point_pro P;

	if((*CertA != 0x2) && (*CertA != 0x3)) return 0;

	//Convert BAS to a public key BA
	switch(curve_id){
	case sect163k1:
		*(BAx + 5) = ((uint32)*CertA << 8)| ((uint32)*(CertA + 1));
		for(i = 0; i < 5; i++)
			*(BAx + (4 - i)) = ((uint32)*(CertA+4*i+2) << 24) | ((uint32)*(CertA+4*i+3) << 16) | ((uint32)*(CertA+4*i+4) << 8) | ((uint32)*(CertA+4*i+5));

		//Compute hash value of the certificate
		AES_MMO(CertA, 384, s);
		break;
	case sect233k1:
		*(BAx + 7) = ((uint32)*CertA << 16)| ((uint32)*(CertA + 1) << 8) | ((uint32)*(CertA + 2));
		for(i = 0; i < 7; i++)
			*(BAx + (6 - i)) = ((uint32)*(CertA+4*i+3) << 24) | ((uint32)*(CertA+4*i+4) << 16) | ((uint32)*(CertA+4*i+5) << 8) | ((uint32)*(CertA+4*i+6));

		//Compute hash value of the certificate
		AES_MMO(CertA, 456, s);
		break;
	default: return 0;
	}

	if(point_decompression(BAx, &BA, curve_id) == 0) return 0;

	//Validate the public key BA
	if(pk_validation(&BA, curve_id) == 0) return 0;

	for(i = 0; i < 4; i++)
	{
		*(e + i) = ((uint32)*(s+4*i) << 24) | ((uint32)*(s+4*i+1) << 16) |  ((uint32)*(s+4*i+1) << 8) |  ((uint32)*(s+4*i+3));
		*(e + i + 4) = 0x0;
	}

	//Compute QA = e*BA + QCA
	if(point_decompression(QCAx, &QCA, curve_id) == 0) return 0;
	TNAF5_random_scalarmul(e, &BA, &QA, curve_id);
	affine_to_project(&QA, &P, curve_id);
	mixed_addition(&P, &QCA, &P, curve_id);
	project_to_affine(&P, &QA, curve_id);
	point_compression(&QA, QAx, curve_id);

	return 1;
}

/**************************************************************************************
  ECQV certificate validation
  *************************************************************************************/
uint32 ECQV_validation(uint32 *QCAx, uint32 *RAx, uint8 *CertA, uint32 *r, uint32 curve_id)
{
	uint8 s[16];
	uint32 i, t;
	uint32 e[N], BAx[N];
	ec_point_aff BA, QCA, RA, R;
	ec_point_pro P;

	if(point_decompression(RAx, &RA, curve_id) == 0) return 0;

	//Validate the public key RA
	if(pk_validation(&RA, curve_id) == 0) return 0;

	if((*CertA != 0x2) && (*CertA != 0x3)) return 0;

	//Convert BAS to a public key BA
	switch(curve_id){
	case sect163k1:
		*(BAx + 5) = ((uint32)*CertA << 8)| ((uint32)*(CertA + 1));
		for(i = 0; i < 5; i++)
			*(BAx + (4 - i)) = ((uint32)*(CertA+4*i+2) << 24) | ((uint32)*(CertA+4*i+3) << 16) | ((uint32)*(CertA+4*i+4) << 8) | ((uint32)*(CertA+4*i+5));

		//Compute hash value of the certificate
		AES_MMO(CertA, 384, s);
		break;
	case sect233k1:
		*(BAx + 7) = ((uint32)*CertA << 16)| ((uint32)*(CertA + 1) << 8) | ((uint32)*(CertA + 2));
		for(i = 0; i < 7; i++)
			*(BAx + (6 - i)) = ((uint32)*(CertA+4*i+3) << 24) | ((uint32)*(CertA+4*i+4) << 16) | ((uint32)*(CertA+4*i+5) << 8) | ((uint32)*(CertA+4*i+6));

		//Compute hash value of the certificate
		AES_MMO(CertA, 456, s);
		break;
	default: return 0;
	}

	if(point_decompression(BAx, &BA, curve_id) == 0) return 0;

	//Validate the public key BA
	if(pk_validation(&BA, curve_id) == 0) return 0;

	for(i = 0; i < 4; i++)
	{
		*(e + i) = ((uint32)*(s+4*i) << 24) | ((uint32)*(s+4*i+1) << 16) |  ((uint32)*(s+4*i+1) << 8) |  ((uint32)*(s+4*i+3));
		*(e + i + 4) = 0x0;
	}

	//Compute e*(BA - RA)+ QCA
	point_negation(&RA, &R, curve_id);
	affine_to_project(&BA, &P, curve_id);
	mixed_addition(&P, &R, &P, curve_id);
	project_to_affine(&P, &BA, curve_id);
	TNAF5_random_scalarmul(e, &BA, &R, curve_id);
	affine_to_project(&R, &P, curve_id);
	point_decompression(QCAx, &QCA, curve_id);
	mixed_addition(&P, &QCA, &P, curve_id);
	project_to_affine(&P, &BA, curve_id);

	//Compute r*G
	TNAF5_fixed_scalarmul(r, &R, curve_id);

	switch(curve_id){
	case sect163k1:
		t = (IFXeqY_163(BA.x, R.x)) && (IFXeqY_163(BA.y, R.y));
		break;
	case sect233k1:
		t = (IFXeqY_233(BA.x, R.x)) && (IFXeqY_233(BA.y, R.y));
		break;
	default: return 0;
	}

	if(t) return 1;
	else return 0;
}

/*********************************************************************************************************************
  ECMQV key agreement
  ********************************************************************************************************************/
uint32 ECMQV(ec_key_pair *keyA1, ec_key_pair *keyA2, uint32 *QB1x, uint32 *QB2x, uint32 len, uint8 *skey, uint32 curve_id)
{
	uint32 QA2bar[N], QB2bar[N];
	uint32 i, s[N];
	uint32 t, Num;
	ec_point_aff P, QB1, QB2;
	ec_point_pro Q;
	uint8 Z[30];

	switch(curve_id){
	case sect163k1:
		Num = 6;
		t = 3;        //t = Num/2

		//Compute integers QA2bar and QB2bar from QA2 and QB2
		*(QA2bar + 2) = (*(keyA2->Qx + 2) & 0x1ffff) | 0x20000;
		*(QB2bar + 2) = (*(QB2x + 2) & 0x1ffff) | 0x20000;
		break;
	case sect233k1:
		Num = 8;
		t = 4;        //t = Num/2

		//Compute integers QA2bar and QB2bar from QA2 and QB2
		*(QA2bar + 3) = (*(keyA2->Qx + 3) & 0xfffff) | 0x100000;
		*(QB2bar + 3) = (*(QB2x + 3) & 0xfffff) | 0x100000;
		break;
	default: return 0;
	}

	//Compute integers QA2bar and QB2bar from QA2 and QB2
	for(i = Num - 1; i != (t - 1); i--)
	{
		*(QA2bar + i) = 0;
		*(QB2bar + i) = 0;
	}

	for(i = t - 2; i != 0; i--)
	{
		*(QA2bar + i) = *(keyA2->Qx + i);
		*(QB2bar + i) = *(QB2x + i);
	}
	*QA2bar = *keyA2->Qx;
	*QB2bar = *QB2x;

	//Compute an integer s = dA2 + QA2bar*dA1 (mod n)
	if(curve_id == sect233k1)
		modpmul128(QA2bar, keyA1->d, s, curve_id);
	else
		modpmul96(QA2bar, keyA1->d, s);
	modpadd(s, keyA2->d, s, curve_id);
	
	//Compute 4*s
	switch(curve_id){
	case sect163k1:
		for(i = 5; i != 0; i--)
			*(s + i) = (*(s + i) << 1) | (*(s + (i - 1)) >> 31);
		*s = *s << 1;
		break;
	case sect233k1:
		for(i = 7; i != 0; i--)
			*(s + i) = (*(s + i) << 2) | (*(s + (i - 1)) >> 30);
		*s = *s << 2;
		break;
	default: return 0;
	}

	while(compare(s, eccn[curve_id], Num) != -1)  
		sub(s, eccn[curve_id], s, Num);

	//Compute 4*s*(QB2 + QB2bar*QB1)
	if(point_decompression(QB1x, &QB1, curve_id) == 0) return 0;
	if(point_decompression(QB2x, &QB2, curve_id) == 0) return 0;
	TNAF5_random_scalarmul(QB2bar, &QB1, &P, curve_id);
	affine_to_project(&QB2, &Q, curve_id);
	mixed_addition(&Q, &P, &Q, curve_id);
	project_to_affine(&Q, &P, curve_id);
	TNAF5_random_scalarmul(s, &P, &P, curve_id);

	//If P is infinity, return error
	switch(curve_id){
	case sect163k1:
		if(IF0_163(P.x) && IF0_163(P.y)) return 0;

		*Z = (uint8)(*(P.x + 5));
		for(i = 1; i < 6; i++)
		{
			//x coordinate
			*(Z+4*(i-1)+1) = (uint8)(*(P.x + (5 - i)) >> 24);
			*(Z+4*(i-1)+2) = (uint8)(*(P.x + (5 - i)) >> 16);
			*(Z+4*(i-1)+3) = (uint8)(*(P.x + (5 - i)) >> 8);
			*(Z+4*(i-1)+4) = (uint8)(*(P.x + (5 - i)));
		}
		break;
	case sect233k1:
		if(IF0_233(P.x) && IF0_233(P.y)) return 0;

		*Z = (uint8)(*(P.x + 7) >> 8);
		*(Z + 1) = (uint8)*(P.x + 7);
		for(i = 1; i < 8; i++)
		{
			//x coordinate
			*(Z+4*(i-1)+2) = (uint8)(*(P.x + (7 - i)) >> 24);
			*(Z+4*(i-1)+3) = (uint8)(*(P.x + (7 - i)) >> 16);
			*(Z+4*(i-1)+4) = (uint8)(*(P.x + (7 - i)) >> 8);
			*(Z+4*(i-1)+5) = (uint8)(*(P.x + (7 - i)));
		}
		break;
	default: return 0;
	}

	//Get len bytes from the result as the shared key
	for(i = 0; i < len; i++)
		*(skey + i) = *(Z + i);

	return 1;
}