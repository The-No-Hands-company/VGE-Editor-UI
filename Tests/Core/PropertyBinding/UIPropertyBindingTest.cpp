#include "Property/UIPropertyBinding.h"
#include "Property/UIPropertyReflection.h"
#include <gtest/gtest.h>
#include <string>

using namespace VGE::UI;

// Test classes
class TestSource {
public:
    int GetIntValue() const { return m_IntValue; }
    void SetIntValue(int value) { m_IntValue = value; }
    
    float GetFloatValue() const { return m_FloatValue; }
    void SetFloatValue(float value) { m_FloatValue = value; }
    
    std::string GetStringValue() const { return m_StringValue; }
    void SetStringValue(const std::string& value) { m_StringValue = value; }

private:
    int m_IntValue = 0;
    float m_FloatValue = 0.0f;
    std::string m_StringValue;
};

class TestTarget {
public:
    int GetIntValue() const { return m_IntValue; }
    void SetIntValue(int value) { m_IntValue = value; }
    
    float GetFloatValue() const { return m_FloatValue; }
    void SetFloatValue(float value) { m_FloatValue = value; }
    
    std::string GetStringValue() const { return m_StringValue; }
    void SetStringValue(const std::string& value) { m_StringValue = value; }

private:
    int m_IntValue = 0;
    float m_FloatValue = 0.0f;
    std::string m_StringValue;
};

// Register test classes with reflection system
REGISTER_TYPE(TestSource)
    .Property("IntValue", &TestSource::GetIntValue, &TestSource::SetIntValue)
    .Property("FloatValue", &TestSource::GetFloatValue, &TestSource::SetFloatValue)
    .Property("StringValue", &TestSource::GetStringValue, &TestSource::SetStringValue);

REGISTER_TYPE(TestTarget)
    .Property("IntValue", &TestTarget::GetIntValue, &TestTarget::SetIntValue)
    .Property("FloatValue", &TestTarget::GetFloatValue, &TestTarget::SetFloatValue)
    .Property("StringValue", &TestTarget::GetStringValue, &TestTarget::SetStringValue);

class UIPropertyBindingTest : public ::testing::Test {
protected:
    void SetUp() override {
        GetBindingManager().ClearAllBindings();
    }
    
    void TearDown() override {
        GetBindingManager().ClearAllBindings();
    }
};

// Test one-way binding
TEST_F(UIPropertyBindingTest, OneWayBinding) {
    TestSource source;
    TestTarget target;
    
    // Create binding
    Bind(&source, "IntValue", &target, "IntValue");
    
    // Test initial update
    source.SetIntValue(42);
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(target.GetIntValue(), 42);
    
    // Test value change
    source.SetIntValue(123);
    GetBindingManager().NotifyPropertyChanged(&source, "IntValue");
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(target.GetIntValue(), 123);
    
    // Target changes should not affect source
    target.SetIntValue(456);
    GetBindingManager().NotifyPropertyChanged(&target, "IntValue");
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(source.GetIntValue(), 123);
}

// Test two-way binding
TEST_F(UIPropertyBindingTest, TwoWayBinding) {
    TestSource source;
    TestTarget target;
    
    // Create binding
    Bind(&source, "IntValue", &target, "IntValue", BindingMode::TwoWay);
    
    // Test source to target
    source.SetIntValue(42);
    GetBindingManager().NotifyPropertyChanged(&source, "IntValue");
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(target.GetIntValue(), 42);
    
    // Test target to source
    target.SetIntValue(123);
    GetBindingManager().NotifyPropertyChanged(&target, "IntValue");
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(source.GetIntValue(), 123);
}

// Test expression binding
TEST_F(UIPropertyBindingTest, ExpressionBinding) {
    TestTarget target;
    
    // Create expression binding that always returns 42
    auto expression = std::make_shared<UIPropertyExpression>(
        "42",
        [](const std::unordered_map<std::string, std::any>&) -> std::any {
            return 42;
        }
    );
    
    BindExpression(&target, "IntValue", expression);
    
    // Test initial update
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(target.GetIntValue(), 42);
}

// Test value conversion
TEST_F(UIPropertyBindingTest, ValueConversion) {
    TestSource source;
    TestTarget target;
    
    // Register int to string converter
    auto converter = std::make_shared<UIPropertyExpression>(
        "toString(value)",
        [](const std::unordered_map<std::string, std::any>& vars) -> std::any {
            return std::to_string(std::any_cast<int>(vars.at("value")));
        }
    );
    
    GetBindingManager().RegisterConverter(
        std::type_index(typeid(int)),
        std::type_index(typeid(std::string)),
        converter
    );
    
    // Create binding from int to string
    Bind(&source, "IntValue", &target, "StringValue");
    
    // Test conversion
    source.SetIntValue(42);
    GetBindingManager().NotifyPropertyChanged(&source, "IntValue");
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(target.GetStringValue(), "42");
}

// Test value validation
TEST_F(UIPropertyBindingTest, ValueValidation) {
    TestSource source;
    TestTarget target;
    
    // Register validator that only allows positive numbers
    auto validator = std::make_shared<UIPropertyExpression>(
        "value > 0",
        [](const std::unordered_map<std::string, std::any>& vars) -> std::any {
            return std::any_cast<int>(vars.at("value")) > 0;
        }
    );
    
    GetBindingManager().RegisterValidator(std::type_index(typeid(int)), validator);
    
    // Create binding
    Bind(&source, "IntValue", &target, "IntValue");
    
    // Test valid value
    source.SetIntValue(42);
    GetBindingManager().NotifyPropertyChanged(&source, "IntValue");
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(target.GetIntValue(), 42);
    
    // Test invalid value
    source.SetIntValue(-1);
    GetBindingManager().NotifyPropertyChanged(&source, "IntValue");
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(target.GetIntValue(), 42); // Should not change
}

// Test binding removal
TEST_F(UIPropertyBindingTest, BindingRemoval) {
    TestSource source;
    TestTarget target;
    
    // Create binding
    Bind(&source, "IntValue", &target, "IntValue");
    
    // Test initial update
    source.SetIntValue(42);
    GetBindingManager().NotifyPropertyChanged(&source, "IntValue");
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(target.GetIntValue(), 42);
    
    // Remove binding
    GetBindingManager().RemoveBinding(&source, "IntValue", &target, "IntValue");
    
    // Changes should no longer propagate
    source.SetIntValue(123);
    GetBindingManager().NotifyPropertyChanged(&source, "IntValue");
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(target.GetIntValue(), 42);
}

// Test multiple bindings
TEST_F(UIPropertyBindingTest, MultipleBindings) {
    TestSource source;
    TestTarget target1;
    TestTarget target2;
    
    // Create bindings
    Bind(&source, "IntValue", &target1, "IntValue");
    Bind(&source, "IntValue", &target2, "IntValue");
    
    // Test updates to both targets
    source.SetIntValue(42);
    GetBindingManager().NotifyPropertyChanged(&source, "IntValue");
    GetBindingManager().UpdateBindings();
    EXPECT_EQ(target1.GetIntValue(), 42);
    EXPECT_EQ(target2.GetIntValue(), 42);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 