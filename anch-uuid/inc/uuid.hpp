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
#include <random>
#include <functional>
#include <atomic>
#include <mutex>
#include <map>


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

  private:
    /*! Mutex used for static members' initialization */
    static std::mutex _mutex;

    /*! Random engine initialization state */
    static std::atomic_bool _seeded;

    /*! UUID providers */
    static std::map<UUID::Version, std::function<UUID(const std::string&)>> _providers;

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
    anch::UUID::Version _version;
    // Attributes -


    // Constructors +
  public:
    /*!
     * \ref UUID default constructor
     */
    UUID();

    /*!
     * \ref UUID copy constructor
     *
     * \param uuid The \ref UUID to copy
     */
    UUID(const UUID& uuid);

    /*!
     * \ref UUID from std::string constructor
     *
     * \param uuid the std::string to parse
     *
     * \throw std::out_of_range invalid UUID length
     * \throw std::invalid_argument invalid UUID subpart
     */
    UUID(const std::string& uuid);

    /*!
     * \ref UUID constructor
     *
     * \param lowTime the low time part
     * \param midTime the mid time part
     * \param highTime the high time part
     * \param clockSeqLow the clock sequence low part
     * \param clockSeqHighRes the clock sequence high resolution part
     * \param node the node part
     * \param version the \ref UUID version
     */
    UUID(uint32_t lowTime, uint16_t midTime, uint16_t highTime, uint16_t clockSeqLow, uint16_t clockSeqHighRes, uint64_t node, UUID::Version version);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref UUID destructor
     */
    virtual ~UUID();
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
     * Generate a new UUID with version 4 (random) algorithm
     *
     * \return the new UUID
     */
    static UUID random();

    /*!
     * Get UTC timestamp
     *
     * \return The UTC timestamp
     */
    static uint64_t getUtcTimestamp();

    /*!
     * Random engine getter
     *
     * \return the random engine
     */
    static std::mt19937_64& getRandomEngine();

    /*!
     * Distribution sequence getter
     *
     * \return the distribution sequence
     */
    static std::uniform_int_distribution<uint16_t>& getDistSeq();

    /*!
     * Convert \ref UUID to std::string
     *
     * \return The string representation of \ref UUID
     */
    std::string toString() const;

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

  inline std::string
  UUID::toString() const {
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

  inline UUID::operator std::string() const {
    return toString();
  }

  inline uint32_t
  UUID::getLowTime() const {
    return _lowTime;
  }

  inline uint16_t
  UUID::getMidTime() const {
    return _midTime;
  }

  inline uint16_t
  UUID::getHighTime() const {
    return _highTime;
  }

  inline uint16_t
  UUID::getClockSeqLow() const {
    return _clockSeqLow;
  }

  inline uint16_t
  UUID::getClockSeqHighRes() const {
    return _clockSeqHighRes;
  }

  inline uint64_t
  UUID::getNode() const {
    return _node;
  }

  inline anch::UUID::Version
  UUID::getVersion() const {
    return _version;
  }

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
operator<<(std::basic_ostream<CharT, Traits>& out, const anch::UUID& uuid) {
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
