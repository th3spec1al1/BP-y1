#include <lib/interpreter.h>
#include <gtest/gtest.h>

TEST(NumFunctionTestSuite, CorrectAbsTest) {
    std::string code = R"(
        a = -5
        print(abs(a))
    )";

    std::string expected = "5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumFunctionTestSuite, IncorrectAbsTest) {
    std::string code = R"(
        a = -5
        print(abs(a, a))
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}

TEST(NumFunctionTestSuite, CorrectCeilTest) {
    std::string code = R"(
        a = 4.2
        print(ceil(a))
    )";

    std::string expected = "5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumFunctionTestSuite, IncorrectCeilTest) {
    std::string code = R"(
        a = 4.2
        print(ceil(a, a))
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}

TEST(NumFunctionTestSuite, CorrectFloorTest) {
    std::string code = R"(
        a = 4.7
        print(floor(a))
    )";

    std::string expected = "4";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumFunctionTestSuite, IncorrectFloorTest) {
    std::string code = R"(
        a = 4.7
        print(floor(a, a))
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}

TEST(NumFunctionTestSuite, CorrectRoundTest) {
    std::string code = R"(
        a = 4.7
        print(round(a))
    )";

    std::string expected = "5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumFunctionTestSuite, IncorrectRoundTest) {
    std::string code = R"(
        a = 4.7
        print(round(a, a))
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}

TEST(NumFunctionTestSuite, CorrectSqrtTest) {
    std::string code = R"(
        a = 9
        print(sqrt(a))
    )";

    std::string expected = "3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumFunctionTestSuite, IncorrectSqrtTest) {
    std::string code = R"(
        a = 9
        print(sqrt(a, a))
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}

TEST(NumFunctionTestSuite, CorrectRndTest) {
    std::string code = R"(
        a = 6
        print(rnd(a))
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
}

TEST(NumFunctionTestSuite, IncorrectRndAbsTest) {
    std::string code = R"(
        a = 7
        print(rnd(a, a))
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}

TEST(NumFunctionTestSuite, CorrectParseNumTest) {
    std::string code = R"(
        a = "-5"
        print(parse_num(a) + 2)
    )";

    std::string expected = "-3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumFunctionTestSuite, IncorrectParseNumTest) {
    std::string code = R"(
        a = "-5"
        print(parse_num(a, a))
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}

TEST(NumFunctionTestSuite, CorrectToStringTest) {
    std::string code = R"(
        a = -5
        print(to_string(a) * 2)
    )";

    std::string expected = "-5-5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumFunctionTestSuite, IncorrectToStringTest) {
    std::string code = R"(
        a = -5
        print(to_string(a, a))
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}