#include <doctest/doctest.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

extern "C" {
#include "http.h"
}

TEST_SUITE("server") {

TEST_CASE("integration (light)") {
  // This mirrors some of test_server.c but avoids creating many sockets to
  // reduce flakiness in CI. It performs a few file operations and checks
  // HTTP helpers.
  char *dup = strdup_safe("test");
  CHECK(std::strcmp(dup, "test") == 0);
  free(dup);
  mem_pool_t pool; mem_pool_init(&pool, 5);
  void *p1 = mem_pool_alloc(&pool);
  CHECK(p1 != NULL);
  mem_pool_free(&pool, p1);

  CHECK_EQ(http_method_from_string("PATCH"), HTTP_METHOD_PATCH);
  CHECK(std::strcmp(http_method_to_string(HTTP_METHOD_PATCH), "PATCH") == 0);

  file_content_t fc;
  FILE *ft = fopen("t.txt", "w"); fputc('x', ft); fclose(ft);
  CHECK_EQ(file_read("t.txt", &fc), 0);
  file_free(&fc);
  unlink("t.txt");

  // HEAD method and file send tests are environment-dependent (sockets),
  // but we invoke some http_conn_xxx functions to ensure API surface compiles
  http_conn_t conn; http_conn_init(&conn);
  http_conn_close(&conn);
}

} // TEST_SUITE(server)
