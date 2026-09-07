#define _POSIX_C_SOURCE 200809L // mkdtemp, fdopen, chdir on strict -std=c2x

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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
  TEST_ASSERT_NULL(response.headers);
  TEST_ASSERT_NULL(response.body);
}

static void test_add_http_response_header(void) {
  http_response response = {0};

  add_http_header(&response, "Content-Type", "text/html");

  TEST_ASSERT_EQUAL_INT(1, response.header_count);
  TEST_ASSERT_EQUAL_STRING("Content-Type", response.headers[0].key);
  TEST_ASSERT_EQUAL_STRING("text/html", response.headers[0].value);

  free_http_response(&response);
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

  free_http_response(&response);
  free(actual);
}

static void test_sanitize_path_valid(void) {
  char *path = "/index.html";
  char *expected = "./www/index.html";
  char actual[128];
  sanitize_path(path, actual, sizeof(actual));

  TEST_ASSERT_EQUAL_STRING(expected, actual);
}

static void test_sanitize_path_invalid_dots(void) {
  char *path = "../index.html";
  char *expected = "./www/404.html";
  char actual[128];
  sanitize_path(path, actual, sizeof(actual));

  TEST_ASSERT_EQUAL_STRING(expected, actual);
}

// serve_file() opens files with fopen(..., "rb+")
// fixtures are staged in a throwaway directory.

static char g_tmpl[] = "/tmp/myhttp_test_XXXXXX";
static char *g_tmpdir;

static void ensure_tmpdir(void) {
  if (!g_tmpdir) {
    g_tmpdir = mkdtemp(g_tmpl);
    TEST_ASSERT_NOT_NULL(g_tmpdir);
  }
}

static void write_fixture(const char *name, const char *content, char *out,
                          size_t out_size) {
  ensure_tmpdir();
  snprintf(out, out_size, "%s/%s", g_tmpdir, name);
  FILE *f = fopen(out, "wb");
  TEST_ASSERT_NOT_NULL(f);
  size_t len = strlen(content);
  TEST_ASSERT_EQUAL_INT((int)len, (int)fwrite(content, 1, len, f));
  fclose(f);
}

static const char *header_value(const http_response *r, const char *key) {
  for (size_t i = 0; i < r->header_count; i++)
    if (!strcmp(r->headers[i].key, key))
      return r->headers[i].value;
  return NULL;
}

static void serve_fixture(const char *name, const char *content,
                          http_response *response) {
  char path[512];
  write_fixture(name, content, path, sizeof(path));
  init_http_response(response);
  serve_file(path, response);
}

static void test_serve_file_keeps_200_status(void) {
  http_response response = {0};
  serve_fixture("ok.html", "<html><body>hi</body></html>", &response);

  TEST_ASSERT_EQUAL_INT(200, response.status_code);
  TEST_ASSERT_EQUAL_STRING("OK", response.reason_phrase);

  free_http_response(&response);
}

static void test_serve_file_reads_body(void) {
  const char *content = "print('hello')\n";
  http_response response = {0};
  serve_fixture("app.js", content, &response);

  TEST_ASSERT_EQUAL_INT((int)strlen(content), (int)response.body_length);
  TEST_ASSERT_NOT_NULL(response.body);
  TEST_ASSERT_EQUAL_MEMORY(content, response.body, strlen(content));

  free_http_response(&response);
}

static void test_serve_file_sets_content_length_header(void) {
  http_response response = {0};
  serve_fixture("data.bin", "abcdefghij", &response);

  TEST_ASSERT_EQUAL_STRING("10", header_value(&response, "Content-Length"));

  free_http_response(&response);
}

static void test_serve_file_content_type_html(void) {
  http_response response = {0};
  serve_fixture("page.html", "<h1>x</h1>", &response);

  TEST_ASSERT_EQUAL_STRING("text/html",
                           header_value(&response, "Content-Type"));

  free_http_response(&response);
}

static void test_serve_file_content_type_css(void) {
  http_response response = {0};
  serve_fixture("style.css", "body{}", &response);

  TEST_ASSERT_EQUAL_STRING("text/css", header_value(&response, "Content-Type"));

  free_http_response(&response);
}

static void test_serve_file_content_type_js(void) {
  http_response response = {0};
  serve_fixture("app.js", "var x=1;", &response);

  TEST_ASSERT_EQUAL_STRING("application/javascript",
                           header_value(&response, "Content-Type"));

  free_http_response(&response);
}

static void test_serve_file_content_type_png(void) {
  http_response response = {0};
  serve_fixture("pixel.png", "\x89PNG", &response);

  TEST_ASSERT_EQUAL_STRING("image/png",
                           header_value(&response, "Content-Type"));

  free_http_response(&response);
}

static void test_serve_file_content_type_unknown_is_octet_stream(void) {
  http_response response = {0};
  serve_fixture("notes.txt", "plain text", &response);

  TEST_ASSERT_EQUAL_STRING("application/octet-stream",
                           header_value(&response, "Content-Type"));

  free_http_response(&response);
}

static void test_serve_file_missing_returns_404(void) {
  char cwd[4096];
  TEST_ASSERT_NOT_NULL(getcwd(cwd, sizeof(cwd)));

  ensure_tmpdir();
  char wwwdir[512];
  snprintf(wwwdir, sizeof(wwwdir), "%s/www", g_tmpdir);
  TEST_ASSERT_EQUAL_INT(0, mkdir(wwwdir, 0755));
  char path[512];
  write_fixture("www/404.html", "<h1>Not Found</h1>", path, sizeof(path));

  // serve_file() falls back to the relative "./www/404.html"; run from beside
  // it.
  TEST_ASSERT_EQUAL_INT(0, chdir(g_tmpdir));

  http_response response = {0};
  init_http_response(&response);
  serve_file("this-file-does-not-exist.html", &response);

  TEST_ASSERT_EQUAL_INT(0, chdir(cwd));

  TEST_ASSERT_EQUAL_INT(404, response.status_code);
  TEST_ASSERT_EQUAL_STRING("Not Found", response.reason_phrase);
  // the recursive fallback serves the 404.html body, so its headers land too
  TEST_ASSERT_EQUAL_STRING("text/html",
                           header_value(&response, "Content-Type"));
  TEST_ASSERT_EQUAL_INT(18, (int)response.body_length);

  free_http_response(&response);
}

// When 404.html is missing too, serve_file() must still terminate (return a
// bodyless 404) rather than recurse forever.
static void test_serve_file_missing_without_404_page(void) {
  char cwd[4096];
  TEST_ASSERT_NOT_NULL(getcwd(cwd, sizeof(cwd)));

  ensure_tmpdir();
  char bare[512];
  snprintf(bare, sizeof(bare), "%s/bare", g_tmpdir);
  TEST_ASSERT_EQUAL_INT(0, mkdir(bare, 0755));
  TEST_ASSERT_EQUAL_INT(0, chdir(bare));

  http_response response = {0};
  init_http_response(&response);
  serve_file("nope.html", &response);

  TEST_ASSERT_EQUAL_INT(0, chdir(cwd));

  TEST_ASSERT_EQUAL_INT(404, response.status_code);
  TEST_ASSERT_EQUAL_STRING("Not Found", response.reason_phrase);
  TEST_ASSERT_NULL(response.body);
  TEST_ASSERT_EQUAL_INT(0, (int)response.header_count);

  free_http_response(&response);
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
  RUN_TEST(test_sanitize_path_valid);
  RUN_TEST(test_sanitize_path_invalid_dots);
  RUN_TEST(test_serve_file_keeps_200_status);
  RUN_TEST(test_serve_file_reads_body);
  RUN_TEST(test_serve_file_sets_content_length_header);
  RUN_TEST(test_serve_file_content_type_html);
  RUN_TEST(test_serve_file_content_type_css);
  RUN_TEST(test_serve_file_content_type_js);
  RUN_TEST(test_serve_file_content_type_png);
  RUN_TEST(test_serve_file_content_type_unknown_is_octet_stream);
  RUN_TEST(test_serve_file_missing_returns_404);
  RUN_TEST(test_serve_file_missing_without_404_page);
  return UNITY_END();
}
