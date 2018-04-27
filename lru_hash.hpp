#ifndef _LRU_HASH_HPP_ 
#define _LRU_HASH_HPP_

#define  __STDC_CONSTANT_MACROS
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace LRU_ALG
{
uint32_t hash_fnv1_64(const char *key, size_t key_length);

uint32_t hash_fnv1a_64(const char *key, size_t key_length);

uint32_t murmur3_32(const char *key, uint32_t key_length, uint32_t seed);

}

#endif
