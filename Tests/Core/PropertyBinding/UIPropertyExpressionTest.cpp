#include "Property/UIPropertyExpression.h"
#include <gtest/gtest.h>
#include <string>

using namespace VGE::UI;

class UIPropertyExpressionTest : public ::testing::Test {
protected:
    UIExpressionParser parser;
    std::unordered_map<std::string, std::any> variables;
};

// Test literal values
TEST_F(UIPropertyExpressionTest, LiteralValues) {
    // Integer
    auto node = parser.Parse("42");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 42);
    
    // Float
    node = parser.Parse("3.14");
    ASSERT_TRUE(node != nullptr);
    EXPECT_FLOAT_EQ(std::any_cast<float>(node->Evaluate(variables)), 3.14f);
    
    // String
    node = parser.Parse("'hello'");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<std::string>(node->Evaluate(variables)), "hello");
}

// Test arithmetic operators
TEST_F(UIPropertyExpressionTest, ArithmeticOperators) {
    // Addition
    auto node = parser.Parse("2 + 3");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 5);
    
    // Subtraction
    node = parser.Parse("5 - 3");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 2);
    
    // Multiplication
    node = parser.Parse("4 * 3");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 12);
    
    // Division
    node = parser.Parse("10 / 2");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 5);
    
    // Complex expression
    node = parser.Parse("2 + 3 * 4");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 14);
    
    // Parentheses
    node = parser.Parse("(2 + 3) * 4");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 20);
}

// Test comparison operators
TEST_F(UIPropertyExpressionTest, ComparisonOperators) {
    // Equal
    auto node = parser.Parse("2 == 2");
    ASSERT_TRUE(node != nullptr);
    EXPECT_TRUE(std::any_cast<bool>(node->Evaluate(variables)));
    
    // Not equal
    node = parser.Parse("2 != 3");
    ASSERT_TRUE(node != nullptr);
    EXPECT_TRUE(std::any_cast<bool>(node->Evaluate(variables)));
    
    // Less than
    node = parser.Parse("2 < 3");
    ASSERT_TRUE(node != nullptr);
    EXPECT_TRUE(std::any_cast<bool>(node->Evaluate(variables)));
    
    // Greater than
    node = parser.Parse("3 > 2");
    ASSERT_TRUE(node != nullptr);
    EXPECT_TRUE(std::any_cast<bool>(node->Evaluate(variables)));
    
    // Less than or equal
    node = parser.Parse("2 <= 2");
    ASSERT_TRUE(node != nullptr);
    EXPECT_TRUE(std::any_cast<bool>(node->Evaluate(variables)));
    
    // Greater than or equal
    node = parser.Parse("3 >= 3");
    ASSERT_TRUE(node != nullptr);
    EXPECT_TRUE(std::any_cast<bool>(node->Evaluate(variables)));
}

// Test logical operators
TEST_F(UIPropertyExpressionTest, LogicalOperators) {
    // AND
    auto node = parser.Parse("true && true");
    ASSERT_TRUE(node != nullptr);
    EXPECT_TRUE(std::any_cast<bool>(node->Evaluate(variables)));
    
    node = parser.Parse("true && false");
    ASSERT_TRUE(node != nullptr);
    EXPECT_FALSE(std::any_cast<bool>(node->Evaluate(variables)));
    
    // OR
    node = parser.Parse("true || false");
    ASSERT_TRUE(node != nullptr);
    EXPECT_TRUE(std::any_cast<bool>(node->Evaluate(variables)));
    
    node = parser.Parse("false || false");
    ASSERT_TRUE(node != nullptr);
    EXPECT_FALSE(std::any_cast<bool>(node->Evaluate(variables)));
    
    // NOT
    node = parser.Parse("!true");
    ASSERT_TRUE(node != nullptr);
    EXPECT_FALSE(std::any_cast<bool>(node->Evaluate(variables)));
    
    // Complex expression
    node = parser.Parse("(true && !false) || (false && true)");
    ASSERT_TRUE(node != nullptr);
    EXPECT_TRUE(std::any_cast<bool>(node->Evaluate(variables)));
}

// Test variables
TEST_F(UIPropertyExpressionTest, Variables) {
    variables["x"] = 42;
    variables["y"] = 3.14f;
    variables["name"] = std::string("test");
    variables["flag"] = true;
    
    // Integer variable
    auto node = parser.Parse("x");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 42);
    
    // Float variable
    node = parser.Parse("y");
    ASSERT_TRUE(node != nullptr);
    EXPECT_FLOAT_EQ(std::any_cast<float>(node->Evaluate(variables)), 3.14f);
    
    // String variable
    node = parser.Parse("name");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<std::string>(node->Evaluate(variables)), "test");
    
    // Boolean variable
    node = parser.Parse("flag");
    ASSERT_TRUE(node != nullptr);
    EXPECT_TRUE(std::any_cast<bool>(node->Evaluate(variables)));
    
    // Expression with variables
    node = parser.Parse("x + 10");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 52);
}

// Test functions
TEST_F(UIPropertyExpressionTest, Functions) {
    // min function
    auto node = parser.Parse("min(5, 3)");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 3);
    
    // max function
    node = parser.Parse("max(5, 3)");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 5);
    
    // clamp function
    node = parser.Parse("clamp(10, 0, 5)");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 5);
    
    // Function with variables
    variables["x"] = 10;
    node = parser.Parse("clamp(x, 0, 5)");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 5);
    
    // Custom function
    parser.RegisterFunction("square", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() != 1) throw std::runtime_error("square requires 1 argument");
        int value = std::any_cast<int>(args[0]);
        return value * value;
    });
    
    node = parser.Parse("square(4)");
    ASSERT_TRUE(node != nullptr);
    EXPECT_EQ(std::any_cast<int>(node->Evaluate(variables)), 16);
}

// Test error handling
TEST_F(UIPropertyExpressionTest, ErrorHandling) {
    // Invalid syntax
    auto node = parser.Parse("2 +");
    ASSERT_TRUE(node == nullptr);
    EXPECT_FALSE(parser.GetError().empty());
    
    // Unknown variable
    node = parser.Parse("unknown_var");
    ASSERT_TRUE(node != nullptr);
    EXPECT_THROW(node->Evaluate(variables), std::runtime_error);
    
    // Unknown function
    node = parser.Parse("unknown_func()");
    ASSERT_TRUE(node != nullptr);
    EXPECT_THROW(node->Evaluate(variables), std::runtime_error);
    
    // Type mismatch
    variables["str"] = std::string("test");
    node = parser.Parse("str + 42");
    ASSERT_TRUE(node != nullptr);
    EXPECT_THROW(node->Evaluate(variables), std::runtime_error);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 