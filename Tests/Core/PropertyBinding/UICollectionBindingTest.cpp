#include "Property/UIPropertyBinding.h"
#include "Property/UIPropertyReflection.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace VGE::UI;

// Test classes
class TestCollectionSource {
public:
    const std::vector<std::any>& GetItems() const { return m_Items; }
    void SetItems(const std::vector<std::any>& items) { m_Items = items; }
    
    const std::vector<std::string>& GetStrings() const { return m_Strings; }
    void SetStrings(const std::vector<std::string>& strings) { m_Strings = strings; }
    
    const std::vector<int>& GetNumbers() const { return m_Numbers; }
    void SetNumbers(const std::vector<int>& numbers) { m_Numbers = numbers; }

private:
    std::vector<std::any> m_Items;
    std::vector<std::string> m_Strings;
    std::vector<int> m_Numbers;
};

class TestCollectionTarget {
public:
    const std::vector<std::any>& GetItems() const { return m_Items; }
    void SetItems(const std::vector<std::any>& items) { m_Items = items; }
    
    const std::vector<std::string>& GetStrings() const { return m_Strings; }
    void SetStrings(const std::vector<std::string>& strings) { m_Strings = strings; }
    
    const std::vector<int>& GetNumbers() const { return m_Numbers; }
    void SetNumbers(const std::vector<int>& numbers) { m_Numbers = numbers; }

private:
    std::vector<std::any> m_Items;
    std::vector<std::string> m_Strings;
    std::vector<int> m_Numbers;
};

// Register test classes with reflection system
REGISTER_TYPE(TestCollectionSource)
    .Property("Items", &TestCollectionSource::GetItems, &TestCollectionSource::SetItems)
    .Property("Strings", &TestCollectionSource::GetStrings, &TestCollectionSource::SetStrings)
    .Property("Numbers", &TestCollectionSource::GetNumbers, &TestCollectionSource::SetNumbers);

REGISTER_TYPE(TestCollectionTarget)
    .Property("Items", &TestCollectionTarget::GetItems, &TestCollectionTarget::SetItems)
    .Property("Strings", &TestCollectionTarget::GetStrings, &TestCollectionTarget::SetStrings)
    .Property("Numbers", &TestCollectionTarget::GetNumbers, &TestCollectionTarget::SetNumbers);

class UICollectionBindingTest : public ::testing::Test {
protected:
    void SetUp() override {
        GetBindingManager().ClearAllBindings();
    }
    
    void TearDown() override {
        GetBindingManager().ClearAllBindings();
    }
};

// Test one-way collection binding
TEST_F(UICollectionBindingTest, OneWayBinding) {
    TestCollectionSource source;
    TestCollectionTarget target;
    
    // Create binding
    CollectionBindingOptions options;
    options.mode = CollectionBindingMode::OneWay;
    BindCollection(&source, "Numbers", &target, "Numbers", options);
    
    // Test initial state
    std::vector<int> numbers = {1, 2, 3};
    source.SetNumbers(numbers);
    
    CollectionChangeEvent event;
    event.type = CollectionChangeType::Reset;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    EXPECT_EQ(target.GetNumbers(), numbers);
    
    // Test add item
    event.type = CollectionChangeType::Add;
    event.index = numbers.size();
    event.item = 4;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    numbers.push_back(4);
    EXPECT_EQ(target.GetNumbers(), numbers);
    
    // Test remove item
    event.type = CollectionChangeType::Remove;
    event.index = 1;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    numbers.erase(numbers.begin() + 1);
    EXPECT_EQ(target.GetNumbers(), numbers);
    
    // Test replace item
    event.type = CollectionChangeType::Replace;
    event.index = 0;
    event.newItem = 10;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    numbers[0] = 10;
    EXPECT_EQ(target.GetNumbers(), numbers);
    
    // Test move item
    event.type = CollectionChangeType::Move;
    event.index = 0;
    event.newIndex = 2;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    auto item = numbers[0];
    numbers.erase(numbers.begin());
    numbers.insert(numbers.begin() + 2, item);
    EXPECT_EQ(target.GetNumbers(), numbers);
}

// Test two-way collection binding
TEST_F(UICollectionBindingTest, TwoWayBinding) {
    TestCollectionSource source;
    TestCollectionTarget target;
    
    // Create binding
    CollectionBindingOptions options;
    options.mode = CollectionBindingMode::TwoWay;
    BindCollection(&source, "Numbers", &target, "Numbers", options);
    
    // Test source to target
    std::vector<int> numbers = {1, 2, 3};
    source.SetNumbers(numbers);
    
    CollectionChangeEvent event;
    event.type = CollectionChangeType::Reset;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    EXPECT_EQ(target.GetNumbers(), numbers);
    
    // Test target to source
    numbers = {4, 5, 6};
    target.SetNumbers(numbers);
    GetBindingManager().NotifyCollectionChanged(&target, "Numbers", event);
    
    EXPECT_EQ(source.GetNumbers(), numbers);
}

// Test collection item conversion
TEST_F(UICollectionBindingTest, ItemConversion) {
    TestCollectionSource source;
    TestCollectionTarget target;
    
    // Register converter for int to string
    auto converter = std::make_shared<UIPropertyExpression>(
        "toString(value)",
        [](const std::unordered_map<std::string, std::any>& vars) -> std::any {
            return std::to_string(std::any_cast<int>(vars.at("value")));
        }
    );
    
    GetBindingManager().RegisterCollectionConverter(
        std::type_index(typeid(int)),
        std::type_index(typeid(std::string)),
        converter
    );
    
    // Create binding
    CollectionBindingOptions options;
    BindCollection(&source, "Numbers", &target, "Strings", options);
    
    // Test conversion
    std::vector<int> numbers = {1, 2, 3};
    source.SetNumbers(numbers);
    
    CollectionChangeEvent event;
    event.type = CollectionChangeType::Reset;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    std::vector<std::string> expected = {"1", "2", "3"};
    EXPECT_EQ(target.GetStrings(), expected);
}

// Test collection item validation
TEST_F(UICollectionBindingTest, ItemValidation) {
    TestCollectionSource source;
    TestCollectionTarget target;
    
    // Register validator that only allows positive numbers
    auto validator = std::make_shared<UIPropertyExpression>(
        "value > 0",
        [](const std::unordered_map<std::string, std::any>& vars) -> std::any {
            return std::any_cast<int>(vars.at("value")) > 0;
        }
    );
    
    GetBindingManager().RegisterCollectionValidator(std::type_index(typeid(int)), validator);
    
    // Create binding
    CollectionBindingOptions options;
    BindCollection(&source, "Numbers", &target, "Numbers", options);
    
    // Test validation
    std::vector<int> numbers = {1, -2, 3, -4, 5};
    source.SetNumbers(numbers);
    
    CollectionChangeEvent event;
    event.type = CollectionChangeType::Reset;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    std::vector<int> expected = {1, 3, 5}; // Only positive numbers
    EXPECT_EQ(target.GetNumbers(), expected);
}

// Test collection binding options
TEST_F(UICollectionBindingTest, BindingOptions) {
    TestCollectionSource source;
    TestCollectionTarget target;
    
    // Create binding with specific options
    CollectionBindingOptions options;
    options.updateTargetOnAdd = true;
    options.updateTargetOnRemove = false;
    options.updateTargetOnReplace = true;
    options.updateTargetOnMove = false;
    
    BindCollection(&source, "Numbers", &target, "Numbers", options);
    
    // Initial state
    std::vector<int> numbers = {1, 2, 3};
    source.SetNumbers(numbers);
    
    CollectionChangeEvent event;
    event.type = CollectionChangeType::Reset;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    EXPECT_EQ(target.GetNumbers(), numbers);
    
    // Test add (should update)
    event.type = CollectionChangeType::Add;
    event.index = numbers.size();
    event.item = 4;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    numbers.push_back(4);
    EXPECT_EQ(target.GetNumbers(), numbers);
    
    // Test remove (should not update)
    event.type = CollectionChangeType::Remove;
    event.index = 1;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    EXPECT_EQ(target.GetNumbers(), numbers); // Target should remain unchanged
    
    // Test replace (should update)
    event.type = CollectionChangeType::Replace;
    event.index = 0;
    event.newItem = 10;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    numbers[0] = 10;
    EXPECT_EQ(target.GetNumbers(), numbers);
    
    // Test move (should not update)
    event.type = CollectionChangeType::Move;
    event.index = 0;
    event.newIndex = 2;
    GetBindingManager().NotifyCollectionChanged(&source, "Numbers", event);
    
    EXPECT_EQ(target.GetNumbers(), numbers); // Target should remain unchanged
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 