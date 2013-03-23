#ifndef TABLE_HPP_
#define TABLE_HPP_

//#include <boost/pool/pool.hpp>
#include <cassert>
#include <vector>
#include <algorithm>
#include <cstdio>

#include "defs.hpp"

namespace utils
{
   const INT NumObjectsInPage = 512;
   const INT LogOfObjectsInPage = 9;
   const INT NumInitialPages = 512;

  template <typename T>
  class tTABLE_ALLOCATOR
  {
  public:
    tTABLE_ALLOCATOR(INT numObjectsInPage = NumObjectsInPage, 
		     INT logOfObjectsInPage = LogOfObjectsInPage,
		     INT numInitialPages = NumInitialPages,
		     INT numObjects = 0);
    T* at(INT i);
    ~tTABLE_ALLOCATOR();
    void purge();
  private:
    void init(INT numObjects);
    inline INT FirstLevelIndex(INT i);
    inline INT IndexInPage(INT i);

  private:
    INT m_numObjectsInPage;
    INT m_numInitialPages;
    INT m_logOfObjectsInPage;

    //boost::pool<> m_pages;
    INT m_numPossiblePages;
    INT m_numAllocatedPages;
    T **m_pPageTable;
  };


  template <typename T, typename _TABLE_TYPE>
  class tTABLE_ITERATOR {
    typedef tTABLE_ITERATOR<T, _TABLE_TYPE> cSELF;
  public:
    /*
     * typedefs for public usage.
     */
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;

  public:
    tTABLE_ITERATOR(INT i, _TABLE_TYPE &table) : m_index(i),
						 m_table(table)
    {}

    cSELF& operator++() {
      m_index++;
      return *this;
    }

    cSELF& operator++(int) {
      return ++*this;
    }

    cSELF& operator--() {
      m_index--;
      return *this;
    }

    cSELF& operator--(int) {
      return --*this;
    }

    //Boost error message. --B 08/02/2009.
    cSELF& operator=(const cSELF &obj) {
      m_index = obj.m_index ;
      m_table = (obj.m_table);
      return *this;
    }

    BOOL operator==(const cSELF &obj) const {
      return &m_table == &obj.m_table &&
	m_index == obj.m_index ? true : false;
    }

    BOOL operator!=(const cSELF &obj) const {
      return !(*this == obj);
    }

    reference operator*() { return m_table[m_index]; }
    pointer operator->() { return m_table.object_at(m_index); }

    const reference operator*() const { return m_table[m_index]; }
    const pointer operator->() const { return m_table.object_at(m_index); }

  protected:
    INT m_index;
    _TABLE_TYPE &m_table;
  };

  template <typename T>
  class tTABLE
  {
    typedef tTABLE<T> cSELF;
  public:
    typedef tTABLE_ITERATOR<T, cSELF> iterator;

  public:
    inline tTABLE(INT numObjectsInPage = NumObjectsInPage, 
		  INT logOfObjectsInPage = LogOfObjectsInPage,
		  INT numInitialPages = NumInitialPages,
		  INT numInitialObjects = 0);
    ~tTABLE();

    inline iterator begin() { return iterator(0, *this); }
    inline iterator end() { return iterator(m_numObjects, *this); }

    inline const T& operator[](INT index) const;
    inline T& operator[](INT index);

    inline void push_back(const T& obj);
    inline INT size() const;
    inline BOOL empty() const;

    inline T* object_at(INT index);
    inline T* new_object(); //appends new element at the end of table.
    
    inline void clear();

    /*
     *Calls destructors on existing objects, but retains the storage.
     */
    inline void flush();
    
  private:
    tTABLE_ALLOCATOR<T> m_allocator;
    INT m_numObjects;
  };

  template <typename T>
  inline INT tTABLE_ALLOCATOR<T>::FirstLevelIndex(INT i) { return i >> m_logOfObjectsInPage ; }

  template <typename T>
  inline INT tTABLE_ALLOCATOR<T>::IndexInPage(INT i) { return i & (m_numObjectsInPage - 1); }

  template <typename T>
  tTABLE_ALLOCATOR<T>::tTABLE_ALLOCATOR(INT numObjectsInPage, INT logOfObjectsInPage,
					INT numInitialPages, INT numObjects) : m_numObjectsInPage(numObjectsInPage),
									       m_numInitialPages(numInitialPages),
									       m_logOfObjectsInPage(logOfObjectsInPage)
  {
    m_pPageTable = NULL;
    m_numAllocatedPages = 0;
    init(numObjects);
 
  }

  template <typename T>
  void tTABLE_ALLOCATOR<T>::init(INT numObjects)
  {
    if(numObjects < m_numObjectsInPage * m_numInitialPages)
      m_numPossiblePages = m_numInitialPages;
    else
      m_numPossiblePages = numObjects >> m_logOfObjectsInPage;
    m_pPageTable = (T**)realloc(m_pPageTable, sizeof(T*)*m_numPossiblePages);

    for(INT i = m_numAllocatedPages; i < m_numPossiblePages; i++)
      m_pPageTable[i] = NULL;
  }

  template <typename T>
  tTABLE_ALLOCATOR<T>::~tTABLE_ALLOCATOR()
  {
    for(INT i = 0 ; i < m_numAllocatedPages ; i++ ) {
      free(m_pPageTable[i]);
    }

    free(m_pPageTable);
  }

  template <typename T>
  T* tTABLE_ALLOCATOR<T>::at(INT i)
  {
    INT pageIndex = FirstLevelIndex(i);
    INT indexInPage = IndexInPage(i);

    if (pageIndex < m_numAllocatedPages)
      return &((m_pPageTable[pageIndex])[indexInPage]);
    else {
      ASSERT(pageIndex == m_numAllocatedPages);
      if(m_numAllocatedPages == m_numPossiblePages)
	init(2*m_numPossiblePages*m_numObjectsInPage);

      m_pPageTable[m_numAllocatedPages++] = (T*)malloc(sizeof(T)*m_numObjectsInPage); //m_pages.malloc();
      return &((m_pPageTable[pageIndex])[indexInPage]);
    }
  }

  template <typename T>
  void tTABLE_ALLOCATOR<T>::purge() {
    for(INT i = 0; i < m_numAllocatedPages ; i++) 
      free(m_pPageTable[i]);

	//m_pages.purge_memory();
    m_numAllocatedPages = 0;
    free(m_pPageTable);
    m_pPageTable = NULL;
  }

  template <typename T>
  tTABLE<T>::tTABLE(INT numObjectsInPage, INT logOfObjectsInPage,
		    INT numInitialPages, INT numInitialObjects) : m_allocator(numObjectsInPage, logOfObjectsInPage,
									      numInitialPages, numInitialObjects),
								  m_numObjects(0)
  {}
  
  template <typename T>
  INT tTABLE<T>::size() const
  {
    return m_numObjects;
  }

  template <typename T>
  BOOL tTABLE<T>::empty() const
  {
    return m_numObjects == 0;
  }

  template <typename T>
  T* tTABLE<T>::new_object()
  {
    T *newObj = m_allocator.at(m_numObjects++);
    ::new (newObj)T();
    return newObj;
  }

  template <typename T>
  void tTABLE<T>::push_back(const T& obj)
  {
    T *newObj = m_allocator.at(m_numObjects++);
    ::new (newObj)T(obj);
  }

  template <typename T>
  T* tTABLE<T>::object_at(INT index)
  {
    if(index < 0 || index >= m_numObjects)
      return NULL;

    return m_allocator.at(index);
  }

  template <typename T>
  const T& tTABLE<T>::operator[](INT index) const
  {
    const T& retObj = *object_at(index);
    return retObj;
  }

  template <typename T>
  T& tTABLE<T>::operator[](INT index)
  {
    T& retObj = *object_at(index);
    return retObj;
  }

  template <typename T>
  tTABLE<T>::~tTABLE()
  {
    if(m_numObjects > 0) {
      for(INT i = 0; i < m_numObjects; i++)
	object_at(i)->~T();
    }
  }

  template <typename T>
  void tTABLE<T>::clear()
  {
    for(INT i = 0; i < m_numObjects; i++)
      object_at(i)->~T();

    m_allocator.purge();
    m_numObjects = 0;
  }

  template <typename T>
  void tTABLE<T>::flush()
  {
    for(INT i = 0; i < m_numObjects; i++)
      object_at(i)->~T();
    
    m_numObjects = 0;
  }


  /* 
   * Methods which support sorting of table elements. 
   * The sorted order is returned in indices vector.
   * Table elements need to support < operator.
   */
  inline VOID SwapIndices(std::vector<INT> &indices, INT i, INT j)
  {
    INT tmp = indices[j];
    indices[j] = indices[i];
    indices[i] = tmp;
  }

  template <typename T>
  INT QuickSortPartition(tTABLE<T> &table, std::vector<INT> &indices,
			 INT p, INT r)
  {
    INT i = p;
    INT j = r+1;

    const T& pivot = table[indices[p]] ;
    do {
      do {
	i = i + 1;
      }while(i < r && table[indices[i]] < pivot);

      do {
	j = j-1;
      } while(j > p && pivot < table[indices[j]]);
      
      if(i < j)
	SwapIndices(indices, i, j);
    }while(i < j);

    SwapIndices(indices, p, j);
    return j;
  }


  template <typename T>
  VOID QuickSortIndices(tTABLE<T> &table, std::vector<INT> &indices, 
			INT p, INT r)
  {
    if(p < r) {
      INT q = QuickSortPartition<T>(table, indices, p, r);
      QuickSortIndices<T>(table, indices, p, q - 1);
      QuickSortIndices<T>(table, indices, q + 1, r);
    }
  }

  template <typename T>
  VOID SortedIndices(tTABLE<T> &table,
		     std::vector<INT> &indices)
  {
    /*
     *  some STL problem. reserve fails.
     */
    //    indices.reserve(table.size());
    
    for(INT i = 0 ; i < table.size() ; i++ )
      indices.push_back(i);
    
    std::random_shuffle(indices.begin(), indices.end());
    
    QuickSortIndices<T>(table, indices, 0, indices.size() - 1);
  
    //     for(INT i = 0; i < table.size() - 1; i++) {
    //       for( INT j = i+1; j <table.size(); j++) {
    //             if(table[indices[j]] < table[indices[i]]) {
    //           INT t = indices[j];
    //           indices[j] = indices[i];
    //           indices[i] = t;
    //         }
    //       }
    //     }
  }
}


#endif /*TABLE_HPP_*/
