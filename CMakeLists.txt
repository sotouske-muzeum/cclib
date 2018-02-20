
cmake_minimum_required(VERSION 3.1.0)

project(cclib)
#SET(CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_BUILD_TYPE Release)

enable_testing()

include_directories(tests include lib)

find_package(PkgConfig)
pkg_search_module(LIBXML2 REQUIRED libxml-2.0)
pkg_search_module(LIBHTTP REQUIRED libmicrohttpd)
pkg_search_module(LIBCURL REQUIRED libcurl)
pkg_search_module(LIBWEBS REQUIRED libwebsockets)
pkg_search_module(LIBCRYPTO REQUIRED libcrypto)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CCLIB_LIB_MODE STATIC) # [STATIC | SHARED | MODULE]

# - add ccl libraries -
add_library(cclstr ${CCLIB_LIB_MODE} lib/cclstr.cc)
add_library(cclvar ${CCLIB_LIB_MODE} lib/cclvar.cc)

add_library(cclvarval ${AMTRAK_LIB_MODE} lib/cclvarval.cc)
target_link_libraries(cclvarval cclvar cclstr)

add_library(cclsys ${CCLIB_LIB_MODE} lib/cclsys.cc)
target_link_libraries(cclsys rt)

add_library(ccljson ${CCLIB_LIB_MODE} lib/ccljson.cc lib/ccljson_parse_actions.cc)
target_link_libraries(ccljson cclvar)

add_library(cclxml ${CCLIB_LIB_MODE} lib/cclxml.cc)
target_include_directories(cclxml PUBLIC ${LIBXML2_INCLUDE_DIRS})
target_link_libraries(cclxml ${LIBXML2_LIBRARIES})

add_library(cclhttp ${CCLIB_LIB_MODE} lib/cclhttp.cc)
target_include_directories(cclhttp PUBLIC ${LIBHTTP_INCLUDE_DIRS})
target_link_libraries(cclhttp microhttpd)

add_library(cclcurl ${CCLIB_LIB_MODE} lib/cclcurl.cc)
target_include_directories(cclcurl PUBLIC ${LIBCURL_INCLUDE_DIRS})
target_link_libraries(cclcurl ${LIBCURL_LIBRARIES})

add_library(cclwebs ${CCLIB_LIB_MODE} lib/cclwebs.cc)
target_include_directories(cclwebs PUBLIC ${LIBWEBS_INCLUDE_DIRS})
target_link_libraries(cclwebs ${LIBWEBS_LIBRARIES})

add_library(cclcrypto ${CCLIB_LIB_MODE} lib/cclcrypto.cc)
target_include_directories(cclcrypto PUBLIC ${LIBCRYPTO_INCLUDE_DIRS})
target_link_libraries(cclcrypto ${LIBCRYPTO_LIBRARIES})

# - string tools test -
add_executable(cclstr_test tests/cclstr_test.cc)
target_link_libraries(cclstr_test cclstr)
add_test(cclstr_test_read_file cclstr_test read_file)
add_test(cclstr_test_split cclstr_test split)
add_test(cclstr_test_join cclstr_test join)
add_test(cclstr_test_format cclstr_test format)
add_test(cclstr_test_replace cclstr_test replace)
add_test(cclstr_test_regex cclstr_test regex)

# - variable tools test -
add_executable(cclvar_test tests/cclvar_test.cc)
target_link_libraries(cclvar_test cclvar)
add_test(cclvar_test_all cclvar_test all)

# - variable validation tools test -
add_executable(cclvarval_test tests/cclvarval_test.cc)
target_link_libraries(cclvarval_test cclvarval)
add_test(cclvarval_test_all cclvarval_test all)

# - system tools test -
add_executable(cclsys_test tests/cclsys_test.cc)
target_link_libraries(cclsys_test cclstr cclsys)
add_test(cclsys_test_poll cclsys_test poll)
add_test(cclsys_test_signals cclsys_test signals)
add_test(cclsys_test_pid cclsys_test pid)
add_test(cclsys_test_timer cclsys_test timer)
add_test(cclsys_test_pipe cclsys_test pipe)
add_test(cclsys_test_file cclsys_test file)
add_test(cclsys_test_socket_tcp cclsys_test socket_tcp)
add_test(cclsys_test_socket_udp cclsys_test socket_udp)
add_test(cclsys_test_clock cclsys_test clock)

# - json tools test -
add_executable(ccljson_test tests/ccljson_test.cc)
target_link_libraries(ccljson_test cclstr ccljson)
add_test(ccljson_test_create ccljson_test create)
add_test(ccljson_test_parse ccljson_test parse)

# - xml tools test -
add_executable(cclxml_test tests/cclxml_test.cc)
target_link_libraries(cclxml_test cclstr cclxml)
add_test(cclxml_test_parse cclxml_test parse)
add_test(cclxml_test_generate cclxml_test generate)

# - http tools test -
add_executable(cclhttp_test tests/cclhttp_test.cc)
target_link_libraries(cclhttp_test cclsys cclhttp)
add_test(cclhttp_test_all cclhttp_test all)
add_test(cclhttp_test_auth_all cclhttp_test auth_all)

# - curl tools test -
add_executable(cclcurl_test tests/cclcurl_test.cc)
target_link_libraries(cclcurl_test cclstr cclsys cclhttp cclcurl)
add_test(cclcurl_test_get cclcurl_test get)
add_test(cclcurl_test_put cclcurl_test put)
add_test(cclcurl_test_delete cclcurl_test delete)
add_test(cclcurl_test_head cclcurl_test head)

# - webs tools test -
add_executable(cclwebs_test tests/cclwebs_test.cc)
target_link_libraries(cclwebs_test cclsys cclwebs)
#add_test(cclwebs_test_all cclwebs_test all)

# - crypto tools test -
add_executable(cclcrypto_test tests/cclcrypto_test.cc)
target_link_libraries(cclcrypto_test cclsys cclcrypto)
add_test(cclcrypto_test_all cclcrypto_test all)

add_definitions(-O3 -Wall -Wfatal-errors -DMINT_DEVEL_PC)
#add_definitions(-g -Wall -Wfatal-errors -DMINT_DEVEL_PC)
#add_definitions(-Wall -Wfatal-errors -DMINT_DEVEL_PC)

