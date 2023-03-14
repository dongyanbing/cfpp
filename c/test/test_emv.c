/*
 * test_emv.c
 *
 *  Created on: 17 Jun 2022
 *      Author: idubinsky
 */
#include "emv.h"

#include "test_io.h"
#include "payments.h"
#include "crypto.h"

#include <string.h>
#include <openssl/sha.h>

typedef struct tagEMV_ISSUER_RECOVERY_TEST {
	uint8_t ca_pki;
	uint8_t * issuer_pk_cert;
	size_t issuer_pk_cert_len;
	uint8_t * issuer_pk_remainder;
	size_t issuer_pk_remainder_len;
	uint32_t issuer_pk_exponent;
} EMV_ISSUER_RECOVERY_TEST;

typedef struct tagEMV_ISSUER_E2E_TEST {
	uint8_t ca_pki;
	uint8_t issuer_identifier[4];
	uint8_t certificate_expiration[2];
	uint8_t certificate_serial[3];

	uint32_t issuer_pk_exponent;
	size_t issuer_pk_exponent_len;
	uint8_t *issuer_pk;
	size_t issuer_pk_len;
} EMV_ISSUER_E2E_TEST;

typedef struct tagEMV_ICC_E2E_TEST {
	uint8_t application_pan[10];
	uint8_t certificate_expiration[2];
	uint8_t certificate_serial[3];

	uint8_t * icc_pk;
	size_t icc_pk_len;
	uint32_t icc_pk_exponent;
	size_t icc_pk_exponent_len;

	uint8_t *issuer_pk;
	size_t issuer_pk_len;
	uint8_t *issuer_priv;
	size_t issuer_priv_len;
	uint32_t issuer_pk_exponent;
	size_t issuer_pk_exponent_len;

	uint8_t *auth_data;
	size_t auth_data_len;
} EMV_ICC_E2E_TEST;

typedef struct EMV_MASTER_SESSION_KEY_TEST  {
	uint8_t unpacked_pan [MAX_PAN_LENGTH];
	size_t unpacked_pan_len;
	uint8_t unpacked_csn[CSN_LENGTH];
	uint8_t master_key [AES_KEY_LENGTH_3];
	size_t master_key_len;
	int algorithm;
	size_t output_len;
	uint8_t arc[2];
	uint8_t csu[4];
	uint8_t pad[8];
	size_t pad_len;
} EMV_MASTER_SESSION_KEY_TEST ;


uint8_t test_issuer_pub_key[] = { 0x00, 0xc1, 0x17, 0xad, 0xed, 0x75, 0x67,
		0x7d, 0x4d, 0x74, 0x54, 0x12, 0x84, 0x17, 0x45, 0xf6, 0xca, 0x48, 0xe5,
		0xc3, 0x72, 0x11, 0xfb, 0x4c, 0x7c, 0xa8, 0x49, 0x57, 0x47, 0xed, 0x18,
		0x3e, 0x8a, 0x9f, 0x39, 0x95, 0x6f, 0xe3, 0x54, 0xff, 0x65, 0x7d, 0xe2,
		0xd4, 0x64, 0x01, 0x9b, 0xa3, 0x1b, 0x2d, 0x6f, 0x85, 0x83, 0xd9, 0x83,
		0x2a, 0xa8, 0x02, 0x8d, 0x8a, 0x75, 0xff, 0x8d, 0x63, 0x4e, 0x92, 0xca,
		0x6d, 0x1a, 0x29, 0x62, 0x05, 0x58, 0x5f, 0x11, 0x27, 0x13, 0x40, 0xa5,
		0x37, 0xe9, 0x3e, 0xb0, 0xec, 0xd5, 0xb3, 0x41, 0x09, 0xff, 0x8b, 0xfa,
		0x1f, 0x8c, 0x1e, 0x06, 0x92, 0xe4, 0x89, 0xb9, 0x30, 0x9b, 0xcf, 0x8e,
		0x18, 0x17, 0xd9, 0x7d, 0x8d, 0x07, 0x4f, 0x88, 0xd5, 0x15, 0xc8, 0xc1,
		0xae, 0x81, 0x34, 0x05, 0x96, 0xdb, 0x58, 0xa9, 0x01, 0xd4, 0x9e, 0x23,
		0xec, 0xae, 0xa7, 0x32, 0x97, 0x9b, 0xcd, 0xf3, 0xef, 0x5d, 0xcf, 0x25,
		0xbf, 0x86, 0x7f, 0xfc, 0x08, 0xf1, 0xc1, 0xaa, 0xf8, 0x83, 0xd4, 0x60,
		0x04, 0xb4, 0x67, 0x6e, 0x53, 0x4f, 0xc8, 0xde, 0x00, 0x2e, 0x20, 0xb6,
		0x03, 0x80, 0x4f, 0x2d, 0x73, 0x41, 0xb2, 0xd0, 0xc8, 0xe1, 0x76, 0xc2,
		0x14, 0xe4, 0x46, 0xa1, 0xe3, 0x76, 0x6e, 0x6e, 0x86, 0x1b, 0x81, 0xa9,
		0x3a, 0xc2, 0x98, 0x30, 0xca, 0x61 };

uint8_t test_issuer_pri_key[] = { 0x00, 0x80, 0xba, 0x73, 0xf3, 0xa3, 0x9a,
		0x53, 0x88, 0xf8, 0x38, 0x0c, 0x58, 0x0f, 0x83, 0xf9, 0xdc, 0x30, 0x99,
		0x2c, 0xf6, 0xb6, 0xa7, 0x88, 0x53, 0x1a, 0xdb, 0x8f, 0x85, 0x48, 0xba,
		0xd4, 0x5c, 0x6a, 0x26, 0x63, 0x9f, 0xec, 0xe3, 0x54, 0xee, 0x53, 0xec,
		0x8d, 0x98, 0x01, 0x12, 0x6c, 0xbc, 0xc8, 0xf5, 0x03, 0xad, 0x3b, 0xac,
		0xc7, 0x1a, 0xac, 0x5e, 0x5c, 0x4e, 0xaa, 0x5e, 0x42, 0x34, 0x61, 0xdc,
		0x48, 0xbc, 0x1b, 0x96, 0xae, 0x3a, 0xea, 0x0b, 0x6f, 0x62, 0x2b, 0x18,
		0xcf, 0xf0, 0xd4, 0x75, 0xf3, 0x39, 0x22, 0x2b, 0x5b, 0xff, 0xb2, 0xa6,
		0xbf, 0xb2, 0xbe, 0xaf, 0x0c, 0x97, 0x32, 0x9b, 0xf6, 0x66, 0xec, 0xd1,
		0x9e, 0xf3, 0xff, 0xd2, 0x01, 0x0d, 0xe2, 0x8e, 0x5a, 0xd8, 0x6f, 0xc1,
		0x9a, 0x8a, 0x05, 0x09, 0xb3, 0x25, 0x5a, 0xc0, 0x13, 0x7d, 0x88, 0xc4,
		0x2c, 0x89, 0x4a, 0xb2, 0xb9, 0xe0, 0xa5, 0xf2, 0x6a, 0x2f, 0x6f, 0x0f,
		0xae, 0x53, 0xc4, 0xab, 0x9b, 0xf5, 0x7f, 0xba, 0x90, 0xdd, 0x58, 0x0e,
		0x3d, 0x8d, 0x3a, 0x10, 0x64, 0xa4, 0x05, 0x4b, 0x2c, 0xd0, 0xbb, 0x94,
		0x31, 0xc0, 0xf7, 0xd5, 0x87, 0x87, 0x81, 0xd9, 0x5d, 0xc4, 0x40, 0x18,
		0x1a, 0x1d, 0x17, 0xe2, 0x03, 0x5f, 0xdf, 0xa4, 0x13, 0x09, 0xb4, 0xce,
		0x92, 0xcc, 0xfd, 0xef, 0xc7, 0xcb };

uint8_t test_icc_pub_key [] = { 0x00, 0xc7, 0xb3, 0xf1, 0xf5, 0x0b, 0xb8, 0x58,
		0xa2, 0xf6, 0xbd, 0x3b, 0xef,
		0x32, 0x6a, 0xc7, 0x57, 0x01, 0x1c, 0xdd, 0x0e, 0xf9, 0xc1, 0xb3, 0x98,
		0x11, 0x48, 0x19, 0xcd, 0xcc, 0x93, 0x3c, 0xae, 0x1a, 0xd5, 0x1f, 0x6c,
		0x1b, 0x2a, 0x1f, 0x61, 0x2e, 0x86, 0x63, 0xce, 0x39, 0xd3, 0x14, 0xd3,
		0xa2, 0xff, 0xb0, 0x87, 0x72, 0xdf, 0xa7, 0x1a, 0x30, 0x12, 0xa0, 0x6b,
		0xf5, 0x57, 0xef, 0x9c, 0x18, 0xfa, 0x75, 0xd4, 0x3f, 0x05, 0x5e, 0x91,
		0x5a, 0xa0, 0x3a, 0x7c, 0x7a, 0xa2, 0xf2, 0x73, 0xab, 0xf6, 0x99, 0xc8,
		0x86, 0xbf, 0xa4, 0x83, 0x83, 0xd1, 0x9f, 0x2e, 0x93, 0x1f, 0x5a, 0xc9,
		0x6f, 0x98, 0x44, 0x80, 0xd1, 0xea, 0x91, 0xdc, 0x03, 0x32, 0x06, 0xef,
		0x03, 0x4d, 0x62, 0x10, 0x97, 0xcc, 0x0f, 0xf4, 0x88, 0xc8, 0x1e, 0x02,
		0xc8, 0x11, 0x1d, 0x26, 0xd0, 0x80, 0x34, 0x57 };

uint8_t test_icc_priv_key[] = { 0x00, 0x85, 0x22, 0xa1, 0x4e, 0x07, 0xd0, 0x3b,
		0x17, 0x4f, 0x28, 0xd2, 0x9f, 0x76, 0xf1, 0xda, 0x3a, 0x00, 0xbd, 0xe8,
		0xb4, 0xa6, 0x81, 0x22, 0x65, 0x60, 0xda, 0xbb, 0xde, 0x88, 0x62, 0x28,
		0x74, 0x11, 0xe3, 0x6a, 0x48, 0x12, 0x1c, 0x14, 0xeb, 0x74, 0x59, 0x97,
		0xde, 0xd1, 0x37, 0x63, 0x37, 0xc1, 0xff, 0xcb, 0x04, 0xf7, 0x3f, 0xc4,
		0xbc, 0x20, 0x0c, 0x6a, 0xf2, 0xa3, 0x8f, 0xf5, 0x11, 0x8c, 0x98, 0x84,
		0xe5, 0x61, 0x11, 0xda, 0x3e, 0xb5, 0xde, 0x52, 0xe4, 0xbd, 0x8b, 0x70,
		0x57, 0xe4, 0xeb, 0xbd, 0x04, 0xe7, 0xe1, 0xd9, 0xf7, 0xad, 0x4d, 0x05,
		0x7f, 0xab, 0xea, 0x41, 0x3e, 0xbf, 0xe4, 0x5e, 0x58, 0xab, 0x7b, 0x02,
		0x99, 0x34, 0xef, 0x96, 0x86, 0x2f, 0xf2, 0xa6, 0x1d, 0xcc, 0xf8, 0xb6,
		0xc0, 0xc3, 0x7b, 0xe6, 0x28, 0xd7, 0xed, 0x20, 0x96, 0x42, 0x6b, 0x55,
		0x0b };

uint8_t test_icc_dyn_data[] = {0xFE, 0xEB, 0xDA, 0xED};
uint8_t test_term_dyn_data[] = {0xDE, 0xAD, 0xBE, 0xEF};

uint8_t test_ipk_1_cert[] = { 0x3C, 0x96, 0xF7, 0x65, 0x8F, 0xBC, 0x29, 0xA2, 0x02,
		0xF1, 0x91, 0x46, 0xBD, 0xE9, 0x21, 0x66, 0xB0, 0xF6, 0x22, 0x1B, 0xBC,
		0xCB, 0x02, 0xE3, 0x26, 0x71, 0x0B, 0x9E, 0x22, 0x9D, 0x16, 0xFA, 0xE9,
		0xAD, 0x0C, 0x87, 0x4C, 0x06, 0x85, 0x91, 0x6E, 0x19, 0xF0, 0xE3, 0x26,
		0x93, 0xEE, 0x20, 0x1B, 0xCE, 0x23, 0x59, 0x50, 0x9A, 0x6D, 0x65, 0x72,
		0xF8, 0xEC, 0x3F, 0xC3, 0x73, 0x12, 0x6B, 0x34, 0x3F, 0x9C, 0xB8, 0x15,
		0x3D, 0x61, 0xB7, 0xEA, 0xB2, 0xD4, 0x2D, 0xE1, 0x9D, 0x56, 0x08, 0x31,
		0x85, 0xA0, 0x3D, 0xD1, 0x4C, 0x26, 0x8D, 0x40, 0xDF, 0x08, 0x35, 0xC5,
		0x5E, 0xAB, 0xFA, 0x38, 0xED, 0x28, 0xBC, 0xE4, 0x2C, 0xD0, 0x01, 0x3D,
		0xA9, 0x4F, 0x80, 0x05, 0x18, 0xB7, 0x53, 0xC2, 0x46, 0xEF, 0xFB, 0xA0,
		0x8F, 0xD2, 0x02, 0x9B, 0xAD, 0x5D, 0xFC, 0xF0, 0xDA, 0xF0, 0x7B, 0x7D,
		0x80, 0x1C, 0x46, 0x5F, 0xFD, 0x25, 0x2C, 0x70, 0xB9, 0x21, 0x53, 0xB3,
		0x30, 0xD9, 0x5D, 0xCA, 0x2F, 0xA1, 0xFA, 0xAE, 0x2D, 0x01, 0x68, 0xA4,
		0xEA, 0x8B, 0x47, 0x5C, 0xD8, 0x05, 0xDC, 0x32, 0xAA, 0x96, 0x4C, 0x17,
		0xBF, 0xCD, 0x2C, 0xD5, 0xD0, 0x30, 0x9A, 0xB0, 0xEA, 0x76, 0x1B };

uint8_t test_ipk_1_remainder[] = { 0x50, 0xDA, 0x20, 0xDD, 0xA8, 0x95, 0x3B,
		0x69, 0x3F, 0xED, 0x84, 0x36, 0x68, 0x31, 0xBA, 0x1E, 0xEA, 0x97, 0xF7,
		0x8F, 0x79, 0x2A, 0xCF, 0x8C, 0xB9, 0x8F, 0xDF, 0x01, 0x49, 0xA7, 0xB7,
		0x8F, 0xDA, 0x1C, 0x49, 0x67 };

uint8_t test_icc_1_cert[] = { 0xA7, 0x0D, 0x8D, 0x44, 0xD2, 0x23, 0xB4, 0x09,
		0xC4, 0xBE, 0x6B, 0x76, 0xF5, 0xC4, 0xD5, 0xD7, 0x8A, 0xCB, 0x53, 0x7C,
		0xAD, 0x32, 0x0E, 0x96, 0x83, 0xB6, 0x0E, 0x0B, 0xE5, 0xF8, 0x50, 0x2C,
		0xB0, 0x9F, 0xC3, 0xE8, 0x8E, 0xE4, 0x4E, 0xD3, 0x66, 0x88, 0x5C, 0x33,
		0x74, 0x98, 0x07, 0x3E, 0xE8, 0x7B, 0x3C, 0x09, 0xF0, 0x71, 0xA5, 0xA9,
		0xAC, 0x96, 0x8B, 0x92, 0xAD, 0x25, 0xFD, 0x82, 0x52, 0xA1, 0x6A, 0xC9,
		0x47, 0x47, 0xA8, 0x7C, 0x4F, 0xB9, 0x58, 0x0E, 0xD8, 0x28, 0x64, 0xA7,
		0x45, 0x86, 0xFD, 0x7A, 0x49, 0xA0, 0x80, 0xB7, 0x6B, 0x7E, 0xFA, 0xDF,
		0xBD, 0xFC, 0x98, 0x55, 0x97, 0xA8, 0xE4, 0xDC, 0x20, 0x67, 0x02, 0xB5,
		0x06, 0xF3, 0xD5, 0xE6, 0x25, 0x37, 0x1D, 0x25, 0xCC, 0x2D, 0x53, 0x77,
		0xE0, 0x07, 0x1D, 0xCE, 0x0E, 0xE8, 0xBF, 0x25, 0x6F, 0xB3, 0x9F, 0xAD,
		0xA4, 0xCA, 0x4F, 0x9F, 0x3B, 0x50, 0xB7, 0x2A, 0x44, 0x17, 0xF2, 0x84,
		0x69, 0x41, 0x53, 0xC2, 0x78, 0xA5, 0xD2, 0x9B, 0xD1, 0xD7, 0xE7, 0xD4,
		0xEA, 0xD3, 0xD5, 0x43, 0x96, 0x07, 0xBA, 0x51, 0x2D, 0x73, 0x0D, 0x7D,
		0x4E, 0x12, 0xB2, 0x32, 0x5D, 0x7D, 0x62, 0x40, 0x8D, 0xCD, 0xFD, 0x86 };

uint8_t test_arqc_data[] = { 0x00, 0x00, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x08, 0x40, 0x18, 0x12, 0x00, 0x01, 0x06, 0x01, 0x12,
		0x03, 0x00, 0x00, 0x00, 0x00 };

uint8_t test_sda_data[] = { 0x5A, 0x08, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67,
		0x89, 0x5F, 0x24, 0x03, 0x32, 0x12, 0x31 };

EMV_ISSUER_RECOVERY_TEST emv_issuer_recovery_tests[] = {
	{0x92,
	test_ipk_1_cert, sizeof(test_ipk_1_cert),
	test_ipk_1_remainder, sizeof(test_ipk_1_remainder),
	RSA_3},

};

EMV_ISSUER_E2E_TEST emv_issuer_e2e_tests[] = {
	{0x01, /* CA key index*/
		{ 0x40, 0x58, 0x25, 0xFF}, /* issuer identifier */
		{ 0x10, 0x25 }, /* certificate expiry date */
		{ 0x12, 0x34, 0x45}, /* certificate serial number */
		RSA_3, /* issuer public exponent */
		1, /* exponent length */
		test_issuer_pub_key,
		sizeof(test_issuer_pub_key)
	}
};

EMV_ICC_E2E_TEST emv_icc_e2e_tests[] = { {
	{ 0x12, 0x34, 0x56, 0x78, 0x90, 0x12,
		0x34, 0x56, 0x78, 0x90 }, 	/* application PAN */
	{ 0x12, 0x30 },  				/* certificate expiry */
	{ 0x11, 0x22, 0x33}, 			/* certificate serial number */
	test_icc_pub_key,					/* ICC public key */
	sizeof(test_icc_pub_key),			/* ICC public key len */
	RSA_3,							/* ICC exponent */
	1,								/* ICC exponent len */
	test_issuer_pub_key+1,			/* Issuer public key - working around the leading zero*/
	sizeof(test_issuer_pub_key)-1,	/* Issuer public key len */
	test_issuer_pri_key,			/* Issuer private key */
	sizeof(test_issuer_pri_key) ,	/* Issuer private key len */
	RSA_3,							/* Issuer exponent */
	1,								/* Issuer exponent len */
	NULL,
	0}
};

//uint8_t arc[2];
//uint8_t csu[4];
//uint8_t pad[8];
//size_t pad_len;
EMV_MASTER_SESSION_KEY_TEST emv_master_session_key_tests [] = {
	{
		{9,0,1,2,3,4,5,4,3,2,1,0,1,2,3,2}, /* PAN */
		16, 							   /* PAN length */
		{0, 1},							  /* CSN */
		{ 0xDF,0xAD,0xBF,0xEF,0x01,0x23,0x45,0x67,0x89,0x86,0x64,0x43,0xDF,0xAD,0xBF,0xEF }, /* key */
		16,
		ALGORITHM_TDES,
		16,
		{ 0, 1 },
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0, 0},
		0
	},
	{
		{9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9}, /* PAN */
		19, 							   		 /* PAN length */
		{0, 1},									 /* CSN */
		{ 0xDF,0xAD,0xBF,0xEF,0x01,0x23,0x45,0x67,0x89,0x86,0x64,0x43,0xDF,0xAD,0xBF,0xEF }, /* key */
		16,
		ALGORITHM_TDES,
		16,
		{ 0, 0 },
		{ 0xAD, 0x13, 0xBE, 0x45},
		{ 0x12, 0x34, 0, 0, 0, 0, 0},
		2
	},
	{
		{9,0,1,2,3,4,5,4,3,2,1,0,1,2,3,2}, /* PAN */
		16, 							   /* PAN length */
		{0, 1},							  /* CSN */
		{ 0xDF,0xAD,0xBF,0xEF,0x01,0x23,0x45,0x67,0x89,0x86,0x64,0x43,0xDF,0xAD,0xBF,0xEF }, /* key */
		16,
		ALGORITHM_AES,
		16,
		{ 0, 1 },
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0, 0},
		0
	},
	{
		{9,0,1,2,3,4,5,4,3,2,1,0,1,2,3,2}, /* PAN */
		16, 							   /* PAN length */
		{0, 1},							  /* CSN */
		{ 0xDF,0xAD,0xBF,0xEF,0x01,0x23,0x45,0x67,0x89,0x86,0x64,0x43,0xDF,0xAD,0xBF,0xEF }, /* key */
		16,
		ALGORITHM_AES,
		24,
		{ 0, 0 },
		{ 0xAD, 0x13, 0xBE, 0x45},
		{ 0x12, 0x34, 0, 0, 0, 0, 0},
		2
	},
	{
		{9,0,1,2,3,4,5,4,3,2,1,0,1,2,3,2}, /* PAN */
		16, 							   /* PAN length */
		{0, 1},							  /* CSN */
		{ 0xDF,0xAD,0xBF,0xEF,0x01,0x23,0x45,0x67,0x89,0x86,0x64,0x43,0xDF,0xAD,0xBF,0xEF }, /* key */
		16,
		ALGORITHM_AES,
		32,
		{ 0, 0 },
		{ 0xAD, 0x13, 0xBE, 0x45},
		{ 0x12, 0x34, 0, 0, 0, 0, 0},
		2
	}
};

void test_issuer_pk() {
	for (size_t c = 0; c < sizeof(emv_issuer_recovery_tests)/sizeof(EMV_ISSUER_RECOVERY_TEST); c++) {
		EMV_ISSUER_RECOVERY_TEST *p = emv_issuer_recovery_tests + c;

		ISSUER_PK_DETAILS_HEADER header;
		uint8_t issuer_pk_buffer[EMV_MAX_CA_KEY_SIZE];

		emv_recover_issuer_public_key(p->ca_pki, p->issuer_pk_cert, p->issuer_pk_cert_len,
				p->issuer_pk_remainder, p->issuer_pk_remainder_len,
				p->issuer_pk_exponent,
				issuer_pk_buffer, &header);

		printf("\n\tKey data header:\n");
		print_issuer_pk_details_header(&header);
	}
}

void test_issuer_e2e() {
	for (size_t c = 0; c < sizeof(emv_issuer_e2e_tests)/sizeof(EMV_ISSUER_E2E_TEST); c++) {
		EMV_ISSUER_E2E_TEST *p=emv_issuer_e2e_tests+c;

		uint8_t output_cert[EMV_MAX_CA_KEY_SIZE];
		size_t output_cert_len;
		uint8_t output_remainder[EMV_MAX_CA_KEY_SIZE];
		size_t output_remainder_len;
		ISSUER_PK_DETAILS_HEADER header;
		print_test_step(1, "Signing issuer certificate");

		memcpy(&header.issuer_identifier, p->issuer_identifier, sizeof(p->issuer_identifier));
		memcpy(&header.certificate_expiration, p->certificate_expiration, sizeof(p->certificate_expiration));
		memcpy(&header.certificate_serial, p->certificate_serial, sizeof(p->certificate_serial));
		header.issuer_pk_exponent_len = p->issuer_pk_exponent_len;
		header.issuer_pk_len = p->issuer_pk_len;

		emv_sign_issuer_public_key(p->ca_pki, p->issuer_pk, p->issuer_pk_exponent, &header, output_cert, &output_cert_len,
				output_remainder, &output_remainder_len);


		printf("\n\n");
		print_array("Output certificate: ", output_cert, output_cert_len, "\n");
		printf("\n\n");
		print_array("Output remainder: ", output_remainder, output_remainder_len, "\n");

		print_test_step(2, "Recovering issuer PK");

		uint8_t recovered_pk[4096];
		ISSUER_PK_DETAILS_HEADER recovered_header;

		memset(recovered_pk, 0, 4096);

		emv_recover_issuer_public_key(p->ca_pki, output_cert, output_cert_len,
				output_remainder,
				output_remainder_len, p->issuer_pk_exponent, recovered_pk,
				&recovered_header);
		printf("\n\nOriginal data header:\n");
		print_issuer_pk_details_header(&header);

		printf("\n\nRecovered data header:\n");
		print_issuer_pk_details_header(&recovered_header);

		print_array("Recovered key:  ", recovered_pk, p->issuer_pk_len, "\n");
	}
}

void test_icc_e2e () {
	for (size_t c = 0; c < sizeof(emv_icc_e2e_tests)/sizeof(EMV_ICC_E2E_TEST); c++) {
		EMV_ICC_E2E_TEST *p = emv_icc_e2e_tests + c;
		print_test_step(1, "Sign the ICC certificate");

		uint8_t icc_cert [EMV_MAX_CA_KEY_SIZE]; /* this is a bit too long on purpose*/
		size_t icc_cert_len;

		uint8_t icc_cert_remainder [ EMV_MAX_CA_KEY_SIZE];
		size_t icc_cert_remainder_len;

		ICC_PK_DETAILS_HEADER details;
		memcpy(&details.applicaton_pan, p->application_pan, sizeof(details.applicaton_pan));
		memcpy(&details.certificate_expiration, p->certificate_expiration, sizeof(details.certificate_expiration));
		memcpy(&details.certificate_serial, p->certificate_serial, sizeof(details.certificate_serial));
		details.icc_pk_exponent_len = p->icc_pk_exponent_len;
		details.icc_pk_len = p->icc_pk_len;

		print_icc_pk_details_header(&details);

		print_array("ICC public key: \t\t", p->icc_pk, p->icc_pk_len, "\n");
		icc_cert_len = emv_sign_icc_public_key(p->issuer_pk, p->issuer_pk_len,
				p->issuer_pk_exponent, p->issuer_priv, p->issuer_priv_len,
				p->icc_pk,  p->icc_pk_exponent, &details,
				p->auth_data, p->auth_data_len, icc_cert,
				icc_cert_remainder, &icc_cert_remainder_len);

		print_test_step(2, "Recover the ICC PK");

		uint8_t recovered_icc_key[EMV_MAX_CA_KEY_SIZE];
		ICC_PK_DETAILS_HEADER recovered_header;
		emv_recover_icc_key(icc_cert, icc_cert_len, icc_cert_remainder, icc_cert_remainder_len,
				p->icc_pk_exponent, p->issuer_pk, p->issuer_pk_len,
				p->issuer_pk_exponent, recovered_icc_key, &recovered_header, p->auth_data, p->auth_data_len);
	}
}

void test_sda() {
	SDA_DETAILS_HEADER sda_details;
	sda_details.data_auth_code[0] = 0xBE;
	sda_details.data_auth_code[1] = 0xEF;

	print_test_step(1, "Signing the SDA");
	uint8_t buffer[EMV_LARGE_BUFFER];
	int buffer_size = emv_sign_static_data(test_issuer_pub_key+1, sizeof(test_issuer_pub_key)-1, RSA_3,
			test_issuer_pri_key, sizeof(test_issuer_pri_key), test_sda_data, sizeof(test_sda_data), &sda_details,
			buffer);
	if (buffer_size > 0){
		print_array("\t\tSDA result:       ", buffer, buffer_size, "\n");
	}
	print_test_step(2, "Validating the SDA");

	emv_validate_sda(buffer, test_issuer_pub_key+1, sizeof(test_issuer_pub_key)-1, RSA_3,
			test_sda_data, sizeof(test_sda_data), &sda_details);
}

void test_dda() {
	DDA_DETAILS_HEADER dda_details;
	dda_details.dd_len = sizeof(test_icc_dyn_data);

	print_test_step(1, "Signing the DDA");

	uint8_t buffer[EMV_LARGE_BUFFER];
	int buffer_size = emv_sign_dynamic_data(test_icc_pub_key+1, sizeof(test_icc_pub_key)-1, RSA_3,
			test_icc_priv_key, sizeof(test_icc_priv_key), test_icc_dyn_data, test_term_dyn_data, sizeof(test_term_dyn_data),
			&dda_details, buffer);

	print_array("\t\tDDA result:       ", buffer, buffer_size, "\n");
	print_test_step(2, "Validating the DDA");

	emv_validate_dda(buffer, buffer_size, test_icc_pub_key+1, sizeof(test_icc_pub_key)-1,
			RSA_3, test_term_dyn_data, sizeof(test_term_dyn_data), &dda_details);
}

void test_master_session_key_derivation(EMV_MASTER_SESSION_KEY_TEST *p) {
	uint8_t master[AES_KEY_LENGTH_3];
	uint8_t atc[2] = {0,1};

	print_test_step(1, "Master key derivation");
	emv_derive_icc_master_key(p->unpacked_pan, p->unpacked_pan_len,
			p->unpacked_csn, p->master_key, p->master_key_len, p->algorithm, master, p->output_len);

	print_array("\tOutput: ", master, p->output_len, "\n");

	print_test_step(2, "Session key derivation");

	uint8_t session[AES_KEY_LENGTH_3];
	emv_derive_icc_session_key(master, p->output_len, p->algorithm, atc, session, p->output_len);
	print_array("\tSession key: ", session, p->output_len, "\n");

	uint8_t arqc[EMV_ARQC_MAX_LEN];
	print_test_step(3, "ARQC generation");
	emv_generate_arqc(session, p->output_len, p->algorithm, test_arqc_data, sizeof(test_arqc_data), arqc, EMV_ARQC_MAX_LEN);
	print_array("\tARQC: ", arqc, EMV_ARQC_MAX_LEN, "\n");

	uint8_t arpc[EMV_ARPC_MAX_LEN];
	uint8_t *arc;
	print_test_step(4, "ARPC generation");

	if (p->arc[1] == 0) {/* use this as an indicator to use method 2 */
		arc = NULL;
		print_array("\tMethod 2:\n\t\tCSU: ", p->csu, EMV_ARPC_CSU_LEN, "\n");
		print_array("\t\tPAD: ", p->pad, p->pad_len, "\n");
	}
	else {
		arc = &p->arc[0];
		print_array("\tMethod 1:\n\t\tARC: ", arc, 2, "\n");
	}
	int arpc_len = emv_generate_arpc(arqc, arc, p->csu, p->pad, p->pad_len, p->algorithm, session, p->output_len, arpc);
	print_array("\tARPC: ", arpc, arpc_len, "\n");

}

TEST emv_tests[] = {
	{"Issuer PK recovery", test_issuer_pk, NULL},
	{"Issuer PK signing and recovery", test_issuer_e2e, NULL },
	{"ICC PK signing and recovery", test_icc_e2e, NULL},
	{"SDA signing and recovery", test_sda, NULL},
	{"DDA signing and recovery", test_dda, NULL},
	{"Master, session, ARQC for TDES, 16-digit PAN",
			(test_f)test_master_session_key_derivation, (void*)&(emv_master_session_key_tests[0])},
	{"Master, session, ARQC for TDES, 19-digit PAN",
			(test_f)test_master_session_key_derivation, (void*)&(emv_master_session_key_tests[1])},
	{"Master, session, ARQC for AES, 16-digit PAN, 128-bit key output",
			(test_f)test_master_session_key_derivation, (void*)&(emv_master_session_key_tests[2])},
	{"Master, session, ARQC for AES, 16-digit PAN, 192-bit key output",
			(test_f)test_master_session_key_derivation, (void*)&(emv_master_session_key_tests[3])},
	{"Master, session, ARQC for AES, 16-digit PAN, 256-bit key output",
			(test_f)test_master_session_key_derivation, (void*)&(emv_master_session_key_tests[4])}
};



int main (void) {

	for (size_t c = 0; c< sizeof(emv_tests)/sizeof(TEST); c++)
		run_test(emv_tests+c);
}

