/**
 * @file unit_test.cc
 * @author author
 * @copyright 2020 author
 */

#include <gtest/gtest.h>

#include "invport/detail/common.h"
#include "invport/detail/env.h"

namespace env = inv::env;

TEST(Env, GetHome)
{
  const auto get_res_success = env::GetEnv("HOME");
  EXPECT_NE(get_res_success.first, "");
  EXPECT_EQ(get_res_success.second, iex::ErrorCode());
}

TEST(Env, SetGetUnset)
{
  EXPECT_EQ(env::SetEnv("IEX_TEST", "TEST_VAR_VALUE"), "");

  const auto get_res_success = env::GetEnv("IEX_TEST");
  EXPECT_EQ(get_res_success.first, "TEST_VAR_VALUE");
  EXPECT_EQ(get_res_success.second, iex::ErrorCode());

  const auto unset_ec = env::UnsetEnv("IEX_TEST");
  EXPECT_EQ(unset_ec, iex::ErrorCode());

  const auto get_res_failure = env::GetEnv("IEX_TEST");
  EXPECT_EQ(get_res_failure.first, "");
  EXPECT_NE(get_res_failure.second, iex::ErrorCode());
}

TEST(Env, IllegalNamesAndValues)
{
  const auto get_res_empty = env::GetEnv("");
  EXPECT_EQ(get_res_empty.first, "");
  EXPECT_NE(get_res_empty.second, iex::ErrorCode());

  const auto get_res_equal_char = env::GetEnv("test_=_test");
  EXPECT_EQ(get_res_equal_char.first, "");
  EXPECT_NE(get_res_equal_char.second, iex::ErrorCode());

  EXPECT_NE(env::SetEnv("", "TEST_VAR_VALUE"), iex::ErrorCode());
  EXPECT_NE(env::SetEnv("TEST_VAR_NAME", ""), iex::ErrorCode());
  EXPECT_NE(env::SetEnv("", ""), iex::ErrorCode());

  EXPECT_NE(env::SetEnv("test_=_test", "TEST_VAR_VALUE"), iex::ErrorCode());
  EXPECT_NE(env::SetEnv("TEST_VAR_NAME", "test_=_test"), iex::ErrorCode());
  EXPECT_NE(env::SetEnv("test_=_test", "test_=_test"), iex::ErrorCode());

  EXPECT_NE(env::UnsetEnv(""), iex::ErrorCode());
  EXPECT_NE(env::UnsetEnv("test_=_test"), iex::ErrorCode());
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
