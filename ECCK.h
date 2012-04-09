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

#ifndef __ECCK_H__
#define __ECCK_H__

/**
 * Koblitz curve sect233k1: y^2 + xy = x^3 + x^2 + 1
 * Finite Field: GF(2^163) = F_2[x]/(x^163 + x^7 + x^6 + x^3 + 1)
 *
 * Koblitz curve sect233k1: y^2 + xy = x^3 + 1
 * Finite Field: GF(2^233) = F_2[x]/(x^233 + x^74 + 1)
 */

/**
 * Point compression
 */
void point_compression(ec_point_aff *P, uint32 *Pc, uint32 curve_id);

/**
 * Point decompression
 */
uint32 point_decompression(uint32 *Pc, ec_point_aff *P, uint32 curve_id);
						   
/**
 * Point negation
 */
void point_negation(ec_point_aff *P, ec_point_aff *negP, uint32 curve_id);

/**
 * Point conversion from project coordinate to affine coordinate
 */
void project_to_affine(ec_point_pro *Qp, ec_point_aff *Qa, uint32 curve_id);

/**
 * Point conversion from affine coordinate to projective coordinate
 */
void affine_to_project(ec_point_aff *Qa, ec_point_pro *Qp, uint32 curve_id);

/**
 * Point addition
 */
void mixed_addition(ec_point_pro *P, ec_point_aff *Q, ec_point_pro *R, uint32 curve_id);

/**
 * Point doubling
 */
void point_doubling(ec_point_pro *P, ec_point_pro *Q, uint32 curve_id);

/**
 * Frobenius mapping
 */
void point_frobenius(ec_point_pro *P, uint32 curve_id);

/**
 * Partial reduction
 */
void partial_mod(uint32 *k, uint32 *r0, uint32 *r1, uint32 curve_id);

/**
 * Width-5 TNAF expansion 
 */
void TNAF5_expansion(uint32 *r0, uint32 *r1, char *u, uint32 curve_id);

/**
 * Scalar multiplication with the width-5 TNAF (fixed point)
 */
void TNAF5_fixed_scalarmul(uint32 *k, ec_point_aff *Q, uint32 curve_id);

/**
 * Scalar multiplication with the width-5 TNAF (random point)
 */
void TNAF5_random_scalarmul(uint32 *k, ec_point_aff *P, ec_point_aff *Q, uint32 curve_id);

#endif /* __ECCK_H__ */