#include <assert.h>
#include <unistd.h>

#include <tcp.h>

// Port 0 asks the kernel for an ephemeral port; binding it should always
// succeed on a healthy machine and hands back a valid socket fd.
static void test_bind_ephemeral_port(void) {
  tcp_server server = {0};
  server_status_e status = bind_tcp_port(&server, 0);
  assert(status == SERVER_OK);
  assert(server.socket_fd > 0);
  close(server.socket_fd);
}

int main(void) {
  test_bind_ephemeral_port();
  return 0;
}
