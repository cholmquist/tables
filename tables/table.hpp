#ifndef TABLES_TABLE_HPP_INCLUDED
#define TABLES_TABLE_HPP_INCLUDED

#include <boost/optional/optional.hpp>
#include <boost/intrusive/detail/parent_from_member.hpp>

namespace tables
{
	namespace detail
	{
		template<class T>
		struct operations
		{
			typedef boost::optional<T&> optional;
			typedef boost::optional<T const &> coptional;

			template<bool IsConst>
			struct value_result;

			template<>
			struct value_result<false>
			{
				value_result() : result_() {}

				optional result() const
				{
					return result_ ? optional(*result_) : optional();
				}

				T* result_;
			};

			template<>
			struct value_result<true>
			{
				value_result() : result_() {}

				coptional result() const
				{
					return result_ ? coptional(*result_) : coptional();
				}

				T const * result_;
			};

			struct bool_result
			{
				bool_result() : result_() {}

				bool result() const
				{
					return result_;
				}

				bool result_;
			};

			struct push_back
			{
				push_back(T& obj)
					: obj_(obj)
				{}

				template<class Index, class Sequence>
				void operator()(Index T::*index, Sequence& seq)
				{
					seq.push_back(obj_.*index);
				}

				T& obj_;
			};

			template<class Key>
			struct insert : bool_result
			{
				insert(T& obj)
					: obj_(obj)
				{}

				template<class Index, class Sequence>
				void operator()(Index T::*index, Sequence& seq)
				{
					result_ = seq.insert(obj_.*index).second;

					/*				typename Index::sequence_type::insert_commit_data insert_data;
					const Key& key_ = (obj_.*index);
					if(seq.insert_check(key_, typename Index::compare(), insert_data).second)
					{
					//					(obj_.*index).construct(key_);
					seq.insert_commit((obj_.*index), insert_data);
					result_ = true;
					}*/
				}

				T& obj_;
			};

			struct rehash
			{
				template<class Index, class Sequence>
				void operator()(Index T::*index, Sequence& seq)
				{
					seq.rehash();
				}
			};

			template<class Key, bool IsConst>
			struct find : value_result<IsConst>
			{
				find(const Key& key) : key_(key) {}

				template<class Index, class Sequence>
				void operator()(Index T::*index, Sequence& seq)
				{
					typename Sequence::iterator i = seq.find(key_, Index::compare());
					if(i != seq.end())
					{
						result_ = boost::intrusive::detail::parent_from_member<T>(&(*i), index);
					}
				}

				const Key& key_;
			};

			struct clear
			{
				template<class Index, class Sequence>
				void operator()(Index T::*, Sequence& seq)
				{
					seq.clear();
				}
			};

			struct empty : bool_result
			{
				template<class Index, class Sequence>
				void operator()(Index T::*, Sequence& seq)
				{
					result_ = seq.empty();
				}
			};

			struct size
			{
				template<class Index, class Sequence>
				void operator()(Index T::*, Sequence& seq)
				{
					size_ = seq.size();
				}

				std::size_t result()
				{
					return size_;
				}

				std::size_t size_;
			};

			struct erase
			{
				erase(T& obj)
					: obj_(obj)
				{
				}
				template<class Index, class Sequence>
				void operator()(Index T::*index, Sequence& seq)
				{
					seq.erase(Sequence::s_iterator_to(obj_.*index));
				}

				T& obj_;
			};

			template<class Key>
			struct erase_key
			{
				erase_key(const Key& key)
					: key_(key)
					, result_()
				{
				}

				template<class Index, class Sequence>
				void operator()(Index T::*index, Sequence& seq)
				{
					typename Sequence::iterator itr = seq.find(key_, Index::compare());
					if(itr != seq.end())
					{
						Index& i = *itr;
						result_ = boost::intrusive::detail::parent_from_member<T>(&i, index);
						seq.erase(itr);
					}
				}

				optional result()
				{
					return result_ ? optional(*result_) : optional();
				}

				const Key& key_;
				T* result_;
			};

			struct first
			{
				first()	: result_()
				{}

				template<class Index, class Sequence>
				void operator()(Index T::*index, Sequence& sequence)
				{
					typename Sequence::iterator i = sequence.begin();
					if(i != sequence.end())
					{
						result_ = boost::intrusive::detail::parent_from_member<T>(&(*i), index);
					}
				}

				optional result() const
				{
					return result_ ? optional(*result_) : optional();
				}

				T* result_;
			};

			struct last
			{
				last() : result_()
				{}

				template<class Index, class Sequence>
				void operator()(Index T::*index, Sequence& sequence)
				{
					typename Sequence::iterator i = sequence.end();
					if(i != sequence.begin())
					{
						--i;
						result_ = boost::intrusive::detail::parent_from_member<T>(&(*i), index);
					}
				}

				optional result() const
				{
					return result_ ? optional(*result_) : optional();
				}

				T* result_;
			};

			struct next
			{
				next(T& pos)
					: pos_(pos)
					, result_() {}

				template<class Index, class Sequence>
				void operator()(Index T::*index, Sequence& sequence)
				{
					typename Sequence::iterator i = sequence.iterator_to(pos_.*index);
					++i;
					if(i != sequence.end())
					{
						result_ = boost::intrusive::detail::parent_from_member<T>(&(*i), index);
					}
				}

				optional result() const
				{
					return result_ ? optional(*result_) : optional();
				}

				T& pos_;
				T* result_;
			};

			struct prev
			{
				prev(T& pos)
					: pos_(pos)
					, result_() {}

				template<class Index, class Sequence>
				void operator()(Index T::*index, Sequence& sequence)
				{
					typename Sequence::iterator i = sequence.iterator_to(pos_.*index);
					if(i != sequence.begin())
					{
						--i;
						result_ = boost::intrusive::detail::parent_from_member<T>(&(*i), index);
					}
				}

				optional result() const
				{
					return result_ ? optional(*result_) : optional();
				}

				T& pos_;
				T* result_;
			};

		};
	}

	template<class T,
	class A0, A0 T::*a0,
	class A1 = A0, A1 T::*a1 = a0,
	class A2 = A0, A2 T::*a2 = a0
	>
	class table
	{
		struct sentinel_index
		{
			template<class Index, class Function>
			void visit(Index T::*, Function&)
			{
				BOOST_ASSERT(false);
			}
			template<class Index, class Function>
			void visit(Index T::*, Function&) const
			{
				BOOST_ASSERT(false);
			}
		};

		template<class Index, Index T::*index_ptr, class Next = sentinel_index, bool IsHead = false>
		struct index : Next
		{
			typedef typename Index::sequence_type sequence_type;
			sequence_type sequence;
			Next next;

			static const bool is_head = IsHead;

			template<class Function>
			void visit(Index T::*i, Function& f)
			{
				if(i == index_ptr)
				{
					f(i, sequence);
				}
				else
				{
					next.visit(i, f);
				}
			}

			template<class Function>
			void visit(Index T::*i, Function& f) const
			{
				if(i == index_ptr)
				{
					f(i, sequence);
				}
				else
				{
					next.visit(i, f);
				}
			}

			template<class Unrelated, class Function>
			void visit(Unrelated T::*i, Function& f)
			{
				next.visit(i, f);
			}

			template<class Unrelated, class Function>
			void visit(Unrelated T::*i, Function& f) const
			{
				next.visit(i, f);
			}
		};

		typedef detail::operations<T> operations;


	public:
		table() {}

		typedef boost::optional<T&> optional;
		typedef boost::optional<T const&> coptional;

		template<class Index>
		optional erase_key(Index T::*index, const typename Index::value_type& key)
		{
			operations::erase_key<typename Index::value_type> op(key);
			m_index.visit(index, op);
			return op.result();
		}

		template<class Index>
		void erase(T& t, Index T::*index)
		{
			operations::erase op(t);
			m_index.visit(index, op);
		}

		template<class Index>
		optional find(Index T::*index, const typename Index::value_type& key)
		{
			operations::find<typename Index::value_type, false> op(key);
			m_index.visit(index, op);
			return op.result();
		}

		template<class Index>
		coptional cfind(Index T::*index, const typename Index::value_type& key)
		{
			operations::find<typename Index::value_type, true> op(key);
			m_index.visit(index, op);
			return op.result();
		}

		template<class Index>
		bool insert(T& t, Index T::*index)
		{
			operations::insert<typename Index::value_type> op(t);
			m_index.visit(index, op);
			return op.result();
		}

		template<class Index>
		void push_back(T& t, Index T::*index)
		{
			operations::push_back op(t);
			m_index.visit(index, op);
		}

		template<class Index>
		void clear(Index T::*index)
		{
			operations::clear op;
			m_index.visit(index, op);
		}

		template<class Index>
		std::size_t size(Index T::*index) const
		{
			operations::size op;
			m_index.visit(index, op);
			return op.result();
		}

		template<class Index>
		bool empty(Index T::*index) const
		{
			operations::empty op;
			m_index.visit(index, op);
			return op.result();
		}

		template<class Index>
		optional first(Index T::*index)
		{
			operations::first op;
			m_index.visit(index, op);
			return op.result();
		}

		template<class Index>
		optional last(Index T::*index)
		{
			operations::last op;
			m_index.visit(index, op);
			return op.result();
		}

		template<class Index>
		optional prev(T& t, Index T::*index)
		{
			operations::prev op(t);
			m_index.visit(index, op);
			return op.result();
		}

		template<class Index>
		optional next(T& t, Index T::*index)
		{
			operations::next op(t);
			m_index.visit(index, op);
			return op.result();
		}

		template<class Index>
		void rehash(Index T::*index)
		{
			operations::rehash op;
			m_index.visit(index, op);
		}

	private:
		typedef index<A0, a0, index<A1, a1>, true> unreduced_index;
		unreduced_index m_index;

	};
}

#endif
