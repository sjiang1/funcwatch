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

TEST(AppendTest, AbcString){
  DynString dynString;
  dynstring_init(&dynString);
  dynstring_append(&dynString, "abc");
  
  int len = strlen(dynString.text);
  EXPECT_EQ(3, len);
  EXPECT_EQ(4, dynString.size);
  EXPECT_EQ(STRING_INITIAL_CAPACITY, dynString.capacity);
  dynstring_inner_free(dynString);
}

TEST(AppendTest, TwoAppends){
  DynString dynString;
  dynstring_init(&dynString);
  dynstring_append(&dynString, "abc");
  dynstring_append(&dynString, "efgh");
  
  int len = strlen(dynString.text);
  EXPECT_EQ(7, len);
  EXPECT_EQ(8, dynString.size);
  EXPECT_EQ(STRING_INITIAL_CAPACITY, dynString.capacity);
  dynstring_inner_free(dynString);
}

TEST(AppendTest, ExceedsInitialCapacity){
  DynString dynString;
  dynstring_init(&dynString);
  int expectedLength = strlen("abcdefghijklmnopqrstuvwxyz");
  dynstring_append(&dynString, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
  
  int len = strlen(dynString.text);
  EXPECT_EQ(expectedLength*5, len);
  EXPECT_EQ(expectedLength*5+1, dynString.size);
  EXPECT_EQ(STRING_INITIAL_CAPACITY*2, dynString.capacity);
  dynstring_inner_free(dynString);
}

TEST(AppendTest, ExceedsInitialCapacityTwoAppends){
  DynString dynString;
  dynstring_init(&dynString);
  int expectedLength = strlen("abcdefghijklmnopqrstuvwxyz");
  dynstring_append(&dynString, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
  dynstring_append(&dynString, "abcdefghijklmnopqrstuvwxyz");
  
  int len = strlen(dynString.text);
  EXPECT_EQ(expectedLength*4, len);
  EXPECT_EQ(expectedLength*4+1, dynString.size);
  EXPECT_EQ(STRING_INITIAL_CAPACITY*2, dynString.capacity);
  dynstring_inner_free(dynString);
}
