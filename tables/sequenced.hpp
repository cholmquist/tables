#ifndef TABLES_SEQUENCED_HPP_INCLUDED
#define TABLES_SEQUENCED_HPP_INCLUDED

#include <boost/intrusive/list.hpp>

namespace tables
{
	namespace intrusive = boost::intrusive;

	struct sequenced : intrusive::make_list_base_hook<
	intrusive::link_mode<intrusive::safe_link> >::type
	{
		typedef intrusive::make_list<sequenced, intrusive::constant_time_size<true> >::type sequence_type;

		sequenced() {}
		sequenced(const sequenced&) {}

		sequenced operator=(const sequenced& x)
		{
			BOOST_ASSERT(!this->is_linked());
			return *this;
		}

	};

}

#endif
