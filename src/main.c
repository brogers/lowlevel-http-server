#include <http.h>
#include <stdlib.h>
#include <tcp.h>

#include <main.h>

int main(void) {
  tcp_server server = {0};
  server_status_e status = bind_tcp_port(&server, 8080);
  if (status != SERVER_OK) {
    debug_log("Server initialization failed");
    exit(EXIT_FAILURE);
  }

  for (;;) {
    int client_fd = accept_client(server.socket_fd);
    if (client_fd == -1) {
      debug_log("Failed to accept client connection");
      close(server.socket_fd);
      exit(EXIT_FAILURE);
    }

    debug_log("Client connected");

    http_request req = {0};
    http_response res = {0};

    init_http_response(&res);

    if (read_http_request(client_fd, &req) != HTTP_PARSE_OK) {
      debug_log("Failed to read or parse HTTP request");
      close(client_fd);
      return 0;
    }

    if (parse_http_headers(req.buffer, &req) != HTTP_PARSE_OK) {
      debug_log("Failed to read or parse HTTP request");
      close(client_fd);
      return 0;
    }

    char sanitized_path[1024] = {0};
    sanitize_path(req.path, sanitized_path, sizeof(sanitized_path));
    serve_file(sanitized_path, &res);

    send_http_response(client_fd, &res);

    close(client_fd);
  }
  close(server.socket_fd);
  return 0;
}
