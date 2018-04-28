#ifndef _lru_alg_util_impl_hpp_
#define _lru_alg_util_impl_hpp_

/**
 * @file: lru_alg_util_impl.hpp
 * @brief:  lru算法模板的实现文件
 * @author:  wusheng Hu
 * @version: v0x0001
 * @date: 2018-04-27
 */


#include "LibStdCout.h"
#include "lru_alg_util.hpp"

namespace LRU_ALG
{
    /**< static var define  */
    template<typename K, typename U>
    const int32_t KVMem<K,U>::CNT_STATIC_HASHPOWER;
    
    /**<  ItemVal class implement begin */
    template<typename K, typename U>
    ItemVal<K,U>::ItemVal() :m_pData(NULL), m_pLruNextNode(NULL), m_pLruPreNode(NULL),m_pHashNext(NULL),
                        m_tOpTm(time(NULL)), m_iLruQueLen(0)
    {
    }

    template<typename K, typename U>
    ItemVal<K,U>::ItemVal(const K& inKey, const U& inVal): m_pLruNextNode(NULL),m_pLruPreNode(NULL), 
                m_pHashNext(NULL), m_tOpTm(time(NULL)), m_iLruQueLen(0)
    {
        m_pData  = new U(inVal);
        m_key = inKey;
    }

    template<typename K, typename U>
    ItemVal<K,U>::~ItemVal()
    {
        if (m_pData)
        {
            delete m_pData; m_pData = NULL;
        }
        m_pHashNext = m_pLruPreNode = m_pLruNextNode = NULL;
        m_iLruQueLen = 0; 
    }

    template<typename K, typename U>
    bool ItemVal<K,U>::SetLruNextNode(ItemVal<K,U>* p_LruNode)
    {
        if (p_LruNode == NULL)
        {
            m_pLruNextNode = NULL;
            return true;
        }
        m_pLruNextNode->m_pLruPreNode = p_LruNode;
        p_LruNode->m_pLruPreNode =  this;

        p_LruNode->m_pLruNextNode = m_pLruNextNode;
        m_pLruNextNode = p_LruNode;
        
        p_LruNode->SetLruPreNode(this);
        return true;
    }

    template<typename K, typename U>
    bool ItemVal<K,U>::SetLruPreNode(ItemVal<K,U>* p_LruNode)
    {
        if (p_LruNode == NULL)
        {
            m_pLruPreNode = NULL;
            return true;
        }
        p_LruNode->m_pLruNextNode = this;
        m_pLruPreNode = p_LruNode;
        return true;
    }

    template<typename K, typename U>
    bool ItemVal<K,U>::AddLruNode(ItemVal<K,U>* pNode, int iDirection)
    {
        if ( pNode == NULL || (PRE_DIR != iDirection && NEXT_DIR != iDirection) )
        {
            ERROR_LOG("to add node is null or direct of add is not clearly");
            return false;
        }
        if (iDirection == PRE_DIR)
        {
            return SetLruPreNode(pNode);
        }
        if (iDirection == NEXT_DIR)
        {
            return SetLruNextNode(pNode);
        }
        return true;
    }
    
    template<typename K, typename U>
    bool ItemVal<K,U>::DelItemKeyVal()
    {
        return true;
    }

    //============================================ ================//
    //============================================ ================//
    //============================================ ================//
    /**  KVMem class implement begin ***/ 
    template<typename K, typename U>
    KVMem<K,U>::KVMem(): m_pItemList(NULL), m_bAllocator(false)
    {
        m_pItemList = new TYPE_PTR_ITEM_VAL [ HashSize(CNT_STATIC_HASHPOWER)*sizeof(TYPE_PTR_ITEM_VAL) ]; 
        if (m_pItemList == NULL)
        {
            return ;
        }
        m_bAllocator = true;
    }
    
    template<typename K, typename U>
    KVMem<K,U>::~KVMem()
    {
        if (m_pItemList != NULL)
        {
            uint32_t uiHashListLen = HashSize(CNT_STATIC_HASHPOWER);
            for (uint32_t uiIndex = 0; uiIndex < uiHashListLen; ++uiIndex)
            {
                ItemVal<K,U>* p_HashListHead = m_pItemList[uiIndex];
                while (p_HashListHead != NULL)
                {
                    ItemVal<K,U>* p_ToDelNode =  p_HashListHead->GetHashNext(); 
                    delete p_HashListHead;
                    p_HashListHead =  p_ToDelNode;
                }
            }
            delete [] m_pItemList;
            m_pItemList = NULL;
        }
        m_bAllocator = false;
    }

    template<typename K, typename U>
    ItemVal<K,U>* KVMem<K,U>::GetItemFromMem(const K& inKey)
    {
        TYPE_PTR_ITEM_VAL pItem = NULL;
        if (m_pItemList == NULL)
        {
            DEBUG_LOG("kv hash mem addr is empty, key: %s", inKey.c_str());
            return pItem;
        }

        int iLen = inKey.size();
        uint32_t uiHashVal = HashFunc(inKey, iLen);
        uint32_t uiIndex = uiHashVal & HashMask();
        pItem = m_pItemList[uiIndex];
        DEBUG_LOG("hash val: %u, hash index: %d, key: %s, hash head: %p",
                  uiHashVal, uiIndex, inKey.c_str(), pItem);
        
        while(pItem)
        {
            if (pItem->GetKey() == inKey)
            {
                DEBUG_LOG("find key in lru hash mem, key: %s, item_val_addr: %p", inKey.c_str(), pItem);
                break;
            } 
            pItem = pItem->GetHashNext();
            DEBUG_LOG("hash next node addr: %p", pItem);
            sleep(1);
        }
        return pItem;
    }

    template<typename K, typename U>
    bool KVMem<K,U>::AddItemIntoMem(const K& inKey, ItemVal<K,U>* pItemval)
    {
        if (m_pItemList == NULL)
        {
            return false;
        }

        ItemVal<K,U>* p_GetNode  = GetItemFromMem(inKey);
        if (p_GetNode == NULL)
        {
            DEBUG_LOG("new node insert lru hash mem, key: %s", inKey.c_str());
            int iLen = inKey.size();
            uint32_t uiHashVal = HashFunc(inKey, iLen);
            uint32_t uiIndex = uiHashVal & HashMask();
            ItemVal<K,U>* p_HashHead = m_pItemList[uiIndex];
            DEBUG_LOG("key hash val: %u, hash index: %d, key: %s",uiHashVal, uiIndex, inKey.c_str());
            
            if (p_HashHead == NULL)
            {
                DEBUG_LOG("lru hash mem list is empty, insert node in hash mem list head, key: %s", inKey.c_str());
                m_pItemList[uiIndex] = pItemval; //pItemval is heap mem.
                return true;
            }

            pItemval->SetHashNext(p_HashHead);
            m_pItemList[uiIndex] = pItemval;
            return true;
        } 
        else 
        {
            ERROR_LOG("to add item existed in lru hash mem, key: %s", inKey.c_str());
            return false;
        }
        return true;
    }

    template<typename K, typename U>
    ItemVal<K,U>* KVMem<K,U>:: DelItemByKey(const K& inKey)
    {
        if (m_pItemList == NULL)
        {
            return NULL;
        }

        int iLen = inKey.size();
        uint32_t uiHashVal = HashFunc(inKey, iLen);
        uint32_t uiIndex = uiHashVal & HashMask();
        ItemVal<K,U>* p_ToFindNode = m_pItemList[uiIndex];
        if (p_ToFindNode == NULL)
        {
            return NULL;
        }

        ItemVal<K,U>* p_PrehashNode = NULL;
        while (p_ToFindNode)
        {
            if (p_ToFindNode->GetKey() == inKey)
            {
                DEBUG_LOG("find key: %s in hash mem", inKey.c_str());
                if (m_pItemList[uiIndex] == p_ToFindNode) //first node
                {
                    m_pItemList[uiIndex] = p_ToFindNode->GetHashNext();
                    DEBUG_LOG("find node is lru hash mem list head, hash list head: %p, next node: %p", 
                              p_ToFindNode, m_pItemList[uiIndex]);
                }
                else 
                {
                    p_PrehashNode->SetHashNext(p_ToFindNode->GetHashNext());
                }
                p_ToFindNode->SetHashNext(NULL);
                return p_ToFindNode;
            }

            p_PrehashNode = p_ToFindNode;
            p_ToFindNode = p_PrehashNode->GetHashNext();
        }
        return NULL; 
    }
    //------------------------------------------------------------------//
    //
    //
    //------------------------------------------------------------------//
    template<typename K, typename U>
    LruNodeFreeList<K,U>::LruNodeFreeList() :m_pFreeNodeHead(NULL), m_uiFreeListCurSize(0)
    {
    }

    template<typename K, typename U>
    LruNodeFreeList<K,U>::~LruNodeFreeList()
    {
    }

    template<typename K, typename U>
    ItemVal<K,U>* LruNodeFreeList<K,U>::AllocateNode()
    {
        ItemVal<K,U>* pFreeNode = NULL;
        if (m_uiFreeListCurSize <= 0 || m_pFreeNodeHead == NULL)
        {
            DEBUG_LOG("node free list not has useable node");
            return pFreeNode;
        }
        pFreeNode = m_pFreeNodeHead;
        m_pFreeNodeHead = pFreeNode->GetNextNode();
        m_uiFreeListCurSize--;
        //
        return pFreeNode;
    }

    template<typename K, typename U>
    bool LruNodeFreeList<K,U>::ReCycleNode(ItemVal<K,U>* pNode)
    {
        if (pNode == NULL)
        {
            return true;
        }
        pNode->ResetNode();
        DEBUG_LOG("recycle node to free list node, key: %s, node ptr: %p",
                  pNode->GetKey().c_str(), pNode);
        if (m_pFreeNodeHead)
        {
            m_pFreeNodeHead->SetLruPreNode(pNode);
        }
        m_pFreeNodeHead = pNode;
        m_uiFreeListCurSize ++;
        return true;
    }


    //------------------------------------------------------------------//
    //
    //
    //------------------------------------------------------------------//
    template<typename K, typename U>
    LruAlg<K,U>::LruAlg(const int iQueLen): m_pHeadLru(NULL), m_pTailLru(NULL), 
                                            m_pkvMem(NULL), m_iQueLen(iQueLen),
                                            m_pFreeList(NULL)
    {
        m_pFreeList = new LruNodeFreeList<K,U>();
    }

    template<typename K, typename U>
    LruAlg<K,U>::~LruAlg()
    {
        if (m_pkvMem)
        {
            delete m_pkvMem;
            m_pkvMem = NULL;
        }
        m_pHeadLru = m_pTailLru = NULL;
        if (m_pFreeList)
        {
            delete m_pFreeList;
            m_pFreeList = NULL;
        }
        // 
    }

    template<typename K, typename U>
    bool LruAlg<K,U>::GetItem(const K& inKey, U* oPVal)
    {
        ItemVal<K,U>* pItemVal = LruAlgNewKVMem()->GetItemFromMem(inKey);
        if (pItemVal == NULL)
        {
            ERROR_LOG("node not exist in hash mem, key: %s", inKey.c_str());
            return false;
        }
        DEBUG_LOG("node exist in hash mem, key: %s, val addr: %p", inKey.c_str(), pItemVal);
        
        if (UnlinkItem(pItemVal) == false)
        {
            ERROR_LOG("unlink node from lru list fail, key: %s, node addr: %p", inKey.c_str(), pItemVal);
            return false;
        }
        uint32_t tmBeforeOp = pItemVal->GetItemTimeStamp();
        pItemVal->UpdateOpTm(time(NULL));

        if (IsEmptyLru())
        {
            m_pHeadLru =  pItemVal;
            m_pTailLru = pItemVal;
        }

        DEBUG_LOG("timestamp: %u before op, timestamp: %u after op, key: %s", 
                  tmBeforeOp, pItemVal->GetItemTimeStamp(), inKey.c_str());
        if (LinkItem(pItemVal) == false)
        {
            std::cout << "link node into head fail, key: " << inKey << std::endl;
            return false;
        }
        
        if (pItemVal->GetpVal() == NULL)
        {
            return false;
        }
        //
        *oPVal =  *(pItemVal->GetpVal());
        DEBUG_LOG("get item val from lru list succ, key: %s, tm_op: %u", inKey.c_str(),
                  pItemVal->GetItemTimeStamp());
        return true;
    }

    template<typename K, typename U>
    bool LruAlg<K,U>::DelItem(const K& inKey)
    {
        ItemVal<K,U>* pItemVal =  LruAlgNewKVMem()->GetItemFromMem(inKey);
        if (pItemVal == NULL)
        {
            std::cout << "DelItem(), val is empty from mem  by key: " << inKey << std::endl;
            return false;
        }

        if (UnlinkItem(pItemVal) == false)
        {
            std::cout << "DelItem(), unlink node from lru list fail, key: " << inKey << std::endl;
            return false;
        }
        //
        ItemVal<K,U>* pDelNode = NULL;
        pDelNode = LruAlgNewKVMem()->DelItemByKey(inKey);
        if ( pDelNode == NULL )
        {
            std::cout << "DelItem(), del item node for key: " << inKey << std::endl;
            return false;
        }
        m_pFreeList->ReCycleNode(pDelNode);

        DEBUG_LOG("DelItem() succ, key: %s", inKey.c_str());
        return true;
    }

    template<typename K, typename U>
    bool LruAlg<K,U>::AddItem(const K& inKey, const U& inVal)
    {
        DEBUG_LOG("=========================>> key: %s", inKey.c_str());
        ItemVal<K,U>* p_OldItem = LruAlgNewKVMem()->GetItemFromMem(inKey);
        if (p_OldItem != NULL)
        {
            ERROR_LOG("add existed node in lru list has mem, key: %s", inKey.c_str());
            return false;
        }
        DEBUG_LOG("new node add to lru hash mem, do next step, key: %s", inKey.c_str());
        
        ItemVal<K,U>* p_NewItem  = NULL;
        if (m_pFreeList && (NULL != (p_NewItem = m_pFreeList->AllocateNode())))
        {
            p_NewItem->SetKeyVal(inKey, inVal);
            DEBUG_LOG("allocate node from free list, node addr: %p", p_NewItem);
        }
        else
        {
            p_NewItem = new ItemVal<K,U>(inKey, inVal);
        }
        if (IsEmptyLru())
        {
            m_pHeadLru =  p_NewItem;
            m_pTailLru = p_NewItem;
        }

        ItemVal<K,U>* p_ItemHead = m_pHeadLru;
        DEBUG_LOG("add new item val addr: %p, lru list head addr: %p, new item timestamp: %u", 
                  p_NewItem, m_pHeadLru, p_NewItem->GetItemTimeStamp());
            
        if (p_ItemHead->GetQueLen() < m_iQueLen)
        {
            DEBUG_LOG("cur lru list size: %d, max size: %d", p_ItemHead->GetQueLen(), m_iQueLen);
            LinkItem(p_NewItem);
            m_pHeadLru->SetQueLen(p_ItemHead->GetQueLen() + 1);
        }
        else 
        {
            ItemVal<K,U>* p_itemTail = m_pTailLru;
            p_NewItem->SetQueLen(m_iQueLen);
            DEBUG_LOG("lru list is full, add new node need to swap old node, old key: %s", 
                      p_itemTail->GetKey().c_str());
            UnlinkItem(p_itemTail);
            LruAlgNewKVMem()->DelItemByKey(p_itemTail->GetKey());
            m_pFreeList->ReCycleNode(p_itemTail);
            LinkItem(p_NewItem);
        }
        
        LruAlgNewKVMem()->AddItemIntoMem(inKey, p_NewItem);
        return true;
    }

    template<typename K, typename U>
    KVMem<K,U>* LruAlg<K,U>::LruAlgNewKVMem()
    {
        if (m_pkvMem == NULL)
        {
            m_pkvMem = NewKVMem();
        }
        return m_pkvMem;
    }

    template<typename K, typename U>
    bool LruAlg<K,U>::IsEmptyLru()
    {
        if (m_pHeadLru == NULL && m_pHeadLru == m_pTailLru)
        {
            DEBUG_LOG("lru list empty");
            return true;
        }
        return false;
    }

    template<typename K, typename U>
    bool LruAlg<K,U>::Update(const K& inKey, const U& inVal)
    {
        return true;
    }

    template<typename K, typename U>
    bool LruAlg<K,U>::UnlinkItem(ItemVal<K,U>* pItem)
    {
        if (pItem == NULL)
        {
            ERROR_LOG("in node addr is null");
            return false;
        }
        if (IsEmptyLru())
        {
            ERROR_LOG("lru list empty");
            return false;
        }
        if (m_pTailLru == m_pHeadLru)
        {
            if (m_pTailLru == pItem)
            {
                DEBUG_LOG("unlink the only one node in lru list, item addr: %p", pItem);
                m_pHeadLru =  m_pTailLru = NULL;
                return true;
            }
            ERROR_LOG("not find item in lru list, item addr: %p", pItem);
            return false;
        }
        
        //more than one node.
        if (m_pHeadLru == pItem)
        {
            m_pHeadLru = pItem->GetNextNode();
            (m_pHeadLru)->SetQueLen(pItem->GetQueLen() - 1);
            return true;
        }

        if (m_pTailLru == pItem)
        {
            ItemVal<K,U>* pNewTail =  (m_pTailLru)->GetPreNode();
            pNewTail->SetLruNextNode(NULL);
            (m_pHeadLru)->SetQueLen((m_pHeadLru)->GetQueLen() - 1);
            m_pTailLru = pNewTail;
            return true;
        }

        ItemVal<K,U>* p_PreNode =  pItem->GetPreNode();
        ItemVal<K,U>* p_NextNode = pItem->GetNextNode();
        
        (m_pHeadLru)->SetQueLen((m_pHeadLru)->GetQueLen() - 1);
        p_NextNode->SetLruPreNode(p_PreNode);
        p_PreNode->SetHashNext(p_NextNode);
        return true;
    }


    template<typename K, typename U>
    bool LruAlg<K,U>::LinkItem(ItemVal<K,U>* pItem)
    {
        if (pItem == NULL)
        {
            return false;
        }

        if (pItem == m_pHeadLru)
        {
            DEBUG_LOG("lru first add node, add node on head");
            return true;
        }
        if (m_pHeadLru == m_pTailLru && m_pHeadLru != NULL)
        {
            (m_pHeadLru)->AddLruNode(pItem, PRE_DIR);
            m_pTailLru = m_pHeadLru;
            m_pHeadLru = pItem;
            return true;
        }

        m_pHeadLru->AddLruNode(pItem, PRE_DIR);
        m_pHeadLru = pItem;
        return true;
    }

    template<typename K, typename U>
    bool LruAlg<K,U>::TraverseLruList()
    {
        ItemVal<K,U>* pTraverseNode =  m_pHeadLru;
        while(pTraverseNode)
        {
            TraverseLruListNode(pTraverseNode);
            if (pTraverseNode == m_pTailLru)
            {
                return true;
            }

            pTraverseNode = pTraverseNode->GetNextNode();
        }
        return true;
    }
}
#endif
