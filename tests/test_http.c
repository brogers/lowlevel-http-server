#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include <http.h>
#include <unity.h>
#include <unity_internals.h>

void setUp(void) {}
void tearDown(void) {}

// read_http_request() reads straight off a file descriptor, so drive it with
// the read end of a pipe primed with a raw request line.
static void parse_request(const char *raw, http_request *out, int rc) {
  int fds[2];
  TEST_ASSERT_EQUAL_INT(0, pipe(fds));

  size_t len = strlen(raw);
  TEST_ASSERT_EQUAL_INT((ssize_t)len, write(fds[1], raw, len));
  close(fds[1]);

  TEST_ASSERT_EQUAL_INT(rc, read_http_request(fds[0], out));
  close(fds[0]);
}

static void parse_request_pass(const char *raw, http_request *out) {
  parse_request(raw, out, 0);
}
static void parse_request_fail(const char *raw, http_request *out) {
  parse_request(raw, out, 1);
}

static void test_extracts_get_method(void) {
  http_request request = {0};
  parse_request_pass("GET /index.html HTTP/1.1\r\n\r\n", &request);
  TEST_ASSERT_EQUAL_STRING("GET", request.method);
}

static void test_extracts_post_method(void) {
  http_request request = {0};
  parse_request_pass("POST /submit HTTP/1.1\r\n\r\n", &request);
  TEST_ASSERT_EQUAL_STRING("POST", request.method);
}

static void test_extracts_path(void) {
  http_request request = {0};
  parse_request_pass("POST /submit HTTP/1.1\r\n\r\n", &request);
  TEST_ASSERT_EQUAL_STRING("/submit", request.path);
}

static void test_extracts_protocol(void) {
  http_request request = {0};
  parse_request_pass("POST /submit HTTP/1.1\r\n\r\n", &request);
  TEST_ASSERT_EQUAL_STRING("HTTP/1.1", request.protocol);
}

static void test_returns_error_on_invalid_method(void) {
  http_request request = {0};
  parse_request_fail("WHAT /submit HTTP/1.1\r\n\r\n", &request);
}

static void test_returns_error_on_invalid_protocol(void) {
  http_request request = {0};
  parse_request_fail("POST /submit HTTP/1.2\r\n\r\n", &request);
}

static void test_parse_http_header_count(void) {
  const char *raw_request = "GET /index.html HTTP/1.1\r\n"
                            "Host: localhost:8080\r\n"
                            "User-Agent: curl/7.68.0\r\n"
                            "Accept: */*\r\n"
                            "\r\n";
  http_request request = {0};
  parse_http_headers(raw_request, &request);
  TEST_ASSERT_EQUAL_INT(3, request.header_count);
  free_http_headers(&request);
}

static void test_parse_http_header_key_value(void) {
  const char *raw_request = "GET /index.html HTTP/1.1\r\n"
                            "Host: localhost:8080\r\n"
                            "\r\n";
  http_request request = {0};
  parse_http_headers(raw_request, &request);
  TEST_ASSERT_EQUAL_STRING("Host", request.headers[0].key);
  TEST_ASSERT_EQUAL_STRING("localhost:8080", request.headers[0].value);
  free_http_headers(&request);
}

static void test_free_http_headers(void) {
  const char *raw_request = "GET /index.html HTTP/1.1\r\n"
                            "Host: localhost:8080\r\n"
                            "\r\n";
  http_request request = {0};
  parse_http_headers(raw_request, &request);
  TEST_ASSERT_EQUAL_INT(1, request.header_count);

  free_http_headers(&request);

  TEST_ASSERT_NULL(request.headers);
  TEST_ASSERT_EQUAL_INT(0, request.header_count);
}

static void test_init_http_response(void) {
  http_response response = {0};
  init_http_response(&response);

  TEST_ASSERT_EQUAL_INT(200, response.status_code);
  TEST_ASSERT_EQUAL_STRING("OK", response.reason_phrase);
  TEST_ASSERT_EQUAL_INT(0, response.header_count);
  TEST_ASSERT_EQUAL_INT(0, response.body_length);
}

static void test_add_http_response_header(void) {
  http_response response = {0};

  add_http_header(&response, "Content-Type", "text/html");

  TEST_ASSERT_EQUAL_INT(1, response.header_count);
  TEST_ASSERT_EQUAL_STRING("Content-Type", response.headers[0].key);
  TEST_ASSERT_EQUAL_STRING("text/html", response.headers[0].value);
}

static void test_free_http_response_headers(void) {
  http_response response = {0};

  add_http_header(&response, "Content-Type", "text/html");

  TEST_ASSERT_EQUAL_INT(1, response.header_count);

  free_http_response(&response);

  TEST_ASSERT_NULL(response.headers);
  TEST_ASSERT_EQUAL_INT(0, response.header_count);
}

static void test_set_http_body(void) {
  http_response response;
  init_http_response(&response);

  char *body = "<html><body><h1>Hello, world!</h1></body></html>";

  set_http_body(&response, body);

  TEST_ASSERT_EQUAL_INT(48, response.body_length);
  TEST_ASSERT_EQUAL_STRING(body, response.body);

  free_http_response(&response);
}

static void test_construct_http_response(void) {
  char *body = "<html><body><h1>Hello, world!</h1></body></html>";
  char *headers =
      "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n";
  char expected[128];
  snprintf(expected, 128, "%s\r\n%s", headers, body);

  http_response response = {0};
  init_http_response(&response);
  size_t response_length = {0};

  add_http_header(&response, "Content-Type", "text/html");
  add_http_header(&response, "Connection", "close");

  set_http_body(&response, body);

  char *actual = construct_http_response(&response, &response_length);

  TEST_ASSERT_EQUAL_STRING(expected, actual);
  TEST_ASSERT_EQUAL_INT(111, response_length);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_extracts_get_method);
  RUN_TEST(test_extracts_post_method);
  RUN_TEST(test_extracts_path);
  RUN_TEST(test_extracts_protocol);
  RUN_TEST(test_returns_error_on_invalid_protocol);
  RUN_TEST(test_returns_error_on_invalid_method);
  RUN_TEST(test_parse_http_header_count);
  RUN_TEST(test_parse_http_header_key_value);
  RUN_TEST(test_free_http_headers);
  RUN_TEST(test_init_http_response);
  RUN_TEST(test_add_http_response_header);
  RUN_TEST(test_free_http_response_headers);
  RUN_TEST(test_set_http_body);
  RUN_TEST(test_construct_http_response);
  return UNITY_END();
}
