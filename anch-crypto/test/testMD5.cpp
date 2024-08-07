#include "crypto/hash/md5.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::crypto::MD5;


// Wikipedia example algorithm +
/*
 * Simple MD5 implementation from wikipedia
 */

// leftrotate function definition
uint32_t k[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
		   0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
		   0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
		   0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
		   0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
		   0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
		   0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
		   0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
		   0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
		   0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
		   0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
		   0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
		   0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
		   0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
		   0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
		   0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

// These vars will contain the hash
uint32_t h0, h1, h2, h3;

void md5(const uint8_t *initial_msg, size_t initial_len) {

  // Message (to prepare)
  uint8_t *msg = NULL;

  // Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating

  // r specifies the per-round shift amounts

  uint32_t r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
		  5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
		  4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
		  6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

  // Use binary integer part of the sines of integers (in radians) as constants// Initialize variables:
  h0 = 0x67452301;
  h1 = 0xefcdab89;
  h2 = 0x98badcfe;
  h3 = 0x10325476;

  // Pre-processing: adding a single 1 bit
  //append "1" bit to message
  /* Notice: the input bytes are considered as bits strings,
     where the first bit is the most significant bit of the byte.[37] */

  // Pre-processing: padding with zeros
  //append "0" bit until message length in bit ≡ 448 (mod 512)
  //append length mod (2 pow 64) to message

  int new_len;
  for(new_len = static_cast<int>(initial_len * 8 + 1) ; new_len % 512 != 448 ; ++new_len);
  new_len /= 8;

  msg = (uint8_t*)calloc(static_cast<std::size_t>(new_len + 64), 1); // also appends "0" bits
  // (we alloc also 64 extra bytes...)
  memcpy(msg, initial_msg, initial_len);
  msg[initial_len] = 128; // write the "1" bit

  uint32_t bits_len = static_cast<uint32_t>(8 * initial_len); // note, we append the len
  memcpy(msg + new_len, &bits_len, 4);           // in bits at the end of the buffer

  // Process the message in successive 512-bit chunks:
  //for each 512-bit chunk of message:
  int offset;
  for(offset=0; offset<new_len; offset += (512/8)) {

    // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
    uint32_t *w = (uint32_t *) (msg + offset);

#ifdef DEBUG
    printf("offset: %d %x\n", offset, offset);

    int j;
    for(j =0; j < 64; j++) printf("%x ", ((uint8_t *) w)[j]);
    puts("");
#endif

    // Initialize hash value for this chunk:
    uint32_t a = h0;
    uint32_t b = h1;
    uint32_t c = h2;
    uint32_t d = h3;

    // Main loop:
    uint32_t i;
    for(i = 0; i<64; i++) {

      uint32_t f, g;

      if (i < 16) {
	f = (b & c) | ((~b) & d);
	g = i;
      } else if (i < 32) {
	f = (d & b) | ((~d) & c);
	g = (5*i + 1) % 16;
      } else if (i < 48) {
	f = b ^ c ^ d;
	g = (3*i + 5) % 16;
      } else {
	f = c ^ (b | (~d));
	g = (7*i) % 16;
      }

      uint32_t temp = d;
      d = c;
      c = b;
      b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
      a = temp;

    }

    // Add this chunk's hash to result so far:

    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;

  }

  // cleanup
  free(msg);

}

void md5(const char* msg, std::string& result) {

  size_t len = strlen(msg);

  md5((const uint8_t*)msg, len);

  //var char digest[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)
  uint8_t* p;

  // display result

  char digest[33];
  p=(uint8_t *)&h0;
  sprintf(digest,"%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3]);

  p=(uint8_t *)&h1;
  sprintf(digest + 8,"%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3]);

  p=(uint8_t *)&h2;
  sprintf(digest + 16,"%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3]);

  p=(uint8_t *)&h3;
  sprintf(digest + 24,"%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3]);

  result = digest;
}

void md5(const char* msg, std::array<uint8_t,16>& result) {

  size_t len = strlen(msg);

  md5((const uint8_t*)msg, len);

  //var char digest[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)
  uint8_t* p;

  // display result

  p = (uint8_t *)&h0;
  result[0] = p[0];
  result[1] = p[1];
  result[2] = p[2];
  result[3] = p[3];

  p = (uint8_t *)&h1;
  result[4] = p[0];
  result[5] = p[1];
  result[6] = p[2];
  result[7] = p[3];

  p = (uint8_t *)&h2;
  result[8] = p[0];
  result[9] = p[1];
  result[10] = p[2];
  result[11] = p[3];

  p = (uint8_t *)&h3;
  result[12] = p[0];
  result[13] = p[1];
  result[14] = p[2];
  result[15] = p[3];
}
// Wikipedia example algorithm -

void
testMD5string() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

  std::string msg("\"Ta mère suce des bites en enfer\" - Linda Blair - L'Exorciste - 1973");
  std::cout << "Hash message: " << msg << std::endl;
  start = std::chrono::high_resolution_clock::now();
  MD5 hash(msg);
  const std::array<uint8_t,16>& res = hash.digest();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds anchDuration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());

  std::array<uint8_t,16> sum;
  start = std::chrono::high_resolution_clock::now();
  md5(msg.data(), sum);
  end = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds md5Duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());

  std::ostringstream out;
  out << hash;
  std::string strRes = out.str();
  anch::ut::assertTrue(res == sum);
  // if(res != sum) {
  //   char digest[33];
  //   sprintf(digest,"%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
  // 	    sum[0],sum[1],sum[2],sum[3],sum[4],sum[5],sum[6],sum[7],sum[8],sum[9],sum[10],sum[11],sum[12],sum[13],sum[14],sum[15]);
  //   std::cerr << "Hash are differents:" << std::endl;
  //   std::cerr << "AnCH result: " << strRes << std::endl;
  //   std::cerr << "Instead of:  " << digest << std::endl;
  //   return 1;
  // } else {
  //   std::cout << "Found value: " << strRes << std::endl;
  //   std::cout << "AnCH implementation time:      " << anchDuration.count() << " µs" << std::endl;
  //   std::cout << "Reference implementation time: " << md5Duration.count() << " µs" << std::endl << std::endl;
  // }
  std::cout << "Found value: " << strRes << std::endl;
  std::cout << "AnCH implementation time:      " << anchDuration.count() << " µs" << std::endl;
  std::cout << "Reference implementation time: " << md5Duration.count() << " µs" << std::endl << std::endl;
}

void
testMillionA() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

  std::ostringstream in;
  for(int i = 0 ; i < 1000000 ; i++) {
    in << 'a';
  }
  std::string msg = in.str();
  std::cout << "Hash message: a million of 'a'" << std::endl;
  start = std::chrono::high_resolution_clock::now();
  MD5 hash(msg);
  const std::array<uint8_t,16>& res = hash.digest();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds anchDuration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());

  std::array<uint8_t,16> sum;
  start = std::chrono::high_resolution_clock::now();
  md5(msg.data(), sum);
  end = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds md5Duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());

  std::ostringstream out;
  out << hash;
  std::string strRes = out.str();
  anch::ut::assertTrue(res == sum);
  // if(res != sum) {
  //   char digest[33];
  //   sprintf(digest,"%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
  // 	    sum[0],sum[1],sum[2],sum[3],sum[4],sum[5],sum[6],sum[7],sum[8],sum[9],sum[10],sum[11],sum[12],sum[13],sum[14],sum[15]);
  //   std::cerr << "Hash are differents:" << std::endl;
  //   std::cerr << "AnCH result: " << strRes << std::endl;
  //   std::cerr << "Instead of:  " << digest << std::endl;
  //   return 1;
  // } else {
  //   std::cout << "Found value: " << strRes << std::endl;
  //   std::cout << "AnCH implementation time:      " << anchDuration.count() << " µs" << std::endl;
  //   std::cout << "Reference implementation time: " << md5Duration.count() << " µs" << std::endl << std::endl;
  // }
  std::cout << "Found value: " << strRes << std::endl;
  std::cout << "AnCH implementation time:      " << anchDuration.count() << " µs" << std::endl;
  std::cout << "Reference implementation time: " << md5Duration.count() << " µs" << std::endl << std::endl;
}

void
testMakefile() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

  std::cout << "Hash file Makefile" << std::endl;
  std::ifstream file("Makefile");
  start = std::chrono::high_resolution_clock::now();
  MD5 hash(file);
  file.close();
  const std::array<uint8_t,16>& res = hash.digest();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds anchDuration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());

  std::ifstream fileBis("Makefile");
  std::ostringstream msg;
  start = std::chrono::high_resolution_clock::now();
  char buffer[1024];
  while(!fileBis.eof()) {
    fileBis.read(buffer, 1024);
    for(int i = 0 ; i < fileBis.gcount() ; ++i) {
      msg << buffer[i];
    }
  }
  fileBis.close();

  std::array<uint8_t,16> sum;
  md5(msg.str().data(), sum);
  end = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds md5Duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());

  std::ostringstream out;
  out << hash;
  std::string strRes = out.str();
  anch::ut::assertTrue(res == sum);
  // if(res != sum) {
  //   char digest[33];
  //   sprintf(digest,"%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
  // 	    sum[0],sum[1],sum[2],sum[3],sum[4],sum[5],sum[6],sum[7],sum[8],sum[9],sum[10],sum[11],sum[12],sum[13],sum[14],sum[15]);
  //   std::cerr << "Hash are differents:" << std::endl;
  //   std::cerr << "AnCH result: " << strRes << std::endl;
  //   std::cerr << "Instead of:  " << digest << std::endl;
  //   return 1;
  // } else {
  //   std::cout << "Found value: " << strRes << std::endl;
  //   std::cout << "AnCH implementation time:      " << anchDuration.count() << " µs" << std::endl;
  //   std::cout << "Reference implementation time: " << md5Duration.count() << " µs" << std::endl << std::endl;
  // }
  std::cout << "Found value: " << strRes << std::endl;
  std::cout << "AnCH implementation time:      " << anchDuration.count() << " µs" << std::endl;
  std::cout << "Reference implementation time: " << md5Duration.count() << " µs" << std::endl << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH MD5 unit tests")
    .description("Test AnCH MD5 library")
    .add("md5-str", testMD5string)
    .add("md5-1Ma", testMillionA)
    .add("md5-makefile", testMakefile)
    ;
}
