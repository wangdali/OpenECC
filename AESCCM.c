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
#include "AESCCM.h"
#include "AES128.h"

/* The implementation of the AES-CCM* should combine the input tranformation
 * with the authentication transformation, since the Cortex-M3 does not have 
 * the enough RAM to store the long message.
 */
void ccm_auth_only(uint8 *m, uint8 *a, uint8 *nonce, ccm_parameter param, uint8 *tag)
{
	uint8 lm[8];         //byte representation of message length
	uint8 la[8];         //byte representation of associate data length
	uint8 T[16];         //128-bit message block and AES-CBC-MAC result         
    uint64 i;
	uint32 j;
	uint64 blocknum;     //number of message blocks
	uint32 lastnum;      //number of bytes in the last message block
	uint32 lastbits;     //remaining bits in the last byte
	uint64 adatanum;     //number of associate data blocks
	uint32 alastnum;     //number of bytes in the last associate data block 
	uint32 alastbits;    //remaining bits in the last byte of the associate data

	blocknum = param.msg_len >> 7;
	lastnum  = (param.msg_len >>3) & 0xf;
	lastbits = param.msg_len & 0x7;

	for(j = 0; j < 8; j++)
		lm[j] = (uint8)(param.msg_len >> (j*8));

	for(j = 0; j < 8; j++)
		la[j] = (uint8)(param.hdr_len>> (j*8));

	//case 1: l(a) = 0, no associate data
	if(param.hdr_len == 0)
	{
		// Form B0 = curve_ids || Nonce N || l(m), where the byte curve_ids = Reserved || Adata (0) || M || L
		if(param.tag_len >= 2) 
		{
			T[0] = (param.tag_len - 2) >> 1;  
		    T[0] = (T[0] << 3) | (param.L - 1);
		}
		else T[0] = param.L - 1;
		// Concatenate nonce
		for(i = 0; i < 15 - param.L; i++)
			T[i + 1] = *(nonce + i);
		// Concatenate l(m)
		for(i = 15 - param.L; i < 16; i++)
			T[i + 1] = lm[i - (15 - param.L)];
		
		// AES-CBC-MAC for B0
		aes_block_enc(T, param.k);
	}
	else
	{
		// Form B0 = curve_ids || Nonce N || l(m), where the byte curve_ids = Reserved || Adata (1) || M || L
		if(param.tag_len >= 2) 
		{
			T[0] = (param.tag_len - 2) >> 1;  
			T[0] = (0x1 << 6) | (T[0] << 3) | (param.L - 1);
		}
		else T[0] = (0x1 << 6) | (param.L - 1);
		// Concatenate nonce
		for(i = 0; i < 15 - param.L; i++)
			T[i + 1] = *(nonce + i);
		// Concatenate l(m)
		for(i = 15 - param.L; i < 15; i++)
			T[i + 1] = lm[i - (15 - param.L)];

		// AES-CBC-MAC for B0
		aes_block_enc(T, param.k);
		
		//case 2: 0 < l(a) < 2^16 - 2^8, the associate data is 2-octet representation of l(a)
		if(param.hdr_len > 0 && param.hdr_len < 65280)
		{
			adatanum  = (param.hdr_len + 16) >> 7;
			alastnum  = ((param.hdr_len + 16) >> 3) & 0xf;
			alastbits = (param.hdr_len + 16) & 0x7;

			//Deal with associate data blocks
			T[0] = T[0] ^ la[0];
			T[1] = T[1] ^ la[1];
			// Form the associate data = l(a) || a || 00...00
			if(adatanum >= 1)
			{
				for(j = 2; j < 16; j++) 
					T[j] = T[j] ^ *(a + j - 2);
				aes_block_enc(T, param.k);
				for(i = 1; i < adatanum; i++)
				{
					for(j = 0; j < 16; j++) 
						T[j] = T[j] ^ *(a + 16*i + j - 2);
					aes_block_enc(T, param.k);
				}
				j = 0;
				// Deal with the last block of the associate data
				if(alastnum != 0)
				{
					for(j = 0; j < alastnum; j++) 
						T[j] = T[j] ^ *(a + 16*i + j - 2);
					if(alastbits == 0)
					{
						aes_block_enc(T, param.k);
					}
				}
				// Deal with the last bits of the associated data
				if(alastbits != 0)
				{
					T[j] = T[j] ^ (*(a + 16*i + j - 2) >> (8 - lastbits));
					aes_block_enc(T, param.k);
				}
			}
			else
			{
				for(j = 2; j < alastnum; j++) 
					T[j] = T[j] ^ *(a + j - 2);
				if(alastbits == 0)
				{
					aes_block_enc(T, param.k);
				}
				else
				{
					// Deal with the last bits of the associated data
					T[j] = T[j] ^ (*(a + j - 2) >> (8 - lastbits));
					aes_block_enc(T, param.k);
				}
			}
		}
		else if (param.hdr_len >= 65280 && param.hdr_len < 4294967296) //case 3: 2^16 - 2^8 =< l(a) < 2^32, the associate data is the right-concatenation of the octet 0xff, the octet 0xfe, and the 4-octet representation of l(a)
		{
			adatanum  = (param.hdr_len + 48) >> 7;
			alastnum  = ((param.hdr_len + 48) >> 3) & 0xf;
			alastbits = (param.hdr_len + 48) & 0x7;

			//Deal with associate data blocks
			T[0] = T[0] ^ 0xff;
			T[1] = T[1] ^ 0xfe;
			T[2] = T[2] ^ la[0];
			T[3] = T[3] ^ la[1];
			T[4] = T[4] ^ la[2];
			T[5] = T[5] ^ la[3];
			// Form the associate data = 0xff || 0xfe || l(a) || a || 00...00
			if(adatanum >= 1)
			{
				for(j = 6; j < 16; j++) 
					T[j] = T[j] ^ *(a + j - 6);
				aes_block_enc(T, param.k);
				for(i = 1; i < adatanum; i++)
				{
					for(j = 0; j < 16; j++) 
						T[j] = T[j] ^ *(a + 16*i + j - 6);
					aes_block_enc(T, param.k);
				}
				j = 0;
				// Deal with the last block of the associate data
				if(alastnum != 0)
				{
					for(j = 0; j < alastnum; j++) 
						T[j] = T[j] ^ *(a + 16*i + j - 6);
					if(alastbits == 0)
					{
						aes_block_enc(T, param.k);
					}
				}
				// Deal with the last bits of the associated data
				if(alastbits != 0)
				{
					T[j] = T[j] ^ (*(a + 16*i + j - 6) >> (8 - lastbits));
					aes_block_enc(T, param.k);
				}
			}
			else
			{
				for(j = 2; j < alastnum; j++) 
					T[j] = T[j] ^ *(a + j - 6);
				if(alastbits == 0)
				{
					aes_block_enc(T, param.k);
				}
				else
				{
					// Deal with the last bits of the associated data
					T[j] = T[j] ^ (*(a + j - 6) >> (8 - lastbits));
					aes_block_enc(T, param.k);
				}
			}
		}
		else //case 4: 2^32 =< l(a) < 2^64, the associate data is the right-concatenation of the octet 0xff, the octet 0xff, and the 8-octet representation of l(a)
		{
			adatanum  = (param.hdr_len + 80) >> 7;
			alastnum  = ((param.hdr_len + 80) >> 3) & 0xf;
			alastbits = (param.hdr_len + 80) & 0x7;

			//Deal with associate data blocks
			T[0] = T[0] ^ 0xff;
			T[1] = T[1] ^ 0xfe;
			T[2] = T[2] ^ la[0];
			T[3] = T[3] ^ la[1];
			T[4] = T[4] ^ la[2];
			T[5] = T[5] ^ la[3];
			T[6] = T[6] ^ la[4];
			T[7] = T[7] ^ la[5];
			T[8] = T[8] ^ la[6];
			T[9] = T[9] ^ la[7];
			// Form the associate data = 0xff || 0xff || l(a) || a || 00...00
			if(adatanum >= 1)
			{
				for(j = 10; j < 16; j++) 
					T[j] = T[j] ^ *(a + j - 10);
				aes_block_enc(T, param.k);
				for(i = 1; i < adatanum; i++)
				{
					for(j = 0; j < 16; j++) 
						T[j] = T[j] ^ *(a + 16*i + j - 10);
					aes_block_enc(T, param.k);
				}
				j = 0;
				// Deal with the last block of the associate data
				if(alastnum != 0)
				{
					for(j = 0; j < alastnum; j++) 
						T[j] = T[j] ^ *(a + 16*i + j - 10);
					if(alastbits == 0)
					{
						aes_block_enc(T, param.k);
					}
				}
				// Deal with the last bits of the associated data
				if(alastbits != 0)
				{
					T[j] = T[j] ^ (*(a + 16*i + j - 10) >> (8 - lastbits));
					aes_block_enc(T, param.k);
				}
			}
			else
			{
				for(j = 2; j < alastnum; j++) 
					T[j] = T[j] ^ *(a + j - 10);
				if(alastbits == 0)
				{
					aes_block_enc(T, param.k);
				}
				else
				{
					// Deal with the last bits of the associated data
					T[j] = T[j] ^ (*(a + j - 10) >> (8 - lastbits));
					aes_block_enc(T, param.k);
				}
			}
		}
	}

	// Deal with message blocks
	for(i = 0; i < blocknum; i++)
	{
		for(j = 0; j < 16; j++) 
			T[j] = T[j] ^ *(m + 16*i + j);
		aes_block_enc(T, param.k);
	}
	j = 0;
	// Deal with the last block of the message
	if(lastnum != 0)
	{
		for(j = 0; j < lastnum; j++) 
			T[j] = T[j] ^ *(m + 16*i + j);
		if(lastbits == 0)
		{
			aes_block_enc(T, param.k);
		}
	}
	// Deal with the last bits of the message
	if(lastbits != 0)
	{
		T[j] = T[j] ^ (*(m + 16*i + j) >> (8 - lastbits));
		aes_block_enc(T, param.k);
	}

	for(i = 0; i < 16; i++)
		*(tag + i) = T[i];
}

//CCM* authenticated encryption mode
void ccm_auth_enc(uint8 *m, uint8 *a, uint8 *nonce, ccm_parameter param, uint8 *c, uint8 *authtag)
{
	uint8 T[16];        //128-bit AES-CBC-MAC result
	uint8 A[16];        //128-bit data block for CTR mode
	uint8 Counter[8];   //the maximum number of counter
	uint64 i;
	uint32 j;
	uint64 blocknum;    //number of message blocks
	uint32 lastnum;     //number of bytes in the last message block
	uint32 lastbits;    //remaining bits in the last byte

	blocknum = param.msg_len >> 7;
	lastnum  = (param.msg_len >>3) & 0xf;
	lastbits = param.msg_len & 0x7;

    // Generate the authentication tag
	ccm_auth_only(m, a, nonce, param, T);

	//Forms 128-bit data block A_i = curve_ids || Nonce N || Counter_i
	A[0] = param.L - 1;
	// Concatenate nonce
	for(j = 0; j < (uint32)(15 - param.L); j++)
		A[j + 1] = *(nonce + j);
	// Concatenate counter_0
	for(j = 15 - param.L; j < 15; j++)
		A[j + 1] = 0;
	aes_block_enc(A, param.k);
	// Compute AuthTag as the leftmost tag_len octets of S_0
	for(j = 0; j < param.tag_len; j++)
		*(authtag + j) = T[j] ^ A[j];

	// CTR encryption for message blocks M_1, ..., M_(t-1)
    for(i = 0; i < blocknum; i++)
	{
		//Convert counter_i to bytes
		for(j = 0; j < 8; j++)
			Counter[j] = (uint8)(i >> (j*8));
		//Forms 128-bit data block A_i = curve_ids || Nonce N || Counter_i
		A[0] = param.L - 1;
		// Concatenate nonce
		for(j = 0; j < (uint32)(15 - param.L); j++)
			A[j + 1] = *(nonce + j);
		// Concatenate counter_i
		for(j = 15 - param.L; j < 15; j++)
			A[j + 1] = Counter[j - (15 - param.L)];
		aes_block_enc(A, param.k);
        for(j = 0; j < 16; j++)
			*(c + i*16 + j) = A[j] ^ *(m + i*16 + j);
	}

	//CTR encryption for the last message block M_t
	if(lastnum != 0 || lastbits != 0)
	{
		//Convert counter_t to bytes
		for(j = 0; j < 8; j++)
			Counter[j] = (uint8)((blocknum + 1) >> (j*8));
		//Forms 128-bit data block A_i = curve_ids || Nonce N || Counter_i
		A[0] = param.L - 1;
		// Concatenate nonce
		for(j = 0; j < (uint32)(15 - param.L); j++)
			A[j + 1] = *(nonce + j);
		// Concatenate counter_i
		for(j = 15 - param.L; j < 15; j++)
			A[j + 1] = Counter[j - (15 - param.L)];
		aes_block_enc(A, param.k);
		for(j = 0; j < lastnum; j++)
			*(c + blocknum*16 + j) = A[j] ^ *(m + blocknum*16 + j);
		if(lastbits != 0)
		{
			 T[0] = A[lastnum] ^ *(m + blocknum*16 + lastnum);
			 *(c + blocknum*16 + lastnum) = T[0] >> (8 - lastbits);
		}
	}
}

//CCM* authenticated decryption mode
void ccm_auth_dec(uint8 *c, uint8 *authtag, uint8 *nonce, ccm_parameter param, uint8 *m, uint8 *u)
{
	uint8 A[16];        //128-bit data block for CTR mode
	uint8 Counter[8];   //the maximum number of counter
	uint64 i;
	uint32 j;
	uint64 blocknum;    //number of message blocks
	uint32 lastnum;     //number of bytes in the last message block
	uint32 lastbits;    //remaining bits in the last byte

	blocknum = param.msg_len >> 7;
	lastnum  = (param.msg_len >>3) & 0xf;
	lastbits = param.msg_len & 0x7;

	//Forms 128-bit data block A_i = curve_ids || Nonce N || Counter_i
	A[0] = param.L - 1;
	// Concatenate nonce
	for(j = 0; j < (uint32)(15 - param.L); j++)
		A[j + 1] = *(nonce + j);
	// Concatenate counter_0
	for(j = 15 - param.L; j < 15; j++)
		A[j + 1] = 0;
	aes_block_enc(A, param.k);
    // Compute u
	for(j = 0; j < param.tag_len; j++)
		*(u + j) = A[j] ^ *(authtag + j);

	// CTR decryption for ciphertext blocks C_1, ..., C_(t-1)
	for(i = 0; i < blocknum; i++)
	{
		//Convert counter_i to bytes
		for(j = 0; j < 8; j++)
			Counter[j] = (uint8)(i >> (j*8));
		//Forms 128-bit data block A_i = curve_ids || Nonce N || Counter_i
		A[0] = param.L - 1;
		// Concatenate nonce
		for(j = 0; j < (uint32)(15 - param.L); j++)
			A[j + 1] = *(nonce + j);
		// Concatenate counter_i
		for(j = 15 - param.L; j < 15; j++)
			A[j + 1] = Counter[j - (15 - param.L)];
		aes_block_enc(A, param.k);
		for(j = 0; j < 16; j++)
			*(m + i*16 + j) = A[j] ^ *(c + i*16 + j);
	}

	//CTR decryption for the last message block M_t
	if(lastnum != 0 || lastbits != 0)
	{
		//Convert counter_t to bytes
		for(j = 0; j < 8; j++)
			Counter[j] = (uint8)((blocknum + 1) >> (j*8));
		//Forms 128-bit data block A_i = curve_ids || Nonce N || Counter_i
		A[0] = param.L - 1;
		// Concatenate nonce
		for(j = 0; j < (uint32)(15 - param.L); j++)
			A[j + 1] = *(nonce + j);
		// Concatenate counter_i
		for(j = 15 - param.L; j < 15; j++)
			A[j + 1] = Counter[j - (15 - param.L)];
		aes_block_enc(A, param.k);
		for(j = 0; j < lastnum; j++)
			*(m + blocknum*16 + j) = A[j] ^ *(c + blocknum*16 + j);
		if(lastbits != 0)
		{
			A[0] = A[lastnum] ^ *(c + blocknum*16 + lastnum);
			*(m + blocknum*16 + lastnum) = A[0] >> (8 - lastbits);
		}
	}
}


//CCM* verification mode
uint8 ccm_veri_only(uint8 *m, uint8 *a, uint8 *nonce, ccm_parameter param, uint8 *u)
{
	uint8 T[16];        //128-bit AES-CBC-MAC result
	uint32 i;

    // Generate the authentication tag
	ccm_auth_only(m, a, nonce, param, T);

    for(i = 0; i < param.tag_len; i++)
	{
		if(*(u + i) != T[i])
			return 0;                //Authentication tag is invalid!
	}
	return 1;                        //Authentication tag is valid!
}