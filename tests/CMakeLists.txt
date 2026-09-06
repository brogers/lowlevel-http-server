add_executable(test_tcp test_tcp.c)
target_link_libraries(test_tcp PRIVATE myhttp::lib myhttp::warnings)

add_test(NAME tcp.bind_ephemeral_port COMMAND test_tcp)
