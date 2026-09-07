#include <http.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcp.h>

#include <main.h>

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

  debug_log("Client connected");

  http_request request = {0};

  if (read_http_request(client_fd, &request) != HTTP_PARSE_OK) {
    debug_log("Failed to read or parse HTTP request");
    close(client_fd);
    return 0;
  }

  if (parse_http_headers(request.buffer, &request) != HTTP_PARSE_OK) {
    debug_log("Failed to read or parse HTTP request");
    close(client_fd);
    return 0;
  }
  printf("Parsed HTTP Headers:\n");
  for (size_t i = 0; i < request.header_count; i++) {
    printf("%s: %s\n", request.headers[i].key, request.headers[i].value);
  }

  free_http_headers(&request);

  close(client_fd);
  close(server.socket_fd);
  return 0;
}
