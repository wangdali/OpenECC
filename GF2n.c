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

/****************************************************************
 Modular Addition Function:
 ****************************************************************/
void modadd(uint32 *a, uint32 *b, uint32 *c, uint32 curve_id)
{
	uint32 i;
	uint32 Num;

	switch(curve_id){
    case sect163k1: Num = 6;
		break;
	case sect233k1: Num = 8;
		break;
	default: return;
	}

	for(i = Num - 1; i != 0; i--)
		*(c + i) = *(a + i) ^ *(b + i);
	*c = (*a) ^ (*b);
}

/**********************************************************************
 Modular Multiplication Function:
************************************************************************/
void modmul(uint32 *a, uint32 *b, uint32 *c, uint32 curve_id)
{
	uint32 P[15][N];            //store the precomputation results
	uint32 R[2*N-1] = {0x0};    //store the intermediate result
	uint32 u, i, j, k, Num;
	uint32 *p;

	p = P[0];

	switch(curve_id){
	case sect163k1: Num = 6;
		break;
	case sect233k1: Num = 8;
		break;
	default: return;
	}

	//precomputation
	for(j = Num - 1; j != 0; j--)
		*(p + j) = *(a + j);
	*p = *a;

	for(j = Num - 1; j != 0; j--)
	{
		*(p + 8 + j)  = (*(p + j) << 1) | (*(p + j - 1) >> 31);
		*(p + 16 + j) = *(p + 8 + j) ^ *(p + j);
		*(p + 24 + j) = (*(p + j) << 2) | (*(p + j - 1) >> 30);
		*(p + 32 + j) = *(p + 24 + j) ^ *(p + j);
		*(p + 40 + j) = *(p + 24 + j) ^ *(p + 8 + j);
		*(p + 48 + j) = *(p + 40 + j) ^ *(p + j);
		*(p + 56 + j) = (*(p + j) << 3) | (*(p + j - 1) >> 29);
		*(p + 64 + j) = *(p + 56 + j) ^ *(p + j);
		*(p + 72 + j) = *(p + 56 + j) ^ *(p + 8 + j);
		*(p + 80 + j) = *(p + 72 + j) ^ *(p + j);
		*(p + 88 + j) = *(p + 56 + j) ^ *(p + 24 + j);
		*(p + 96 + j) = *(p + 88 + j) ^ *(p + j);
		*(p + 104 + j) = *(p + 88 + j) ^ *(p + 8 + j);
		*(p + 112 + j) = *(p + 104 + j) ^ *(p + j);
	}
	*(p + 8)  = *p << 1;
	*(p + 16) = *(p + 8) ^ *p;
	*(p + 24) = *p << 2; 
	*(p + 32) = *(p + 24) ^ *p;
	*(p + 40) = *(p + 24) ^ *(p + 8);
	*(p + 48) = *(p + 40) ^ *p;
	*(p + 56) = *p << 3;
	*(p + 64) = *(p + 56) ^ *p;
	*(p + 72) = *(p + 56) ^ *(p + 8);
	*(p + 80) = *(p + 72) ^ *p;
	*(p + 88) = *(p + 56) ^ *(p + 24);
	*(p + 96) = *(p + 88) ^ *p;
	*(p + 104) = *(p + 88) ^ *(p + 8);
	*(p + 112) = *(p + 104) ^ *p;

	//main loop
	for(i = 7; i != 0; i--)
	{
		for(j = Num - 1; j != 0; j--)
		{
			u = (*(b + j) >> (i * 4)) & 0xf;
			if(u != 0)
			{
				u--;
				for(k = Num - 1; k != 0; k--)
					*(R + j + k) ^= *(p + 8 * u + k);
				*(R + j) ^= *(p + 8 * u); 
			}
		}
		u = ((*b) >> (i * 4)) & 0xf;
		if(u != 0)
		{
			u--;
			for(k = Num - 1; k != 0; k--)
				*(R + k) ^= *(p + 8 * u + k); 
			*R ^= *(p + 8 * u);
		}

		//R <- x^4*R
		switch(curve_id){
	    case sect163k1: 
			if(i == 1)
				*(R + 10) = *(R + 9) >> 28;
			for(j = 9; j != 0 ; j--)
				*(R + j) = (*(R + j) << 4) | (*(R + j - 1) >> 28);
			break;
		case sect233k1: 
			if((i == 2) || (i == 1))
				*(R + 14) = (*(R + 14) << 4) | (*(R + 13) >> 28);
			for(j = 13; j != 0 ; j--)
				*(R + j) = (*(R + j) << 4) | (*(R + j - 1) >> 28);
		    break;
	    default: return;
		}
		*R = *R << 4; 
	}

	//last loop
	for(j = Num - 1; j != 0; j--)
	{
		u = *(b + j) & 0xf;
		if(u != 0)
		{
			u--;
			for(k = Num - 1; k != 0; k--)
				*(R + j + k) ^= *(p + 8 * u + k);
			*(R + j) ^= *(p + 8 * u);
		}
	}
	u = (*b) & 0xf;
	if(u != 0)
	{
		u--;
		for(k = Num - 1; k != 0; k--)
			*(R + k) ^= *(p + 8 * u + k);
		*R ^= *(p + 8 * u);
	}

	//fast modular reduction, one word at a time
	switch(curve_id){
		case sect163k1: 
			*(R + 4) ^= (*(R + 10) << 29);
			*(R + 5) ^= *(R + 10) ^ (*(R + 10) << 3) ^ (*(R + 10) << 4) ^ (*(R + 10) >> 3);

			for(j = 5; j != 1; j--)
			{
				*(R + j)     ^= (*(R + j + 4) >> 29) ^ (*(R + j + 4) >> 28);
				*(R + j - 1) ^= *(R + j + 4) ^ (*(R + j + 4) << 3) ^ (*(R + j + 4) << 4) ^ (*(R + j + 4) >> 3);
				*(R + j - 2) ^= (*(R + j + 4) << 29);
			}

			u = *(R + 5) >> 3;
			*R ^= u ^ (u << 3) ^ (u << 6) ^ (u << 7);
			*(R + 1) ^= (u >> 25) ^ (u >> 26);

			*(c + 5) = *(R + 5) & 0x7;
			break;
		case sect233k1: 
			*(R + 6) ^= (*(R + 14) << 23);
			*(R + 7) ^= (*(R + 14) >> 9);
			*(R + 9) ^= (*(R + 14) << 1);

			for(j = 9; j != 3; j--)
			{
				*(R + j)     ^= (*(R + j + 4) >> 31);
				*(R + j - 1) ^= (*(R + j + 4) << 1);
				*(R + j - 3) ^= (*(R + j + 4) >> 9);
				*(R + j - 4) ^= (*(R + j + 4) << 23);
			}

			u = *(R + 7) >> 9;   
			*R ^= u;
			*(R + 2) ^= (u << 10);
			*(R + 3) ^= (u >> 22);

			*(c + 7) = *(R + 7) & 0x1ff;
			break;
		default: return;
	}

	//store the reduction result
	for(j = Num - 2; j != 0; j--)
		*(c + j) = *(R +j);
	*c = *R;
 }
 
 /****************************************************************
 Square function:
 ****************************************************************/
 void modsq(uint32 *a, uint32 *b, uint32 curve_id)
 {
	 uint32 R[2*N-1];
	 uint32 i, j, Num;
	 
     //precomputation
	 uint16 T[256] = {\
		 0,1,4,5,16,17,20,21,64,65,68,69,80,81,84,85,256,257,260,261,272,273,276,277,320,\
		 321,324,325,336,337,340,341,1024,1025,1028,1029,1040,1041,1044,1045,1088,1089,\
		 1092,1093,1104,1105,1108,1109,1280,1281,1284,1285,1296,1297,1300,1301,1344,1345,\
		 1348,1349,1360,1361,1364,1365,4096,4097,4100,4101,4112,4113,4116,4117,4160,4161,\
		 4164,4165,4176,4177,4180,4181,4352,4353,4356,4357,4368,4369,4372,4373,4416,4417,\
		 4420,4421,4432,4433,4436,4437,5120,5121,5124,5125,5136,5137,5140,5141,5184,5185,\
		 5188,5189,5200,5201,5204,5205,5376,5377,5380,5381,5392,5393,5396,5397,5440,5441,\
		 5444,5445,5456,5457,5460,5461,16384,16385,16388,16389,16400,16401,16404,16405,\
		 16448,16449,16452,16453,16464,16465,16468,16469,16640,16641,16644,16645,16656,\
		 16657,16660,16661,16704,16705,16708,16709,16720,16721,16724,16725,17408,17409,\
		 17412,17413,17424,17425,17428,17429,17472,17473,17476,17477,17488,17489,17492,\
		 17493,17664,17665,17668,17669,17680,17681,17684,17685,17728,17729,17732,17733,\
		 17744,17745,17748,17749,20480,20481,20484,20485,20496,20497,20500,20501,20544,\
		 20545,20548,20549,20560,20561,20564,20565,20736,20737,20740,20741,20752,20753,\
		 20756,20757,20800,20801,20804,20805,20816,20817,20820,20821,21504,21505,21508,\
		 21509,21520,21521,21524,21525,21568,21569,21572,21573,21584,21585,21588,21589,\
		 21760,21761,21764,21765,21776,21777,21780,21781,21824,21825,21828,21829,21840,\
		 21841,21844,21845};

	 switch(curve_id){
	 case sect163k1: 
		 Num = 6;
		 if(IF1_163(a))
		 {
			 XIS1(b, 6);
			 return;
		 }
		 break;
	 case sect233k1:
		 Num = 8;
		 if(IF1_233(a))
		 {
			 XIS1(b, 8);
			 return;
		 }
		 break;
	 default: return;
	 }
   
	 //compute R[2*Num-2]
	 for(i = Num - 2; i != 0 ; i--)
	 {
		 *(R + 2 * i)     = (uint32)*(T + (*(a + i) & 0xff))         | ((uint32)*(T + ((*(a + i) >> 8) & 0xff)) << 16);
		 *(R + 2 * i + 1) = (uint32)*(T + ((*(a + i) >> 16) & 0xff)) | ((uint32)*(T + ((*(a + i) >> 24) & 0xff)) << 16);
	 }
	 *R = (uint32)*(T + (*a & 0xff)) | ((uint32)*(T + ((*a >> 8) & 0xff)) << 16);
	 *(R + 1) = (uint32)*(T + ((*a >> 16) & 0xff)) | ((uint32)*(T + ((*a >> 24) & 0xff)) << 16);

	 //fast modular reduction, one word at a time
	 switch(curve_id){
		case sect163k1: 
			*(R + 10) = (uint32)*(T + (*(a + 5) & 0xff));

			*(R + 4) ^= (*(R + 10) << 29);
			*(R + 5) ^= *(R + 10) ^ (*(R + 10) << 3) ^ (*(R + 10) << 4) ^ (*(R + 10) >> 3);

			for(j = 5; j != 1; j--)
		    {
				*(R + j)     ^= (*(R + j + 4) >> 29) ^ (*(R + j + 4) >> 28);
			    *(R + j - 1) ^= *(R + j + 4) ^ (*(R + j + 4) << 3) ^ (*(R + j + 4) << 4) ^ (*(R + j + 4) >> 3);
			    *(R + j - 2) ^= (*(R + j + 4) << 29);
		    }

			*(R + 6) = *(R + 5) >> 3; 
			*R ^= *(R + 6) ^ (*(R + 6) << 3) ^ (*(R + 6) << 6) ^ (*(R + 6) << 7);
			*(R + 1) ^= (*(R + 6) >> 25) ^ (*(R + 6) >> 26);

			*(b + 5) = *(R + 5) & 0x7;
			break;
		case sect233k1: 
			*(R + 14) = (uint32)*(T + (*(a + 7) & 0xff)) | ((uint32)*(T + ((*(a + 7) >> 8) & 0xff)) << 16);

			*(R + 6) ^= (*(R + 14) << 23);
			*(R + 7) ^= (*(R + 14) >> 9);
			*(R + 9) ^= (*(R + 14) << 1);

			for(j = 9; j != 3; j--)
		    {
				*(R + j)     ^= (*(R + j + 4) >> 31);
			    *(R + j - 1) ^= (*(R + j + 4) << 1);
			    *(R + j - 3) ^= (*(R + j + 4) >> 9);
			    *(R + j - 4) ^= (*(R + j + 4) << 23);
		    }

			*(R + 8) = *(R + 7) >> 9;  
			*R ^= *(R + 8);
			*(R + 2) ^= (*(R + 8) << 10);
			*(R + 3) ^= (*(R + 8) >> 22);

			*(b + 7) = *(R + 7) & 0x1ff;
			break;
		default: return;
	 }

     //store the reduction result
     for(j = Num - 2; j != 0; j--)
		*(b + j) = *(R + j);
	 *b = *R; 
 }

 /****************************************************************
  Compare two big integers a and b with m*32 bits
  ***************************************************************/
sint32 compare(uint32 *a, uint32 *b, uint32 m)
 {
	 uint32 i;

	 for(i = m; i != 0; i--)
	 {
		 if(*(a + i - 1) < *(b + i - 1)) return -1;  
		 if(*(a + i - 1) > *(b + i - 1)) return 1;  
	 }
	 return 0;                                      
 }

 /****************************************************************
  Modular Inversion Function: Binary Extended Euclidean Algorithm
  ***************************************************************/
 void modinv(uint32 *a, uint32 *b, uint32 curve_id)
 {
	 uint32 g1[8] = {0x1};
	 uint32 g2[8] = {0x0};
	 uint32 u[8];
	 uint32 v[8] = {0x1};
	 uint32 i, Num, uv, t;

	 switch(curve_id){
     case sect163k1:
		 Num = 6;
		 XTOY(u, a, 6);
		 //x^163 + x^7 + x^6 + x^3 + 1
		 v[0] = 0xc9;
		 v[5] = 0x8;
         uv = (IFN1_163(u)) && (IFN1_163(v));
		 break;
	 case sect233k1:
		 Num = 8;
		 XTOY(u, a, 8);
		 //x^233 + x^74 + 1
		 v[2] = 0x400;
		 v[7] = 0x200;
         uv = (IFN1_233(u)) && (IFN1_233(v));
		 break;
	 default: return;
	 }

	 while(uv)
	 {
		 while((u[0] & 0x1) == 0) 
		 {
			 for(i = 0; i < Num - 1; i++)
				 *(u + i) = (*(u + i) >> 1) | (*(u + i + 1) << 31);
			 *(u + Num - 1) = *(u + Num - 1) >> 1;

			 if((g1[0] & 0x1) != 0)
			 {
				 switch(curve_id){
	             case sect163k1:
					 //x^163 + x^7 + x^6 + x^3 + 1
					 g1[0] ^= 0xc9;
					 g1[5] ^= 0x8;
					 break;
	             case sect233k1: 
					 //x^233 + x^74 + 1
					 g1[0] ^= 0x1;
					 g1[2] ^= 0x400;
					 g1[7] ^= 0x200;
		             break;
	             default: return;
				 }
			 }

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
			 {
				 switch(curve_id){
	             case sect163k1:
					 //x^163 + x^7 + x^6 + x^3 + 1
					 g2[0] ^= 0xc9;
					 g2[5] ^= 0x8;
		             break;
	             case sect233k1:
					 //x^233 + x^74 + 1
					 g2[0] ^= 0x1;
					 g2[2] ^= 0x400;
					 g2[7] ^= 0x200;
                     break;
	             default: return;
				 }
			 }
			 
			 for(i = 0; i < Num - 1; i++)
				 *(g2 + i) = (*(g2 + i) >> 1) | (*(g2 + i + 1) << 31);
			 *(g2 + Num - 1) = *(g2 + Num - 1) >> 1;
		 }
		 
		 if(compare(u, v, Num) == 1)
		 {
			 for(i = Num - 1; i != 0; i--)
			 {
				 *(u + i)  ^= *(v + i);
				 *(g1 + i) ^= *(g2 + i);
			 }
			 *u  ^= *v;
			 *g1 ^= *g2;
		 }
		 else
		 {
			 for(i = Num - 1; i != 0; i--)
			 {
				 *(v + i)  ^= *(u + i);
				 *(g2 + i) ^= *(g1 + i);
			 }
			 *v  ^= *u;
			 *g2 ^= *g1;
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
		 t  = IF1_163(u);
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

/****************************************************************
  Compute the halftrace over GF(2^n)
 ***************************************************************/
 void halftrace(uint32 *a, uint32 *b, uint32 curve_id)
 {
	 uint32 i, j;
	 uint32 c[8];
	 uint32 Num, BitLen;

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

	 XTOY(c, a, Num);

	 for(i = 1; i <= (BitLen - 1)/2; i++)
	 {
		 modsq(c, c, curve_id);
		 modsq(c, c, curve_id);
         for(j = Num - 1; j != 0; j--)
			 *(c + j) ^= *(a + j);
		 *c ^= *a;
	 }

	 XTOY(b, c, Num);
 }