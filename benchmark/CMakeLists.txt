#            Copyright (c) Christian Holmquist 2011.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include_directories(${TABLES_SOURCE_DIR})

set(BENCHMARKS
	unordered.cpp
)

foreach( test ${BENCHMARKS} )
	string( REPLACE "/" "_" test_exe ${test} )
	string( REPLACE ".cpp" "" test_exe ${test_exe} )
	add_executable(${test_exe} ${test})
	target_link_libraries(${test_exe} ${Boost_LIBRARIES})
endforeach( test )
