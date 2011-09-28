#include <tables/table.hpp>
#include <tables/unordered.hpp>
#include <string>

template<int Size>
struct data
{
	tables::unordered<std::string> str;
	char payload[Size];
};

typedef data<16> data_16;
typedef tables::table<data_16, tables::unordered<std::string>, &data_16::str> table_16;

void test_1(int count)
{
	std::vector<data_16> v;
	table_16 t;
	v.reserve(count);
	std::string str;
	for(int i = 0; i < count; ++i)
	{
		str.clear();
		std::generate_n(std::back_inserter(str), 16, &rand);
		data_16 elem;
		elem.str = str;
		v.push_back(elem);

	}
	for(int i = 0; i < count; ++i)
	{
		t.insert(v[i], &data_16::str);
	}

	t.clear(&data_16::str);
}

int main()
{
	srand(42);
	test_1(1000);
	return 0;
}
