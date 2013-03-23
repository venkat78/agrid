#ifndef _UTILS_LIST_HPP_
#define _UTILS_LIST_HPP_

#include "defs.hpp"

namespace utils {
  //---------------------------------------------
  // Defining node of a single linked list.
  //---------------------------------------------
  template <typename T>
  struct tLIST_NODE 
  {
    typedef tLIST_NODE< T > Self;
  
  public:
    tLIST_NODE() {
      m_next = NULL;
    }
  
    tLIST_NODE(const T &item) : m_item(item) {
      m_next = NULL;
    }
  
    ~tLIST_NODE() {
      m_next = NULL;
    }
  
  public:
    T m_item;
    tLIST_NODE* m_next;
  };


  //-------------------------------------------
  // Iterator for the linked list.
  //-------------------------------------------
  template < typename T >
  struct tLIST_ITERATOR
  {
    typedef tLIST_ITERATOR< T > Self;
    typedef tLIST_NODE< T >* LIST_NODE;

    VOID operator++(int)
    {
      m_prev = m_curr;
      m_curr = m_curr->m_next;
    }
  
    BOOL operator==(const Self& itr) const
    {
      return m_curr == itr.m_curr;
    }

    BOOL operator != (const Self& itr) const
    {
      return !(*this == itr);
    }
    
    tLIST_ITERATOR(LIST_NODE itr)
    {
      m_curr = itr;
      m_prev = NULL;
    
    }

    tLIST_ITERATOR(LIST_NODE curr,
                   LIST_NODE prev)
    {
      m_curr = curr;
      m_prev = prev;
    }

    T& operator*() const
    {
      return m_curr->m_item;
    }

    T* operator->() const 
    { return &(m_curr->m_item); }
  
  public:
    tLIST_NODE< T > *m_curr;
    tLIST_NODE< T > *m_prev;
  };

  //------------------------------------------------------
  // Definition of single linked list to hold pointers.
  // Very light weight and tries to mimic a tiny subset 
  // of std::list functionality
  //------------------------------------------------------
  template <typename T>
  struct tLIST
  {
    typedef tLIST_NODE<T > sLIST_NODE;
    typedef tLIST <T> Self;
    typedef tLIST_ITERATOR<T> iterator;
    typedef INT size_type;

  public:
    tLIST()
    {
      m_head = m_tail = NULL;
      m_numEntries = 0;
    }

    tLIST( const tLIST &srcList ) 
    {
      m_head = m_tail = NULL;
      m_numEntries = 0;

      iterator currEntry = srcList.begin();
      iterator lastEntry = srcList.end();

      for( ; currEntry != lastEntry ; currEntry++)
        push_back(*currEntry);
    }

    ~tLIST()
    {
      if(m_head)
        clear();

      m_numEntries = 0;
      m_head = m_tail = NULL;
    }

    VOID clear();
    BOOL empty() { return m_head == NULL; }
    VOID push_front(const T& item);
    VOID push_back(const T &item);
    iterator begin() const { return iterator(m_head);  }
    iterator end() const { return iterator(NULL); }
    iterator erase(const iterator &curr);
    VOID remove(const T& item);
    size_type size() const { return m_numEntries; }

    VOID pop_front() {
      erase(begin());
    }

    const T& front() const {
      return begin().operator*();
    }

    const T& back() const {
      return m_tail->m_item;
    }
  public:

  public:
    INT m_numEntries;
    sLIST_NODE *m_head;
    sLIST_NODE *m_tail;
  };

  template <typename T>
  typename tLIST<T>::iterator
  tLIST<T>::erase(const iterator &curr)
  {
    if(m_head == m_tail && curr.m_curr == m_head) {
      sLIST_NODE *tmp = m_head;
      m_head = m_tail = NULL;

      delete tmp;
      m_numEntries = 0;
      return end();
    }
  
    if(m_head == curr.m_curr) {
      sLIST_NODE *tmp = m_head;
      m_head = m_head->m_next;
      delete tmp;
    
      m_numEntries--;
      return begin();
    }
  
    if(curr.m_curr == m_tail) {
      sLIST_NODE *tmp = m_tail;
      m_tail = curr.m_prev;
      curr.m_prev->m_next = NULL;

      delete tmp;
      m_numEntries--;
      return end();
    }
  
    sLIST_NODE *tmp = curr.m_curr;
    curr.m_prev->m_next = curr.m_curr->m_next;

    //Constructing a new valid iterator.
    iterator nextItr(curr.m_curr->m_next, curr.m_prev);

    delete tmp;
    m_numEntries--;

    return nextItr;
  
  }

  template <typename T>
  VOID tLIST<T>::clear()
  {
    sLIST_NODE* curr = m_head;
    while(curr != NULL) {
      sLIST_NODE* to_erase = curr;
      curr = curr->m_next;
      delete to_erase;
    }
  
    m_head = m_tail = NULL;
    m_numEntries = 0;
  }

  template <typename T>
  VOID tLIST<T>::push_front(const T& item)
  {
    sLIST_NODE* new_node = new sLIST_NODE(item);
    new_node->m_next = m_head;
    m_head = new_node;
  
    if(m_head->m_next == NULL)
      m_tail = m_head;
  
    m_numEntries++;
  }

  template <typename T>
  VOID tLIST<T>::push_back(const T &item)
  {
    sLIST_NODE* new_node = new sLIST_NODE(item);
    if(m_tail) {
      m_tail->m_next = new_node;
      m_tail = new_node;
    }
    else {
      m_head = m_tail = new_node;
    }
  
    m_numEntries++;
  }

  template <typename T>
  VOID tLIST<T>::remove(const T &item)
  {
    iterator currEntry = begin();
    iterator lastEntry = end();
    for( ; currEntry != lastEntry ; currEntry++) {
      if(*currEntry == item) {
        erase(currEntry);
        break;
      }
    }      
  }
}

#endif //single linked list.
