/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// FileFragments code borrowed from Shareaza
// http://shareaza.sourceforge.net/

#ifndef FILEFRAGMENTS_HPP_INCLUDED
#define FILEFRAGMENTS_HPP_INCLUDED

#include "Ranges.hpp"

namespace Fragments
{


template< class RangeT, class ContainerT >
class ListTraits
{
public:
	typedef RangeT range_type;
	typedef ContainerT container_type;
	typedef typename range_type::size_type range_size_type;
	typedef typename range_type::payload_type payload_type;
	typedef Ranges::RangeCompare< payload_type, range_size_type > compare_type;
	typedef typename container_type::iterator iterator;
	typedef std::pair< iterator, iterator > iterator_pair;

public:
	range_size_type limit() const { return m_limit; }
	range_size_type length_sum() const { return m_length_sum; }
	range_size_type missing() const { return limit() - length_sum(); }
	void ensure(range_size_type limit)
	{
		m_limit = qMax( m_limit, limit );
	}

// the following functions have to be declared
protected:
	typedef range_size_type ctor_arg_type;
	explicit ListTraits(ctor_arg_type limit) : m_limit( limit ), m_length_sum( 0 ) { }
	void clear() { m_length_sum = 0; }
	void swap(ListTraits& other)
	{
		std::swap( m_limit, other.m_limit );
		std::swap( m_length_sum, other.m_length_sum );
	}
	range_size_type erase(const iterator where)
	{
		m_length_sum -= where->size();
		return where->size();
	}
	template< class container_type >
	range_size_type merge_and_replace(container_type& set,
		iterator_pair sequence, const range_type& new_range)
	{
		Q_ASSERT( sequence.first != sequence.second );
		if ( sequence.first->begin() <= new_range.begin()
			&& sequence.first->end() >= new_range.end() ) return 0;
		range_size_type old_sum = m_length_sum;
		range_size_type low = qMin( sequence.first->begin(), new_range.begin() );
		range_size_type high = qMax( ( --sequence.second )->end(), new_range.end() );
		for ( ++sequence.second; sequence.first != sequence.second; )
		{
			range_size_type length = sequence.first->size();
			set.erase( sequence.first++ );
			m_length_sum -= length;
		}
		set.insert( sequence.second, range_type( low, high ) );
		m_length_sum += high - low;
		return m_length_sum - old_sum;
	}
	template< class container_type >
	range_size_type simple_merge(container_type& set, iterator where, const range_type& new_range)
	{
		set.insert( where, new_range );
		m_length_sum += new_range.size();
		return new_range.size();
	}

private:
	range_size_type m_limit;
	range_size_type m_length_sum;
};

using Ranges::Exception;

typedef Ranges::Range< quint64 > Fragment;
typedef Ranges::RangeError< Fragment > FragmentError;
typedef Ranges::ListError< Fragment > ListError;
typedef Ranges::List< Fragment, ListTraits > List;
typedef Ranges::Queue< Fragment > Queue;

} // namespace Fragments

#include "Compatibility.hpp"

#endif // #ifndef FILEFRAGMENTS_HPP_INCLUDED
