#ifndef TABLES_UNUNORDERED_HPP_INCLUDED
#define TABLES_UNUNORDERED_HPP_INCLUDED

#include <boost/intrusive/unordered_set.hpp>
#include <vector>

namespace tables
{
	namespace intrusive = boost::intrusive;

	namespace detail
	{
		typedef intrusive::make_unordered_set_base_hook<
			intrusive::link_mode<intrusive::safe_link> >::type unordered_base_hook;

		template<class Base>
		struct value_predicate : Base
		{
			template<class I>
			bool operator()(const I& x, const I& y) const
			{
				return Base::operator()(x.value(), y.value());
			}
		};

		template<class Base>
		struct value_func : Base
		{
			template<class I>
			typename Base::result_type operator()(const I& x) const
			{
				return Base::operator()(x.value());
			}
		};

	}

	template<class T, class Hash = boost::hash<T>, class Equal = std::equal_to<T> >
	struct unordered : detail::unordered_base_hook
	{
		struct sequence_type
		{
			struct bucket_traits
			{
				typedef typename intrusive::unordered_bucket<
					intrusive::base_hook<detail::unordered_base_hook> >::type bucket_type;
				typedef typename intrusive::unordered_bucket_ptr<
					intrusive::base_hook<detail::unordered_base_hook> >::type bucket_ptr_type;

				bucket_traits(std::vector<bucket_type>* b)
					: buckets_(b)
				{}

				bucket_ptr_type bucket_begin() const
				{
					return &((*buckets_)[0]);
				}
				std::size_t bucket_count() const { return buckets_->size(); }

				std::vector<bucket_type>* buckets_;
			};

			typedef typename intrusive::make_unordered_set<unordered,
				intrusive::constant_time_size<true>,
				intrusive::cache_begin<true>,
				intrusive::bucket_traits<bucket_traits>,
				intrusive::hash<detail::value_func<Hash> >,
				intrusive::equal<detail::value_predicate<Equal> > >::type intrusive_set;

			sequence_type()
				: buckets_(100)
				, sequence_(bucket_traits(&buckets_))
			{

			}

			std::pair<typename intrusive_set::iterator, bool> insert(unordered& x)
			{
				return sequence_.insert(x);
			}

			void clear()
			{
				sequence_.clear();
			}

			void rehash()
			{
				if(sequence_.size() > buckets_.size())
				{
					bucket_vector new_buckets(sequence_.size());
					sequence_.rehash(bucket_traits(&new_buckets));
					buckets_.swap(new_buckets);
				}
				else
				{
					sequence_.rehash(bucket_traits(&buckets_));
				}
			}

		private:
			typedef std::vector<typename bucket_traits::bucket_type> bucket_vector;
			bucket_vector buckets_;
			intrusive_set sequence_;

		};
		typedef T value_type;

		explicit unordered(const T& x) : m_value(x) {}
		unordered(const unordered& x) : m_value(x.m_value) {}
		unordered() : m_value() {}

		unordered& operator=(const T& x)
		{
			BOOST_ASSERT(!this->is_linked());
			m_value = x;
			return *this;
		}

		unordered& operator=(const unordered& x)
		{
			BOOST_ASSERT(!this->is_linked());
			m_value = x.m_value;
			return *this;
		}

		const value_type& value() const
		{
			return m_value;
		}

	private:
		value_type m_value;

	};

}

#endif
