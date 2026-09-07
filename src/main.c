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

  free_http_headers(&request);

  http_response response = {0};
  init_http_response(&response);
  add_http_header(&response, "Content-Type", "text/html");
  set_http_body(&response, "<html><body><h1>Hello, world!</h1></body></html>");
  char content_length[16];
  snprintf(content_length, 16, "%lu", response.body_length);
  add_http_header(&response, "Content-Length", content_length);
  add_http_header(&response, "Connection", "close");

  send_http_response(client_fd, &response);

  free_http_response(&response);

  close(client_fd);
  close(server.socket_fd);
  return 0;
}
