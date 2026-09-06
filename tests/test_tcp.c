#include <unistd.h>

#include <tcp.h>
#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

// Port 0 asks the kernel for an ephemeral port; binding it should always
// succeed on a healthy machine and hand back a valid socket fd.
static void test_bind_ephemeral_port(void) {
  tcp_server server = {0};
  server_status_e status = bind_tcp_port(&server, 0);
  TEST_ASSERT_EQUAL_INT(SERVER_OK, status);
  TEST_ASSERT_GREATER_THAN_INT(0, server.socket_fd);
  close(server.socket_fd);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_bind_ephemeral_port);
  return UNITY_END();
}
