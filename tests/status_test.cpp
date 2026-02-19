#include <doctest/doctest.h>

extern "C" {
#include "http.h"
}

TEST_SUITE("status") {

TEST_CASE("reasons and default class") {
  CHECK(std::strcmp(http_status_reason(200), "OK") == 0);
  CHECK(std::strcmp(http_status_reason(404), "Not Found") == 0);
  CHECK(std::strcmp(http_status_reason(500), "Internal Server Error") == 0);
  CHECK(std::strcmp(http_status_reason(301), "Moved Permanently") == 0);
  CHECK(std::strcmp(http_status_reason(450), "Client Error") == 0);
  CHECK(std::strcmp(http_status_reason(299), "Success") == 0);
  CHECK(std::strcmp(http_status_reason(700), "Unknown") == 0);
}

} // TEST_SUITE(status)
