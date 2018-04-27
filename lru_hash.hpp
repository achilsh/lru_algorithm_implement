#ifndef _LRU_HASH_HPP_ 
#define _LRU_HASH_HPP_

/**
 * @file: lru_hash.hpp
 * @brief: hash()算法实现，如果有更加高效，后续再增加
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-27
 */


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
