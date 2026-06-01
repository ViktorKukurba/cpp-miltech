#include <gtest/gtest.h>
#include "ballistics.hpp"

const char* GOOD_FILE = "data/sample_vog17.txt";
const char* BAD_FILE = "data/unknown_ammo.txt";

TEST(BallisticsTest, getDataGood)
{
  Data data = getData(GOOD_FILE);
  EXPECT_STREQ(data.ammoName, "VOG-17");
}

TEST(BallisticsTest, getDataBad)
{
  EXPECT_THROW(getData(BAD_FILE), std::runtime_error);
}
