/**
 * @file: lru_alg_util.hpp
 * @brief: 
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-25
 */

#ifndef _LRU_ALG_UTIL_HPP_
#define _LRU_ALG_UTIL_HPP_

#include <stdint.h>
#include <iostream>
#include <deque>
#include <map>

namespace LRU_ALG
{
    enum LRUAddDir
    {
        PRE_DIR     = 2,
        NEXT_DIR    = 4,
    };

    /****************************************
     * @brief: 
     *       define item data
    ****************************************/
    template<typename K, typename U>
    class ItemVal
    {
     public:
      ItemVal();
      ItemVal(const K& inKey, const U& inVal);
      virtual ~ItemVal();
      
      bool AddLruNode(ItemVal<K,U>* pNode, int iDirection);
      void UpdateOpTm(uint32_t uiTm)
      {
        if (uiTm == -1)
        {
            m_tOpTm = time(NULL); //now();
            return ;
        } 
        m_tOpTm  = uiTm;
      }
      uint32_t GetItemTimeStamp()
      {
          return m_tOpTm;
      }

      void SetQueLen(int32_t iLen)
      {
          m_iLruQueLen = iLen;
      }

      int32_t GetQueLen()
      {
          return m_iLruQueLen;
      }

      U* GetpVal() 
      {
          return m_pData;
      }
      K GetKey() const 
      {
          return m_key;
      }

      ItemVal<K,U>* GetHashNext()
      {
          return m_pHashNext;
      }

      void SetHashNext(ItemVal<K,U>* p_HashNewNode)
      {
          m_pHashNext = p_HashNewNode;
      }

      bool SetLruPreNode(ItemVal<K,U>* p_LruPreNode);
      bool SetLruNextNode(ItemVal<K,U>* p_LruNextNode);

       ItemVal<K,U>* GetNextNode()
       {
           return m_pLruNextNode;
       }
       ItemVal<K,U>** GetNextNodePtr()
       {
           return &m_pLruNextNode;
       }

       ItemVal<K,U>* GetPreNode()
       {
           return m_pLruPreNode;
       }
      bool DelItemKeyVal();
     private:
      U* m_pData;
      K  m_key;
      ItemVal<K,U>* m_pLruNextNode; //lru chain
      ItemVal<K,U>* m_pLruPreNode;  //lru chain

      ItemVal<K,U>* m_pHashNext; //hash list
      volatile uint32_t m_tOpTm; //node op time
      int32_t m_iLruQueLen;       //lru curl node nums
    };

    /*****************************************************
     * @brief: 
     *  define <k,v> really store data struction.
     *****************************************************/
    template<typename K, typename U>
    class KVMem
    {
     public:
      KVMem();
      virtual ~KVMem();
         
      typedef ItemVal<K,U>*  TYPE_PTR_ITEM_VAL ;

      ItemVal<K,U>* GetItemFromMem(const K& inKey);
      bool AddItemIntoMem(const K& inKey, ItemVal<K,U>* pItemval);
      bool DelItemByKey(const K& inKey);
      virtual uint32_t HashFunc(const K& inKey, uint32_t len) = 0;
     private:
      uint32_t  HashSize(int32_t n)
      {
        return  1 << n ;
      }
      uint32_t  HashMask()
      {
        return HashSize(CNT_STATIC_HASHPOWER) - 1;
      }
     private:
      static const int32_t CNT_STATIC_HASHPOWER = 16;
      TYPE_PTR_ITEM_VAL* m_pItemList; // list for  addr of  ItemVal<K,U>
      bool m_bAllocator;
    };

    /*******************************************************************************
     * @brief: 
     *      define lru main struct; include head/tail array + item list +  free list, 
     *      has op: add/get/del/update
     ********************************************************************************/
    template<typename K, typename U>
    class LruAlg
    {
     public:
      LruAlg(const int iQueLen);
      virtual ~LruAlg();

      bool GetItem(const K& inKey, U* oPVal);
      bool DelItem(const K& inKey);
      bool AddItem(const K& inKey, const U& inVal);
      bool Update(const K& inKey, const U& inVal);

      bool TraverseLruList();
     private:
      bool UnlinkItem(ItemVal<K,U>* pItem);
      bool LinkItem(ItemVal<K,U>* pItem);
      //
      bool IsEmptyLru();
     protected:
      KVMem<K,U>* LruAlgNewKVMem();
      //
      virtual KVMem<K,U>* NewKVMem() = 0; //lrualg's deriver implement kvmem'dervier
      virtual bool TraverseLruListNode(ItemVal<K,U>* pItem) = 0;
      // 
      ItemVal<K,U>* m_pHeadLru;
      ItemVal<K,U>* m_pTailLru;
      KVMem<K,U>* m_pkvMem;
      int m_iQueLen;
    };
}

#include "lru_alg_util_impl.hpp"

#endif

