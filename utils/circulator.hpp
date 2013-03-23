/*
 * circulator.hpp
 *
 *  Created on: Jun 6, 2009
 */

#ifndef CIRCULATOR_HPP_
#define CIRCULATOR_HPP_

namespace utils {
  const INT CirculatorBeginLabel = 0;
  const INT CirculatorEndLabel = -300;

  template <typename _HEAD_NODE_TYPE>
  class tCIRCULATOR : protected _HEAD_NODE_TYPE {
	  typedef _HEAD_NODE_TYPE cBASE;

  public:
	  typedef tCIRCULATOR<_HEAD_NODE_TYPE> cSELF;

	  typedef typename _HEAD_NODE_TYPE::value_type value_type;
	  typedef typename _HEAD_NODE_TYPE::pointer pointer;
	  typedef typename _HEAD_NODE_TYPE::reference reference;

  public:
    tCIRCULATOR() {}
    tCIRCULATOR(const _HEAD_NODE_TYPE &head, INT index) : cBASE(head),
		m_index(index)
		{}

    BOOL operator==(const cSELF &circulator) const {
    	/*
    	 * Condition to let the circulator move ahead for the first time.
    	 */
    	if((m_index ==  CirculatorBeginLabel && circulator.m_index == CirculatorEndLabel) ||
    	   (circulator.m_index==  CirculatorBeginLabel && m_index == CirculatorEndLabel))
    		return false;
    	else
    		return cBASE::operator==(circulator);
    }

    BOOL operator!=(const cSELF &circulator) const {
    	return !(*this == circulator);
    }

	cSELF& operator++() {
		m_index++;
		cBASE::operator++();
		return *this;
	}

	cSELF& operator++(int) {
		return ++*this;
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

  private:
    INT m_index;
  };

  template <typename _HEAD_NODE_TYPE>
  tCIRCULATOR<_HEAD_NODE_TYPE> CirculatorBegin(const _HEAD_NODE_TYPE &head)
  {
	return tCIRCULATOR<_HEAD_NODE_TYPE>(head, CirculatorBeginLabel);
  }

  template <typename _HEAD_NODE_TYPE>
   tCIRCULATOR<_HEAD_NODE_TYPE> CirculatorEnd(const _HEAD_NODE_TYPE &head)
   {
 	 return tCIRCULATOR<_HEAD_NODE_TYPE>(head, CirculatorEndLabel);
   }

}
#endif /* CIRCULATOR_HPP_ */
