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

namespace anch {
  namespace crypto {

    extern template class BlockCipher<16>;
    extern template class AES<6,12>;
    /*!
     * AES-192 defintion
     */
    using AES192 = AES<6,12>;

    /*!
     * Cipher block mode of operation definitions
     */
    extern template class BlockCipherModeOfOperation<ECB<AES192,ZeroPadding>,AES192>;
    extern template class ECB<AES192,ZeroPadding>;
    extern template class BlockCipherModeOfOperation<ECB<AES192,ANSIX923>,AES192>;
    extern template class ECB<AES192,ANSIX923>;
    extern template class BlockCipherModeOfOperation<ECB<AES192,ISO7816_4Padding>,AES192>;
    extern template class ECB<AES192,ISO7816_4Padding>;
    extern template class BlockCipherModeOfOperation<ECB<AES192,PKCS7Padding>,AES192>;
    extern template class ECB<AES192,PKCS7Padding>;

    extern template class BlockCipherModeOfOperation<CBC<AES192,ZeroPadding>,AES192>;
    extern template class CBC<AES192,ZeroPadding>;
    extern template class BlockCipherModeOfOperation<CBC<AES192,ANSIX923>,AES192>;
    extern template class CBC<AES192,ANSIX923>;
    extern template class BlockCipherModeOfOperation<CBC<AES192,ISO7816_4Padding>,AES192>;
    extern template class CBC<AES192,ISO7816_4Padding>;
    extern template class BlockCipherModeOfOperation<CBC<AES192,PKCS7Padding>,AES192>;
    extern template class CBC<AES192,PKCS7Padding>;

    extern template class BlockCipherModeOfOperation<PCBC<AES192,ZeroPadding>,AES192>;
    extern template class PCBC<AES192,ZeroPadding>;
    extern template class BlockCipherModeOfOperation<PCBC<AES192,ANSIX923>,AES192>;
    extern template class PCBC<AES192,ANSIX923>;
    extern template class BlockCipherModeOfOperation<PCBC<AES192,ISO7816_4Padding>,AES192>;
    extern template class PCBC<AES192,ISO7816_4Padding>;
    extern template class BlockCipherModeOfOperation<PCBC<AES192,PKCS7Padding>,AES192>;
    extern template class PCBC<AES192,PKCS7Padding>;

    extern template class BlockCipherModeOfOperation<CFB<AES192>,AES192>;
    extern template class CFB<AES192>;

    extern template class BlockCipherModeOfOperation<OFB<AES192>,AES192>;
    extern template class OFB<AES192>;

    extern template class BlockCipherModeOfOperation<CTR<AES192>,AES192>;
    extern template class CTR<AES192>;

  }
}
