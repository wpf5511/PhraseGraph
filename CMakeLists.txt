cmake_minimum_required(VERSION 3.3)
project(PhraseGraph)

SET(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3 -Wall -g -ggdb")       

include_directories(cereal-1.2.0/include rapidxml-1.13 /usr/local/include/eigen3)
set(SOURCE_FILES    main.cpp PhraseGraph.cpp ZparTree.cpp CommonTool.cpp SentenceGraph.cpp PhraseTransform.cpp )
add_executable(PhraseGraph ${SOURCE_FILES})

