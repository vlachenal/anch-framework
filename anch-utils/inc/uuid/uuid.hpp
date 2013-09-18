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


namespace anch {
  namespace uuid {

    /*!
     * The UUID version
     *
     * \author Vincent Lachenal
     */
    enum Version {
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

    /*!
     * UUID generator and parser.\n
     * \n
     * For now, only version 1 is implemented.\n
     * See RFC 4122 for more details (http://www.ietf.org/rfc/rfc4122.txt)
     *
     * \author Vincent Lachenal
     */
    class Uuid {
      // Attributes +
    private:
      /*! UTC offset to apply to Epoch timestamp */
      static const uint64_t UTC_OFFSET;

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
      anch::uuid::Version _version;
      // Attributes -


      // Constructors +
    public:
      /*!
       * \ref Uuid default constructor
       */
      Uuid();

      /*!
       * \ref Uuid copy constructor
       *
       * \param uuid The \ref Uuid to copy
       */
      Uuid(const Uuid& uuid);
      // Constructors -


      // Static methods +
    public:
      /*!
       * Generate a new UUID
       *
       * \param uuid The UUID to set
       * \param version The UUID algorithm version to use (default to MAC based algorithm)
       * \param data The data to process (used to SHA1 and MD5 algorithms)
       */
      static void generateUuid(Uuid& uuid,
			       anch::uuid::Version version = anch::uuid::Version::RANDOM,
			       const std::string& data = "");

      /*!
       * Generate a new UUID with version 1 (MAC address based) algorithm
       *
       * \param uuid The UUID to set
       * \param macAddress The MAC address to use. If empty, the first MAC address
       *                   available will be choosen
       */
      static void generateUuidVersion1(Uuid& uuid,
				       const std::string& macAddress = "");

      /*!
       * Generate a new UUID with version 3 (MD5 based) algorithm
       *
       * \param uuid The UUID to set
       * \param data The data to process
       */
      static void generateUuidVersion3(Uuid& uuid, const std::string& data);

      /*!
       * Generate a new UUID with version 4 (random) algorithm
       *
       * \param uuid The UUID to set
       */
      static void generateUuidVersion4(Uuid& uuid);

      /*!
       * Generate a new UUID with version 5 (SHA1 based) algorithm
       *
       * \param uuid The UUID to set
       * \param data The data to process
       */
      static void generateUuidVersion5(Uuid& uuid, const std::string& data);

    private:
      /*!
       * Get UTC timestamp
       *
       * \return The UTC timestamp
       */
      static uint64_t getUtcTimestamp();
      // Static methods -


      // Methods +
    public:
      /*!
       * Convert \ref Uuid to std::string
       *
       * \return The string representation of \ref Uuid
       */
      std::string toString() const;
      // Methods -


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
      inline anch::uuid::Version getVersion() const {
	return _version;
      }
      // Accessors -

    };

  }
}

/*!
 * Ouput stream operator definition for UUID.<br>
 * This function preserves the formatting flags.
 *
 * \param out The output stream
 * \param uuid The UUID
 *
 * \return The output stream
 */
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator << (std::basic_ostream<CharT, Traits>& out, const anch::uuid::Uuid& uuid) {
  std::ios_base::fmtflags flags = out.flags(); // Save current flags
  out << std::hex
      << std::setfill('0') << std::setw(8) << uuid.getLowTime() << '-'
      << std::setfill('0') << std::setw(4) << uuid.getMidTime() << '-'
      << std::setfill('0') << std::setw(1) << uuid.getVersion()
      << std::setfill('0') << std::setw(3) << uuid.getHighTime() << '-'
      << std::setfill('0') << std::setw(2) << uuid.getClockSeqHighRes()
      << std::setfill('0') << std::setw(2) << uuid.getClockSeqLow() << '-'
      << std::setfill('0') << std::setw(12) << uuid.getNode();
  out.flags(flags); // Restore flags
  return out;
}

#endif // _ANCH_UUID_H_
