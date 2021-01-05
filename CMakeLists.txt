cmake_minimum_required(VERSION 2.8)

project(wav2mp3 C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(sources wav2mp3.c lame/lame.cmake)

include_directories(/usr/local/include/lame)

include(lame/lame.cmake)

set(libraries mp3lame)

add_executable(${PROJECT_NAME} ${sources})

target_link_libraries(${PROJECT_NAME} ${libraries})

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(wav2mp3 PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(wav2mp3 "${CMAKE_THREAD_LIBS_INIT}")
endif()
