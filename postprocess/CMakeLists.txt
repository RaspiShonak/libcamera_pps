cmake_minimum_required(VERSION 3.6)

pkg_check_modules(OPENCV REQUIRED opencv)

include_directories(${OPENCV_INCLUDE_DIRS})

add_library(postprocess post_process_stage.cpp negate_stage.cpp sobel_cv_stage.cpp)
target_link_libraries(postprocess ${OPENCV_LIBRARIES})

install(TARGETS postprocess LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

