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

#ifndef __GFp_H__
#define __GFp_H__

/**
 * Big Integer Addition 
 */
uint32 add(uint32 *a, uint32 *b, uint32 *aplusb, uint32 curve_id);

/**
 * Big Integer Subtraction
 */
uint32 sub(uint32 *a, uint32 *b, uint32 *asubb, uint32 m);

/**
 * Modular Addition over GF(p)
 */
void modpadd(uint32 *a, uint32 *b, uint32 *aplusb, uint32 curve_id);

/**
 * Modular Subtraction over GF(p)
 */
void modpsub(uint32 *a, uint32 *b, uint32 *asubb, uint32 curve_id);

/**
 * Modular Multiplication over GF(p) with one operand 128-bit
 */
void modpmul128(uint32 *a, uint32 *b, uint32 *amulb, uint32 curve_id);

/**
 * Modular Multiplication over GF(p) with one operand 96-bit
 */
void modpmul96(uint32 *a, uint32 *b, uint32 *amulb);

#endif  /* __GFp_H__ */
										   
                                                          






















