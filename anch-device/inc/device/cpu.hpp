/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "anch-device-export.hpp"

#include "singleton.hpp"

#if defined(__x86_64__) || defined(__i386__)
#include <cpuid.h>
#endif


namespace anch::device {

  /*!
   * \brief CPU features detection
   *
   * Utility class which provides runtime CPU detection (\c CPUID instruction).\n
   * On other architectures than x86 / x86-64, every feature is reported as not supported.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class ANCH_DEVICE_EXPORT CPU: public anch::Singleton<CPU> {

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
    CPU():
      _aes(false),
      _mmx(false),
      _sse(false),
      _sse2(false),
      _sse3(false),
      _ssse3(false),
      _sse4(false),
      _sse4_1(false),
      _sse4_2(false) {
#if defined(__x86_64__) || defined(__i386__)
      unsigned int eax, ebx, ecx, edx;
      // __get_cpuid returns 0 when leaf 1 is not available
      if(__get_cpuid(1, &eax, &ebx, &ecx, &edx) != 0) {
	_aes = (ecx & bit_AES) != 0;
	_mmx = (edx & bit_MMX) != 0;
	_sse = (edx & bit_SSE) != 0;
	_sse2 = (edx & bit_SSE2) != 0;
	_sse3 = (ecx & bit_SSE3) != 0;
	_ssse3 = (ecx & bit_SSSE3) != 0;
	_sse4_1 = (ecx & bit_SSE4_1) != 0;
	_sse4_2 = (ecx & bit_SSE4_2) != 0;
	_sse4 = _sse4_1 || _sse4_2;
      }
#endif
    }
    // Constructors -


    // Accessors +
  public:
    /*!
     * AES support getter
     *
     * \return AES feature support
     */
    bool isAES() const;

    /*!
     * MMX support getter
     *
     * \return MMX feature support
     */
    bool isMMX() const;

    /*!
     * SSE support getter
     *
     * \return SSE feature support
     */
    bool isSSE() const;

    /*!
     * SSE2 support getter
     *
     * \return SSE2 feature support
     */
    bool isSSE2() const;

    /*!
     * SSE3 support getter
     *
     * \return SSE3 feature support
     */
    bool isSSE3() const;

    /*!
     * SSSE3 support getter
     *
     * \return SSSE3 feature support
     */
    bool isSSSE3() const;

    /*!
     * SSE4 support getter
     *
     * \return SSE4 feature support
     */
    bool isSSE4() const;

    /*!
     * SSE4.1 support getter
     *
     * \return SSE4.1 feature support
     */
    bool isSSE4_1() const;

    /*!
     * SSE4.2 support getter
     *
     * \return SSE4.2 feature support
     */
    bool isSSE4_2() const;
    // Accessors -

  };

}

#include "device/impl/cpu.hpp"
