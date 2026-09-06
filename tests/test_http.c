#include <string.h>
#include <unistd.h>

#include <http.h>
#include <unity.h>

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

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_extracts_get_method);
  RUN_TEST(test_extracts_post_method);
  RUN_TEST(test_extracts_path);
  RUN_TEST(test_extracts_protocol);
  RUN_TEST(test_returns_error_on_invalid_protocol);
  RUN_TEST(test_returns_error_on_invalid_method);
  return UNITY_END();
}
