# ---- cJSON: vendored under cjson/ (v1.7.19, MIT). No network needed. --------
# Upstream ships as a single .c + .h ("just copy it into your project"), so we
# do exactly that. Built as a small static lib; the SYSTEM include dir keeps its
# headers from tripping our -Wpedantic/-Wconversion flags, and it is not linked
# against myhttp::warnings so its own sources build quietly too.
add_library(cjson STATIC cjson/cJSON.c)
add_library(cjson::cjson ALIAS cjson)
target_include_directories(
    cjson
    SYSTEM
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/cjson"
)
if(NOT WIN32)
    target_link_libraries(cjson PUBLIC m)
endif()
