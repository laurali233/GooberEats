//
//  ExpandableHashMap.h
//  Proj4
//
//  Created by Lauraaa on 2020/3/5.
//  Copyright © 2020 Lauraaa. All rights reserved.
//

#ifndef ExpandableHashMap_h
#define ExpandableHashMap_h

#include<list>
// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.


template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    int m_size;
    double m_MaxLoadFactor;
    int m_sizeOfArray;
    struct Pair{
        KeyType m_key;
        ValueType m_value;
    };
    std::list<Pair>* arr;
    unsigned int getBucketNumber(const KeyType& k) const;
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType,ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    m_size = 0; // there is initially no element in the hush table
    m_sizeOfArray = 8; // the original size is 8
    m_MaxLoadFactor = maximumLoadFactor;
    arr = new std::list<Pair>[8];
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    delete [] arr;//delete the pointer to an array
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    delete []arr;
    arr = new std::list<Pair>[8];
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_size; //return the size
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    if(m_size>(m_sizeOfArray*m_MaxLoadFactor)){ // if it exceeds the load factor's maximum
        m_sizeOfArray = m_sizeOfArray*2;//increase its size by 2 times
        std::list<Pair>* temporaryArr;
        temporaryArr = new std::list<Pair> [m_sizeOfArray];
        for(int i = 0;i<m_sizeOfArray/2;i++){//look through the original size so divide the size by 2
            typename std::list<Pair>::iterator it2 = arr[i].begin();
            while(it2!=arr[i].end()){
                int tempBucketNum = getBucketNumber((*it2).m_key);//re-associate the value with the key
                Pair TempP = {(*it2).m_key,(*it2).m_value};
                temporaryArr[tempBucketNum].push_back(TempP);
                it2++;
            }
        }
        delete []arr;
        arr = temporaryArr;
        
    }
    
    bool ifReassign = false; // set the original value
    int bucketNum  = getBucketNumber(key);
    typename std::list<Pair>::iterator it = arr[bucketNum].begin();
    while(it!=arr[bucketNum].end()){
        if((*it).m_key == key){
            (*it).m_value = value;// if there exist the same key
            ifReassign = true; // then we need to reassign
        }
        it++;
    }
    
    if(!ifReassign){
        Pair p = {key, value};//change the value instead of adding a new one
        arr[bucketNum].push_back(p);
        m_size++;}
    
        
        
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    int bucketNum = getBucketNumber(key);//get the position of the key
    typename std::list<Pair>::iterator it = arr[bucketNum].begin();
    while(it!= arr[bucketNum].end()){//find the vector
        if((*it).m_key == key) // if there is collision, then use loop to find it
            return &((*it).m_value);
        it++;
    }
    return nullptr;// if not-existed return
    
}

template<typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>:: getBucketNumber(const KeyType& key)const{
    unsigned int hasher(const KeyType& k);
    unsigned int h = hasher(key);
    return h % m_sizeOfArray; //get the proper bucket
}

#endif /* ExpandableHashMap_h */
