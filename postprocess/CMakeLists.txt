cmake_minimum_required(VERSION 3.6)

set(SRC post_process_stage.cpp negate_stage.cpp)
set(TARGET_LIBS "")

find_package(OpenCV QUIET)

if (OPENCV_FOUND)
    message(STATUS "OpenCV library found:")
    message(STATUS "    version: ${OPENCV_VERSION}")
    message(STATUS "    libraries: ${OPENCV_LIBS}")
    message(STATUS "    include path: ${OPENCV_INCLUDE_DIRS}")
    include_directories(${OPENCV_INCLUDE_DIRS})
    set(SRC ${SRC} sobel_cv_stage.cpp face_detect_cv_stage.cpp)
    set(TARGET_LIBS ${OPENCV_LIBS})
    add_definitions(-DOPENCV_PRESENT)
else()
    message(WARNING "OpenCV not found, some stages will not be built!")
endif()

add_library(postprocess ${SRC})
target_link_libraries(postprocess ${TARGET_LIBS})

install(TARGETS postprocess LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
