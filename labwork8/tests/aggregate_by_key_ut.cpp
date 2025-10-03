#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

struct Employee {
    uint64_t department_id;
    std::string name;

    bool operator==(const Employee& other) const = default;
};

TEST(AggregateByKeyTest, CountingAggregatedValues) {
    std::vector<std::string> input = {"name4", "name0", "name1", "name0", "name2", "name0", "name1"};

    auto result =
        AsDataFlow(input)
            | AggregateByKey(
                std::size_t{0},
                [](std::size_t& accumulated, const std::string&) { ++accumulated; },
                [](const std::string& name) { return name; }  
            )
            | AsVector();

    std::vector<KV<std::string, std::size_t>> expected = {
        {"name4", 1},
        {"name0", 3},
        {"name1", 2},
        {"name2", 1}
    };
    
    std::sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.key < rhs.key;
    });

    std::sort(expected.begin(), expected.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.key < rhs.key;
    });

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_EQ(result[i].key, expected[i].key);
        EXPECT_EQ(result[i].value, expected[i].value);
    }
}

TEST(AggregateByKeyTest, AggregatingWithSeveralOutputsForEachKey) {
    std::vector<Employee> employees = {
        {3, "name1"},
        {1, "name1"},
        {1, "name2"},
        {2, "name1"},
        {2, "name2"},
        {2, "name3"}
    };

    auto result =
        AsDataFlow(employees)
            | AggregateByKey(
                std::vector<Employee>{},
                [](std::vector<Employee>& accumulated, const Employee& employee) {
                    if (accumulated.size() < 2) {
                        accumulated.push_back(employee);
                    }
                },
                [](const Employee& employee) { return employee.department_id; }
            )
            | AsVector();


    std::vector<KV<uint64_t, std::vector<Employee>>> expected = {
        {3, {{3, "name1"}}},
        {1, {{1, "name1"}, {1, "name2"}}},
        {2, {{2, "name1"}, {2, "name2"}}}
    };

    std::sort(result.begin(), result.end(), [](const KV<uint64_t, std::vector<Employee>>& a, const KV<uint64_t, std::vector<Employee>>& b) {
        return a.key < b.key;
    });

    std::sort(expected.begin(), expected.end(), [](const KV<uint64_t, std::vector<Employee>>& a, const KV<uint64_t, std::vector<Employee>>& b) {
        return a.key < b.key;
    });

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_EQ(result[i].key, expected[i].key);
        EXPECT_EQ(result[i].value, expected[i].value);
    }
}
