/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "crypto/cipher/aes.hpp"

#include "crypto/cipher/ecb.hpp"
#include "crypto/cipher/cbc.hpp"
#include "crypto/cipher/pcbc.hpp"
#include "crypto/cipher/cfb.hpp"
#include "crypto/cipher/ofb.hpp"
#include "crypto/cipher/ctr.hpp"

#include "crypto/padding/ansiX923.hpp"
#include "crypto/padding/iso7816_4Padding.hpp"
#include "crypto/padding/pkcs7Padding.hpp"
#include "crypto/padding/zeroPadding.hpp"


using anch::crypto::AES;

using anch::crypto::BlockCipherModeOfOperation;
using anch::crypto::ECB;
using anch::crypto::CBC;
using anch::crypto::PCBC;
using anch::crypto::CFB;
using anch::crypto::OFB;
using anch::crypto::CTR;

using anch::crypto::ZeroPadding;
using anch::crypto::ANSIX923;
using anch::crypto::ISO7816_4Padding;
using anch::crypto::PKCS7Padding;


/*! Cipher subsitution box */
const uint8_t anch::crypto::ANCH_AES_CIPHER_SBOX[256] = {
  0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,
  0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
  0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
  0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
  0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC,
  0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
  0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A,
  0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
  0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
  0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
  0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B,
  0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
  0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85,
  0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
  0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
  0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
  0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17,
  0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
  0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88,
  0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
  0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
  0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
  0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9,
  0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
  0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6,
  0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
  0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
  0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
  0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94,
  0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
  0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,
  0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

/*! Decipher subsitution box */
const uint8_t anch::crypto::ANCH_AES_DECIPHER_SBOX[256] = {
  0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38,
  0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
  0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87,
  0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
  0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D,
  0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
  0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2,
  0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
  0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16,
  0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
  0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA,
  0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
  0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A,
  0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
  0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02,
  0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
  0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA,
  0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
  0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85,
  0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
  0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89,
  0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
  0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20,
  0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
  0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31,
  0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
  0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D,
  0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
  0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0,
  0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26,
  0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

/*! Round constants */
const uint32_t anch::crypto::ANCH_AES_RCON[11] = {
  0x00000000, 0x00000001, 0x00000002, 0x00000004,
  0x00000008, 0x00000010, 0x00000020, 0x00000040,
  0x00000080, 0x0000001B, 0x00000036
};

// AES-128 defintion +
template class AES<4,10>;

template class BlockCipherModeOfOperation<ECB<AES<4,10>,ZeroPadding>,AES<4,10>>;
template class ECB<AES<4,10>,ZeroPadding>;
template class BlockCipherModeOfOperation<ECB<AES<4,10>,ANSIX923>,AES<4,10>>;
template class ECB<AES<4,10>,ANSIX923>;
template class BlockCipherModeOfOperation<ECB<AES<4,10>,ISO7816_4Padding>,AES<4,10>>;
template class ECB<AES<4,10>,ISO7816_4Padding>;
template class BlockCipherModeOfOperation<ECB<AES<4,10>,PKCS7Padding>,AES<4,10>>;
template class ECB<AES<4,10>,PKCS7Padding>;

template class BlockCipherModeOfOperation<CBC<AES<4,10>,ZeroPadding>,AES<4,10>>;
template class CBC<AES<4,10>,ZeroPadding>;
template class BlockCipherModeOfOperation<CBC<AES<4,10>,ANSIX923>,AES<4,10>>;
template class CBC<AES<4,10>,ANSIX923>;
template class BlockCipherModeOfOperation<CBC<AES<4,10>,ISO7816_4Padding>,AES<4,10>>;
template class CBC<AES<4,10>,ISO7816_4Padding>;
template class BlockCipherModeOfOperation<CBC<AES<4,10>,PKCS7Padding>,AES<4,10>>;
template class CBC<AES<4,10>,PKCS7Padding>;

template class BlockCipherModeOfOperation<PCBC<AES<4,10>,ZeroPadding>,AES<4,10>>;
template class PCBC<AES<4,10>,ZeroPadding>;
template class BlockCipherModeOfOperation<PCBC<AES<4,10>,ANSIX923>,AES<4,10>>;
template class PCBC<AES<4,10>,ANSIX923>;
template class BlockCipherModeOfOperation<PCBC<AES<4,10>,ISO7816_4Padding>,AES<4,10>>;
template class PCBC<AES<4,10>,ISO7816_4Padding>;
template class BlockCipherModeOfOperation<PCBC<AES<4,10>,PKCS7Padding>,AES<4,10>>;
template class PCBC<AES<4,10>,PKCS7Padding>;

template class BlockCipherModeOfOperation<CFB<AES<4,10>>,AES<4,10>>;
template class CFB<AES<4,10>>;

template class BlockCipherModeOfOperation<OFB<AES<4,10>>,AES<4,10>>;
template class OFB<AES<4,10>>;

template class BlockCipherModeOfOperation<CTR<AES<4,10>>,AES<4,10>>;
template class CTR<AES<4,10>>;
// AES-128 defintion -

// AES-192 defintion +
template class AES<6,12>;

template class BlockCipherModeOfOperation<ECB<AES<6,12>,ZeroPadding>,AES<6,12>>;
template class ECB<AES<6,12>,ZeroPadding>;
template class BlockCipherModeOfOperation<ECB<AES<6,12>,ANSIX923>,AES<6,12>>;
template class ECB<AES<6,12>,ANSIX923>;
template class BlockCipherModeOfOperation<ECB<AES<6,12>,ISO7816_4Padding>,AES<6,12>>;
template class ECB<AES<6,12>,ISO7816_4Padding>;
template class BlockCipherModeOfOperation<ECB<AES<6,12>,PKCS7Padding>,AES<6,12>>;
template class ECB<AES<6,12>,PKCS7Padding>;

template class BlockCipherModeOfOperation<CBC<AES<6,12>,ZeroPadding>,AES<6,12>>;
template class CBC<AES<6,12>,ZeroPadding>;
template class BlockCipherModeOfOperation<CBC<AES<6,12>,ANSIX923>,AES<6,12>>;
template class CBC<AES<6,12>,ANSIX923>;
template class BlockCipherModeOfOperation<CBC<AES<6,12>,ISO7816_4Padding>,AES<6,12>>;
template class CBC<AES<6,12>,ISO7816_4Padding>;
template class BlockCipherModeOfOperation<CBC<AES<6,12>,PKCS7Padding>,AES<6,12>>;
template class CBC<AES<6,12>,PKCS7Padding>;

template class BlockCipherModeOfOperation<PCBC<AES<6,12>,ZeroPadding>,AES<6,12>>;
template class PCBC<AES<6,12>,ZeroPadding>;
template class BlockCipherModeOfOperation<PCBC<AES<6,12>,ANSIX923>,AES<6,12>>;
template class PCBC<AES<6,12>,ANSIX923>;
template class BlockCipherModeOfOperation<PCBC<AES<6,12>,ISO7816_4Padding>,AES<6,12>>;
template class PCBC<AES<6,12>,ISO7816_4Padding>;
template class BlockCipherModeOfOperation<PCBC<AES<6,12>,PKCS7Padding>,AES<6,12>>;
template class PCBC<AES<6,12>,PKCS7Padding>;

template class BlockCipherModeOfOperation<CFB<AES<6,12>>,AES<6,12>>;
template class CFB<AES<6,12>>;

template class BlockCipherModeOfOperation<OFB<AES<6,12>>,AES<6,12>>;
template class OFB<AES<6,12>>;

template class BlockCipherModeOfOperation<CTR<AES<6,12>>,AES<6,12>>;
template class CTR<AES<6,12>>;
// AES-192 defintion -

// AES-256 defintion +
template class AES<8,14>;

template class BlockCipherModeOfOperation<ECB<AES<8,14>,ZeroPadding>,AES<8,14>>;
template class ECB<AES<8,14>,ZeroPadding>;
template class BlockCipherModeOfOperation<ECB<AES<8,14>,ANSIX923>,AES<8,14>>;
template class ECB<AES<8,14>,ANSIX923>;
template class BlockCipherModeOfOperation<ECB<AES<8,14>,ISO7816_4Padding>,AES<8,14>>;
template class ECB<AES<8,14>,ISO7816_4Padding>;
template class BlockCipherModeOfOperation<ECB<AES<8,14>,PKCS7Padding>,AES<8,14>>;
template class ECB<AES<8,14>,PKCS7Padding>;

template class BlockCipherModeOfOperation<CBC<AES<8,14>,ZeroPadding>,AES<8,14>>;
template class CBC<AES<8,14>,ZeroPadding>;
template class BlockCipherModeOfOperation<CBC<AES<8,14>,ANSIX923>,AES<8,14>>;
template class CBC<AES<8,14>,ANSIX923>;
template class BlockCipherModeOfOperation<CBC<AES<8,14>,ISO7816_4Padding>,AES<8,14>>;
template class CBC<AES<8,14>,ISO7816_4Padding>;
template class BlockCipherModeOfOperation<CBC<AES<8,14>,PKCS7Padding>,AES<8,14>>;
template class CBC<AES<8,14>,PKCS7Padding>;

template class BlockCipherModeOfOperation<PCBC<AES<8,14>,ZeroPadding>,AES<8,14>>;
template class PCBC<AES<8,14>,ZeroPadding>;
template class BlockCipherModeOfOperation<PCBC<AES<8,14>,ANSIX923>,AES<8,14>>;
template class PCBC<AES<8,14>,ANSIX923>;
template class BlockCipherModeOfOperation<PCBC<AES<8,14>,ISO7816_4Padding>,AES<8,14>>;
template class PCBC<AES<8,14>,ISO7816_4Padding>;
template class BlockCipherModeOfOperation<PCBC<AES<8,14>,PKCS7Padding>,AES<8,14>>;
template class PCBC<AES<8,14>,PKCS7Padding>;

template class BlockCipherModeOfOperation<CFB<AES<8,14>>,AES<8,14>>;
template class CFB<AES<8,14>>;

template class BlockCipherModeOfOperation<OFB<AES<8,14>>,AES<8,14>>;
template class OFB<AES<8,14>>;

template class BlockCipherModeOfOperation<CTR<AES<8,14>>,AES<8,14>>;
template class CTR<AES<8,14>>;
// AES-256 defintion -
