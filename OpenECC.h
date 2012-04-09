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

#ifndef __OpenECC_H__
#define __OpenECC_H__

/**
 * Support for both ZigBee Smart Energy 1.x and 1.2
 * Use curve sect163k1 for ZigBee Smart Energy 1.x
 * Use curve sect233k1 for ZigBee Smart Energy 1.2
 * Other koblitz curves defined in SEC 2 will be supported in the future.
 */ 
#define sect163k1 0
#define sect233k1 1
//#define sect239k1 2
//#define sect283k1 3
//#define sect409k1 4
//#define sect571k1 5

/**
 * Define data type
 */
typedef unsigned char uint8;          /* Unsigned 8  bit value */
typedef unsigned short uint16;        /* Unsigned 16 bit value */
typedef unsigned long uint32;         /* Unsigned 32 bit value */
typedef unsigned long long uint64;    /* Unsigned 64 bit value */
typedef long int sint32;              /* Signed 32 bit value */

/**
 * Define macros
 */
#define XTOY(Y, X, n){\
	uint32 i;\
	for(i = n - 1; i != 0; i--)\
	    Y[i] = X[i];\
	Y[0] = X[0];\
}
#define XIS0(X, n){\
	uint32 i;\
	for(i = n - 1; i != 0; i--)\
	    X[i] = 0;\
	X[0] = 0;\
}
#define XIS1(X, n){\
	uint32 i;\
	for(i = n - 1; i != 0; i--)\
	    X[i] = 0;\
	X[0] = 1;\
}
//For the curve sect163k1
#ifdef sect163k1
#define IF1_163(X)	       X[0] == 1 && X[1] == 0 && X[2] == 0 && X[3] == 0 && X[4] == 0 && X[5] == 0 
#define IFN1_163(X)	       X[0] != 1 || X[1] != 0 || X[2] != 0 || X[3] != 0 || X[4] != 0 || X[5] != 0 
#define IF0_163(X)	       X[0] == 0 && X[1] == 0 && X[2] == 0 && X[3] == 0 && X[4] == 0 && X[5] == 0 
#define IFN0_163(X)	       X[0] != 0 || X[1] != 0 || X[2] != 0 || X[3] != 0 || X[4] != 0 || X[5] != 0 
#define IFXeqY_163(X,Y)    X[0] == Y[0] && X[1] == Y[1] && X[2] == Y[2] && X[3] == Y[3] && X[4] == Y[4] && X[5] == Y[5]
#endif

//For the curve sect233k1
#ifdef sect233k1
#define IF1_233(X)	       X[0] == 1 && X[1] == 0 && X[2] == 0 && X[3] == 0 && X[4] == 0 && X[5] == 0 && X[6] == 0 && X[7] == 0
#define IFN1_233(X)	       X[0] != 1 || X[1] != 0 || X[2] != 0 || X[3] != 0 || X[4] != 0 || X[5] != 0 || X[6] != 0 || X[7] != 0 
#define IF0_233(X)	       X[0] == 0 && X[1] == 0 && X[2] == 0 && X[3] == 0 && X[4] == 0 && X[5] == 0 && X[6] == 0 && X[7] == 0 
#define IFN0_233(X)	       X[0] != 0 || X[1] != 0 || X[2] != 0 || X[3] != 0 || X[4] != 0 || X[5] != 0 || X[6] != 0 || X[7] != 0 
#define IFXeqY_233(X,Y)    X[0] == Y[0] && X[1] == Y[1] && X[2] == Y[2] && X[3] == Y[3] && X[4] == Y[4] && X[5] == Y[5] && X[6] == Y[6] && X[7] == Y[7]
#endif

#define N 8

/**
 * Define an affine point (x, y) over the curve sect163k1 or sect233k1
 * A compressed affine point denoted by h||x, where h = 0x02 if y' = 0, or h = 0x03 if y' = 1
 */
typedef struct
{
	uint32 x[N];     /* x coordinate of a point in affine representation */
    uint32 y[N];     /* y coordinate of a point in affine representation */
}ec_point_aff;

/**
 * Define a projective point (X, Y, Z) over the curve sect163k1 or sect233k1
 */
typedef struct
{
	uint32 X[N];     /* X coordinate of a point (x, y) in projective representation */
    uint32 Y[N];     /* Y coordinate of a point (x, y) in projective representation */
	uint32 Z[N];     /* Z coordinate of a point (x, y) in projective representation */
}ec_point_pro;

/**
 * Define a key pair over the curve sect163k1 or sect233k1 
 */
typedef struct
{
	uint32  d[N];     /* private key */
    uint32 Qx[N];     /* public key in compressed form */
}ec_key_pair;

/**
 * Define curve parameters
 */
ec_point_aff G[2];     //base point G for the curve sect163k1 and sect233k1
uint32 eccn[2][N];     //group order n of the curve sect163k1 and sect233k1
uint32 eccu[2][N+1];   //b = 2^32, eccu = [b^(2*Num)/eccn]

/**
 * Public key validation
 *
 * @param Q   a public key
 *    valid   return 1
 *  invalid   return 0
 */
uint32 pk_validation(ec_point_aff *Q, uint32 curve_id);

/**
 * Key generation
 *
 * @param key  a key pair
 * @param rnd  a random number (21-byte for sect163k1 and 30-byte for sect233k1)
 */
void key_generation(ec_key_pair *key, uint8 *rnd, uint32 curve_id);

/**
 * ECQV certificate request
 *
 * @param key  a key pair
 * @param rnd  a random number (21-byte for sect163k1 and 30-byte for sect233k1)
 */
void ECQV_request(ec_key_pair *key, uint8 *rnd, uint32 curve_id);

/**
 * ECQV public key extraction
 *
 * @param QCAx   the CA's public key in compressed form
 * @param CertA  an implicit certificate for user idA
 * @param QAx    a user A's public key in compressed form
 * succeed       return 1
 * fail          return 0
 */
uint32 ECQV_extraction(uint32 *QCAx, uint8 *CertA, uint32 *QAx, uint32 curve_id);

/**
 * ECQV explicit certificate validation
 *
 * @param QCAx   the CA's public key in compressed form 
 * @param RAx    a user A's public key in compressed form
 * @param CertA  an implicit certificate for user idA
 * @param r      CA's private key contribution
 * succeed       return 1
 * fail          return 0
 */
uint32 ECQV_validation(uint32 *QCAx, uint32 *RAx, uint8 *CertA, uint32 *r, uint32 curve_id);

/**
 * ECMQV key agreement scheme 
 *
 * @param keyA1    A's key pair (dA1, QA1) 
 * @param keyA2    A's key pair (dA2, QA2)
 * @param QB1      B's public key QB1
 * @param QB2      B's public key QB2
 * @param len      the length of shared key in bytes
 * @param skey     a shared key between A and B
 * succeed         return 1
 * fail            return 0
 */
uint32 ECMQV(ec_key_pair *keyA1, ec_key_pair *keyA2, uint32 *QB1, uint32 *QB2, uint32 len, uint8 *skey, uint32 curve_id);

#endif  /* __OpenECC_H__ */
