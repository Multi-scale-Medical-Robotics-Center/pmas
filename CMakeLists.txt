#set(CMAKE_C_COMPILER "/home/patrick/projects/ti/am5728/sdk/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-none-linux-gnueabihf-gcc")
#set(CMAKE_CXX_COMPILER "/home/patrick/projects/ti/am5728/sdk/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-none-linux-gnueabihf-g++")

#set(CMAKE_C_COMPILER "/home/patrick/projects/linaro/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc")
#set(CMAKE_CXX_COMPILER "/home/patrick/projects/linaro/gcc-linaro-7.2.1-2017.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++")

set(CMAKE_C_COMPILER "/home/patrick/projects/linaro/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "/home/patrick/projects/linaro/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/bin/arm-linux-gnueabihf-g++")

#set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
#set(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")

cmake_minimum_required(VERSION 3.22)

project(pmas)
set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -D_PLATINUM -O0 -Wall  -fmessage-length=0 -mfpu=vfpv4 -mfloat-abi=hard -mtune=cortex-a15 -std=c++11")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -D_PLATINUM -O0 -Wall  -fmessage-length=0 -mfpu=vfpv4 -mfloat-abi=hard -mtune=cortex-a15 -std=c++11")


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard -D_PLATINUM -O0 -Wall -fmessage-length=0 -mfpu=vfpv4 -mfloat-abi=hard -mtune=cortex-a15 -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard -D_PLATINUM -O0 -Wall -fmessage-length=0 -mfpu=vfpv4 -mfloat-abi=hard -mtune=cortex-a15 -std=c++11")

set(SOURCE_FILES
	src/HandheldDrill.cpp
	src/UDP.cpp
)

set(INCLUDE_DIRECTORY
	include
	maestro/includes
	maestro/includes/cpp
)

add_executable(HandheldDrill.pexe ${SOURCE_FILES})

target_include_directories(HandheldDrill.pexe PRIVATE ${INCLUDE_DIRECTORY})

target_link_directories(HandheldDrill.pexe PRIVATE maestro/lib/platinum)

target_link_libraries(HandheldDrill.pexe MMCPPLIB MMC_APP_LIB pthread rt)

add_executable(HandheldBurr-only.pexe ${SOURCE_FILES})

target_include_directories(HandheldBurr-only.pexe PRIVATE ${INCLUDE_DIRECTORY})

target_link_directories(HandheldBurr-only.pexe PRIVATE maestro/lib/platinum)

target_link_libraries(HandheldBurr-only.pexe MMCPPLIB MMC_APP_LIB pthread rt)

