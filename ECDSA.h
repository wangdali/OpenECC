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

#ifndef __ECDSA_H__
#define __ECDSA_H__

/**
 * Modular multiplication over GF(p)
 */
void modpmul(uint32 *a, uint32 *b, uint32 *amulb, uint32 curve_id);

/**
 * Modular inversion over GF(p)
 */
void modpinv(uint32 *a, uint32 *b, uint32 curve_id);

/**
 * ECDSA signature generation
 *
 * @param d      the private key of a signer
 * @param rnd    a random number (21-byte for sect163k1 and 30-byte for sect233k1)
 * @param msg    a message
 * @param mlen   message length in bytes
 * @param r, s   the signature pair (r, s)    
 * succeed       return 1
 * fail          return 0
 */
uint32 ECDSA_sign(uint32 *d, uint8 *rnd, uint8 *msg, uint32 mlen, uint32 *r, uint32 *s, uint32 curve_id);

/**
 * ECDSA signature verification
 *
 * @param Qx     the public key of a signer in compressed form
 * @param msg    a message
 * @param mlen   message length in bytes
 * @param r, s   the signature pair (r, s)
 * succeed       return 1
 * fail          return 0
 */
uint32 ECDSA_verify(uint32 *Qx, uint8 *msg, uint32 mlen, uint32 *r, uint32 *s, uint32 curve_id);

#endif  /* __ECDSA_H__ */
