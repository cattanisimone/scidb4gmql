#include <vector>
#include <boost/assign.hpp>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "query/Operator.h"
#include "query/FunctionLibrary.h"
#include "query/FunctionDescription.h"
#include "query/TypeSystem.h"

#define SIZE 7
#define KECCAK_ROUNDS 24
#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))

using namespace std;
using namespace scidb;
using namespace boost::assign;

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------


const uint64_t keccakf_rndc[24] =
{
  0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
  0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
  0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
  0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
  0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
  0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
  0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
  0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

const int keccakf_rotc[24] =
{
  1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
  27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
};

const int keccakf_piln[24] =
{
  10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4,
  15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1
};


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------


void keccakf(uint64_t st[25], int rounds)
{
  int i, j, round;
  uint64_t t, bc[5];

  for (round = 0; round < rounds; round++) {

    // Theta
    for (i = 0; i < 5; i++)
    bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];

    for (i = 0; i < 5; i++) {
      t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);
      for (j = 0; j < 25; j += 5)
      st[j + i] ^= t;
    }

    // Rho Pi
    t = st[1];
    for (i = 0; i < 24; i++) {
      j = keccakf_piln[i];
      bc[0] = st[j];
      st[j] = ROTL64(t, keccakf_rotc[i]);
      t = bc[0];
    }

    //  Chi
    for (j = 0; j < 25; j += 5) {
      for (i = 0; i < 5; i++)
      bc[i] = st[j + i];
      for (i = 0; i < 5; i++)
      st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
    }

    //  Iota
    st[0] ^= keccakf_rndc[round];
  }
}

int keccak(const uint8_t *in, int inlen, uint8_t *md, int mdlen)
{
  uint64_t st[25];
  uint8_t temp[201];
  int i, rsiz, rsizw;

  rsiz = 200 - 2 * mdlen;
  rsizw = rsiz / 8;

  memset(st, 0, sizeof(st));

  for ( ; inlen >= rsiz; inlen -= rsiz, in += rsiz) {
    for (i = 0; i < rsizw; i++)
    st[i] ^= ((uint64_t *) in)[i];
    keccakf(st, KECCAK_ROUNDS);
  }

  // last block and padding
  memcpy(temp, in, inlen);
  temp[inlen++] = 1;
  memset(temp + inlen, 0, rsiz - inlen);
  temp[rsiz - 1] |= 0x80;

  for (i = 0; i < rsizw; i++)
  st[i] ^= ((uint64_t *) temp)[i];

  keccakf(st, KECCAK_ROUNDS);

  memcpy(md, st, mdlen);

  return 0;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------


void dimension_hash(const Value** args, Value* res, void*)
{
  if(args[0]->isNull()){
    res->setNull(args[0]->getMissingReason());
    return;
  }

  std::string value = args[0]->getString();
  char str[value.length() + 1];
  strcpy(str, value.c_str());

  uint8_t md[SIZE];
  keccak((uint8_t *) str, strlen(str), md, SIZE);

  uint64_t uhash = 0;
  for(int i=0; i<SIZE; i++)
  uhash = uhash*256 + md[i];

  int64_t hash = (int64_t)uhash;

  res->setInt64( (hash > 0 ? hash : -hash) );
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------


void key_string_sort(const Value** args, Value* res, void*)
{
  if(args[0]->isNull()){
    res->setNull(args[0]->getMissingReason());
    return;
  }
  std::string value = args[0]->getString();
  std::string input = value;
  std::vector<string> words;

  for(std::size_t found = input.find("$$$");
      found != std::string::npos;
      input = input.substr(found+3, input.size()),
      found = input.find("$$$"))
    words.push_back(input.substr(0,found+3));

  for(int i=0; i<words.size(); i++)
    for(int j=0; j<words.size()-i-1; j++)
      if(words[j] > words[j+1])
        swap(words[j], words[j+1]);

  std::string output = "";
  for(int i=0; i<words.size(); i++)
    output += words[i];

  res->setString(output);
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------


REGISTER_FUNCTION(gmql_hash, list_of("string"), "int64", dimension_hash);
REGISTER_FUNCTION(gmql_key_sort, list_of("string"), "string", key_string_sort);
