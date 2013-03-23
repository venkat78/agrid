#ifndef TMESH_ITERATORS_HPP_
#define TMESH_ITERATORS_HPP_

namespace tmesh
{
  /*
   * Mesh iterators are just uni-directional iterators. only ++ operator is supported.
   */
  //skips deleted facets.
  template <typename _OBJECT_FACTORY_ITERATOR>
  class tOBJECTS_ITERATOR : protected _OBJECT_FACTORY_ITERATOR
  {
  private:
    typedef _OBJECT_FACTORY_ITERATOR cBASE;

  public:
    typedef tOBJECTS_ITERATOR<_OBJECT_FACTORY_ITERATOR> cSELF;

    typedef typename cBASE::value_type value_type;
    typedef typename cBASE::reference reference;
    typedef typename cBASE::pointer pointer;

  public:
    tOBJECTS_ITERATOR() : m_numElements(0)
    {
    }

    tOBJECTS_ITERATOR(const cBASE &iterator, INT numElements) :
      cBASE(iterator), m_numElements(numElements)
    {
    }


    BOOL operator==(const cSELF &obj) const
    {
      return cBASE::operator==(obj);
    }

    BOOL operator!=(const cSELF &obj) const
    {
      return !(*this == obj);
    }

    reference operator*()
    {
      return cBASE::operator*();
    }

    pointer operator->()
    {
      return cBASE::operator->();
    }

    const reference operator*() const
    {
      return cBASE::operator*();
    }

    const pointer operator->() const
    {
      return cBASE::operator->();
    }

    cSELF& operator++()
    {
      do
        {
          cBASE::m_index++;
          if (cBASE::m_index == m_numElements)
            return *this;
        }
      while ((*this)->IsDeleted());

      return *this;
    }

    cSELF&
    operator++(int)
    {
      return ++*this;
    }

  private:
    INT m_numElements;
  };
}

#endif /* MESH_ITERATORS_HPP_ */
