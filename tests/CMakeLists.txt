# ---- Unity: vendored under vendor/unity/ (v2.7.0, MIT). No network needed. ----
# Built as a small static lib; SYSTEM include dir keeps its headers from
# tripping our -Wpedantic/-Wconversion flags.
add_library(unity STATIC vendor/unity/unity.c)
add_library(unity::framework ALIAS unity)
target_include_directories(unity SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/unity")

# Register a Unity-based test executable and wire it into CTest in one line.
function(myhttp_add_unity_test name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} PRIVATE myhttp::lib unity::framework myhttp::warnings)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

myhttp_add_unity_test(test_tcp test_tcp.c)
myhttp_add_unity_test(test_http test_http.c)
