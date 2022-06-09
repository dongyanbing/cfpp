#ifndef __CFPP_CRYPTO_H
#define __CFPP_CRYPTO_H


#define ALGORITHM_TDES 0
#define ALGORITHM_AES 1


#define TDES_KEY_LENGTH_1 8
#define TDES_KEY_LENGTH_2 16
#define TDES_KEY_LENGTH_3 24

#define TDES_BLOCK_SIZE 8

#define AES_KEY_LENGTH_1 16
#define AES_KEY_LENGTH_2 24
#define AES_KEY_LENGTH_3 32

#define VALID_AES_KEY_SIZE(x) ( (x)==AES_KEY_LENGTH_1 || (x)==AES_KEY_LENGTH_2 || (x)==AES_KEY_LENGTH_3)

#define AES_BLOCK_SIZE 16

#endif
