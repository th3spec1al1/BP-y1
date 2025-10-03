#include <lib/interpreter.h>
#include <gtest/gtest.h>

TEST(StandartTest, IntTest1) {
    std::string code = R"(
        x = .03
        y = 0.5e-4
        z = 3
        print(x+y+z)
    )";
    std::string expected = "3.03005";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(StandartTest, ListTest1) {
    std::string code = R"(
        a = [1, 2, 3]
        b = pop(a)
        println(b)
        println(a)
    )";
    std::string expected = "3\n[1, 2]\n";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(StandartTest, ListTest2) {
    std::string code = R"(
        a = [1, 2, 3]
        println(len(a, a))
    )";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_FALSE(interpret(input, output));
}

TEST(StandartTest, ListTest6) {
    std::string code = R"(
        a = "aHa"
        println(upper(a))
    )";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    std::cout << output.str() << "\n";
}


TEST(StandartTest, ListTest3) {
    std::string code = R"(
        a = [1, 2, "ahaha"]
        sort(a)
        println(a)
    )";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
}

TEST(StandartTest, ListTest4) {
    std::string code = R"(
        a = [1, 2, "ahaha"]
        a[0] = [3,5,6]
        print(a[0][1])
    )";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ("5", output.str());
}
