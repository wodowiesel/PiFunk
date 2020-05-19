project(pifunk)

add_definitions(-D_USE_MATH_DEFINES)
add_definitions(-D_POSIX_C_SOURCE=200809L)
add_definitions(-D_GNU_SOURCE)

find_package(sox REQUIRED)

include_directories(
	${CMAKE_SOURCE_DIR}/PiFunk
	${CMAKE_SOURCE_DIR}/PiFunk/include
  )

set(pifunk_HEADERS
  	include/pifunk.h
  	include/sndfile.h
  	)

set(pifunk_SOURCES
  	PiFunk/pifunk.c
  	)

add_library(pifunk STATIC
  	${pifunk_HEADERS}
  	${pifunk_SOURCES}
  	)

target_include_directories(pifunk
	PUBLIC .
	)

target_link_libraries(pifunk
  	${pifunk_LIBRARIES}
  	)

add_executable(bin/pifunk
							 bin/pifunk.out
							 bin/pifunk.bin
							 )
