#include "../dynstring.h"
#include "gtest/gtest.h"

TEST(InitTest, EmptyString){
  DynString dynString;
  dynstring_init(&dynString);
  int len = strlen(dynString.text);
  EXPECT_EQ(0, len);
  EXPECT_EQ(1, dynString.size);
  EXPECT_EQ(STRING_INITIAL_CAPACITY, dynString.capacity);
  dynstring_inner_free(dynString);
}

TEST(AppendTest, EmptyString){
  DynString dynString;
  dynstring_init(&dynString);
  dynstring_append(&dynString, "");
  
  int len = strlen(dynString.text);
  EXPECT_EQ(0, len);
  EXPECT_EQ(1, dynString.size);
  EXPECT_EQ(STRING_INITIAL_CAPACITY, dynString.capacity);
  dynstring_inner_free(dynString);
}
