cmake_minimum_required(VERSION 3.0)
project ( server )

add_executable(server 
		LMserver.cpp
		LMserver.h
		lock.cpp
		Lock.h
		main.cpp)
target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT})

