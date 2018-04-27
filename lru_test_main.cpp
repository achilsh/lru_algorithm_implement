/**
 * @file: lru_test_main.cpp
 * @brief: 
 * @author:  wusheng Hu
 * @version: vx0001
 * @date: 2018-04-27
 */

#include "LibStdCout.h"

#include "lru_alg_util_hash.hpp"
#include "lru_alg_util.hpp" 
#include "lru_hash.hpp"

#include <sstream>
#include <string>
#include <vector>

using namespace LRU_ALG;


#define _ReDirect  1

class LruTestItem
{
 public:
  LruTestItem(int x = 0): m_x(x)
  {
  }
  LruTestItem(const LruTestItem& item)
  {
    m_x = item.m_x;      
  }
  LruTestItem& operator=(const LruTestItem& item)
  {
    if (&item != this)
    {
        m_x = item.m_x;
    }
    return *this;
  }

  virtual ~LruTestItem()
  {
  }
  int GetX()
  {
      return m_x;
  }
 private:
  int m_x;
};

#define LRU_TEST_LEN (10)

bool  TestOne()
{
    LruAlg<std::string, LruTestItem>* pTestInstance = \
        new LruAlgKvm<std::string, LruTestItem>( EM_HASH_fnv1_64,  LRU_TEST_LEN);
    if (pTestInstance == NULL)
    {
        return false;
    }
    std::string k1("abcd");
    LruTestItem valone(12);
    DEBUG_LOG("init lru alg  instance succ!");

    bool bRet = pTestInstance->AddItem(k1,valone);
    if (bRet == false)
    {
        ERROR_LOG("add k/v in  lru list fail");
        return false;
    }
    DEBUG_LOG("insert one item into lru succ");
    sleep(1); 
    LruTestItem item_get;
    bRet = pTestInstance->GetItem(k1, &item_get);
    if (bRet == false)
    {
        ERROR_LOG("get item from lru list fail, key: %s", k1.c_str());
        return false;
    }
    DEBUG_LOG("get lru item succ, val: %d", item_get.GetX() );
    return  true;
}

bool TestTwo(int iTestNums)
{
    LruAlg<std::string, LruTestItem>* pTestInstance = \
        new LruAlgKvm<std::string, LruTestItem>( EM_HASH_fnv1_64,  LRU_TEST_LEN);
    if (pTestInstance == NULL)
    {
        return false;
    }

    std::vector<std::string> sKeyList;
    std::vector<LruTestItem> sValList, sGetValList;
    
    for (int i = 0; i < iTestNums; ++i)
    {
        std::stringstream ios;
        ios << i << "_client" ;
        sKeyList.push_back(ios.str());
        sValList.push_back(LruTestItem(i));
    }

    for ( int i= 0; i < iTestNums; ++i )
    {
        DEBUG_LOG("************************************i: %d", i);
        if ( false == pTestInstance->AddItem(sKeyList[i],sValList[i] ) )
        {
            ERROR_LOG("add item fail, test id: %s", sKeyList[i].c_str());
            break;
        }
        sleep(1);
    }

    DEBUG_LOG("\n\r\n\r==========>>>>>> add item done, nums: %d, next step is get item <<<<<<<<============== \n", iTestNums);

    sGetValList.resize(iTestNums);
#ifndef _ReDirect
    for (int i = 0; i < iTestNums; ++i)
    {
        DEBUG_LOG("------------------------------i: %d",i);
        if ( false == pTestInstance->GetItem(sKeyList[i], &sGetValList[i]) )
        {
            ERROR_LOG("get item fail , key: %s", sKeyList[i].c_str());
        }
        sleep(1);
    }
#else
    for (int i = iTestNums-1; i >=0 ; --i)
    {
        DEBUG_LOG("------------------------------i: %d",i);
        if ( false == pTestInstance->GetItem(sKeyList[i], &sGetValList[i]) )
        {
            ERROR_LOG("get item fail , key: %s", sKeyList[i].c_str());
        }
        sleep(1);
    }

#endif


    DEBUG_LOG("==========>-------- begin to traverse lru list node info");
    pTestInstance->TraverseLruList();
    return true;
}

bool TestThree()
{
    //add + del + add + tranverse.
    LruAlg<std::string, LruTestItem>* pTestInstance = \
        new LruAlgKvm<std::string, LruTestItem>( EM_HASH_fnv1_64,  LRU_TEST_LEN);
    if (pTestInstance == NULL)
    {
        return false;
    }
    int iTestNums = LRU_TEST_LEN;

    std::vector<std::string> sKeyList;
    std::vector<LruTestItem> sValList, sGetValList;
    
    for (int i = 0; i < iTestNums + 2; ++i)
    {
        std::stringstream ios;
        ios << i << "_client" ;
        sKeyList.push_back(ios.str());
        sValList.push_back(LruTestItem(i));
    }
    std::string sDelOneKey, sDelTwoKey;
    int iDelIndexOne = 3, iDelIndexTwo = 7; //del 4th and 8th node
    
    int i = 0;
    for ( ; i < iTestNums; ++i )
    {
        DEBUG_LOG("************************************i: %d", i);
        if ( false == pTestInstance->AddItem(sKeyList[i],sValList[i] ) )
        {
            ERROR_LOG("add item fail, test id: %s", sKeyList[i].c_str());
            break;
        }
        //
        if (iDelIndexOne == i)
        {
            sDelOneKey = sKeyList[i];
        }
        if (iDelIndexTwo == i)
        {
            sDelTwoKey = sKeyList[i];
        }
        sleep(1);
    }

    DEBUG_LOG("\n\r\n\r==========>>>>>> add item done, nums: 2 <<<<<<<<============== \n");
    if (pTestInstance->DelItem(sDelOneKey) == false)
    {
        ERROR_LOG("del item fail, key: %s", sDelOneKey.c_str());
        return false;
    }
    if (pTestInstance->DelItem(sDelTwoKey) == false)
    {
        ERROR_LOG("del item fail, key: %s", sDelTwoKey.c_str());
        return false;
    }

    DEBUG_LOG("\n\r\n\r==========>>>>>> del item done, add/ item <<<<<<<<============== \n");
    for ( ; i < iTestNums + 2; ++i )
    {
        DEBUG_LOG("************************************i: %d", i);
        if ( false == pTestInstance->AddItem(sKeyList[i],sValList[i] ) )
        {
            ERROR_LOG("add item fail, test id: %s", sKeyList[i].c_str());
            break;
        }
        sleep(1);
    }

    DEBUG_LOG("==========>-------- begin to traverse lru list node info");
    pTestInstance->TraverseLruList();

    return true;
}

bool TestFour()
{
    LruAlg<std::string, LruTestItem>* pTestInstance = \
        new LruAlgKvm<std::string, LruTestItem>( EM_HASH_fnv1_64,  LRU_TEST_LEN);
    if (pTestInstance == NULL)
    {
        return false;
    }
    int iTestNums = LRU_TEST_LEN;
    std::vector<std::string> sKeyList;
    std::vector<LruTestItem> sValList, sGetValList;
    
    for (int i = 0; i < iTestNums; ++i)
    {
        std::stringstream ios;
        ios << i << "_client" ;
        sKeyList.push_back(ios.str());
        sValList.push_back(LruTestItem(i));
    }

    for ( int i= 0; i < iTestNums; ++i )
    {
        DEBUG_LOG("************************************i: %d", i);
        if ( false == pTestInstance->AddItem(sKeyList[i],sValList[i] ) )
        {
            ERROR_LOG("add item fail, test id: %s", sKeyList[i].c_str());
            break;
        }
        sleep(1);
    }

    DEBUG_LOG("\n\r\n\r==========>>>>>> add item done, nums: %d, next step is get item 2 node <<<<<<<<============== \n", iTestNums);

    sGetValList.resize(iTestNums);
    int iGetOneIndex = 2; //3'th node
    int iGetTwoIndex = 5; //6'th node
    DEBUG_LOG("-------------> get item node, key: %s <-------------", sKeyList[iGetOneIndex].c_str());
    if ( false == pTestInstance->GetItem(sKeyList[iGetOneIndex], &sGetValList[iGetOneIndex]) )
    {
        ERROR_LOG("get item fail , key: %s", sKeyList[iGetOneIndex].c_str());
    }
    sleep(1);

    DEBUG_LOG("-------------> get item node, key: %s <-------------", sKeyList[iGetTwoIndex].c_str());
    if ( false == pTestInstance->GetItem(sKeyList[iGetTwoIndex], &sGetValList[iGetTwoIndex]) )
    {
        ERROR_LOG("get item fail , key: %s", sKeyList[iGetTwoIndex].c_str());
    }
    DEBUG_LOG("==========>-------- begin to traverse lru list node info");
    pTestInstance->TraverseLruList();
    return true;
}

int main(int argc, char **argv)
{
    // TestOne();
    //TestTwo(8);
    //TestTwo(12);
    //TestThree();
    TestFour();
    return 0;
}
