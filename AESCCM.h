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

#ifndef __AESCCM_H__
#define __AESCCM_H__

/**
 * the parameter of the CCM* 
 */
typedef struct
{
	uint8 L;              /* an integer 1 < L < 9*/
	uint8 nonce_len;      /* the nonce length in bytes */      
	uint8 tag_len;        /* the authentication tag length in bytes */
	uint8 k[16];          /* a 128-bit symmetric key k */
	uint64 msg_len;       /* message data length in bits: 0 <= msg_len < 2^(8*L) */
	uint64 hdr_len;       /* the associated data length in bits: 0 <= hdr_len < 2^64*/
} ccm_parameter;

/**
 * authentication only mode in CCM*
 *
 * @param m       an octet message m of length msg_len bits
 * @param a       an octet associate data a of length hdr_len bits
 * @param nonce   an octet string of length nonce_len bytes
 * @param param   the parameters of the CCM* 
 * @param tag     a 128-bit authentication tag
 */
void ccm_auth_only(uint8 *m, uint8 *a, uint8 *nonce, ccm_parameter param, uint8 *tag);

/**
 * authenticated encryption mode in CCM*
 *
 * @param m            an octet message m of length msg_len bits
 * @param a            an octet associate data of length hdr_len bits
 * @param nonce        an octet string of length nonce_len bytes
 * @param param        the parameters of the CCM* 
 * @param c            a ciphertext of length msg_len bits
 * @param authtag      an authentication tag of tag_len bytes
 */
void ccm_auth_enc(uint8 *m, uint8 *a, uint8 *nonce, ccm_parameter param, uint8 *c, uint8 *authtag);

/**
 * authenticated decryption mode in CCM*
 *
 * @param c            a ciphertext of length msg_len bits
 * @param authtag      an authentication tag of tag_len bytes
 * @param nonce        an octet string of length nonce_len bytes
 * @param param        the parameters of the CCM* 
 * @param m            an octet message m of length msg_len bits
 * @param u            an authenticator of length tag_len bytes 
 */
void ccm_auth_dec(uint8 *c, uint8 *authtag, uint8 *nonce, ccm_parameter param, uint8 *m, uint8 *u);

/**
 * verification mode in CCM*
 *
 * @param m       an octet message m of length msg_len bits
 * @param a       an octet associate data a of length hdr_len bits
 * @param nonce   an octet string of length nonce_len bytes
 * @param param   the parameters of the CCM* 
 * @param u       an authenticator of length tag_len bytes
 */
uint8 ccm_veri_only(uint8 *m, uint8 *a, uint8 *nonce, ccm_parameter param, uint8 *u);

#endif  /* __AESCCM_H__ */
