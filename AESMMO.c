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
#include "AESMMO.h"
#include "AES128.h"

//AES MMO construction 
uint32 AES_MMO(uint8 *M, uint64 Mlen, uint8 *h)
{

	uint32 j;
	uint64 i, Mlenp;
	uint8 T[16];         //128-bit message block 
	uint8 S[16];         //128-bit message block 
	uint64 blocknum;     //number of message blocks
	uint32 lastnum;      //number of bytes in the last message block
	uint32 lastbits;     //remaining bits in the last byte
	uint8 k[16] = {0x0};


	if(Mlen == 0) return 0;

	Mlenp = Mlen + 128;

    //Process the first 128-bit message block M_0
	//Prefix-free encoding
    T[0] = (uint8)Mlen;
	T[1] = (uint8)(Mlen >> 8);
	T[2] = (uint8)(Mlen >> 16);
	T[3] = (uint8)(Mlen >> 24);
	T[4] = (uint8)(Mlen >> 32);
	T[5] = (uint8)(Mlen >> 40);
	T[6] = (uint8)(Mlen >> 48);
	T[7] = (uint8)(Mlen >> 56);
	S[0] = T[0];
	S[1] = T[1];
	S[2] = T[2];
	S[3] = T[3];
	S[4] = T[4];
	S[5] = T[5];
	S[6] = T[6];
	S[7] = T[7];
	for(j = 8; j < 16; j++)
	{
		T[j] = 0x0;
        S[j] = 0x0;
	}
	aes_block_enc(T, k);
	for(j = 0; j < 16; j++)
		k[j] = T[j] ^ S[j];

	blocknum = Mlen >> 7;
	lastnum  = (Mlen >> 3) & 0xf;
	lastbits = Mlen & 0x7;
	
	//Process a number of 128-bit blocks
	for(i = 0; i < blocknum; i++)
	{
		for(j = 0; j < 16; j++)
		{
			T[j] = *(M + 16*i + j);
			S[j] = T[j];
		}
        aes_block_enc(T, k);
		for(j = 0; j < 16; j++)
			k[j] = T[j] ^ S[j];
	}

	//Process the remaining bytes
	for(j = 0; j < lastnum; j++)
	{
		T[j] = *(M + 16*blocknum + j);
		S[j] = T[j];
	}

	//Deal with remaining bits
	if(lastbits == 0)
	{
		T[lastnum] = 0x80;
		S[lastnum] = T[lastnum];
	}
	else
	{
		T[lastnum] = (*(M + 16*blocknum + lastnum) << (8 - lastbits)) | (0x1 << (8 - lastbits - 1));
		S[lastnum] = T[lastnum];
	}

	if(Mlenp < 65536)   //MD-strengthening-1
	{
		//Deal with the last one or two blocks
		if((lastnum * 8 + lastbits) >= 112)   //two blocks
		{
			//the second last block
			for(j = lastnum + 1; j < 16; j++)
			{
				T[j] = 0x0;
				S[j] = 0x0;
			}
			aes_block_enc(T, k);
			for(j = 0; j < 16; j++)
				k[j] = T[j] ^ S[j];

			//the last block
			for(j = 0; j < 14; j++)
			{
				T[j] = 0x0;
				S[j] = 0x0;
			}
			T[14] = (uint8)(Mlenp >> 8);
			S[14] = T[14];
			T[15] = (uint8)Mlenp;
			S[15] = T[15];
			aes_block_enc(T, k);
			for(j = 0; j < 16; j++)
				k[j] = T[j] ^ S[j];
		}
		else //one block
		{
			//the last block
			for(j = lastnum + 1; j < 14; j++)
			{
				T[j] = 0x0;
				S[j] = 0x0;
			}
			T[14] = (uint8)(Mlenp >> 8);
			S[14] = T[14];
			T[15] = (uint8)Mlenp;
			S[15] = T[15];
			aes_block_enc(T, k);
			for(j = 0; j < 16; j++)
				k[j] = T[j] ^ S[j];
		}
	}
	else if(Mlenp >= 65536 && Mlenp < 4294967296)   //MD-strengthening-2
	{
		//Deal with the last one or two blocks
		if((lastnum * 8 + lastbits) >= 80)   //two blocks
		{
			//the second last block
			for(j = lastnum + 1; j < 16; j++)
			{
				T[j] = 0x0;
				S[j] = 0x0;
			}
			aes_block_enc(T, k);
			for(j = 0; j < 16; j++)
				k[j] = T[j] ^ S[j];

			//the last block
			for(j = 0; j < 12; j++)
			{
				T[j] = 0x0;
				S[j] = 0x0;
			}
			T[12] = (uint8)(Mlenp >> 24);
			S[12] = T[12];
			T[13] = (uint8)(Mlenp >> 16);
			S[13] = T[13];
			T[14] = (uint8)(Mlenp >> 8);
			S[14] = T[14];
			T[15] = (uint8)Mlenp;
			S[15] = T[15];
			aes_block_enc(T, k);
			for(j = 0; j < 16; j++)
				k[j] = T[j] ^ S[j];
		}
		else //one block
		{
			//the last block
			for(j = lastnum + 1; j < 12; j++)
            {
				T[j] = 0x0;
				S[j] = 0x0;
			}
			T[12] = (uint8)(Mlenp >> 24);
			S[12] = T[12];
			T[13] = (uint8)(Mlenp >> 16);
			S[13] = T[13];
			T[14] = (uint8)(Mlenp >> 8);
			S[14] = T[14];
			T[15] = (uint8)Mlenp;			
			S[15] = T[15];
			aes_block_enc(T, k);
			for(j = 0; j < 16; j++)
				k[j] = T[j] ^ S[j];
		}
	}
	else  //MD-strengthening-3
	{
		//Deal with the last one or two blocks
		if((lastnum * 8 + lastbits) >= 16)   //two blocks
		{
			//the second last block
			for(j = lastnum + 1; j < 16; j++)
			{
				T[j] = 0x0;
				S[j] = 0x0;
			}
			aes_block_enc(T, k);
			for(j = 0; j < 16; j++)
				k[j] = T[j] ^ S[j];

			//the last block
			for(j = 0; j < 8; j++)
			{
				T[j] = 0x0;
				S[j] = 0x0;
			}
			T[8]  = (uint8)(Mlen >> 56);
			S[8] = T[8];
			T[9]  = (uint8)(Mlen >> 48);
			S[9] = T[9];
			T[10] = (uint8)(Mlen >> 40);
			S[10] = T[10];
			T[11] = (uint8)(Mlen >> 32);
			S[11] = T[11];
			T[12] = (uint8)(Mlen >> 24);
			S[12] = T[12];
			T[13] = (uint8)(Mlen >> 16);
			S[13] = T[13];
			T[14] = (uint8)(Mlen >> 8);
			S[14] = T[14];
			T[15] = (uint8)Mlen;
			S[15] = T[15];
			aes_block_enc(T, k);
			for(j = 0; j < 16; j++)
				k[j] = T[j] ^ S[j];
		}
		else //one block
		{
			//the last block
			for(j = lastnum + 1; j < 8; j++)
			{
				T[j] = 0x0;
				S[j] = 0x0;
			}
			T[8]  = (uint8)(Mlen >> 56);
			S[8] = T[8];
			T[9]  = (uint8)(Mlen >> 48);
			S[9] = T[9];
			T[10] = (uint8)(Mlen >> 40);
			S[10] = T[10];
			T[11] = (uint8)(Mlen >> 32);
			S[11] = T[11];
			T[12] = (uint8)(Mlen >> 24);
			S[12] = T[12];
			T[13] = (uint8)(Mlen >> 16);
			S[13] = T[13];
			T[14] = (uint8)(Mlen >> 8);
			S[14] = T[14];
			T[15] = (uint8)Mlen;
			S[15] = T[15];
			aes_block_enc(T, k);
			for(j = 0; j < 16; j++)
				k[j] = T[j] ^ S[j];
		}
	}

	for(j = 0; j < 16; j++)
		*(h + j) = *(k + j);

    /*for(j = 0; j < 4; j++)
	{
		*(h + j) = ((uint32)k[4*j] << 24) | ((uint32)k[4*j+1] << 16) |  ((uint32)k[4*j+2] << 8) |  ((uint32)k[4*j+3]);
		*(h + j + 4) = 0x0;
	}*/
	
	return 1;
}