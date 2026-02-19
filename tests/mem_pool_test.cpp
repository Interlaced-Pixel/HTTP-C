#include <doctest/doctest.h>

extern "C" {
#include "http.h"
}

TEST_SUITE("mem_pool") {

TEST_CASE("allocation") {
  mem_pool_t pool;
  mem_pool_init(&pool, 1024);
  void *p1 = mem_pool_alloc(&pool);
  CHECK(p1 != nullptr);
  void *p2 = mem_pool_alloc(&pool);
  CHECK(p2 != nullptr);
  CHECK(p1 != p2);
  mem_pool_free(&pool, p1);
  void *p3 = mem_pool_alloc(&pool);
  CHECK(p3 == p1);
  mem_pool_free(&pool, p2);
  mem_pool_free(&pool, p3);
}

} // TEST_SUITE(mem_pool)
