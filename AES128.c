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
#include "AES128.h"

//load 4 bytes into a 32-bit word
static void LOAD32(uint8 const *y, uint32 *x) {
	uint8 *xptr = (uint8 *)x;

	*(xptr    ) = *(y + 3);
	*(xptr + 1) = *(y + 2);
	*(xptr + 2) = *(y + 1);
	*(xptr + 3) = *(y    );
}

//convert a 32-bit word to 4 bytes
static void STORE32(uint32 const x, uint8 *y) {
	uint8 *xptr = (uint8 *)&x;

	*(y    ) = *(xptr + 3);
	*(y + 1) = *(xptr + 2);
	*(y + 2) = *(xptr + 1);
	*(y + 3) = *(xptr    );
}

//macros for extracting bytes from a 32-bit word x
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define byte(x, n) (uint32)((uint8 *)&(x))[(n)]

//Four look-up tables T_0, T_1, T_2, T_3
//Generate T_1, T_2, T_3 from T_0 by cyclic shifts
#define Te0(x) TE0[x]
#define Te1(x) ROTR(TE0[x], 8)
#define Te2(x) ROTR(TE0[x], 16)
#define Te3(x) ROTR(TE0[x], 24)

//Extract s-box from the table T_0
#define Te4_0(x) (TE0[x] >> 8) & 0xff
#define Te4_1(x) TE0[x] & 0xff00
#define Te4_2(x) TE0[x] & 0xff0000
#define Te4_3(x) (TE0[x] << 8) & 0xff000000

static const uint32 TE0[256] = {
	0xc66363a5UL, 0xf87c7c84UL, 0xee777799UL, 0xf67b7b8dUL,
	0xfff2f20dUL, 0xd66b6bbdUL, 0xde6f6fb1UL, 0x91c5c554UL,
	0x60303050UL, 0x02010103UL, 0xce6767a9UL, 0x562b2b7dUL,
	0xe7fefe19UL, 0xb5d7d762UL, 0x4dababe6UL, 0xec76769aUL,
	0x8fcaca45UL, 0x1f82829dUL, 0x89c9c940UL, 0xfa7d7d87UL,
	0xeffafa15UL, 0xb25959ebUL, 0x8e4747c9UL, 0xfbf0f00bUL,
	0x41adadecUL, 0xb3d4d467UL, 0x5fa2a2fdUL, 0x45afafeaUL,
	0x239c9cbfUL, 0x53a4a4f7UL, 0xe4727296UL, 0x9bc0c05bUL,
	0x75b7b7c2UL, 0xe1fdfd1cUL, 0x3d9393aeUL, 0x4c26266aUL,
	0x6c36365aUL, 0x7e3f3f41UL, 0xf5f7f702UL, 0x83cccc4fUL,
	0x6834345cUL, 0x51a5a5f4UL, 0xd1e5e534UL, 0xf9f1f108UL,
	0xe2717193UL, 0xabd8d873UL, 0x62313153UL, 0x2a15153fUL,
	0x0804040cUL, 0x95c7c752UL, 0x46232365UL, 0x9dc3c35eUL,
	0x30181828UL, 0x379696a1UL, 0x0a05050fUL, 0x2f9a9ab5UL,
	0x0e070709UL, 0x24121236UL, 0x1b80809bUL, 0xdfe2e23dUL,
	0xcdebeb26UL, 0x4e272769UL, 0x7fb2b2cdUL, 0xea75759fUL,
	0x1209091bUL, 0x1d83839eUL, 0x582c2c74UL, 0x341a1a2eUL,
	0x361b1b2dUL, 0xdc6e6eb2UL, 0xb45a5aeeUL, 0x5ba0a0fbUL,
	0xa45252f6UL, 0x763b3b4dUL, 0xb7d6d661UL, 0x7db3b3ceUL,
	0x5229297bUL, 0xdde3e33eUL, 0x5e2f2f71UL, 0x13848497UL,
	0xa65353f5UL, 0xb9d1d168UL, 0x00000000UL, 0xc1eded2cUL,
	0x40202060UL, 0xe3fcfc1fUL, 0x79b1b1c8UL, 0xb65b5bedUL,
	0xd46a6abeUL, 0x8dcbcb46UL, 0x67bebed9UL, 0x7239394bUL,
	0x944a4adeUL, 0x984c4cd4UL, 0xb05858e8UL, 0x85cfcf4aUL,
	0xbbd0d06bUL, 0xc5efef2aUL, 0x4faaaae5UL, 0xedfbfb16UL,
	0x864343c5UL, 0x9a4d4dd7UL, 0x66333355UL, 0x11858594UL,
	0x8a4545cfUL, 0xe9f9f910UL, 0x04020206UL, 0xfe7f7f81UL,
	0xa05050f0UL, 0x783c3c44UL, 0x259f9fbaUL, 0x4ba8a8e3UL,
	0xa25151f3UL, 0x5da3a3feUL, 0x804040c0UL, 0x058f8f8aUL,
	0x3f9292adUL, 0x219d9dbcUL, 0x70383848UL, 0xf1f5f504UL,
	0x63bcbcdfUL, 0x77b6b6c1UL, 0xafdada75UL, 0x42212163UL,
	0x20101030UL, 0xe5ffff1aUL, 0xfdf3f30eUL, 0xbfd2d26dUL,
	0x81cdcd4cUL, 0x180c0c14UL, 0x26131335UL, 0xc3ecec2fUL,
	0xbe5f5fe1UL, 0x359797a2UL, 0x884444ccUL, 0x2e171739UL,
	0x93c4c457UL, 0x55a7a7f2UL, 0xfc7e7e82UL, 0x7a3d3d47UL,
	0xc86464acUL, 0xba5d5de7UL, 0x3219192bUL, 0xe6737395UL,
	0xc06060a0UL, 0x19818198UL, 0x9e4f4fd1UL, 0xa3dcdc7fUL,
	0x44222266UL, 0x542a2a7eUL, 0x3b9090abUL, 0x0b888883UL,
	0x8c4646caUL, 0xc7eeee29UL, 0x6bb8b8d3UL, 0x2814143cUL,
	0xa7dede79UL, 0xbc5e5ee2UL, 0x160b0b1dUL, 0xaddbdb76UL,
	0xdbe0e03bUL, 0x64323256UL, 0x743a3a4eUL, 0x140a0a1eUL,
	0x924949dbUL, 0x0c06060aUL, 0x4824246cUL, 0xb85c5ce4UL,
	0x9fc2c25dUL, 0xbdd3d36eUL, 0x43acacefUL, 0xc46262a6UL,
	0x399191a8UL, 0x319595a4UL, 0xd3e4e437UL, 0xf279798bUL,
	0xd5e7e732UL, 0x8bc8c843UL, 0x6e373759UL, 0xda6d6db7UL,
	0x018d8d8cUL, 0xb1d5d564UL, 0x9c4e4ed2UL, 0x49a9a9e0UL,
	0xd86c6cb4UL, 0xac5656faUL, 0xf3f4f407UL, 0xcfeaea25UL,
	0xca6565afUL, 0xf47a7a8eUL, 0x47aeaee9UL, 0x10080818UL,
	0x6fbabad5UL, 0xf0787888UL, 0x4a25256fUL, 0x5c2e2e72UL,
	0x381c1c24UL, 0x57a6a6f1UL, 0x73b4b4c7UL, 0x97c6c651UL,
	0xcbe8e823UL, 0xa1dddd7cUL, 0xe874749cUL, 0x3e1f1f21UL,
	0x964b4bddUL, 0x61bdbddcUL, 0x0d8b8b86UL, 0x0f8a8a85UL,
	0xe0707090UL, 0x7c3e3e42UL, 0x71b5b5c4UL, 0xcc6666aaUL,
	0x904848d8UL, 0x06030305UL, 0xf7f6f601UL, 0x1c0e0e12UL,
	0xc26161a3UL, 0x6a35355fUL, 0xae5757f9UL, 0x69b9b9d0UL,
	0x17868691UL, 0x99c1c158UL, 0x3a1d1d27UL, 0x279e9eb9UL,
	0xd9e1e138UL, 0xebf8f813UL, 0x2b9898b3UL, 0x22111133UL,
	0xd26969bbUL, 0xa9d9d970UL, 0x078e8e89UL, 0x339494a7UL,
	0x2d9b9bb6UL, 0x3c1e1e22UL, 0x15878792UL, 0xc9e9e920UL,
	0x87cece49UL, 0xaa5555ffUL, 0x50282878UL, 0xa5dfdf7aUL,
	0x038c8c8fUL, 0x59a1a1f8UL, 0x09898980UL, 0x1a0d0d17UL,
	0x65bfbfdaUL, 0xd7e6e631UL, 0x844242c6UL, 0xd06868b8UL,
	0x824141c3UL, 0x299999b0UL, 0x5a2d2d77UL, 0x1e0f0f11UL,
	0x7bb0b0cbUL, 0xa85454fcUL, 0x6dbbbbd6UL, 0x2c16163aUL,
};

// round constants
static const uint32 rcon[10] = {
	0x01000000UL, 0x02000000UL, 0x04000000UL, 0x08000000UL, 
	0x10000000UL, 0x20000000UL, 0x40000000UL, 0x80000000UL,
	0x1B000000UL, 0x36000000UL
};

//encryption
void aes_block_enc(uint8 *data, uint8 *key) 
{
	uint32 i;
	uint32 k0, k1, k2, k3;
	uint32 s0, s1, s2, s3;
	uint32 t0, t1, t2, t3;
    
	//load 128-bit message into 32-bit words
    LOAD32(data,  	   &s0); 
	LOAD32(data + 4,  &s1); 
	LOAD32(data + 8,  &s2); 
	LOAD32(data + 12, &s3);

	//load 128-bit key into 32-bit words
	LOAD32(key,      &k0);
	LOAD32(key + 4,  &k1);
	LOAD32(key + 8,  &k2);
	LOAD32(key + 12, &k3);

	//add round key
	s0 ^= k0;
	s1 ^= k1;
	s2 ^= k2;
	s3 ^= k3;

	//9 regular rounds
	for (i = 9; i != 0; i--) 
	{
		k0 ^= rcon[9 - i] ^ Te4_3(byte(k3, 2)) ^ Te4_2(byte(k3, 1)) ^ Te4_1(byte(k3, 0)) ^ Te4_0(byte(k3, 3));
		k1 ^= k0;
		k2 ^= k1;
		k3 ^= k2;

		t0 = Te0(byte(s0, 0)) ^ Te1(byte(s1, 1)) ^ Te2(byte(s2, 2)) ^ Te3(byte(s3, 3)) ^ k0;
		t1 = Te0(byte(s1, 3)) ^ Te1(byte(s2, 2)) ^ Te2(byte(s3, 1)) ^ Te3(byte(s0, 0)) ^ k1;
		t2 = Te0(byte(s2, 3)) ^ Te1(byte(s3, 2)) ^ Te2(byte(s0, 1)) ^ Te3(byte(s1, 0)) ^ k2;
		t3 = Te0(byte(s3, 3)) ^ Te1(byte(s0, 2)) ^ Te2(byte(s1, 1)) ^ Te3(byte(s2, 0)) ^ k3;

		s0 = t0; 
		s1 = t1; 
		s2 = t2; 
		s3 = t3;
	}

	// the last round for the full AES
	k0 ^= rcon[9] ^ Te4_3(byte(k3, 2)) ^ Te4_2(byte(k3, 1)) ^ Te4_1(byte(k3, 0)) ^ Te4_0(byte(k3, 3));
	k1 ^= k0;
	k2 ^= k1;
	k3 ^= k2;

	s0 = Te4_3(byte(t0, 3)) ^ Te4_2(byte(t1, 2)) ^ Te4_1(byte(t2, 1)) ^ Te4_0(byte(t3, 0)) ^ k0;
	s1 = Te4_3(byte(t1, 3)) ^ Te4_2(byte(t2, 2)) ^ Te4_1(byte(t3, 1)) ^ Te4_0(byte(t0, 0)) ^ k1;
	s2 = Te4_3(byte(t2, 3)) ^ Te4_2(byte(t3, 2)) ^ Te4_1(byte(t0, 1)) ^ Te4_0(byte(t1, 0)) ^ k2;
	s3 = Te4_3(byte(t3, 3)) ^ Te4_2(byte(t0, 2)) ^ Te4_1(byte(t1, 1)) ^ Te4_0(byte(t2, 0)) ^ k3;
	
	STORE32(s0, data);
	STORE32(s1, data + 4);
	STORE32(s2, data + 8);
	STORE32(s3, data + 12);
}