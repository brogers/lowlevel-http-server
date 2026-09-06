# An INTERFACE target that carries the project's warning flags.
# Link it PRIVATE into first-party targets; it is never propagated to consumers.

add_library(myhttp_warnings INTERFACE)
add_library(myhttp::warnings ALIAS myhttp_warnings)

if(MSVC)
  target_compile_options(myhttp_warnings INTERFACE
    /W4
    $<$<BOOL:${MYHTTP_WARNINGS_AS_ERRORS}>:/WX>
  )
else()
  target_compile_options(myhttp_warnings INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    -Wundef
    -Wdouble-promotion
    -Wstrict-prototypes
    $<$<BOOL:${MYHTTP_WARNINGS_AS_ERRORS}>:-Werror>
  )
endif()
