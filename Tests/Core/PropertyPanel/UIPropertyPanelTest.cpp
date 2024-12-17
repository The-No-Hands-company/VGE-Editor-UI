#include "Runtime/Core/Public/Widget/UIPropertyPanel.h"
#include "Runtime/Core/Public/Widget/UISceneHierarchy.h"
#include "Runtime/Core/Public/Core/Logger.h"
#include <gtest/gtest.h>

namespace VGE {
namespace UI {
namespace Test {

// Test object class
class TestObject {
public:
    bool boolValue = false;
    int32_t intValue = 42;
    float floatValue = 3.14f;
    std::string stringValue = "Hello";
    glm::vec2 vec2Value = glm::vec2(1.0f, 2.0f);
    glm::vec3 vec3Value = glm::vec3(1.0f, 2.0f, 3.0f);
    glm::vec4 vec4Value = glm::vec4(1.0f, 2.0f, 3.0f, 4.0f);
    std::vector<std::any> collectionValue;
};

class UIPropertyPanelTest : public testing::Test {
protected:
    void SetUp() override {
        m_Panel = std::make_unique<UIPropertyPanel>();
        m_TestObject = std::make_unique<TestObject>();
        
        // Register properties
        PropertyMetadata boolMeta{
            "Boolean Value",
            "A test boolean value",
            "Test",
            false,
            false,
            "",
            PropertyValue(false)
        };
        m_Panel->RegisterProperty("boolValue", boolMeta);
        
        PropertyMetadata intMeta{
            "Integer Value",
            "A test integer value",
            "Test",
            false,
            false,
            "",
            PropertyValue(0)
        };
        m_Panel->RegisterProperty("intValue", intMeta);
        
        PropertyMetadata floatMeta{
            "Float Value",
            "A test float value",
            "Test",
            false,
            false,
            "units",
            PropertyValue(0.0f)
        };
        m_Panel->RegisterProperty("floatValue", floatMeta);
        
        PropertyMetadata stringMeta{
            "String Value",
            "A test string value",
            "Test",
            false,
            false,
            "",
            PropertyValue(std::string())
        };
        m_Panel->RegisterProperty("stringValue", stringMeta);
        
        PropertyMetadata vec2Meta{
            "Vector2 Value",
            "A test 2D vector value",
            "Test",
            false,
            false,
            "",
            PropertyValue(glm::vec2(0.0f))
        };
        m_Panel->RegisterProperty("vec2Value", vec2Meta);
        
        PropertyMetadata vec3Meta{
            "Vector3 Value",
            "A test 3D vector value",
            "Test",
            false,
            false,
            "",
            PropertyValue(glm::vec3(0.0f))
        };
        m_Panel->RegisterProperty("vec3Value", vec3Meta);
        
        PropertyMetadata vec4Meta{
            "Vector4 Value",
            "A test 4D vector value",
            "Test",
            false,
            false,
            "",
            PropertyValue(glm::vec4(0.0f))
        };
        m_Panel->RegisterProperty("vec4Value", vec4Meta);
        
        // Set target object
        m_Panel->SetTarget(m_TestObject.get(), "TestObject");
    }
    
    void TearDown() override {
        m_Panel.reset();
        m_TestObject.reset();
    }
    
protected:
    std::unique_ptr<UIPropertyPanel> m_Panel;
    std::unique_ptr<TestObject> m_TestObject;
};

TEST_F(UIPropertyPanelTest, PropertyRegistration) {
    EXPECT_TRUE(m_Panel->GetPropertyValue("boolValue").index() == 0);
    EXPECT_TRUE(m_Panel->GetPropertyValue("intValue").index() == 1);
    EXPECT_TRUE(m_Panel->GetPropertyValue("floatValue").index() == 2);
    EXPECT_TRUE(m_Panel->GetPropertyValue("stringValue").index() == 3);
    EXPECT_TRUE(m_Panel->GetPropertyValue("vec2Value").index() == 4);
    EXPECT_TRUE(m_Panel->GetPropertyValue("vec3Value").index() == 5);
    EXPECT_TRUE(m_Panel->GetPropertyValue("vec4Value").index() == 6);
}

TEST_F(UIPropertyPanelTest, PropertyGetSet) {
    // Test boolean
    m_Panel->SetPropertyValue("boolValue", PropertyValue(true));
    EXPECT_TRUE(std::get<bool>(m_Panel->GetPropertyValue("boolValue")));
    
    // Test integer
    m_Panel->SetPropertyValue("intValue", PropertyValue(123));
    EXPECT_EQ(std::get<int32_t>(m_Panel->GetPropertyValue("intValue")), 123);
    
    // Test float
    m_Panel->SetPropertyValue("floatValue", PropertyValue(1.23f));
    EXPECT_FLOAT_EQ(std::get<float>(m_Panel->GetPropertyValue("floatValue")), 1.23f);
    
    // Test string
    m_Panel->SetPropertyValue("stringValue", PropertyValue(std::string("Test")));
    EXPECT_EQ(std::get<std::string>(m_Panel->GetPropertyValue("stringValue")), "Test");
    
    // Test vector2
    m_Panel->SetPropertyValue("vec2Value", PropertyValue(glm::vec2(1.0f, 2.0f)));
    auto vec2 = std::get<glm::vec2>(m_Panel->GetPropertyValue("vec2Value"));
    EXPECT_FLOAT_EQ(vec2.x, 1.0f);
    EXPECT_FLOAT_EQ(vec2.y, 2.0f);
    
    // Test vector3
    m_Panel->SetPropertyValue("vec3Value", PropertyValue(glm::vec3(1.0f, 2.0f, 3.0f)));
    auto vec3 = std::get<glm::vec3>(m_Panel->GetPropertyValue("vec3Value"));
    EXPECT_FLOAT_EQ(vec3.x, 1.0f);
    EXPECT_FLOAT_EQ(vec3.y, 2.0f);
    EXPECT_FLOAT_EQ(vec3.z, 3.0f);
    
    // Test vector4
    m_Panel->SetPropertyValue("vec4Value", PropertyValue(glm::vec4(1.0f, 2.0f, 3.0f, 4.0f)));
    auto vec4 = std::get<glm::vec4>(m_Panel->GetPropertyValue("vec4Value"));
    EXPECT_FLOAT_EQ(vec4.x, 1.0f);
    EXPECT_FLOAT_EQ(vec4.y, 2.0f);
    EXPECT_FLOAT_EQ(vec4.z, 3.0f);
    EXPECT_FLOAT_EQ(vec4.w, 4.0f);
}

TEST_F(UIPropertyPanelTest, Validation) {
    // Add range validator
    auto rangeValidator = std::make_shared<UIRangeValidator>(0.0, 100.0);
    m_Panel->RegisterValidator("intValue", rangeValidator);
    
    // Test valid value
    EXPECT_TRUE(m_Panel->SetPropertyValue("intValue", PropertyValue(50)));
    
    // Test invalid value
    EXPECT_FALSE(m_Panel->SetPropertyValue("intValue", PropertyValue(-1)));
    EXPECT_FALSE(m_Panel->SetPropertyValue("intValue", PropertyValue(101)));
    
    // Add string length validator
    auto stringValidator = std::make_shared<UIStringLengthValidator>(1, 10);
    m_Panel->RegisterValidator("stringValue", stringValidator);
    
    // Test valid string
    EXPECT_TRUE(m_Panel->SetPropertyValue("stringValue", PropertyValue(std::string("Valid"))));
    
    // Test invalid string
    EXPECT_FALSE(m_Panel->SetPropertyValue("stringValue", PropertyValue(std::string(""))));
    EXPECT_FALSE(m_Panel->SetPropertyValue("stringValue", PropertyValue(std::string("TooLongString"))));
}

TEST_F(UIPropertyPanelTest, UndoRedo) {
    // Make some changes
    m_Panel->SetPropertyValue("intValue", PropertyValue(1));
    m_Panel->SetPropertyValue("floatValue", PropertyValue(1.0f));
    
    // Verify initial state
    EXPECT_EQ(std::get<int32_t>(m_Panel->GetPropertyValue("intValue")), 1);
    EXPECT_FLOAT_EQ(std::get<float>(m_Panel->GetPropertyValue("floatValue")), 1.0f);
    
    // Undo changes
    m_Panel->Undo();
    EXPECT_FLOAT_EQ(std::get<float>(m_Panel->GetPropertyValue("floatValue")), 3.14f);
    
    m_Panel->Undo();
    EXPECT_EQ(std::get<int32_t>(m_Panel->GetPropertyValue("intValue")), 42);
    
    // Redo changes
    m_Panel->Redo();
    EXPECT_EQ(std::get<int32_t>(m_Panel->GetPropertyValue("intValue")), 1);
    
    m_Panel->Redo();
    EXPECT_FLOAT_EQ(std::get<float>(m_Panel->GetPropertyValue("floatValue")), 1.0f);
}

TEST_F(UIPropertyPanelTest, BatchEditing) {
    // Begin batch edit
    m_Panel->BeginBatchEdit();
    
    // Make multiple changes
    m_Panel->SetPropertyValue("intValue", PropertyValue(1));
    m_Panel->SetPropertyValue("floatValue", PropertyValue(1.0f));
    m_Panel->SetPropertyValue("stringValue", PropertyValue(std::string("Test")));
    
    // End batch edit
    m_Panel->EndBatchEdit();
    
    // Verify all changes were applied
    EXPECT_EQ(std::get<int32_t>(m_Panel->GetPropertyValue("intValue")), 1);
    EXPECT_FLOAT_EQ(std::get<float>(m_Panel->GetPropertyValue("floatValue")), 1.0f);
    EXPECT_EQ(std::get<std::string>(m_Panel->GetPropertyValue("stringValue")), "Test");
    
    // Undo batch
    m_Panel->Undo();
    
    // Verify all changes were undone
    EXPECT_EQ(std::get<int32_t>(m_Panel->GetPropertyValue("intValue")), 42);
    EXPECT_FLOAT_EQ(std::get<float>(m_Panel->GetPropertyValue("floatValue")), 3.14f);
    EXPECT_EQ(std::get<std::string>(m_Panel->GetPropertyValue("stringValue")), "Hello");
}

}}} // namespace VGE::UI::Test 