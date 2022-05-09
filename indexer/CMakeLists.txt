
   
cmake_minimum_required(VERSION 3.14)
project(pse LANGUAGES CXX)

find_package(UPCXX REQUIRED)
find_package(CURL REQUIRED) 

add_executable(distribute_indexer distribute_indexer.cpp)
target_link_libraries(distribute_indexer PRIVATE UPCXX::upcxx)
target_include_directories(distribute_indexer PRIVATE "/global/homes/x/xizhang1/parallelized-simple-search-engine/utils/")
target_link_libraries(distribute_indexer PRIVATE "/global/homes/x/xizhang1/parallelized-simple-search-engine/lib/libutils.a")
target_include_directories(distribute_indexer PRIVATE ${CURL_INCLUDE_DIR})
target_link_libraries(distribute_indexer PRIVATE ${CURL_LIBRARIES})