#include "../dynstring.h"
#include "gtest/gtest.h"

TEST(InitTest, EmptyString){
  DynString dynString;
  dynstring_init(&dynString);
  int len = strlen(dynString.text);
  EXPECT_EQ(0, len);
}
