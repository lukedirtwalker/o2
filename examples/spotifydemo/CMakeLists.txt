cmake_minimum_required(VERSION 2.8.11)

project( spotifyexample )

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if(o2_WITH_QT5)
    # Qt5 packages find their own dependencies.
    find_package(Qt5Core    REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Script  REQUIRED)
    find_package(Qt5Network REQUIRED)
else(o2_WITH_QT5)
    message( FATAL_ERROR "No Qt4 support for this example" )
endif(o2_WITH_QT5)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} "../../src" )

set(sp_SRCS
    main.cpp
    spotifydemo.cpp
)

add_executable( spotifyexample ${sp_SRCS} )

if(o2_WITH_QT5)
    qt5_use_modules( spotifyexample Core Widgets Network )
    target_link_libraries( spotifyexample o2 )
endif(o2_WITH_QT5)
