#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

#define HTTP_MAX_REQUEST_LEN 8192 * 4
#define HTTP_METHOD_MAX_LEN 8
#define HTTP_PATH_MAX_LEN 2048
#define HTTP_PROTOCOL_MAX_LEN 16
#define HTTP_MAX_HEADER_KEY_LEN 256
#define HTTP_MAX_HEADER_VALUE_LEN 256
#define HTTP_MAX_HEADER_REASON_LEN 64

typedef enum {
  HTTP_PARSE_OK,
  HTTP_PARSE_INVALID,
} http_parse_e;

typedef struct {
  char key[HTTP_MAX_HEADER_KEY_LEN];
  char value[HTTP_MAX_HEADER_VALUE_LEN];
} http_header_t;

typedef struct {
  char method[HTTP_METHOD_MAX_LEN];
  char path[HTTP_PATH_MAX_LEN];
  char protocol[HTTP_PROTOCOL_MAX_LEN];
  http_header_t *headers;
  size_t header_count;
  char buffer[HTTP_MAX_REQUEST_LEN];
} http_request;

typedef struct {
  int status_code;
  char reason_phrase[HTTP_MAX_HEADER_REASON_LEN];
  http_header_t *headers;
  size_t header_count;
  char *body;
  size_t body_length;
} http_response;

void init_http_response(http_response *response);
void free_http_headers(http_request *request);

http_parse_e parse_http_headers(const char *raw_request, http_request *request);
http_parse_e read_http_request(int socket_fd, http_request *request);

void add_http_header(http_response *response, const char *key,
                     const char *value);
void set_http_body(http_response *response, char *body);

char *construct_http_response(const http_response *response,
                              size_t *response_length);
void send_http_response(int client_fd, const http_response *response);

void free_http_response(http_response *response);

#endif // !HTTP_H
