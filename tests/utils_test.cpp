#include <doctest/doctest.h>
#include <cstring>
#include <ctime>

extern "C" {
#include "http.h"
}

TEST_SUITE("utils") {

TEST_CASE("strdup_safe returns NULL for NULL and duplicates string") {
  const char *s = "hello";
  char *d = strdup_safe(s);
  CHECK(d != nullptr);
  CHECK(std::strcmp(d, s) == 0);
  free(d);

  char *n = strdup_safe(nullptr);
  CHECK(n == nullptr);
}

TEST_CASE("str_trim") {
  char s[] = "   abc  \t\n";
  str_trim(s);
  CHECK(std::strcmp(s, "abc") == 0);
}

TEST_CASE("path_normalize") {
  char dst[1024];
  path_normalize(dst, "/foo/bar/../baz", sizeof(dst));
  CHECK(std::strcmp(dst, "/foo/baz") == 0);
  path_normalize(dst, "/foo/./bar", sizeof(dst));
  CHECK(std::strcmp(dst, "/foo/bar") == 0);
  path_normalize(dst, "/../../etc/passwd", sizeof(dst));
  CHECK(std::strcmp(dst, "/etc/passwd") == 0);
  path_normalize(dst, "////foo//bar/", sizeof(dst));
  CHECK(std::strcmp(dst, "/foo/bar") == 0);
}

TEST_CASE("url_decode") {
  char dst[1024];
  url_decode(dst, "hello%20world", sizeof(dst));
  CHECK(std::strcmp(dst, "hello world") == 0);
  url_decode(dst, "foo+bar", sizeof(dst));
  CHECK(std::strcmp(dst, "foo bar") == 0);
}

TEST_CASE("path_is_safe") {
  CHECK(path_is_safe("foo/bar.txt"));
  CHECK(!path_is_safe("/etc/passwd"));
  CHECK(!path_is_safe("foo/../bar"));
  CHECK(!path_is_safe("C:/Windows"));
}

TEST_CASE("mime_type_from_path") {
  CHECK(std::strcmp(mime_type_from_path("index.html"), "text/html") == 0);
  CHECK(std::strcmp(mime_type_from_path("styles.css"), "text/css") == 0);
  CHECK(std::strcmp(mime_type_from_path("script.js"), "application/javascript") == 0);
  CHECK(std::strcmp(mime_type_from_path("data.json"), "application/json") == 0);
  CHECK(std::strcmp(mime_type_from_path("file.unknown"), "application/octet-stream") == 0);
}

TEST_CASE("http_date roundtrip") {
  struct tm tm = {0};
  tm.tm_year = 1994 - 1900;
  tm.tm_mon = 10;
  tm.tm_mday = 6;
  tm.tm_hour = 8;
  tm.tm_min = 49;
  tm.tm_sec = 37;
#if PLATFORM_WINDOWS
  time_t t = _mkgmtime(&tm);
#else
  time_t t = timegm(&tm);
#endif
  char buf[64];
  CHECK_EQ(http_format_date(buf, sizeof(buf), t), 0);
  CHECK(std::strcmp(buf, "Sun, 06 Nov 1994 08:49:37 GMT") == 0);
  time_t parsed = http_parse_date(buf);
  CHECK(parsed != (time_t)-1);
  CHECK_EQ(parsed, t);
}

TEST_CASE("http_method conversion") {
  CHECK_EQ(http_method_from_string("GET"), HTTP_METHOD_GET);
  CHECK_EQ(http_method_from_string("POST"), HTTP_METHOD_POST);
  CHECK_EQ(http_method_from_string("UNKNOWN"), HTTP_METHOD_UNKNOWN);
  CHECK(std::strcmp(http_method_to_string(HTTP_METHOD_GET), "GET") == 0);
}

} // TEST_SUITE("utils")
