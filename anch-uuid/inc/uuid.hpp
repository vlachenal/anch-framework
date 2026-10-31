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
#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>
#include <functional>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <cinttypes>
#include <array>
#include <compare>
#include <cstddef>


namespace anch {

  /*!
   * \brief UUID generator and parser.
   *
   * See RFC 4122 for more details (http://www.ietf.org/rfc/rfc4122.txt)\n
   * \n
   * This class needs anch-crypto and anch-device libraries.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class UUID {
  public:
    /*!
     * The UUID versions
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    enum class Version {
      /*! Undefined */
      NOT_SET = 0,

      /*! MAC address based algorithm */
      MAC_ADDRESS = 1,

      /*! DCE security (Windows) base algorithm */
      DCE_SECURITY,

      /*! MD5 hash based algorithm */
      MD5_HASH,

      /*! Random based algorithm */
      RANDOM,

      /*! SHA1 hash based algorithm */
      SHA1_HASH,

      /*! MAC address based algorithm ; all time bits are in reserve order compared to MAC_ADDRESS */
      MAC_ADDRESS_R,

      /*! Random and time based algorithm */
      RANDOM_TIME,

      /*! Random and time based algorithm */
      EXP
    };

    // Attributes +
  public:
    /*! Time low part time mask */
    static const uint64_t TIME_LOW_MASK = 0x00000000FFFFFFFF;

    /*! Time middle part time mask */
    static const uint64_t TIME_MID_MASK = 0x000000000000FFFF;

    /*! Time high part time mask */
    static const uint64_t TIME_HIGH_MASK = 0x0000000000000FFF;

    /*! Time high part time mask */
    static const uint64_t VERSION_MASK = 0x000000000000F000;

    /*! Sequence low part time mask */
    static const uint32_t SEQ_LOW_MASK = 0x00FF;

    /*! Sequence high part time mask */
    static const uint32_t SEQ_HIGH_MASK = 0x3F00;

    /*! UUID string representation size (without \c '\\0' ) */
    static constexpr std::size_t STRING_SIZE = 36;

  private:
    /*! Providers registry mutex (exclusive to register providers, shared to generate UUID) */
    static std::shared_mutex _mutex;

    /*! Random engine initialization state */
    static std::atomic_bool _seeded;

    /*! UUID providers */
    static std::map<UUID::Version, std::function<UUID(const std::string&)>> _providers;

    /*! UUID 16 bytes binary representation (RFC 9562, big-endian): parts are computed by accessors */
    std::array<uint8_t,16> _bytes{};
    // Attributes -


    // Constructors +
  public:
    /*!
     * \ref UUID default constructor: nil UUID ( \c 00000000-0000-0000-0000-000000000000 )
     */
    UUID() = default;

    /*!
     * \ref UUID copy constructor
     *
     * \param uuid The \ref UUID to copy
     */
    UUID(const UUID& uuid) = default;

    /*!
     * \ref UUID move constructor
     *
     * \param uuid The \ref UUID to 'move'
     */
    UUID(UUID&& uuid) = default;

    /*!
     * \ref UUID from std::string constructor
     *
     * \param uuid the std::string to parse
     *
     * \throw std::invalid_argument invalid UUID: length is not 36, \c '-' is not at positions 8, 13, 18 and 23,
     * or other characters are not hexadecimal digits
     */
    UUID(const std::string& uuid);

    /*!
     * \ref UUID constructor.\n
     * Each part is truncated to its width (32 bits for \c lowTime , 16 for \c midTime , 12 for \c highTime ,
     * 8 for \c clockSeqLow and \c clockSeqHighRes , 48 for \c node and 4 for \c version ).
     *
     * \param lowTime the low time part
     * \param midTime the mid time part
     * \param highTime the high time part
     * \param clockSeqLow the clock sequence low part
     * \param clockSeqHighRes the clock sequence high resolution part
     * \param node the node part
     * \param version the \ref UUID version
     */
    UUID(uint32_t lowTime,
	 uint16_t midTime,
	 uint16_t highTime,
	 uint16_t clockSeqLow,
	 uint16_t clockSeqHighRes,
	 uint64_t node,
	 UUID::Version version);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref UUID destructor (not virtual: \ref UUID is a value type)
     */
    ~UUID() = default;
    // Destructor -

    // Methods +
  public:
    /*!
     * Register UUID provider
     *
     * \param version the UUID version which can be handled by provider
     * \param provider the provider
     */
    static void registerProvider(UUID::Version version, std::function<UUID(const std::string&)> provider);

    /*!
     * Generate a new UUID
     *
     * \param version The UUID algorithm version to use (default to MAC based algorithm)
     * \param data The data to process (used to SHA1 and MD5 algorithms)
     *
     * \return the new \ref UUID
     *
     * \throw std::runtime_error no provider for specified UUID version
     */
    static UUID generateUUID(anch::UUID::Version version, const std::string& data = "");

    /*!
     * Generate a new name based UUID (version 3 or 5) according to RFC 9562: the provider hashes the namespace
     * \ref UUID (16 bytes, see \ref toBytes) followed by the name.\n
     * For other versions, namespace and name are given to the provider in the same way (random providers ignore them).
     *
     * \param version The UUID algorithm version to use (\c MD5_HASH or \c SHA1_HASH)
     * \param nameSpace the namespace (for example \ref namespaceDNS )
     * \param name the name in the namespace
     *
     * \return the new \ref UUID
     *
     * \throw std::runtime_error no provider for specified UUID version
     */
    static UUID generateUUID(anch::UUID::Version version, const UUID& nameSpace, const std::string& name);

    /*!
     * Build \ref UUID from its 16 bytes binary representation (RFC 9562, big-endian).\n
     * Version is read from the 4 high bits of byte 6 ; bytes are not checked (variant is not forced).
     *
     * \param bytes the 16 bytes
     *
     * \return the \ref UUID
     */
    static UUID fromBytes(const std::array<uint8_t,16>& bytes);

    /*!
     * Get \ref UUID 16 bytes binary representation (RFC 9562, big-endian)
     *
     * \return the 16 bytes
     */
    std::array<uint8_t,16> toBytes() const;

    /*!
     * DNS namespace (\c 6ba7b810-9dad-11d1-80b4-00c04fd430c8 ) for name based UUID (RFC 9562)
     *
     * \return the namespace
     */
    static const UUID& namespaceDNS();

    /*!
     * URL namespace (\c 6ba7b811-9dad-11d1-80b4-00c04fd430c8 ) for name based UUID (RFC 9562)
     *
     * \return the namespace
     */
    static const UUID& namespaceURL();

    /*!
     * ISO OID namespace (\c 6ba7b812-9dad-11d1-80b4-00c04fd430c8 ) for name based UUID (RFC 9562)
     *
     * \return the namespace
     */
    static const UUID& namespaceOID();

    /*!
     * X.500 DN namespace (\c 6ba7b814-9dad-11d1-80b4-00c04fd430c8 ) for name based UUID (RFC 9562)
     *
     * \return the namespace
     */
    static const UUID& namespaceX500();

    /*!
     * Generate a new UUID with version 4 (random) algorithm
     *
     * \return the new UUID
     */
    static UUID random();

    /*!
     * Generate a new UUID with version 7 (random + time) algorithm.\n
     * A 16 bits counter (RFC 9562 section 6.2, method 1) is written in the 12 \c rand_a bits and in the 4 first
     * \c rand_b bits: UUID generated by this process are strictly increasing (string and binary order), including
     * between threads. It is initialized randomly on each new millisecond (at least 32768 UUID per millisecond before
     * timestamp is incremented). The 58 last bits are random.
     *
     * \return the new UUID
     */
    static UUID randomTime();

    /*!
     * Get UUID version 1 / 6 timestamp: number of 100 ns intervals since 1582-10-15 00:00:00 UTC (Gregorian calendar
     * reform, RFC 9562), from system clock
     *
     * \return The timestamp
     */
    static uint64_t getUtcTimestamp();

    /*!
     * Random engine getter
     *
     * \deprecated No longer used by AnCH: \c std::random_device is slow (~435 ns per call) and is shared between
     * threads. Use \ref random (16 cryptographically secure random bytes in one call, see \ref toBytes ) to get
     * random bits.
     *
     * \return the random engine
     */
    [[deprecated("Use anch::UUID::random().toBytes() to get random bits")]]
    static std::random_device& getRandomEngine();

    /*!
     * Clock sequence distribution getter (uniform 14 bits values: \c 0 to \c 0x3FFF )
     *
     * \deprecated No longer used by AnCH: the distribution is shared between threads and is used with
     * \ref getRandomEngine . Use \ref random (14 random bits in bytes 8 and 9, after variant, see \ref toBytes ).
     *
     * \return the clock sequence distribution
     */
    [[deprecated("Use anch::UUID::random().toBytes() bytes 8 and 9 to get a random clock sequence")]]
    static std::uniform_int_distribution<uint16_t>& getDistSeq();

    /*!
     * Parse \ref UUID
     *
     * \param uuid the \ref UUID to parse
     *
     * \return the parsed \ref UUID
     *
     * \throw std::invalid_argument invalid UUID: length is not 36, \c '-' is not at positions 8, 13, 18 and 23,
     * or other characters are not hexadecimal digits
     */
    static UUID parseUUID(const std::string& uuid);

    /*!
     * Parse \ref UUID for this value (usefull with default constructor).\n
     * Uppercase and lowercase hexadecimal digits are accepted. The value is not modified when \c uuid is invalid.
     *
     * \param uuid the \ref UUID to parse
     *
     * \throw std::invalid_argument invalid UUID: length is not 36, \c '-' is not at positions 8, 13, 18 and 23,
     * or other characters are not hexadecimal digits
     */
    void parse(const std::string& uuid);

    /*!
     * Convert \ref UUID to std::string
     *
     * \return The string representation of \ref UUID
     */
    std::string toString() const;

    /*!
     * Write \ref UUID string representation without allocation.\n
     * Exactly \ref STRING_SIZE lowercase hexadecimal characters and \c '-' are written (no \c '\\0' ).
     * Each part is written with its fixed width: higher bits of a part which does not fit (which can only be set
     * with the parts constructor) are ignored.
     *
     * \param first the first character to write (at least \ref STRING_SIZE characters available)
     *
     * \return the pointer after the last written character
     */
    char* toChars(char* first) const;

  private:
    /*!
     * Register random UUID if not already done
     */
    static void registerRandomUUID();
    // Methods -


    // Operators +
  public:
    /*!
     * Cast \ref UUID to std::string
     */
    explicit operator std::string() const;

    /*!
     * Three-way comparison: binary order (RFC 9562), which is also string representation order.\n
     * \c == , \c != , \c < , \c <= , \c > and \c >= are generated from it.
     *
     * \param uuid the \ref UUID to compare with
     *
     * \return the comparison result
     */
    std::strong_ordering operator <=>(const UUID& uuid) const = default;

    /*!
     * \c == operator definition
     *
     * \param uuid the \ref UUID to compare with
     *
     * \return \c true when the two \ref UUID are equals, \c false otherwise
     */
    bool operator ==(const UUID& uuid) const = default;

    /*!
     * \c = operator definition
     *
     * \param uuid the \ref UUID to copy
     *
     * \return \c this
     */
    UUID& operator =(const UUID& uuid) = default;

    /*!
     * Move \c = operator definition
     *
     * \param uuid the \ref UUID to move
     *
     * \return \c this
     */
    UUID& operator =(UUID&& uuid) = default;
    // Operators -


    // Accessors +
  public:
    /*!
     * The timestamp low value getter
     *
     * \return The timestamp low value
     */
    uint32_t getLowTime() const;

    /*!
     * The timestamp mid value getter
     *
     * \return The timestamp mid value
     */
    uint16_t getMidTime() const;

    /*!
     * The timestamp high value getter
     *
     * \return The timestamp high value
     */
    uint16_t getHighTime() const;

    /*!
     * The clock sequence low value getter
     *
     * \return The clock sequence low value
     */
    uint16_t getClockSeqLow() const;

    /*!
     * The clock sequence high value and reserved value getter
     *
     * \return The clock sequence high value and reserved value
     */
    uint16_t getClockSeqHighRes() const;

    /*!
     * The node getter
     *
     * \return The node
     */
    uint64_t getNode() const;

    /*!
     * Version getter
     *
     * \return The version
     */
    anch::UUID::Version getVersion() const;
    // Accessors -

  };

}

/*!
 * \c std::hash specialization for \ref anch::UUID (usable as \c std::unordered_map key)
 *
 * \author Vincent Lachenal
 *
 * \since 0.1
 */
template<>
struct std::hash<anch::UUID> {
  /*!
   * Compute \ref anch::UUID hash
   *
   * \param uuid the \ref anch::UUID
   *
   * \return the hash
   */
  std::size_t operator()(const anch::UUID& uuid) const noexcept;
};

/*!
 * Ouput stream operator definition for UUID.\n
 * This function preserves the formatting flags.
 *
 * \param out The output stream
 * \param uuid The UUID
 *
 * \return The output stream
 */
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& out, const anch::UUID& uuid);

#include "impl/uuid.hpp"
