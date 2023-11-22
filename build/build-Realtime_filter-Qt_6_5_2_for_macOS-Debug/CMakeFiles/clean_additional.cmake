# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/StaticGLEW_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/StaticGLEW_autogen.dir/ParseCache.txt"
  "CMakeFiles/realtime_filter_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/realtime_filter_autogen.dir/ParseCache.txt"
  "StaticGLEW_autogen"
  "realtime_filter_autogen"
  )
endif()
