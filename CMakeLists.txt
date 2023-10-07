cmake_minimum_required(VERSION 3.14)

SET(TIKTOKEN_VER_MAJOR 0) 
SET(TIKTOKEN_VER_MINOR 3) 
project(llm_cpp VERSION ${TIKTOKEN_VER_MAJOR}.${TIKTOKEN_VER_MINOR}.0)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake) 

include(utils)
include(vars)
 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

# 指定版本号的配置文件


option(BUILD_TIKTOKEN_STATIC "build static tiktoken library" ON)
option(BUILD_TIKTOKEN_SHARED "build shared tiktoken library" OFF)
option(BUILD_TOKEN_TEST "build tiktoken test program" ON)
option(INSTALL_ENCODING_FILES "copy encoding files to install destination" ON)

set(LIBTIKTOKEN_SRCDIR ./tiktoken/src)
set(LIBTIKTOKEN_HEADERDIR ./tiktoken/include)
set(TOKEN_TEST_SRCDIR ./token_test)
set(ENCODING_FILE_DIR ./encoding_files)
set(COMMON_DIR ./common)
SET(THIRD_PARTY_PATH ${PROJECT_SOURCE_DIR}/3rdparty) 
SET(CPPCODEC_PATH ${THIRD_PARTY_PATH}/cppcodec-0.2) 
   
#add_compile_options(-std=c++20 -fmodules)
add_compile_options(-std=c++20)
add_compile_options(-O3 -g -W -Wall -Wno-unused-parameter)
#ADD_DEFINITIONS(-O3 -g -W -Wall)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)

check_cxx_source_compiles(
"#include <format>
#include <string>
int main(){
    std::string rtn = std::format(\"right {}\", 42);
    return 0;
}"
CPP20_FORMAT_FIND
)

if(NOT CPP20_FORMAT_FIND)
    message(STATUS "C++ 20 format not support!")    
    ADD_DEFINITIONS(-DNO_CPP20_FORMAT)
endif()

#find_package(Boost COMPONENTS regex system REQUIRED)
find_package(Boost)
if(BOOST_FOUND)
    #message(STATUS "BOOST_INCLUDE_DIRS=${BOOST_INCLUDE_DIRS}")
    #message(STATUS "Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
    find_file(JoinH boost/algorithm/string/join.hpp PATHS ${Boost_INCLUDE_DIRS} NO_DEFAULT_PATH)
    if(NOT JoinH)
        message(FATAL_ERROR "Can't find boost/algorithm/string/join.hpp in ${Boost_INCLUDE_DIRS}")
    endif()
    
    find_file(ReplaceH boost/algorithm/string/replace.hpp PATHS ${Boost_INCLUDE_DIRS} NO_DEFAULT_PATH)
    if(NOT ReplaceH)
        message(FATAL_ERROR "Can't find boost/algorithm/string/replace.hpp in ${Boost_INCLUDE_DIRS}")
    endif()
    
    find_file(StringH boost/algorithm/string.hpp PATHS ${Boost_INCLUDE_DIRS} NO_DEFAULT_PATH)
    if(NOT StringH)
        message(FATAL_ERROR "Can't find boost/algorithm/string/string.hpp in ${Boost_INCLUDE_DIRS}")
    endif()   
    
    if(NOT CPP20_FORMAT_FIND)
        find_file(FormatH boost/format.hpp PATHS ${Boost_INCLUDE_DIRS} NO_DEFAULT_PATH)
        if(NOT FormatH)
            message(FATAL_ERROR "Can't find boost/format.hpp in ${Boost_INCLUDE_DIRS}")
        endif() 
    endif()
        
else(BOOST_FOUND)
    message(FATAL_ERROR "Boost library not found")
endif()

#Boost_FOUND, Boost_INCLUDE_DIRS, Boost_LIBRARY_DIRS, Boost_LIBRARIES, Boost_VERSION, Boost_LIB_VERSION

# external packages
find_package(CURL REQUIRED)
if(CURL_FOUND)
    message("CURL_INCLUDE_DIR ${CURL_INCLUDE_DIR}")
    set(EXTRA_LIBS CURL::libcurl)
    if(WIN32)
        set(EXTRA_LIBS ${EXTRA_LIBS} ws2_32.lib wldap32.lib advapi32.lib crypt32.lib normaliz.lib winmm.lib)
    endif()
else(CURL_FOUND)
    message(FATAL_ERROR "CURL library not found")
endif()


find_package(Iconv REQUIRED)
if(ICONV_FOUND)
    message("Iconv ${ICONV_LIBRARY}")
    set(EXTRA_LIBS ${EXTRA_LIBS} Iconv::Iconv)
else(ICONV_FOUND)
    message(FATAL_ERROR "Iconv library not found")
endif()

#unset(PCRE2_LIBRARY CACHE)
#set(PCRE2_USE_STATIC_LIBS ON)
#find_package(PCRE2 CONFIG COMPONENTS 8BIT)
FIND_PACKAGE(PCRE2 COMPONENTS 8BIT)
if(PCRE2_FOUND)
    message("PCRE2 version: ${PCRE2_VERSION}")
    #ADD_DEFINITIONS(-DPCRE2_STATIC -DPCRE2_CODE_UNIT_WIDTH=8)
    #set(EXTRA_LIBS ${EXTRA_LIBS} PCRE2::8BIT)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${PCRE2_LIBRARIES})   
else(PCRE2_FOUND)
    message(FATAL_ERROR "Pcre2 library not found")
endif()

if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS -D_WIN32_WINNT=0x0600)
    set(EXTRA_LIBS ${EXTRA_LIBS} secur32.lib iphlpapi.lib)
endif()


enum_source_directories(LIBTIKTOKEN_SRCS ${LIBTIKTOKEN_SRCDIR})
set(LIBTIKTOKEN_SRCS ${LIBTIKTOKEN_SRCS} ${TIKTOKEN_COMMON_SRCS})


#if(BUILD_TIKTOKEN_STATIC)
    add_library(tiktoken STATIC ${LIBTIKTOKEN_SRCS})
  
    target_compile_definitions(tiktoken PRIVATE -DPCRE2_STATIC -DPCRE2_CODE_UNIT_WIDTH=8)  

    target_include_directories(tiktoken PRIVATE ${LIBTIKTOKEN_HEADERDIR})  
    target_include_directories(tiktoken PRIVATE ${COMMON_DIR})  
    target_include_directories(tiktoken PRIVATE ${CURL_INCLUDE_DIR})
    target_include_directories(tiktoken PRIVATE ${Boost_INCLUDE_DIRS})
    target_include_directories(tiktoken PRIVATE ${PCRE2_INCLUDE_DIR})  
    target_include_directories(tiktoken PRIVATE ${CPPCODEC_PATH})     
    
    target_link_libraries(tiktoken ${EXTRA_LIBS}) 
    
    set(TIKTOKEN_LIBRARIES tiktoken ${EXTRA_LIBS} CACHE INTERNAL "link tiktoken libraries")
    
    install(TARGETS tiktoken ARCHIVE DESTINATION lib/tiktoken)    
#endif()

#if(BUILD_TIKTOKEN_SHARED)
    set(TIKTOKEN_LIBRARIES tiktoken CACHE INTERNAL "link tiktoken libraries")
#endif()


if(BUILD_TOKEN_TEST)
    aux_source_directory(${TOKEN_TEST_SRCDIR} TOKEN_TEST_SRCS)
    add_executable(token_test ${TOKEN_TEST_SRCS} ${TOKENTEST_COMMON_SRCS})
    
    target_compile_definitions(token_test PRIVATE -DTIKTOKEN_STATICLIB)     

    target_include_directories(token_test PRIVATE ${COMMON_DIR})
    target_include_directories(token_test PRIVATE ${LIBTIKTOKEN_HEADERDIR})    
    
    target_link_libraries(token_test ${TIKTOKEN_LIBRARIES})  
    
    install(TARGETS token_test RUNTIME DESTINATION bin)    
endif()

install(FILES ${TIKTOKEN_PUBLIC_HEADERS} DESTINATION include/tiktoken)

if(INSTALL_ENCODING_FILES)
FILE(GLOB ENCODING_FILES ${PROJECT_SOURCE_DIR}/${ENCODING_FILE_DIR}/*.tiktoken)
INSTALL(FILES ${ENCODING_FILES} DESTINATION bin/cache)
endif()

# CMake config files.
set(TIKTOKEN_CONFIG_IN  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tiktoken-config.cmake.in)
set(TIKTOKEN_CONFIG_OUT ${CMAKE_CURRENT_BINARY_DIR}/cmake/tiktoken-config.cmake)
configure_file(${TIKTOKEN_CONFIG_IN} ${TIKTOKEN_CONFIG_OUT} @ONLY)
set(TIKTOKEN_CONFIG_VERSION_IN  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tiktoken-config-version.cmake.in)
set(TIKTOKEN_CONFIG_VERSION_OUT ${CMAKE_CURRENT_BINARY_DIR}/cmake/tiktoken-config-version.cmake)
configure_file(${TIKTOKEN_CONFIG_VERSION_IN} ${TIKTOKEN_CONFIG_VERSION_OUT} @ONLY)
install(FILES ${TIKTOKEN_CONFIG_OUT} ${TIKTOKEN_CONFIG_VERSION_OUT} DESTINATION lib/tiktoken/cmake)





