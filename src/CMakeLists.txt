# ---- Library: everything except main(), so tests can link against it ----
add_library(myhttp_lib STATIC
  tcp.c
)
add_library(myhttp::lib ALIAS myhttp_lib)

target_include_directories(myhttp_lib
  PUBLIC "${PROJECT_SOURCE_DIR}/include"
)

target_compile_features(myhttp_lib PUBLIC c_std_23)
target_link_libraries(myhttp_lib PRIVATE myhttp::warnings)

# ---- Executable: just the entry point ----
add_executable(myhttp main.c)
target_link_libraries(myhttp PRIVATE myhttp::lib myhttp::warnings)
