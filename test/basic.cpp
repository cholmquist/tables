//#include "pch.h"
#include <tables/table.hpp>
#include <tables/ordered.hpp>
#include <tables/unordered.hpp>
#include <tables/sequenced.hpp>
#include <boost/detail/lightweight_test.hpp>

struct obj
{
	tables::ordered<int> m_id;
	tables::sequenced m_seq;
	tables::unordered<int> m_unordered;

	obj() {}
	obj(int id) : m_id(id) {}

	bool operator==(const obj& rhs) const
	{
		return this == &rhs;
	}
};

typedef tables::table<obj,
tables::ordered<int>, &obj::m_id,
tables::sequenced, &obj::m_seq
> table_t;

void test_basic()
{
	table_t table;
	obj o;

	BOOST_TEST(table.size(&obj::m_id) == 0);
	BOOST_TEST(table.empty(&obj::m_id));

	o.m_id = 3;
	table.insert(o, &obj::m_id);
	BOOST_TEST(o.m_id.is_linked() == true);
	BOOST_TEST(table.size(&obj::m_id) == 1);

	obj o2(o), o3;
	o3 = o;
	BOOST_TEST(o2.m_id.is_linked() == false);
	BOOST_TEST(o3.m_id.is_linked() == false);

	obj& result = table.erase_key(&obj::m_id, 3).get();
	BOOST_TEST(o.m_id.is_linked() == false);

	BOOST_TEST(&result == &o);

}

void test_erase()
{
	obj o1(1);
	table_t table;
	table.insert(o1, &obj::m_id);
	table.push_back(o1, &obj::m_seq);
	table.erase(o1, &obj::m_id);
	table.erase(o1, &obj::m_seq);
	BOOST_TEST(table.empty(&obj::m_id));
	BOOST_TEST(table.empty(&obj::m_seq));
	BOOST_TEST(o1.m_id.is_linked() == false);
	BOOST_TEST(o1.m_seq.is_linked() == false);
}

void test_iteration()
{
	table_t table;

	{
		BOOST_TEST(!table.first(&obj::m_id));
		BOOST_TEST(!table.last(&obj::m_id));
	}


	obj o1, o2;
	o2.m_id = 2;
	o1.m_id = 1;
	table.insert(o2, &obj::m_id);
	table.insert(o1, &obj::m_id);

	{
		table_t::optional first = table.first(&obj::m_id);
		BOOST_TEST(first.get() == o1);
		table_t::optional last = table.last(&obj::m_id);
		BOOST_TEST(last.get() == o2);
	}
	{
		table_t::optional next = table.next(table.first(&obj::m_id).get(), &obj::m_id);
		BOOST_TEST(next.get() == o2);
		table_t::optional prev = table.prev(*next, &obj::m_id);
		BOOST_TEST(*prev == o1);
		BOOST_TEST(!table.prev(*table.first(&obj::m_id), &obj::m_id));
		BOOST_TEST(!table.next(*table.last(&obj::m_id), &obj::m_id));
	}


	{
		obj o3;
		o3.m_id = 3;
		table.insert(o3, &obj::m_id);
		int values[3] = {1, 2, 3};
		int index = 0;
		for(table_t::optional itr = table.first(&obj::m_id); itr; itr = table.next(*itr, &obj::m_id))
		{
			BOOST_TEST(itr->m_id.value() == values[index]);
			index++;
		}
		table.erase_key(&obj::m_id, 3);
	}

	table.clear(&obj::m_id);
}

void test_find()
{
	table_t table;
	obj o1;
	o1.m_id = 1;
	table.insert(o1, &obj::m_id);
	BOOST_TEST(*table.find(&obj::m_id, 1) == o1);
	BOOST_TEST(table.cfind(&obj::m_id, 1)->m_id.value() == 1);
	table.clear(&obj::m_id);
}

void test_list()
{
	table_t table;
	obj o1;
	table.push_back(o1, &obj::m_seq);
	table.clear(&obj::m_seq);
};

void test_unordered()
{
	tables::table<obj, tables::unordered<int>, &obj::m_unordered> table;
	obj o1;
	table.insert(o1, &obj::m_unordered);
	table.rehash(&obj::m_unordered);
	table.clear(&obj::m_unordered);
}

int main(int argc, char* argv[])
{
	test_basic();
	test_iteration();
	test_find();
	test_erase();
	test_list();
	test_unordered();
	int a = sizeof(obj);
	int b = sizeof(boost::optional<obj&>);
	int xx = 0;
	return boost::report_errors();
}
