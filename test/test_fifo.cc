#include "core/fifo.h"
#include "test_helper.h"

#define TEST_NAME esim_fifo_test

class TEST_NAME : public testing::Test {

};

TEST_F(TEST_NAME, empty) {
  esim::core::fifo<int> q;
  EXPECT_TRUE(q.empty());
}

TEST_F(TEST_NAME, try_push) {
  esim::core::fifo<int> q;

  EXPECT_TRUE(q.try_push(10));
  EXPECT_FALSE(q.empty());
}

TEST_F(TEST_NAME, push) {
  esim::core::fifo<int> q;
  q.push(10);
  EXPECT_FALSE(q.empty());
}

TEST_F(TEST_NAME, try_emplace) {
  esim::core::fifo<int> q;

  EXPECT_TRUE(q.try_emplace(10));
  EXPECT_FALSE(q.empty());
}

TEST_F(TEST_NAME, emplace) {
  esim::core::fifo<int> q;
  q.emplace(10);
  EXPECT_FALSE(q.empty());
}

TEST_F(TEST_NAME, try_pop) {
  auto rng = esim_test::gen_testcase();
  esim::core::fifo<int> q;
  int x = esim_test::random(rng, 1, 1000),
      act = 0;

  q.push(x);
  EXPECT_TRUE(q.try_pop(act));
  EXPECT_TRUE(q.empty());
  EXPECT_EQ(act, x);
}

TEST_F(TEST_NAME, pop) {
  auto rng = esim_test::gen_testcase();
  esim::core::fifo<int> q;
  int x = esim_test::random(rng, 1, 1000),
      act = 0;

  q.push(x);
  q.pop(act);
  EXPECT_TRUE(q.empty());
  EXPECT_EQ(act, x);
}

TEST_F(TEST_NAME, push_pop) {
  auto rng = esim_test::gen_testcase();
  std::vector<int> expect, actual;
  esim::core::fifo<int> q;

  for (size_t i = 0; i < 100; ++i) {
    int x = esim_test::random(rng, 0, 2000);
    q.push(x);
    expect.emplace_back(x);
  }

  int x;
  while (q.try_pop(x)) {
    actual.emplace_back(x);
  }
  EXPECT_TRUE(q.empty());
  EXPECT_EQ(actual, expect);
}