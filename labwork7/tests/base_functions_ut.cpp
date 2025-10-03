#include <unrolled_list.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <list>

TEST(BaseFunctions, EmptyTest1) {
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 10; ++i) {
        unrolled_list.push_back(i);
    }

    ASSERT_FALSE(unrolled_list.empty());
    ASSERT_EQ(unrolled_list.size(), 10);
}

TEST(BaseFunctions, EmptyTest2) {
    unrolled_list<int> unrolled_list;

    ASSERT_TRUE(unrolled_list.empty());
    ASSERT_EQ(unrolled_list.size(), 0);
}

TEST(BaseFunctions, ComparisonEqualsTest1) {
    unrolled_list<int> unrolled_list1;
    unrolled_list<int> unrolled_list2;

    for (int i = 0; i < 10; ++i) {
        unrolled_list1.push_back(i);
        unrolled_list2.push_back(i);
    }

    ASSERT_TRUE(unrolled_list1 == unrolled_list2);
}

TEST(BaseFunctions, ComparisonEqualsTest2) {
    unrolled_list<int> unrolled_list1;
    unrolled_list<int> unrolled_list2;

    for (int i = 0; i < 10; ++i) {
        unrolled_list1.push_back(i);
        unrolled_list2.push_back(i / 2);
    }

    ASSERT_FALSE(unrolled_list1 == unrolled_list2);
}

TEST(BaseFunctions, ComparisonNotEqualTest) {
    unrolled_list<int> unrolled_list1;
    unrolled_list<int> unrolled_list2;

    for (int i = 0; i < 10; ++i) {
        unrolled_list1.push_back(i);
        unrolled_list2.push_back(i / 2);
    }

    ASSERT_TRUE(unrolled_list1 != unrolled_list2);
}

TEST(BaseFunctions, OperatorEqualTest) {
    unrolled_list<int> unrolled_list1;
    unrolled_list<int> unrolled_list2;

    for (int i = 0; i < 10; ++i) {
        unrolled_list1.push_back(i);
        unrolled_list2.push_back(i / 2);
    }

    unrolled_list2 = unrolled_list1;

    ASSERT_TRUE(unrolled_list1 == unrolled_list2);
}

TEST(BaseFunctions, AssertTest) {
    unrolled_list<int> unrolled_list1;
    unrolled_list<int> unrolled_list2;
    std::list<int> list1;

    for (int i = 0; i < 10; ++i) {
        unrolled_list1.push_back(i);
        unrolled_list2.push_back(i);
        list1.push_back(i / 2);
    }

    unrolled_list1.assign(list1.begin(), list1.end());

    ASSERT_TRUE(unrolled_list1 != unrolled_list2);
}

TEST(BaseFunctions, GetHeadTest) {
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 10; ++i) {
        unrolled_list.push_back(i);
    }

    auto head = unrolled_list.get_head();
    ASSERT_EQ(head->data_size_, 10);
}

TEST(BaseFunctions, GetTailTest) {
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 10; ++i) {
        unrolled_list.push_back(i);
    }

    auto tail = unrolled_list.get_tail();
    ASSERT_EQ(tail->data_size_, 10);

    unrolled_list.push_back(777);
    
    tail = unrolled_list.get_tail();
    ASSERT_EQ(tail->data_size_, 1);
}