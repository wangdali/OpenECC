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
#include "GF2n.h"
#include "GFp.h"

//For the curve sect163k1 and sect233k1
uint32 eccn[2][8] = {{0x99F8A5EFUL,0xA2E0CC0DUL,0x20108UL,0x0UL,0x0UL,0x4UL,0x0UL,0x0UL},
                     {0xF173ABDFUL,0x6EFB1AD5UL,0xB915BCD4UL,0x69D5BUL,0x0UL,0x0UL,0x0UL,0x80UL}};
uint32 eccu[2][9] = {{0x4E1DF22EUL,0x20108E74UL,0x266075A1UL,0x75D1F33FUL,0xFFFFDFEFUL,0xFFFFFFFFUL,0x3FFFFFFFUL,0x0UL,0x0UL},
                     {0x327734F6UL,0x50855E08UL,0x94A83A31UL,0x0CAE4413UL,0x8A911BA9UL,0xFFFFFFE5UL,0xFFFFFFFFUL,0xFFFFFFFFUL,0x1FFFFFFUL}};

/**************************************************************************************
 add(.): Multi-precision addition
 **************************************************************************************/
uint32 add(uint32 *a, uint32 *b, uint32 *aplusb, uint32 curve_id)
{
	uint32 i, carry, Num;
	uint64 T;

	switch(curve_id){
	case sect163k1: Num = 6;
		break;
	case sect233k1: Num = 8;
		break;
	default: return 0;
	}
	
    carry = 0; 
	for(i = 0; i < Num; i++)
	{
		T = (uint64)*(a + i) + *(b + i) + carry;
		*(aplusb + i) = (uint32)T;
		carry = (uint32)(T >> 32);
	}
	if(carry == 1) return 1;
	else return 0;
}

/**************************************************************************************
 sub(.): Multi-precision subtraction
 **************************************************************************************/
uint32 sub(uint32 *a, uint32 *b, uint32 *asubb, uint32 m)
{
	uint32 i, borrow;
	uint64 T;

    borrow = 1; 
	for(i = 0; i < m; i++)
	{
		T = (uint64)*(a + i) + ~*(b + i) + borrow;
		*(asubb + i) = (uint32)T;
		borrow = (uint32)(T >> 32);
	}
	if(borrow == 0) return 1;     
	else return 0;
}

/**************************************************************************************
modadd(.): Modular multi-precision addition
**************************************************************************************/
void modpadd(uint32 *a, uint32 *b, uint32 *aplusb, uint32 curve_id)
{
	uint32 carry, Num;

	switch(curve_id){
	case sect163k1: Num = 6;
		break;
	case sect233k1: Num = 8;
		break;
	default: return;
	}

	carry = add(a, b, aplusb, curve_id);

	if(carry == 1)                              
	{
		sub(aplusb, eccn[curve_id], aplusb, Num);
		return;
	}
	else                                        
	{
		if(compare(aplusb, eccn[curve_id], Num) != -1)    
			sub(aplusb, eccn[curve_id], aplusb, Num);
	}
}

/**************************************************************************************
 modsub(.): Modular multi-precision subtraction
 **************************************************************************************/
void modpsub(uint32 *a, uint32 *b, uint32 *asubb, uint32 curve_id)
{
	uint32 i, carry;
	uint32 Num;

	switch(curve_id){
	case sect163k1: Num = 6;
		break;
	case sect233k1: Num = 8;
		break;
	default: return;
	}
	
	carry = 0;
	for(i = 0;i < Num; i++)                                              
	{
		if(*(a + i) >= *(b + i))
		{
			*(asubb + i) = *(a + i) - *(b + i);
			if((*(asubb + i) == 0x0) && (carry == 0x1)) 
				*(asubb + i) = 0xffffffff;
			else 
			{
				*(asubb + i) -= carry;
			    carry=0x0;
			}
		}
		else
		{
			*(asubb + i) = *(a + i) + ~*(b + i) + 0x1 - carry;
			carry = 0x1;
		}
	}

	if(carry == 0x1)
		add(asubb, eccn[curve_id], asubb, curve_id);
}

/**************************************************************************************
 modpmul128(.): Modular multiplication over GF(p) with one operand 128-bit
 **************************************************************************************/
void modpmul128(uint32 *a, uint32 *b, uint32 *amulb, uint32 curve_id)
{
	uint32 i, j, Num;
	uint32 c[N+4], p[N+1], q[N+1];
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

	for(i = 1; i < Num + 2; i++)
	{
		T3 = 0;
		for(j = (i < Num ? 0 : (i - Num + 1)); j <= (i < 4 ? i : 3); j++)
		{
			T2 = (uint64)(*(a + j)) * (*(b + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(c + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	T2 = (uint64)(*(a + 3)) * (*(b + Num - 1));
	T1 += (uint32)T2;
	*(c + Num + 2) = (uint32)T1;
	*(c + Num + 3) = (uint32)(T1 >> 32) + (uint32)(T2 >> 32);

	//Barrett Modular Reduction 
	T1 = (uint64)(*(c + Num - 1)) * (*(eccu[curve_id]));
	T1 = T1 >> 32;

	for(i = Num; i < Num * 2; i++)
	{
		T3 = 0;
		for(j = Num - 1; j <= (i < (Num + 4) ? i : (Num + 3)); j++)
		{
			T2 = (uint64)(*(c + j)) * (*(eccu[curve_id] + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		T1 = (T1 >> 32) + T3;
	}

	for(i = 0; i < 3; i++)
	{
		T3 = 0;
		for(j = i + Num; j <= Num + 3; j++)
		{
			T2 = (uint64)(*(c + j)) * (*(eccu[curve_id] + (Num * 2 + i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(p + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	T2 = (uint64)*(c + Num + 3) * (*(eccu[curve_id] + Num));
	T1 += (uint32)T2;
	*(p + 3) = (uint32)T1;
	*(p + 4) = (uint32)(T1 >> 32) + (uint32)(T2 >> 32);

	//q = p * eccn
	T1 = (uint64)(*p) * (*(eccn[curve_id]));
	*q = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < Num - 1; i++)
	{
		T3 = 0;
		for(j = (i < (Num/2) ? 0 : (i - Num/2 + 1)); j <= (i < 5 ? i : 4); j++)
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
	switch(curve_id){
	case sect163k1:
		T2 = (uint64)(*(p + 3)) * (*(eccn[0] + 2));
		T1 += (uint32)T2;
		T3 += (T2 >> 32);
		T2 = (uint64)(*(p + 4)) * (*(eccn[0] + 1));
		T1 += (uint32)T2;
		T3 += (T2 >> 32);
		break;
	case sect233k1:
		T2 = (uint64)(*(p + 4)) * (*(eccn[0] + 11));
		T1 += (uint32)T2;
		T3 += (T2 >> 32);
		break;
	default: return;
	}
	*(q + Num - 1) = (uint32)T1;
	T1 = (T1 >> 32) + T3;

	T2 = (uint64)*(p + 1) * (*(eccn[curve_id] + (Num - 1)));
	T1 += (uint32)T2;
	if(curve_id == sect163k1)
	{
		T2 = (uint64)*(p + 4) * (*(eccn[0] + 2));
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
 modpmul96(.): Modular multiplication over GF(p) with one operand 96-bit
 This function is only used for the curve sect163k1
 **************************************************************************************/
void modpmul96(uint32 *a, uint32 *b, uint32 *amulb)
{
	uint32 i, j;
	uint32 c[8], p[7], q[7];
	uint64 T1, T2, T3;

	//c = a * b
	T1 = (uint64)(*a) * (*b);
	*c = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < 7; i++)
	{
		T3 = 0;
		for(j = (i < 6 ? 0 : (i - 5)); j <= (i < 3 ? i : 2); j++)
		{
			T2 = (uint64)(*(a + j)) * (*(b + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(c + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	*(c + 7) = (uint32)T1 + (*(a + 2)) * (*(b + 5));

	//Barrett Modular Reduction 
	T1 = (uint64)*(c + 5) * (*eccu[0]);
	T1 = T1 >> 32;

	for(i = 6; i < 12; i++)
	{
		T3 = 0;
		for(j = 5; j <= (i < 8 ? i : 7); j++)
		{
			T2 = (uint64)*(c + j) * (*(eccu[0] + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		T1 = (T1 >> 32) + T3;
	}

	T2 = (uint64)*(c + 6) * (*(eccu[0] + 6));
	T1 += (uint32)T2;
	T3 = (T2 >> 32);
	T2 = (uint64)*(c + 7) * (*(eccu[0] + 5));
	T1 += (uint32)T2;
	T3 += (T2 >> 32);
	*p = (uint32)T1;
	T1 = (T1 >> 32) + T3;

	T2 = (uint64)*(c + 7) * (*(eccu[0] + 6));
	T1 += (uint32)T2;
	*(p + 1) = (uint32)T1;
	*(p + 2) = (uint32)(T1 >> 32) + (uint32)(T2 >> 32);

	//q = p * eccn
	T1 = (uint64)(*p) * (*eccn[0]);
	*q = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < 5; i++)
	{
		T3 = 0;
		for(j = (i < 3 ? 0 : (i - 2)); j <= (i < 3 ? i : 2); j++)
		{
			T2 = (uint64)*(p + j) * (*(eccn[0] + (i - j)));
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		*(q + i) = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	T2 = (uint64)(*p) * (*(eccn[0] + 5));
	T1 += (uint32)T2;
	*(q + 5) = (uint32)T1;
	T1 = (T1 >> 32) + (T2 >> 32);

	*(q + 6) = (uint32)T1 + (*(p + 1)) * (*(eccn[0] + 5));

	for(i = 5; i != 0; i--)  
		*(p + i) = *(eccn[0] + i);
	*p = *eccn[0];
	*(p + 6) = 0;

	if(sub(c, q, q, 7) == 1)                 
	{
		for(i = 5; i != 0; i--)  
			*(amulb + i) = *(q + i);
		*amulb = *q;
		return;
	}

	while(compare(q, p, 7) != -1)  
		sub(q, p, q, 7);
	for(i = 5; i != 0; i--)  
		*(amulb + i) = *(q + i);
	*amulb = *q;
}




















