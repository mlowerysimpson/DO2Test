cmake_minimum_required(VERSION 2.8)
project(DO2Test)
# Locate libraries and headers
find_library(wiringPi_LIB wiringPi)
find_package(Threads REQUIRED)

# Include headers
include_directories(${WIRINGPI_INCLUDE_DIRS}) 
include_directories("../RemoteControlTest")

#the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "./*.cpp")
add_executable(DO2Test ${SOURCES} "../RemoteControlTest/AtlasDO2Sensor.cpp" "../RemoteControlTest/Sensor.cpp" "../RemoteControlTest/Util.cpp")

# Link against libraries
target_link_libraries(DO2Test ${wiringPi_LIB} )
target_link_libraries(DO2Test ${CMAKE_THREAD_LIBS_INIT})



