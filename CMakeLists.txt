project(pifunklite)

add_definitions(-D_USE_MATH_DEFINES)
add_definitions(-D_POSIX_C_SOURCE=200809L)
add_definitions(-D_GNU_SOURCE)

find_package(sox REQUIRED)

include_directories(
	${CMAKE_SOURCE_DIR}/PiFunklite
	${CMAKE_SOURCE_DIR}/PiFunklite/include
  )

set(pifunk_HEADERS
  	include/pifunk.h
  	include/sndfile.h
  	)

set(pifunk_SOURCES
  	PiFunk/pifunklite.c
  	)

add_library(pifunklite STATIC
  	${pifunklite_HEADERS}
  	${pifunklite_SOURCES}
  	)

target_include_directories(pifunklite
	PUBLIC .
	)

target_link_libraries(pifunklite
  	${pifunklite_LIBRARIES}
  	)

add_executable(bin/pifunklite
							 bin/pifunklite.out
							 bin/pifunklite.bin
							 )
