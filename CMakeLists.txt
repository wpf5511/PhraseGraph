cmake_minimum_required(VERSION 3.3)
project(PhraseGraph)


SET(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DPERCEPTRON_FOR_DECODING -DLABELED -DTARGET_LANGUAGE=chinese")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -O3 -g -ggdb")
add_subdirectory(deparser)


include_directories(cereal-1.2.0/include rapidxml-1.13 /usr/local/include/eigen3)

include_directories(${deparser_INCLUDE_DIRS})


set(SOURCE_FILES    main.cpp PhraseGraph.cpp ZparTree.cpp CommonTool.cpp SentenceGraph.cpp PhraseTransform.cpp init.cpp)
add_executable(PhraseGraph ${SOURCE_FILES})


add_library(pydirt SHARED init.cpp ZparTree.cpp PhraseGraph.cpp CommonTool.cpp SentenceGraph.cpp PythonLib.cpp)
#add_executable(pydirt init.cpp ZparTree.cpp PhraseGraph.cpp CommonTool.cpp SentenceGraph.cpp PythonLib.cpp)
target_link_libraries(pydirt ${DEPARSER_LIB_NAME})
