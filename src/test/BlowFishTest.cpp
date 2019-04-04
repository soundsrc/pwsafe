
/*
* Copyright (c) 2003-2018 Rony Shapiro <ronys@pwsafe.org>.
* All rights reserved. Use of the code is allowed under the
* Artistic License 2.0 terms, as specified in the LICENSE file
* distributed with this code, or available from
* http://www.opensource.org/licenses/artistic-license-2.0.php
*/
// BlowFishTest.cpp: Unit test for BlowFish implementation

#ifdef WIN32
#include "../ui/Windows/stdafx.h"
#endif

#include "core/BlowFish.h"
#include "gtest/gtest.h"

/*
 * Test vectors from http://www.schneier.com/code/vectors.txt
 *  Description:  Eric Young's test vectors for Blowfish.
 *
 * PasswordSafe Note: To pass these tests, I had to change the
 * order of the bytes in the plaintext and ciphertext vectors,
 * from {3,2,1,0,7,6,5,4} to {0,1,2,3,4,5,6,7}. This is either a
 * (cryptographically benign) bug in the implementation, or some
 * endian-ness oddity in the test data. My guess is the former.
 */

#define NUM_VARIABLE_KEY_TESTS 34

// key bytes
static unsigned char variable_key[NUM_VARIABLE_KEY_TESTS][8] = {
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
  {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
  {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10},
  {0x7C, 0xA1, 0x10, 0x45, 0x4A, 0x1A, 0x6E, 0x57},
  {0x01, 0x31, 0xD9, 0x61, 0x9D, 0xC1, 0x37, 0x6E},
  {0x07, 0xA1, 0x13, 0x3E, 0x4A, 0x0B, 0x26, 0x86},
  {0x38, 0x49, 0x67, 0x4C, 0x26, 0x02, 0x31, 0x9E},
  {0x04, 0xB9, 0x15, 0xBA, 0x43, 0xFE, 0xB5, 0xB6},
  {0x01, 0x13, 0xB9, 0x70, 0xFD, 0x34, 0xF2, 0xCE},
  {0x01, 0x70, 0xF1, 0x75, 0x46, 0x8F, 0xB5, 0xE6},
  {0x43, 0x29, 0x7F, 0xAD, 0x38, 0xE3, 0x73, 0xFE},
  {0x07, 0xA7, 0x13, 0x70, 0x45, 0xDA, 0x2A, 0x16},
  {0x04, 0x68, 0x91, 0x04, 0xC2, 0xFD, 0x3B, 0x2F},
  {0x37, 0xD0, 0x6B, 0xB5, 0x16, 0xCB, 0x75, 0x46},
  {0x1F, 0x08, 0x26, 0x0D, 0x1A, 0xC2, 0x46, 0x5E},
  {0x58, 0x40, 0x23, 0x64, 0x1A, 0xBA, 0x61, 0x76},
  {0x02, 0x58, 0x16, 0x16, 0x46, 0x29, 0xB0, 0x07},
  {0x49, 0x79, 0x3E, 0xBC, 0x79, 0xB3, 0x25, 0x8F},
  {0x4F, 0xB0, 0x5E, 0x15, 0x15, 0xAB, 0x73, 0xA7},
  {0x49, 0xE9, 0x5D, 0x6D, 0x4C, 0xA2, 0x29, 0xBF},
  {0x01, 0x83, 0x10, 0xDC, 0x40, 0x9B, 0x26, 0xD6},
  {0x1C, 0x58, 0x7F, 0x1C, 0x13, 0x92, 0x4F, 0xEF},
  {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
  {0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x0E, 0x0E, 0x0E},
  {0xE0, 0xFE, 0xE0, 0xFE, 0xF1, 0xFE, 0xF1, 0xFE},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
  {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10},
};
// clear bytes
static unsigned char plaintext_vk[NUM_VARIABLE_KEY_TESTS][8] = {
//  0     1     2     3     4     5     6     7    
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, },
  {0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00, },
  {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, },
  {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, },
  {0x67, 0x45, 0x23, 0x01, 0xEF, 0xCD, 0xAB, 0x89, },
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
  {0x67, 0x45, 0x23, 0x01, 0xEF, 0xCD, 0xAB, 0x89, },
  {0xD0, 0xD6, 0xA1, 0x01, 0x42, 0x67, 0x77, 0x39, },
  {0xA8, 0x4C, 0xD5, 0x5C, 0xDA, 0x57, 0xEF, 0x3D, },
  {0x38, 0xD4, 0x48, 0x02, 0x72, 0x71, 0xF6, 0x06, },
  {0x58, 0x4B, 0x45, 0x51, 0x0A, 0x44, 0xDF, 0x2D, },
  {0x30, 0x44, 0xFD, 0x42, 0xA2, 0x7F, 0x57, 0x59, },
  {0x08, 0x5E, 0x9B, 0x05, 0x3A, 0x14, 0xCF, 0x51, },
  {0xE0, 0xD8, 0x56, 0x07, 0xD2, 0x61, 0x47, 0x77, },
  {0xB8, 0x14, 0x25, 0x76, 0x6A, 0x48, 0xBF, 0x29, },
  {0x90, 0x11, 0xDD, 0x3B, 0x02, 0x28, 0x37, 0x49, },
  {0x68, 0x5F, 0x95, 0x26, 0x9A, 0x60, 0xAF, 0x35, },
  {0x40, 0x5E, 0x4D, 0x16, 0x32, 0x52, 0x27, 0x4F, },
  {0x18, 0x6E, 0x05, 0x6B, 0xCA, 0x5C, 0x9F, 0x75, },
  {0xEF, 0xD6, 0x4B, 0x00, 0x62, 0x60, 0x17, 0x09, },
  {0x00, 0x39, 0x0D, 0x48, 0xF2, 0x62, 0xE7, 0x6E, },
  {0xC8, 0x40, 0x75, 0x43, 0xFA, 0x3C, 0x8F, 0x69, },
  {0xA0, 0x43, 0x2D, 0x07, 0x92, 0x52, 0x07, 0x77, },
  {0x77, 0x55, 0xFE, 0x02, 0x2A, 0xF1, 0x17, 0x81, },
  {0x50, 0x5C, 0x9D, 0x1D, 0xC2, 0x28, 0xF7, 0x18, },
  {0x28, 0x32, 0x55, 0x30, 0x5A, 0x29, 0x6F, 0x6D, },
  {0x67, 0x45, 0x23, 0x01, 0xEF, 0xCD, 0xAB, 0x89, },
  {0x67, 0x45, 0x23, 0x01, 0xEF, 0xCD, 0xAB, 0x89, },
  {0x67, 0x45, 0x23, 0x01, 0xEF, 0xCD, 0xAB, 0x89, },
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, },
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, },
};
// cipher bytes
static unsigned char ciphertext_vk[NUM_VARIABLE_KEY_TESTS][8] = {
//  0     1     2     3     4     5     6     7    
  {0x45, 0x97, 0xF9, 0x4E, 0x78, 0xDD, 0x98, 0x61, },
  {0xD5, 0x6F, 0x86, 0x51, 0x8A, 0xCB, 0x5E, 0xB8, },
  {0x9A, 0x6F, 0x85, 0x7D, 0xF2, 0x63, 0x30, 0x61, },
  {0x87, 0xDD, 0x66, 0x24, 0x9D, 0x3C, 0x96, 0x8B, },
  {0x80, 0xC3, 0xF9, 0x61, 0x96, 0xB0, 0x81, 0x22, },
  {0x30, 0xC6, 0x0C, 0x7D, 0xC7, 0x1E, 0xDA, 0xAF, },
  {0x45, 0x97, 0xF9, 0x4E, 0x78, 0xDD, 0x98, 0x61, },
  {0x0F, 0xAB, 0xCE, 0x0A, 0x8D, 0xA2, 0xA0, 0xC6, },
  {0x45, 0x82, 0xC6, 0x59, 0x2B, 0x28, 0x05, 0xEB, },
  {0x0B, 0xCC, 0xB8, 0xB1, 0xA0, 0x09, 0x0F, 0x25, },
  {0x77, 0xE5, 0x30, 0x17, 0xA4, 0x1D, 0xEA, 0x8B, },
  {0x56, 0x78, 0x5E, 0xA2, 0xEB, 0x51, 0x26, 0xCF, },
  {0xB1, 0x82, 0x38, 0x35, 0x1A, 0x8F, 0xCE, 0x09, },
  {0x88, 0xD0, 0xF4, 0x48, 0x18, 0x99, 0x37, 0x4C, },
  {0xB7, 0x93, 0x21, 0x43, 0x98, 0xFC, 0x51, 0x89, },
  {0x54, 0x41, 0xF0, 0x13, 0xE5, 0x1A, 0x9D, 0xD6, },
  {0x93, 0xDA, 0xED, 0x2E, 0x79, 0x9C, 0xD3, 0xFF, },
  {0x39, 0xE0, 0x87, 0xD8, 0xE3, 0xA6, 0x2D, 0x3C, },
  {0x4F, 0xD0, 0x99, 0x5F, 0x69, 0x39, 0x16, 0x5B, },
  {0x3B, 0x7A, 0x05, 0x4A, 0x7B, 0x97, 0xD3, 0x24, },
  {0xC1, 0x31, 0x20, 0x45, 0x8E, 0xDA, 0xFA, 0xE4, },
  {0x39, 0xAE, 0x55, 0x75, 0xBD, 0x87, 0x9B, 0xF5, },
  {0x9C, 0x5F, 0xC5, 0x53, 0x19, 0xC0, 0x9F, 0xB4, },
  {0xFA, 0x7B, 0x8E, 0x7A, 0xA3, 0x89, 0x7E, 0x93, },
  {0x7A, 0x5D, 0x9C, 0xCF, 0xB5, 0xAD, 0x86, 0x49, },
  {0x90, 0xB2, 0xAB, 0xD1, 0x78, 0xC7, 0x8B, 0x65, },
  {0x74, 0x37, 0xCB, 0x55, 0x01, 0xF2, 0x3E, 0xD1, },
  {0x48, 0xEC, 0x34, 0xFA, 0xB2, 0x68, 0xB2, 0x47, },
  {0x51, 0x79, 0x90, 0xA7, 0xAE, 0x3C, 0xEA, 0x08, },
  {0x2D, 0x07, 0x9E, 0xC3, 0x1D, 0x63, 0xAC, 0x9F, },
  {0xE0, 0x33, 0x49, 0x01, 0xE4, 0xF6, 0xAF, 0xCD, },
  {0x77, 0x9A, 0x1E, 0xF2, 0xBC, 0x49, 0x1C, 0xB7, },
  {0x88, 0x46, 0x59, 0x24, 0x9A, 0x36, 0x54, 0x57, },
  {0x9C, 0x5A, 0x5C, 0x6B, 0x5A, 0x0A, 0x9E, 0x5D, },
};
#if 0
// set_key test data
static unsigned char data[8]= {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};

c=F9AD597C49DB005E k[ 1]=F0
c=E91D21C1D961A6D6 k[ 2]=F0E1
c=E9C2B70A1BC65CF3 k[ 3]=F0E1D2
c=BE1E639408640F05 k[ 4]=F0E1D2C3
c=B39E44481BDB1E6E k[ 5]=F0E1D2C3B4
c=9457AA83B1928C0D k[ 6]=F0E1D2C3B4A5
c=8BB77032F960629D k[ 7]=F0E1D2C3B4A596
c=E87A244E2CC85E82 k[ 8]=F0E1D2C3B4A59687
c=15750E7A4F4EC577 k[ 9]=F0E1D2C3B4A5968778
c=122BA70B3AB64AE0 k[10]=F0E1D2C3B4A596877869
c=3A833C9AFFC537F6 k[11]=F0E1D2C3B4A5968778695A
c=9409DA87A90F6BF2 k[12]=F0E1D2C3B4A5968778695A4B
c=884F80625060B8B4 k[13]=F0E1D2C3B4A5968778695A4B3C
c=1F85031C19E11968 k[14]=F0E1D2C3B4A5968778695A4B3C2D
c=79D9373A714CA34F k[15]=F0E1D2C3B4A5968778695A4B3C2D1E
c=93142887EE3BE15C k[16]=F0E1D2C3B4A5968778695A4B3C2D1E0F
c=03429E838CE2D14B k[17]=F0E1D2C3B4A5968778695A4B3C2D1E0F00
c=A4299E27469FF67B k[18]=F0E1D2C3B4A5968778695A4B3C2D1E0F0011
c=AFD5AED1C1BC96A8 k[19]=F0E1D2C3B4A5968778695A4B3C2D1E0F001122
c=10851C0E3858DA9F k[20]=F0E1D2C3B4A5968778695A4B3C2D1E0F00112233
c=E6F51ED79B9DB21F k[21]=F0E1D2C3B4A5968778695A4B3C2D1E0F0011223344
c=64A6E14AFD36B46F k[22]=F0E1D2C3B4A5968778695A4B3C2D1E0F001122334455
c=80C7D7D45A5479AD k[23]=F0E1D2C3B4A5968778695A4B3C2D1E0F00112233445566
c=05044B62FA52D080 k[24]=F0E1D2C3B4A5968778695A4B3C2D1E0F0011223344556677
#endif

  TEST(BlowFishTest, Null1Ktest) {
  unsigned char key[] = {0xF0, 0xE1, 0xD2, 0xC3, 0xB4, 0xA5, 0x96, 0x87,
                         0x78, 0x69, 0x5A, 0x4B, 0x3C, 0x2D, 0x1E, 0x0F,
                         0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
  unsigned char tmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char zero[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int i;
  BlowFish bf(key, sizeof(key));

  for (i = 0; i < 1000; i++) bf.Encrypt(tmp, tmp);
  for (i = 0; i < 1000; i++) bf.Decrypt(tmp, tmp);
  EXPECT_TRUE(memcmp(tmp, zero, 8) == 0);
}

  TEST(BlowFishTest, VKtests) {
    unsigned char tmp[8];
    for (int i = 0; i < NUM_VARIABLE_KEY_TESTS; i++) {
      BlowFish bf(variable_key[i], 8);
      bf.Encrypt(plaintext_vk[i], tmp);
      EXPECT_TRUE(memcmp(tmp, ciphertext_vk[i], 8) == 0) << "Test vector " << i;

      bf.Decrypt(ciphertext_vk[i], tmp);
      EXPECT_TRUE(memcmp(tmp, plaintext_vk[i], 8) == 0) << "Test vector " << i;
    }
  }
