#ifndef TABLES_ORDERED_HPP_INCLUDED
#define TABLES_ORDERED_HPP_INCLUDED

#include <boost/intrusive/set.hpp>

namespace tables
{
	namespace intrusive = boost::intrusive;

	template<class T, class Compare = std::less<T> >
	struct ordered : intrusive::make_set_base_hook<intrusive::link_mode</*intrusive::auto_unlink*/ intrusive::safe_link>,
		intrusive::optimize_size<true> >::type,
		Compare
	{
		typedef typename intrusive::make_set<ordered,
			intrusive::constant_time_size<true> >::type sequence_type;
		typedef T value_type;

		ordered(const ordered& x) : m_value(x.m_value) {}
		ordered() : m_value() {}
		explicit ordered(const T& x) : m_value(x) {}

		ordered& operator=(const T& x)
		{
			BOOST_ASSERT(!this->is_linked());
			m_value = x;
			return *this;
		}

		ordered& operator=(const ordered& x)
		{
			BOOST_ASSERT(!this->is_linked());
			m_value = x.m_value;
			return *this;
		}

		const value_type& value() const
		{
			return m_value;
		}

		bool operator<(const ordered& rhs) const
		{
			return Compare()(m_value, rhs.m_value);
		}

		struct compare : Compare
		{
			bool operator()(const ordered& lhs, const value_type& rhs) const
			{
				return Compare::operator()(lhs.m_value, rhs);
			}
			bool operator()(const value_type& lhs, const ordered& rhs) const
			{
				return Compare::operator()(lhs, rhs.m_value);
			}
		};

	private:
		value_type m_value;

	};

}

#endif
