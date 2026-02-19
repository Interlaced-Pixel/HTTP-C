#include <doctest/doctest.h>
#include <cstring>

extern "C" {
#include "http.h"
}

// C-style callbacks used by parser tests (C linkage required)
extern "C" {
static int _td_on_request_line_called = 0;
static int _td_on_headers_complete_called = 0;
static int _td_on_body_called = 0;
static int _td_on_complete_called = 0;

static void _td_on_request_line(void *user_data, http_method_t method, const char *uri, const char *version) {
  (void)user_data; (void)method; (void)uri; (void)version; _td_on_request_line_called++;
}
static void _td_on_headers_complete(void *user_data) { (void)user_data; _td_on_headers_complete_called++; }
static void _td_on_body(void *user_data, const char *data, size_t len) { (void)user_data; (void)data; (void)len; _td_on_body_called++; }
static void _td_on_complete(void *user_data) { (void)user_data; _td_on_complete_called++; }
}

TEST_SUITE("parser") {

TEST_CASE("basic get request (impl)") {
  http_parser_t parser;
  _td_on_request_line_called = _td_on_headers_complete_called = _td_on_body_called = _td_on_complete_called = 0;
  http_parser_settings_t settings = {_td_on_request_line, NULL, _td_on_body, _td_on_headers_complete, _td_on_complete};
  http_parser_init(&parser);
  const char *req = "GET /hello HTTP/1.1\r\nHost: localhost\r\n\r\n";
  size_t n = http_parser_execute(&parser, &settings, req, strlen(req));
  CHECK_EQ(n, strlen(req));
  CHECK_EQ(_td_on_request_line_called, 1);
  CHECK_EQ(_td_on_headers_complete_called, 1);
  CHECK_EQ(_td_on_complete_called, 1);
  CHECK(http_parser_is_done(&parser));
}

TEST_CASE("chunked request parsing") {
  http_parser_t parser;
  _td_on_request_line_called = _td_on_headers_complete_called = _td_on_body_called = _td_on_complete_called = 0;
  http_parser_settings_t settings = {_td_on_request_line, NULL, _td_on_body, _td_on_headers_complete, _td_on_complete};
  http_parser_init(&parser);
  const char *req1 = "GET /hel";
  const char *req2 = "lo HTTP/1.1\r\n\r\n";
  http_parser_execute(&parser, &settings, req1, strlen(req1));
  CHECK_EQ(_td_on_request_line_called, 0);
  http_parser_execute(&parser, &settings, req2, strlen(req2));
  CHECK_EQ(_td_on_request_line_called, 1);
  CHECK_EQ(_td_on_complete_called, 1);
}

TEST_CASE("post request with body") {
  http_parser_t parser;
  _td_on_request_line_called = _td_on_headers_complete_called = _td_on_body_called = _td_on_complete_called = 0;
  http_parser_settings_t settings = {_td_on_request_line, NULL, _td_on_body, _td_on_headers_complete, _td_on_complete};
  http_parser_init(&parser);
  const char *post_req = "POST /submit HTTP/1.1\r\nContent-Length: 5\r\n\r\nhello";
  http_parser_execute(&parser, &settings, post_req, strlen(post_req));
  CHECK_EQ(_td_on_body_called, 5);
  CHECK_EQ(_td_on_complete_called, 1);
}

TEST_CASE("incremental byte-by-byte parsing") {
  http_parser_t parser;
  _td_on_request_line_called = _td_on_headers_complete_called = _td_on_body_called = _td_on_complete_called = 0;
  http_parser_settings_t settings = {_td_on_request_line, NULL, _td_on_body, _td_on_headers_complete, _td_on_complete};
  const char *req = "GET /incr HTTP/1.1\r\nHost: x\r\n\r\n";
  http_parser_init(&parser);
  for (size_t i = 0; i < strlen(req); i++) {
    size_t n = http_parser_execute(&parser, &settings, req + i, 1);
    CHECK(n <= 1);
  }
  CHECK_EQ(_td_on_request_line_called, 1);
  CHECK_EQ(_td_on_headers_complete_called, 1);
  CHECK_EQ(_td_on_complete_called, 1);
}

TEST_CASE("header line overflow detected") {
  http_parser_t parser;
  _td_on_request_line_called = _td_on_headers_complete_called = _td_on_body_called = _td_on_complete_called = 0;
  http_parser_settings_t settings = {_td_on_request_line, NULL, _td_on_body, _td_on_headers_complete, _td_on_complete};
  http_parser_init(&parser);
  size_t overflow_len = MAX_HEADER_LINE_LEN + 100;
  char *req = (char *)malloc(overflow_len + 128);
  strcpy(req, "GET /big HTTP/1.1\r\n");
  strcat(req, "X-Long-Header: ");
  size_t prefix_len = strlen(req);
  for (size_t i = 0; i < overflow_len; i++) req[prefix_len + i] = 'A';
  req[prefix_len + overflow_len] = '\0';
  strcat(req, "\r\n\r\n");
  size_t total_len = strlen(req);
  size_t consumed = http_parser_execute(&parser, &settings, req, total_len);
  CHECK(_td_on_headers_complete_called == 0);
  CHECK(_td_on_complete_called == 0);
  CHECK(consumed < total_len);
  free(req);
}

} // TEST_SUITE(parser)
