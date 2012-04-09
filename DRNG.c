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
#include "DRNG.h"
#include "AES128.h"

ctr_drbg rnd;             //a global random number generator

//Initialize the DRNG
void ctr_init(uint8 *seed)
{
	rnd.K[0]  =     (*seed)  ^ 0x58;
	rnd.K[1]  = *(seed + 1)  ^ 0xE2;
	rnd.K[2]  = *(seed + 2)  ^ 0xFC;
	rnd.K[3]  = *(seed + 3)  ^ 0xCE;
	rnd.K[4]  = *(seed + 4)  ^ 0xFA;
	rnd.K[5]  = *(seed + 5)  ^ 0x7E;
	rnd.K[6]  = *(seed + 6)  ^ 0x30;
	rnd.K[7]  = *(seed + 7)  ^ 0x61;
	rnd.K[8]  = *(seed + 8)  ^ 0x36;
	rnd.K[9]  = *(seed + 9)  ^ 0x7F;
	rnd.K[10] = *(seed + 10) ^ 0x1D;
	rnd.K[11] = *(seed + 11) ^ 0x57;
	rnd.K[12] = *(seed + 12) ^ 0xA4;
	rnd.K[13] = *(seed + 13) ^ 0xE7;
	rnd.K[14] = *(seed + 14) ^ 0x45;
	rnd.K[15] = *(seed + 15) ^ 0x5A;

	rnd.V[0]  = *(seed + 16) ^ 0x03;
	rnd.V[1]  = *(seed + 17) ^ 0x88;
	rnd.V[2]  = *(seed + 18) ^ 0xDA;
	rnd.V[3]  = *(seed + 19) ^ 0xCE;
	rnd.V[4]  = *(seed + 20) ^ 0x60;
	rnd.V[5]  = *(seed + 21) ^ 0xB6;
	rnd.V[6]  = *(seed + 22) ^ 0xA3;
	rnd.V[7]  = *(seed + 23) ^ 0x92;
	rnd.V[8]  = *(seed + 24) ^ 0xF3;
	rnd.V[9]  = *(seed + 25) ^ 0x28;
	rnd.V[10] = *(seed + 26) ^ 0xC2;
	rnd.V[11] = *(seed + 27) ^ 0xB9;
	rnd.V[12] = *(seed + 28) ^ 0x71;
	rnd.V[13] = *(seed + 29) ^ 0xB2;
	rnd.V[14] = *(seed + 30) ^ 0xFE;
	rnd.V[15] = *(seed + 31) ^ 0x78;

	rnd.counter = 1;
}

//Update the DRNG
void ctr_update()
{
	uint8 temp1[16];
	uint8 temp2[16];
	uint32 i,t,c;

	t = (uint32)(rnd.V[0]) + 0x1;
	temp1[0] = (uint8)(t & 0xff);
	c = t >> 8;
	i = 1;
	//V+1 (mod 2^128)
	while(c != 0 && i < 16)
	{
		t = (uint32)(rnd.V[i]) + c;
		temp1[i] = (uint8)(t & 0xff);
		c = t >> 8;
		i++;
	}

	t = (uint32)(rnd.V[0]) + 0x2;
	temp2[0] = (uint8)(t & 0xff);
	c = t >> 8;
	i = 1;
	//V+2 (mod 2^128)
	while(c != 0 && i < 16)
	{
		t = (uint32)(rnd.V[i]) + c;
		temp2[i] = (uint8)(t & 0xff);
		c = t >> 8;
		i++;
	}

	aes_block_enc(temp1, rnd.K);
	aes_block_enc(temp2, rnd.K);

	//updated key K
	rnd.K[0]  = temp1[0];
	rnd.K[1]  = temp1[1];
	rnd.K[2]  = temp1[2];
	rnd.K[3]  = temp1[3];
	rnd.K[4]  = temp1[4];
	rnd.K[5]  = temp1[5];
	rnd.K[6]  = temp1[6];
	rnd.K[7]  = temp1[7];
	rnd.K[8]  = temp1[8];
	rnd.K[9]  = temp1[9];
	rnd.K[10] = temp1[10];
	rnd.K[11] = temp1[11];
	rnd.K[12] = temp1[12];
	rnd.K[13] = temp1[13];
	rnd.K[14] = temp1[14];
	rnd.K[15] = temp1[15];

	//updated state V
	rnd.V[0]  = temp2[0];
	rnd.V[1]  = temp2[1];
	rnd.V[2]  = temp2[2];
	rnd.V[3]  = temp2[3];
	rnd.V[4]  = temp2[4];
	rnd.V[5]  = temp2[5];
	rnd.V[6]  = temp2[6];
	rnd.V[7]  = temp2[7];
	rnd.V[8]  = temp2[8];
	rnd.V[9]  = temp2[9];
	rnd.V[10] = temp2[10];
	rnd.V[11] = temp2[11];
	rnd.V[12] = temp2[12];
	rnd.V[13] = temp2[13];
	rnd.V[14] = temp2[14];
	rnd.V[15] = temp2[15];
}

//Generate a random number and update the internal state
uint32 ctr_generate(uint32 rlen, uint8 *rndnum)
{
	uint8  temp[16];
	uint32 i,j,t,c; 
	uint32 blocknum;     //number of AES-128 calls
	uint32 lastnum;      //number of bytes in the last random number block
	uint32 lastbits;     //remaining bits in the last byte

	if(rnd.counter > 281474976710656) return 0;   //If counter > 2^48, return ERROR
	if(rlen > 65536 || rlen == 0) return 0;       //If rlen > 2^16, return ERROR
     
	blocknum = rlen >> 7;
	lastnum  = (rlen >> 3) & 0xf;
	lastbits = rlen & 0x7;

    for(i = 0; i < blocknum; i++)
	{
		t = (uint32)(rnd.V[0]) + 0x1;
	    temp[0] = (uint8)(t & 0xff);
		rnd.V[0] = temp[0];
		c = t >> 8;
		j = 1;
		//V+1 (mod 2^128)
		while(c != 0 && j < 16)
		{
			t = (uint32)(rnd.V[j]) + c;
			temp[j] = (uint8)(t & 0xff);
			rnd.V[j] = temp[j];
			c = t >> 8;
			j++;
		}
		aes_block_enc(temp,rnd.K);
		for(j = 0; j < 16; j++)
			*(rndnum + i*16 + j) = temp[j];
	}

    if(lastnum != 0 || lastbits != 0)    //need one more AES-128 call
	{
		t = (uint32)(rnd.V[0]) + 0x1;
		temp[0] = (uint8)(t & 0xff);
		rnd.V[0] = temp[0];
		c = t >> 8;
		j = 1;
		//V+1 (mod 2^128)
		while(c != 0 && j < 16)
		{
			t = (uint32)(rnd.V[j]) + c;
			temp[j] = (uint8)(t & 0xff);
			rnd.V[j] = temp[j];
			c = t >> 8;
			j++;
		}
		aes_block_enc(temp,rnd.K);
		for(j = 0; j < lastnum; j++)
			*(rndnum + blocknum*16 + j) = temp[j];
		if(lastbits != 0)
		{
			 *(rndnum + blocknum*16 + lastnum) = (temp[lastnum] >> (8 - lastbits));
		}
	}
    	
	//update the DRNG
    ctr_update();
	rnd.counter++;

	return 1;  //SUCCESS
}