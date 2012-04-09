
//system libraries
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include "string.h"
//user libraries
#include "OpenECC.h"
#include "GF2n.h"
#include "GFp.h"
#include "ECCK.h"
#include "AES128.h"
#include "AESMMO.h"
#include "DRNG.h"
#include "ECDSA.h"

// The base point G (extern variable defined in ECCK.c)
extern ec_point_aff G[2];

void main()
{
	uint8 seed[16];          //seed for DRNG
    uint8 rndnum[21];        //163-bit random numbers
	uint32 i, r;
	ec_key_pair keypair;
	uint8 Uid[26] = {0x00,0x0d,0x6f,0x00,0x00,0x06,0x7a,0x1f,0x54,0x45,0x53,0x54,0x53,0x45,0x43,0x41,0x01,0x09,0x00,0x0b,0x00,0x00,0x00,0x00,0x00,0x00};
	//The following are for ECMQV
	ec_key_pair keyA1, keyA2, keyB1, keyB2;
	uint8 skey[21];
	//The following are for ECDSA
	uint32 s[6], t[6];

	for(i = 0; i < 16; i++) seed[i] = 0x0;
	srand((unsigned) time (NULL));
	for(i = 0; i < 8; i++)
	{
		r = rand();
		seed[2*i] = (uint8)r;
		seed[2*i + 1] = (uint8)(r >> 8);
	}
    printf("Seed = ");for(i = 0; i < 16; i++){printf("%02x ",seed[i]);}printf("\n");

	//Initialize the DRNG
	ctr_init(seed);
    for(i = 0; i < 21; i++) rndnum[i] = 0x0;

	//ECMQV testing
	if(ctr_generate(163, rndnum) == 1)
		key_generation(&keyA1, rndnum, sect163k1);
	if(ctr_generate(163, rndnum) == 1)
		key_generation(&keyA2, rndnum, sect163k1);
	if(ctr_generate(163, rndnum) == 1)
		key_generation(&keyB1, rndnum, sect163k1);
    if(ctr_generate(163, rndnum) == 1)
		key_generation(&keyB2, rndnum, sect163k1);
	printf("\nTwo key pairs for the user A...\n");
	printf("The first private key:\n");
	printf("   dA1 = ");for(i = 0; i < 6; i++){printf("%08x ",keyA1.d[5 - i]);}printf("\n");
	printf("The first public key:\n");
	printf("  QA1x = ");for(i = 0; i < 6; i++){printf("%08x ",keyA1.Qx[5 - i]);}printf("\n");
	printf("The second private key:\n");
	printf("   dA2 = ");for(i = 0; i < 6; i++){printf("%08x ",keyA2.d[5 - i]);}printf("\n");
	printf("The second public key:\n");
	printf("  QA2x = ");for(i = 0; i < 6; i++){printf("%08x ",keyA2.Qx[5 - i]);}printf("\n");
	printf("\nTwo key pairs for the user B...\n");
	printf("The first private key:\n");
	printf("   dB1 = ");for(i = 0; i < 6; i++){printf("%08x ",keyB1.d[5 - i]);}printf("\n");
	printf("The first public key:\n");
	printf("  QB1x = ");for(i = 0; i < 6; i++){printf("%08x ",keyB1.Qx[5 - i]);}printf("\n");
	printf("The second private key:\n");
	printf("   dB2 = ");for(i = 0; i < 6; i++){printf("%08x ",keyB2.d[5 - i]);}printf("\n");
	printf("The second public key:\n");
	printf("  QB2x = ");for(i = 0; i < 6; i++){printf("%08x ",keyB2.Qx[5 - i]);}printf("\n");
	printf("\nECMQV Key Agreement...");
	if(ECMQV(&keyA1, &keyA2, keyB1.Qx, keyB2.Qx, 21, skey, sect163k1) == 1)
	{
		printf("The shared key computed by A:\n");
		printf("  skA = ");for(i = 0; i < 21; i++){printf("%02x",skey[i]);}printf("\n");
	}
	else 
	{
		printf("ERROR!\n");
		goto L;
	} 
	if(ECMQV(&keyB1, &keyB2, keyA1.Qx, keyA2.Qx, 21, skey, sect163k1) == 1)
	{
		printf("The shared key computed by B:\n");
		printf("  skB = ");for(i = 0; i < 21; i++){printf("%02x",skey[i]);}printf("\n");
	}
	else printf("ERROR!\n");
	
	//ECDSA testing
	printf("\nECDSA Signature Generation...\n");
	if(ctr_generate(163, rndnum) == 1)
		key_generation(&keypair,rndnum, sect163k1);
	printf("Signer's Private key:\n");
	printf("  k = ");for(i = 0; i < 6; i++){printf("%08x ",keypair.d[5 - i]);}printf("\n");
	printf("Signer's Public key:\n");
	printf("  x = ");for(i = 0; i < 6; i++){printf("%08x ",keypair.Qx[5 - i]);}printf("\n");
	printf("Message:\n");
	printf("msg = ");for(i = 0; i < 26; i++){printf("%02x",Uid[i]);}printf("\n");
	ctr_generate(232, rndnum);
    if(ECDSA_sign(keypair.d, rndnum, Uid, 26, s, t, sect163k1) == 1)
	{
		printf("The signature are:\n");
		printf("  r = ");for(i = 0; i < 6; i++){printf("%08x ",s[5 - i]);}printf("\n");
		printf("  s = ");for(i = 0; i < 6; i++){printf("%08x ",t[5 - i]);}printf("\n");
	}
	else 
	{
		printf("ERROR!\n");
		goto L;
	}
	if(ECDSA_verify(keypair.Qx, Uid, 26, s, t, sect163k1) == 1)
		printf("The signature is valid!\n");
	else 
		printf("The signature is invalid!\n");
L:;
}
