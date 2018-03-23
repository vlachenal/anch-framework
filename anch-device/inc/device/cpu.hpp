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

      /*! MMX instruction set support */
      bool _mmx;

      /*! SSE instruction set support */
      bool _sse;

      /*! SSE2 instruction set support */
      bool _sse2;

      /*! SSE3 instruction set support */
      bool _sse3;

      /*! SSSE3 instruction set support */
      bool _ssse3;

      /*! SSE4 instruction set support */
      bool _sse4;

      /*! SSE4.1 instruction set support */
      bool _sse4_1;

      /*! SSE4.2 instruction set support */
      bool _sse4_2;
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
	  _mmx = (info[3] & ((int)1 << 23)) != 0;
	  _sse = (info[3] & ((int)1 << 25)) != 0;
	  _sse2 = (info[3] & ((int)1 << 26)) != 0;
	  _sse3 = (info[2] & ((int)1 << 0)) != 0;
	  _ssse3 = (info[2] & ((int)1 << 9)) != 0;
	  _sse4_1 = (info[2] & ((int)1 << 19)) != 0;
	  _sse4_2 = (info[2] & ((int)1 << 20)) != 0;
	  _sse4 = _sse4_1 || _sse4_2;
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
      inline bool isAES() const;

      /*!
       * MMX support getter
       *
       * \return MMX feature support
       */
      inline bool isMMX() const;

      /*!
       * SSE support getter
       *
       * \return SSE feature support
       */
      inline bool isSSE() const;

      /*!
       * SSE2 support getter
       *
       * \return SSE2 feature support
       */
      inline bool isSSE2() const;

      /*!
       * SSE3 support getter
       *
       * \return SSE3 feature support
       */
      inline bool isSSE3() const;

      /*!
       * SSSE3 support getter
       *
       * \return SSSE3 feature support
       */
      inline bool isSSSE3() const;

      /*!
       * SSE4 support getter
       *
       * \return SSE4 feature support
       */
      inline bool isSSE4() const;

      /*!
       * SSE4.1 support getter
       *
       * \return SSE4.1 feature support
       */
      inline bool isSSE4_1() const;

      /*!
       * SSE4.2 support getter
       *
       * \return SSE4.2 feature support
       */
      inline bool isSSE4_2() const;
      // Accessors -

    };

    inline bool CPU::isAES() const {
      return _aes;
    }

    inline bool CPU::isMMX() const {
      return _mmx;
    }

    inline bool CPU::isSSE() const {
      return _sse;
    }

    inline bool CPU::isSSE2() const {
      return _sse2;
    }

    inline bool CPU::isSSE3() const {
      return _sse3;
    }

    inline bool CPU::isSSSE3() const {
      return _ssse3;
    }

    inline bool CPU::isSSE4() const {
      return _sse4;
    }

    inline bool CPU::isSSE4_1() const {
      return _sse4_1;
    }

    inline bool CPU::isSSE4_2() const {
      return _sse4_2;
    }

  }
}

#endif // _ANCH_DEVICE_CPU_H_
