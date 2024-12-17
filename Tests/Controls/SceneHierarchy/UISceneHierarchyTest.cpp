#include "Runtime/UI/Public/Controls/UITreeView.h"
#include "Runtime/UI/Public/Window/UIWindow.h"
#include "Runtime/Core/Public/Core/UICore.h"
#include "Runtime/UI/Public/Theme/UIStyle.h"
#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace VGE {
namespace UI {
namespace Test {

// ... [Previous code remains unchanged until class definition] ...

class UISceneHierarchyTest : public testing::Test {
protected:
    // ... [Previous methods remain unchanged] ...

    bool m_SelectionChanged = false;
    bool m_DragStarted = false;
    bool m_DragEnded = false;
    std::vector<std::shared_ptr<SceneHierarchyItem>> m_LastSelectedItems;
    std::shared_ptr<SceneHierarchyItem> m_LastDraggedItem;
    std::shared_ptr<SceneHierarchyItem> m_LastDroppedItem;

    void SetupCallbacks() {
        // Selection changed
        m_SceneHierarchy->SetOnSelectionChanged([this](const std::vector<std::shared_ptr<SceneHierarchyItem>>& items) {
            m_SelectionChanged = true;
            m_LastSelectedItems = items;
        });
        
        // Drag and drop
        m_SceneHierarchy->SetOnBeginDrag([this](const std::shared_ptr<SceneHierarchyItem>& item) {
            m_DragStarted = true;
            m_LastDraggedItem = item;
        });
        
        m_SceneHierarchy->SetOnEndDrag([this](const std::shared_ptr<SceneHierarchyItem>& item) {
            m_DragEnded = true;
            m_LastDroppedItem = item;
        });
    }
};

// ... [Previous test cases remain unchanged] ...

TEST_F(UISceneHierarchyTest, CallbackTesting) {
    auto hero = m_SceneHierarchy->FindItem("Hero");
    auto car = m_SceneHierarchy->FindItem("Car");
    
    // Test selection callback
    m_SelectionChanged = false;
    m_SceneHierarchy->SetSelection({hero});
    EXPECT_TRUE(m_SelectionChanged);
    EXPECT_EQ(m_LastSelectedItems.size(), 1);
    EXPECT_EQ(m_LastSelectedItems[0], hero);
    
    // Test drag callbacks
    m_DragStarted = false;
    m_DragEnded = false;
    m_SceneHierarchy->BeginDrag(hero);
    EXPECT_TRUE(m_DragStarted);
    EXPECT_EQ(m_LastDraggedItem, hero);
    
    m_SceneHierarchy->EndDrag(car);
    EXPECT_TRUE(m_DragEnded);
    EXPECT_EQ(m_LastDroppedItem, car);
}

TEST_F(UISceneHierarchyTest, BatchOperations) {
    auto models = m_SceneHierarchy->FindItem("Models");
    
    // Begin batch operation
    m_SceneHierarchy->BatchBegin();
    
    // Create multiple items
    std::vector<std::shared_ptr<SceneHierarchyItem>> newItems;
    for (int i = 0; i < 5; ++i) {
        auto item = CreateItem("BatchItem_" + std::to_string(i), "TestType", true);
        m_SceneHierarchy->AddItem(item, models);
        newItems.push_back(item);
    }
    
    // End batch operation
    m_SceneHierarchy->BatchEnd();
    
    // Verify all items were added
    auto modelChildren = m_SceneHierarchy->GetChildren(models);
    for (const auto& item : newItems) {
        EXPECT_TRUE(std::find(modelChildren.begin(), modelChildren.end(), item) != modelChildren.end());
    }
}

TEST_F(UISceneHierarchyTest, UndoRedoOperations) {
    auto models = m_SceneHierarchy->FindItem("Models");
    auto initialChildCount = m_SceneHierarchy->GetChildren(models).size();
    
    // Add an item
    auto newItem = CreateItem("UndoRedoTest", "TestType", true);
    m_SceneHierarchy->AddItem(newItem, models);
    EXPECT_EQ(m_SceneHierarchy->GetChildren(models).size(), initialChildCount + 1);
    
    // Undo the addition
    m_SceneHierarchy->Undo();
    EXPECT_EQ(m_SceneHierarchy->GetChildren(models).size(), initialChildCount);
    
    // Redo the addition
    m_SceneHierarchy->Redo();
    EXPECT_EQ(m_SceneHierarchy->GetChildren(models).size(), initialChildCount + 1);
}

TEST_F(UISceneHierarchyTest, ColumnSorting) {
    // Sort by type
    m_SceneHierarchy->SortByColumn("Type", true);
    auto items = m_SceneHierarchy->GetAllItems();
    for (size_t i = 1; i < items.size(); ++i) {
        EXPECT_LE(items[i-1]->type, items[i]->type);
    }
    
    // Sort by visibility
    m_SceneHierarchy->SortByColumn("Visible", false);
    items = m_SceneHierarchy->GetAllItems();
    for (size_t i = 1; i < items.size(); ++i) {
        EXPECT_GE(items[i-1]->isVisible, items[i]->isVisible);
    }
}

TEST_F(UISceneHierarchyTest, KeyboardShortcuts) {
    auto hero = m_SceneHierarchy->FindItem("Hero");
    m_SceneHierarchy->SetSelection({hero});
    
    // Test delete shortcut
    m_SceneHierarchy->HandleKeyboardShortcut(Key::Delete, ModifierKeys::None);
    EXPECT_TRUE(m_SceneHierarchy->FindItem("Hero") == nullptr);
    
    // Test undo shortcut
    m_SceneHierarchy->HandleKeyboardShortcut(Key::Z, ModifierKeys::Control);
    EXPECT_TRUE(m_SceneHierarchy->FindItem("Hero") != nullptr);
    
    // Test copy/paste shortcuts
    auto car = m_SceneHierarchy->FindItem("Car");
    m_SceneHierarchy->SetSelection({car});
    m_SceneHierarchy->HandleKeyboardShortcut(Key::C, ModifierKeys::Control);
    m_SceneHierarchy->HandleKeyboardShortcut(Key::V, ModifierKeys::Control);
    
    auto items = m_SceneHierarchy->GetAllItems();
    int carCount = std::count_if(items.begin(), items.end(),
        [](const auto& item) { return item->type == "Vehicle"; });
    EXPECT_EQ(carCount, 3); // Original car + bike + pasted car
}

}}} // namespace VGE::UI::Test 