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
#ifndef _ANCH_DEVICE_CPU_H_
#define _ANCH_DEVICE_CPU_H_

#include "singleton.hpp"

#ifdef ANCH_WINDOWS

//  Windows
#define anch_cpuid(info, x)    __cpuidex(info, x, 0)

#elif defined ANCH_POSIX

//  GCC Intrinsics
#include <cpuid.h>
void anch_cpuid(int info[4], int InfoType){
  __cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]);
}

#endif


namespace anch {
  namespace device {

    /*!
     * \brief CPU features detection
     *
     * Utility class which provides runtime CPU detection.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    class CPU: public anch::Singleton<CPU> {

      friend class anch::Singleton<CPU>;

      // Attributes +
    private:
      /*! AES instruction set support */
      bool _aes;
      // Attributes -


      // Constructors +
    private:
      /*!
       * \ref CPU private constructor
       */
      CPU() {
	int info[4];
	anch_cpuid(info, 0);
	int nIds = info[0];
	if(nIds >= 0x00000001) {
	  anch_cpuid(info,0x00000001);
	  _aes = (info[2] & ((int)1 << 25)) != 0;
	}
      }
      // Constructors -


      // Accessors +
    public:
      /*!
       * AES support getter
       *
       * \return AES feature support
       */
      inline bool isAES() const {
	return _aes;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_DEVICE_CPU_H_
