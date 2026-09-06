#include <http.h>
#include <main.h>
#include <stdlib.h>
#include <tcp.h>

int main(void) {
  tcp_server server = {0};
  server_status_e status = bind_tcp_port(&server, 8080);
  if (status != SERVER_OK) {
    debug_log("Server initialization failed");
    exit(EXIT_FAILURE);
  }

  int client_fd = accept_client(server.socket_fd);
  if (client_fd == -1) {
    debug_log("Failed to accept client connection");
    close(server.socket_fd);
    exit(EXIT_FAILURE);
  }

  http_request request = {0};

  if (read_http_request(client_fd, &request) == HTTP_PARSE_INVALID) {
    debug_log("Failed reading request");
    close(client_fd);
    close(server.socket_fd);
    exit(EXIT_FAILURE);
  }

  if (*request.method)
    printf("Method: %s\n", request.method);
  if (*request.path)
    printf("Path: %s\n", request.path);
  if (*request.protocol)
    printf("Protocol: %s\n", request.protocol);

  debug_log("Client connected");

  close(client_fd);
  close(server.socket_fd);
  return 0;
}
