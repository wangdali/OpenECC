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

#ifndef __GF2n_H__
#define __GF2n_H__

/**
 * Support fast arithmetic for two binary fields
 * GF(2^163): F_2[x]/(x^163 + x^7 + x^6 + x^3 + 1)
 * GF(2^233): F_2[x]/(x^233 + x^74 + 1)
 */

/**
 * Modular Addition over GF(2^n)
 */
void modadd(uint32 *a, uint32 *b, uint32 *c, uint32 curve_id);

/**
 * Modular Multiplication over GF(2^n)
 */
void modmul(uint32 *a, uint32 *b, uint32 *c, uint32 curve_id);

/**
 * Modular Squaring over GF(2^n)
 */
void modsq(uint32 *a, uint32 *b, uint32 curve_id);

/**
 * Modular Inversion over GF(2^n)
 */
void modinv(uint32 *a, uint32 *b, uint32 curve_id);

/**
 * Compute Halftrace over GF(2^n)
 */
void halftrace(uint32 *a, uint32 *b, uint32 curve_id);

/**
 * Big Integer Comparison
 */
sint32 compare(uint32 *a, uint32 *b, uint32 m);

#endif  /* __GF2n_H__ */