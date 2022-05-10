#include "core/observer.h"
#include "core/publisher.h"
#include "test_helper.h"

#define TEST_NAME esim_subject_observer_test

class TEST_NAME : public testing::Test {
public:
  class subject_impl : public esim::core::publisher {
  public:
    inline void notify_test(int data) noexcept {
      notify(&data);
    }
  };
  
  class observer_impl : public esim::core::observer {
  public:
    inline void update(esim::rptr<void> msg) noexcept final {
      data = *static_cast<int*>(msg);
    }

    int data;
  };
};

TEST_F(TEST_NAME, subscribe) {
  auto rng = esim_test::gen_testcase();
  subject_impl subj;
  observer_impl obsv;
  int data = esim_test::random(rng, 0, 10000);

  obsv.subscribe(&subj);
  obsv.data = 0;

  subj.notify_test(data);
  EXPECT_EQ(obsv.data, data);
}

TEST_F(TEST_NAME, subscribe_multiple_observer) {
  auto rng = esim_test::gen_testcase();
  subject_impl subj;
  observer_impl obsv1, obsv2;
  int data = esim_test::random(rng, 0, 10000);

  obsv1.subscribe(&subj);
  obsv2.subscribe(&subj);
  obsv1.data = 0;
  obsv2.data = 0;

  subj.notify_test(data);
  EXPECT_EQ(obsv1.data, data);
  EXPECT_EQ(obsv2.data, data);
}