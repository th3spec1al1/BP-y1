#include "gtest/gtest.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include "../lib/Scheduler.h"

TEST(SchedulerTestSuite, Test1) {
    Scheduler::TTaskScheduler scheduler;
    auto id = scheduler.add([](float a, float c) { return (a * c / 7); }, 77, 7);
    ASSERT_EQ(scheduler.getResult<float>(id), 77);
}

TEST(SchedulerTestSuite, Test2) {
    Scheduler::TTaskScheduler scheduler;
    auto id1 = scheduler.add([](float a, float c) { return (a * c / 7); }, 77, 7);
    auto id2 = scheduler.add([](float a, float b) { return b * b + a; }, 777, scheduler.getFutureResult<float>(id1));
    ASSERT_EQ(scheduler.getResult<float>(id2), 6706);
}

TEST(SchedulerTestSuite, Test3) {
    Scheduler::TTaskScheduler scheduler;
    auto id = scheduler.add([](std::string a) { return a.size(); }, "CristianoRonaldo");
    ASSERT_EQ(scheduler.getResult<size_t>(id), 16);
}

TEST(SchedulerTestSuite, Test4) {
    Scheduler::TTaskScheduler scheduler;
    auto id = scheduler.add([](int a) { return std::vector<int>(7, a); }, 77);
    ASSERT_EQ(scheduler.getResult<std::vector<int>>(id), std::vector<int>({77, 77, 77, 77, 77, 77, 77}));
}

TEST(SchedulerTestSuite, Test5) {
    Scheduler::TTaskScheduler scheduler;
    auto id = scheduler.add([]() { return 3456 + 2345 - 1234; });
    ASSERT_EQ(scheduler.getResult<int>(id), 4567);
}

TEST(SchedulerTestSuite, Test6) {
    Scheduler::TTaskScheduler scheduler;
    auto id = scheduler.add([]() {
        std::string a = "CR";
        std::string b = "7";
        return a + b;
    });
    ASSERT_EQ(scheduler.getResult<std::string>(id), "CR7");
}

TEST(SchedulerTestSuite, Test7) {
    Scheduler::TTaskScheduler scheduler;
    float a = 1;
    float b = -2;
    float c = 0;
    auto id1 = scheduler.add([](float a, float c) { return (-4 * a * c); }, a, c);
    auto id2 = scheduler.add([](float b, float v) { return b * b + v; }, b, scheduler.getFutureResult<float>(id1));
    auto id3 = scheduler.add([](float b, float d) { return -b + std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
    auto id4 = scheduler.add([](float b, float d) { return -b - std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
    auto id5 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id3));
    auto id6 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id4));
    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<float>(id6), 0);
}

TEST(SchedulerTestSuite, Test8) {
    Scheduler::TTaskScheduler scheduler;
    auto id1 = scheduler.add([](int a, int c) { return a + c; }, 77, 7);
    auto id2 = scheduler.add([](int a) { return a * 777; }, scheduler.getFutureResult<int>(id1));
    auto id3 = scheduler.add([](){ return 4 + 6; });
    auto id4 = scheduler.add([](int a, int b) { return a + b; }, scheduler.getFutureResult<int>(id2), scheduler.getFutureResult<int>(id3));
    scheduler.executeAll();
    ASSERT_EQ(scheduler.getResult<int>(id4), 65278);
}

TEST(SomeTypeTestSuite, Test1) {
    Scheduler::SomeType any;
    any = 7;
    ASSERT_EQ(any.Cast<int>(), 7);
}

TEST(SomeTypeTestSuite, Test2) {
    Scheduler::SomeType any;
    std::string s = "HarryKane";
    any = s;
    ASSERT_EQ(any.Cast<std::string>(), "HarryKane");
}

TEST(SomeTypeTestSuite, Test3) {
    Scheduler::SomeType any_1;
    any_1 = 5;
    Scheduler::SomeType any_2 = any_1;
    ASSERT_EQ(any_2.Cast<int>(), 5);
}

TEST(SomeTypeTestSuite, Test4) {
    std::stringstream output;
    Scheduler::SomeType any;
    int a = 5;
    any = a;
    output << any.Cast<int>();
    float b = 2.0;
    any = b;
    output << any.Cast<float>();
    ASSERT_EQ(output.str(), "52");
}

TEST(SomeTypeTestSuite, Test5) {
    std::stringstream output;
    Scheduler::SomeType any;
    std::string a = "CR";
    any = a;
    output << any.Cast<std::string>();
    int b = 7;
    any = b;
    output << any.Cast<int>();
    ASSERT_EQ(output.str(), "CR7");
}
