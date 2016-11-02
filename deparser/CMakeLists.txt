cmake_minimum_required (VERSION 2.8.0)
project (deparser)
set (SOURCE_DIR             ${PROJECT_SOURCE_DIR}/include/dep/Zpar/src)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)


include_directories (${SOURCE_DIR}/chinese)
include_directories(${SOURCE_DIR}/include)

set (CMAKE_CXX_FLAGS "-std=c++11 -g -w -W -O3 -DNDEBUG -DPERCEPTRON_FOR_DECODING -DLABELED -DTARGET_LANGUAGE=chinese")
#set(CMAKE_CXX_FLAGS " -std=c++11 -DLABELED  -DTARGET_LANGUAGE=chinese")
include_directories (${SOURCE_DIR}/common/depparser)
include_directories (${SOURCE_DIR}/common/depparser/implementations/arceager)

include_directories(${PROJECT_SOURCE_DIR}/include/dep/Zpar)

include_directories(${SOURCE_DIR}/chinese/tagger/implementations/agenda)
include_directories(${SOURCE_DIR}/chinese/tagger)

set(${PROJECT_NAME}_INCLUDE_DIRS ${SOURCE_DIR}/include
                                 ${SOURCE_DIR}/chinese
                                 ${SOURCE_DIR}/common/depparser
                                 ${SOURCE_DIR}/common/depparser/implementations/arceager
                                 ${PROJECT_SOURCE_DIR}/include/dep/Zpar
                                 ${SOURCE_DIR}/chinese/tagger
                                 ${SOURCE_DIR}/chinese/tagger/implementations/agenda
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(DEPARSER_FILE
${SOURCE_DIR}/libs/reader.cpp
${SOURCE_DIR}/libs/writer.cpp
${SOURCE_DIR}/libs/options.cpp
${SOURCE_DIR}/libs/linguistics/lemma.cpp
${SOURCE_DIR}/libs/linguistics/conll.cpp
${SOURCE_DIR}/libs/learning/dbn.cpp
        ${SOURCE_DIR}/chinese/tagger/implementations/agenda/tagger.cpp
        ${SOURCE_DIR}/chinese/tagger/implementations/agenda/weight.cpp
${SOURCE_DIR}/common/depparser/implementations/arceager/depparser.cpp
${SOURCE_DIR}/common/depparser/implementations/arceager/depparser_weight.cpp
${PROJECT_SOURCE_DIR}/src/dep/Zpar/Zpar.cpp
)
set(DEPARSER_LIB_NAME ${PROJECT_NAME} CACHE INTERNAL "deparser-lib library name" FORCE)
add_library(${DEPARSER_LIB_NAME} SHARED ${DEPARSER_FILE})


add_executable (depparser_test
        ${SOURCE_DIR}/libs/reader.cpp
        ${SOURCE_DIR}/libs/writer.cpp
        ${SOURCE_DIR}/libs/options.cpp
        ${SOURCE_DIR}/libs/linguistics/lemma.cpp
        ${SOURCE_DIR}/libs/linguistics/conll.cpp
        ${SOURCE_DIR}/libs/learning/dbn.cpp
        ${SOURCE_DIR}/chinese/tagger/implementations/agenda/tagger.cpp
        ${SOURCE_DIR}/chinese/tagger/implementations/agenda/weight.cpp
        ${SOURCE_DIR}/common/depparser/implementations/arceager/depparser.cpp
        ${SOURCE_DIR}/common/depparser/implementations/arceager/depparser_weight.cpp
        ${PROJECT_SOURCE_DIR}/src/dep/Zpar/Zpar.cpp
        ${PROJECT_SOURCE_DIR}/src/dep/Zpar/main.cpp
        )

