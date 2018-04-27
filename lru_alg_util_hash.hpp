/**
 * @file: lru_alg_util_hash.hpp
 * @brief:  lru算法结合具体hash()函数实现，是lru算法模板的实例化实现。 
 *          如果有更高效的hash()函数，可以重写该文件
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-27
 */

#ifndef _LRU_ALG_UTIL_HASH_HPP_
#define _LRU_ALG_UTIL_HASH_HPP_

#include "LibStdCout.h"
#include "lru_alg_util.hpp"
#include "lru_hash.hpp"
namespace LRU_ALG
{

    enum HASH_TYPE
    {
        EM_HASH_fnv1_64     = 1,
        EM_HASH_fnv1a_64    = 2,
        EM_HASH_murmur3_32  = 3,
    };

    template<typename K, typename U>
    class KVMemHash: public KVMem<K,U>
    {
     public:
      KVMemHash(int iHashType);
      virtual ~KVMemHash();
      virtual uint32_t HashFunc(const K& inKey, uint32_t len);
     private:
      int m_iHashType;
    };

    template<typename K,typename U>
    KVMemHash<K,U>::KVMemHash(int iHashType): KVMem<K,U>(),m_iHashType(iHashType)
    {
    }

    template<typename K,typename U>
    KVMemHash<K,U>::~KVMemHash()
    {
    }

    template<typename K,typename U>
    uint32_t KVMemHash<K,U>::HashFunc(const K& inKey, uint32_t len)
    {
        int iRet = 0;
        switch(m_iHashType)
        {
            case EM_HASH_fnv1_64:
                iRet = hash_fnv1_64(inKey.c_str(), len);
                break;
            case EM_HASH_fnv1a_64:
                iRet = hash_fnv1a_64(inKey.c_str(), len);
                break;
            case EM_HASH_murmur3_32:
                iRet = murmur3_32(inKey.c_str(), len,0x000001b3);
                break;
            default:
                break;
        }
        return iRet;
    }

    /**-----------------------------------------------------**/
    /**-----------------------------------------------------**/
    /**-----------------------------------------------------**/
    template<typename K, typename U>
    class LruAlgKvm: public LruAlg<K,U>
    {
     public:
      LruAlgKvm(int iHashType, const int iQueLen);
      virtual ~LruAlgKvm();
      virtual KVMem<K,U>* NewKVMem();
      virtual bool TraverseLruListNode(ItemVal<K,U>* pItem);
     private:
      int m_iHashType;
    };

    template<typename K, typename U>
    LruAlgKvm<K,U>::LruAlgKvm(int iHashType, const int iQueLen): LruAlg<K,U>(iQueLen), m_iHashType(iHashType)
    {
    }

    template<typename K, typename U>
    LruAlgKvm<K,U>::~LruAlgKvm()
    {
    }

    template<typename K, typename U>
    KVMem<K,U>* LruAlgKvm<K,U>::NewKVMem()
    {
        KVMem<K,U>* p_Kvm = NULL;   
        p_Kvm = new KVMemHash<K,U>(m_iHashType);

        return p_Kvm;
    }

    template<typename K, typename U>
    bool LruAlgKvm<K,U>::TraverseLruListNode(ItemVal<K,U>* pItem)
    {
        if (pItem == NULL)
        {
            return false;
        }
        DEBUG_LOG("key: %s, op_tm: %u", pItem->GetKey().c_str(), pItem->GetItemTimeStamp());        
        return true;
    }
}

#endif
