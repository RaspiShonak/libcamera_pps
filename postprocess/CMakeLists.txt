cmake_minimum_required(VERSION 3.6)

pkg_check_modules(OPENCV REQUIRED opencv)

include_directories(${OPENCV_INCLUDE_DIRS})

message(STATUS "${OPENCV_LIBRARIES}")

find_package(OpenCV 3.4.7 REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

add_library(postprocess post_process_stage.cpp negate_stage.cpp sobel_cv_stage.cpp face_detect_cv_stage.cpp)
target_link_libraries(postprocess ${OpenCV_LIBS})

install(TARGETS postprocess LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

