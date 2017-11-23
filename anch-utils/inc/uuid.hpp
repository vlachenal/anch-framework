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
#ifndef _ANCH_UUID_H_
#define _ANCH_UUID_H_

#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>
#include <limits>
#include <random>
#include <functional>
#include <regex>

#include "device/network.hpp"
#include "crypto/hash/md5.hpp"
#include "crypto/hash/sha1.hpp"


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
  class Uuid {
  public:
    /*!
     * The UUID versions
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    enum class Version {
      /*! MAC address based algorithm */
      MAC_ADDRESS = 1,

      /*! DCE security (Windows) base algorithm */
      DCE_SECURITY,

      /*! MD5 hash based algorithm */
      MD5_HASH,

      /*! Random based algorithm */
      RANDOM,

      /*! SHA1 hash based algorithm */
      SHA1_HASH
    };

    // Attributes +
  private:
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

    /*! The timestamp low value */
    uint32_t _lowTime;

    /*! The timestamp mid value */
    uint16_t _midTime;

    /*! The timestamp high value */
    uint16_t _highTime;

    /*! The clock sequence low value */
    uint16_t _clockSeqLow;

    /*! The clock sequence high value and reserved value */
    uint16_t _clockSeqHighRes;

    /*! The node */
    uint64_t _node;

    /*! Version */
    anch::Uuid::Version _version;
    // Attributes -


    // Constructors +
  public:
    /*!
     * \ref Uuid default constructor
     */
    Uuid() {
      // Nothing to do
    }

    /*!
     * \ref Uuid copy constructor
     *
     * \param uuid The \ref Uuid to copy
     */
    Uuid(const Uuid& uuid): _lowTime(uuid._lowTime),
			    _midTime(uuid._midTime),
			    _highTime(uuid._highTime),
			    _clockSeqLow(uuid._clockSeqLow),
			    _clockSeqHighRes(uuid._clockSeqHighRes),
			    _node(uuid._node) {
      // Nothing to do
    }

    /*!
     * \ref Uuid from std::string constructor
     *
     * \param uuid the std::string to parse
     *
     * \throw std::bad_cast unable to parse UUID
     */
    Uuid(const std::string& uuid): _lowTime(0),
				   _midTime(0),
				   _highTime(0),
				   _clockSeqLow(0),
				   _clockSeqHighRes(0),
				   _node(0) {
      static std::regex regexp("([0-9A-Fa-f]{8})-([0-9A-Fa-f]{4})-([0-9A-Fa-f]{1})([0-9A-Fa-f]{3})-([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})-([0-9A-Fa-f]{12})");
      std::cmatch matches;
      if(!std::regex_match(uuid.data(), matches, regexp)) {
	throw std::bad_cast();
      }
      std::istringstream iss(matches.str(1));
      iss >> std::hex >> _lowTime;
      iss.clear();
      iss.str(matches.str(2));
      iss >> std::hex >> _midTime;
      iss.clear();
      iss.str(matches.str(3));
      int version;
      iss >> std::hex >> version;
      _version = static_cast<anch::Uuid::Version>(version);
      iss.clear();
      iss.str(matches.str(4));
      iss >> std::hex >> _highTime;
      iss.clear();
      iss.str(matches.str(5));
      iss >> std::hex >> _clockSeqHighRes;
      iss.clear();
      iss.str(matches.str(6));
      iss >> std::hex >> _clockSeqLow;
      iss.clear();
      iss.str(matches.str(7));
      iss >> std::hex >> _node;
    }
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Uuid destructor
     */
    virtual ~Uuid() {
      // Nothing to do
    }
    // Destructor -

    // Methods +
  public:
    /*!
     * Generate a new UUID
     *
     * \param uuid The UUID to set
     * \param version The UUID algorithm version to use (default to MAC based algorithm)
     * \param data The data to process (used to SHA1 and MD5 algorithms)
     */
    static void generateUuid(Uuid& uuid,
			     anch::Uuid::Version version = anch::Uuid::Version::RANDOM,
			     const std::string& data = "") {
      // Initialize random seed +
      std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
      auto epoch = now.time_since_epoch();
      int64_t timestamp = (std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count());
      getRandomEngine().seed(timestamp);
      // Initialize random seed -

      uuid._version = version;

      switch(version) {
      case Uuid::Version::MAC_ADDRESS:
	generateUuidVersion1(uuid, data);
	break;
      case Uuid::Version::DCE_SECURITY:
	// \todo implements this algorithm version
	break;
      case Uuid::Version::MD5_HASH:
	generateUuidVersion3(uuid,data);
	break;
      case Uuid::Version::RANDOM:
	generateUuidVersion4(uuid);
	break;
      case Uuid::Version::SHA1_HASH:
	generateUuidVersion5(uuid,data);
	break;
      default:
	// Nothing to do
	break;
      }
    }

    /*!
     * Generate a new UUID with version 1 (MAC address based) algorithm
     *
     * \param uuid The UUID to set
     * \param macAddress The MAC address to use. If empty, the first MAC address
     *                   available will be choosen
     */
    static void generateUuidVersion1(Uuid& uuid,
				     const std::string& macAddress = "") {
      // Timestamp +
      uint64_t timestamp = getUtcTimestamp();
      uuid._lowTime = timestamp & TIME_LOW_MASK;
      uuid._midTime = (timestamp >> 32) & TIME_MID_MASK;
      uuid._highTime = (timestamp >> 48) & TIME_HIGH_MASK;
      // Timestamp -

      // Sequence +
      uint16_t clockSeq = getDistSeq()(getRandomEngine());
      uuid._clockSeqLow = clockSeq & SEQ_LOW_MASK;
      uuid._clockSeqHighRes = (clockSeq & SEQ_HIGH_MASK) >> 8;
      uuid._clockSeqHighRes |= 0x80;
      // Sequence -

      // MAC address +
      std::string macAddr = macAddress;
      if(macAddr == "") {
	for(auto iter : anch::device::Network::getInterfaces()) {
	  const anch::device::NetworkInterface iface = iter.second;
	  if(!iface.isLocalhost()) {
	    macAddr = iface.getMacAddress();
	  }
	}
      }
      std::size_t pos = 0;
      std::size_t last = 0;
      std::string macHexStr = "";
      do {
	if(pos != 0) {
	  last = pos + 1;
	}
	pos = macAddr.find(':', last);
	macHexStr += macAddr.substr(last, pos - last);
      } while(pos != std::string::npos);
      uuid._node = std::stoull(macHexStr,0,16);
      // MAC address -
    }

    /*!
     * Generate a new UUID with version 3 (MD5 based) algorithm
     *
     * \param uuid The UUID to set
     * \param data The data to process
     */
    static void generateUuidVersion3(Uuid& uuid, const std::string& data) {
      anch::crypto::MD5 hash(data);
      const std::array<uint8_t,16>& digest = hash.digest();

      // Timestamp +
      uuid._lowTime = digest[0]
	+ (static_cast<uint32_t>(digest[1]) << 8)
	+ (static_cast<uint32_t>(digest[2]) << 16)
	+ (static_cast<uint32_t>(digest[3]) << 24);
      uuid._midTime = digest[4]
	+ (static_cast<uint16_t>(digest[5]) << 8);
      uuid._highTime = digest[6]
	+ (static_cast<uint16_t>(digest[7]) << 8);
      uuid._highTime = uuid._highTime & TIME_HIGH_MASK;
      // Timestamp -

      // Sequence +
      uuid._clockSeqLow = digest[9];
      uuid._clockSeqHighRes = digest[8];
      uuid._clockSeqHighRes |= 0x80;
      // Sequence -

      // Node +
      uuid._node = digest[10]
	+ (static_cast<uint64_t>(digest[11]) << 8)
	+ (static_cast<uint64_t>(digest[12]) << 16)
	+ (static_cast<uint64_t>(digest[13]) << 24)
	+ (static_cast<uint64_t>(digest[14]) << 32)
	+ (static_cast<uint64_t>(digest[15]) << 40);
      // Node -
    }

    /*!
     * Generate a new UUID with version 4 (random) algorithm
     *
     * \param uuid The UUID to set
     */
    static void generateUuidVersion4(Uuid& uuid) {
      static std::uniform_int_distribution<uint16_t> dist16;
      static std::uniform_int_distribution<uint32_t> dist32;
      static std::uniform_int_distribution<uint64_t> dist64(0, 0xFFFFFFFFFFFF); // Max 12 hexadecimal digits

      // Timestamp +
      uuid._lowTime = dist32(getRandomEngine());
      uuid._midTime = dist16(getRandomEngine());
      uuid._highTime = dist16(getRandomEngine()) & TIME_HIGH_MASK;
      // Timestamp -

      // Sequence +
      uint16_t clockSeq = getDistSeq()(getRandomEngine());
      uuid._clockSeqLow = clockSeq & SEQ_LOW_MASK;
      uuid._clockSeqHighRes = (clockSeq & SEQ_HIGH_MASK) >> 8;
      uuid._clockSeqHighRes |= 0x80;
      // Sequence -

      // Node +
      uuid._node = dist64(getRandomEngine());
      // Node -
    }

    /*!
     * Generate a new UUID with version 5 (SHA1 based) algorithm
     *
     * \param uuid The UUID to set
     * \param data The data to process
     */
    static void generateUuidVersion5(Uuid& uuid, const std::string& data) {
      anch::crypto::SHA1 hash(data);
      const std::array<uint8_t,20>& digest = hash.digest();

      // Timestamp +
      uuid._lowTime = digest[0]
	+ (static_cast<uint32_t>(digest[1]) << 8)
	+ (static_cast<uint32_t>(digest[2]) << 16)
	+ (static_cast<uint32_t>(digest[3]) << 24);
      uuid._midTime = digest[4]
	+ (static_cast<uint16_t>(digest[5]) << 8);
      uuid._highTime = digest[6]
	+ (static_cast<uint16_t>(digest[7]) << 8);
      uuid._highTime = uuid._highTime & TIME_HIGH_MASK;
      // Timestamp -

      // Sequence +
      uuid._clockSeqLow = digest[9];
      uuid._clockSeqHighRes = digest[8];
      uuid._clockSeqHighRes |= 0x80;
      // Sequence -

      // Node +
      uuid._node = digest[10]
	+ (static_cast<uint64_t>(digest[11]) << 8)
	+ (static_cast<uint64_t>(digest[12]) << 16)
	+ (static_cast<uint64_t>(digest[13]) << 24)
	+ (static_cast<uint64_t>(digest[14]) << 32)
	+ (static_cast<uint64_t>(digest[15]) << 40);
      // Node -
    }

  private:
    /*!
     * Get UTC timestamp
     *
     * \return The UTC timestamp
     */
    static uint64_t getUtcTimestamp() {
      static const uint64_t UTC_OFFSET = 0x01B21DD213814000;
      std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
      auto epoch = now.time_since_epoch();
      int64_t timestamp = (std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count()) / 100;
      return UTC_OFFSET + (uint64_t)timestamp;
    }

    /*!
     * Random engine getter
     *
     * \return the random engine
     */
    static inline std::mt19937& getRandomEngine() {
      static std::mt19937 engine;
      return engine;
    }

    /*!
     * Distribution sequence getter
     *
     * \return the distribution sequence
     */
    static inline std::uniform_int_distribution<uint16_t>& getDistSeq() {
      static std::uniform_int_distribution<uint16_t> distSeq(0, SEQ_HIGH_MASK);
      return distSeq;
    }

  public:
    /*!
     * Convert \ref Uuid to std::string
     *
     * \return The string representation of \ref Uuid
     */
    std::string toString() const {
      std::ostringstream out;
      out << std::hex
	  << std::setfill('0') << std::setw(8) << _lowTime << '-'
	  << std::setfill('0') << std::setw(4) << _midTime << '-'
	  << std::setfill('0') << std::setw(1) << static_cast<int>(_version)
	  << std::setfill('0') << std::setw(3) << _highTime << '-'
	  << std::setfill('0') << std::setw(2) << _clockSeqHighRes
	  << std::setfill('0') << std::setw(2) << _clockSeqLow << '-'
	  << std::setfill('0') << std::setw(12) << _node;
      return out.str();
    }
    // Methods -


    // Operators +
    /*!
     * Cast \ref UUID to std::string
     */
    explicit operator std::string() const {
      return toString();
    }
    // Operators -


    // Accessors +
  public:
    /*!
     * The timestamp low value getter
     *
     * \return The timestamp low value
     */
    inline uint32_t getLowTime() const {
      return _lowTime;
    }

    /*!
     * The timestamp mid value getter
     *
     * \return The timestamp mid value
     */
    inline uint16_t getMidTime() const {
      return _midTime;
    }

    /*!
     * The timestamp high value getter
     *
     * \return The timestamp high value
     */
    inline uint16_t getHighTime() const {
      return _highTime;
    }

    /*!
     * The clock sequence low value getter
     *
     * \return The clock sequence low value
     */
    inline uint16_t getClockSeqLow() const {
      return _clockSeqLow;
    }

    /*!
     * The clock sequence high value and reserved value getter
     *
     * \return The clock sequence high value and reserved value
     */
    inline uint16_t getClockSeqHighRes() const {
      return _clockSeqHighRes;
    }

    /*!
     * The node getter
     *
     * \return The node
     */
    inline uint64_t getNode() const {
      return _node;
    }

    /*!
     * Version getter
     *
     * \return The version
     */
    inline anch::Uuid::Version getVersion() const {
      return _version;
    }
    // Accessors -

  };

}

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
operator<<(std::basic_ostream<CharT, Traits>& out, const anch::Uuid& uuid) {
  std::ios_base::fmtflags flags = out.flags(); // Save current flags
  out << std::hex
      << std::setfill('0') << std::setw(8) << uuid.getLowTime() << '-'
      << std::setfill('0') << std::setw(4) << uuid.getMidTime() << '-'
      << std::setfill('0') << std::setw(1) << static_cast<int>(uuid.getVersion())
      << std::setfill('0') << std::setw(3) << uuid.getHighTime() << '-'
      << std::setfill('0') << std::setw(2) << uuid.getClockSeqHighRes()
      << std::setfill('0') << std::setw(2) << uuid.getClockSeqLow() << '-'
      << std::setfill('0') << std::setw(12) << uuid.getNode();
  out.flags(flags); // Restore flags
  return out;
}

#endif // _ANCH_UUID_H_
