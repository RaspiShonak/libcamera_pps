cmake_minimum_required(VERSION 3.6)

add_library(postprocess post_process_stage.cpp negate_stage.cpp sobel_cv_stage.cpp)

install(TARGETS encoders LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

