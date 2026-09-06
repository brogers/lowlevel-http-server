#include <http.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

http_parse_e read_http_request(int socket_fd, http_request *request) {
  char buffer[HTTP_MAX_REQUEST_LEN] = {0};
  ssize_t bytes_read = read(socket_fd, buffer, sizeof(buffer) - 1);

  if (bytes_read <= 0)
    return HTTP_PARSE_INVALID;

  buffer[bytes_read] = '\0';

  if (sscanf(buffer, "%7s %2047s %15s", request->method, request->path,
             request->protocol) != 3)
    return HTTP_PARSE_INVALID;

  char *http_methods[5] = {"GET", "POST", "PUT", "PATCH", "DELETE"};

  bool valid = false;

  for (int i = 0; i < 5; i++)
    if (!strcmp(http_methods[i], request->method))
      valid = true;

  if (!valid)
    return HTTP_PARSE_INVALID;

  if (strcmp("HTTP/1.1", request->protocol))
    return HTTP_PARSE_INVALID;

  return HTTP_PARSE_OK;
}
