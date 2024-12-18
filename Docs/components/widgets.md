# Widget System

## Overview
The VersaUI widget system provides the building blocks for creating user interfaces. Each widget is designed to be:
- Self-contained
- Themeable
- Accessible
- Performance-optimized

## Core Widgets

### Basic Widgets
```cpp
// Button widget example
class Button : public Widget {
    void onClick(const ClickEvent& event);
    void setLabel(const std::string& text);
    void setIcon(const Icon& icon);
};

// Panel widget example
class Panel : public Widget {
    void setLayout(std::unique_ptr<Layout> layout);
    void addChild(std::shared_ptr<Widget> child);
};
```

### Layout Widgets
- Stack Panel
- Grid
- Dock Panel
- Flow Layout

### Input Widgets
- TextBox
- Slider
- CheckBox
- RadioButton

## Creating Custom Widgets

### Basic Template
```cpp
class CustomWidget : public Widget {
public:
    CustomWidget();
    
    // Override necessary methods
    void paint(RenderContext& context) override;
    void onMouseEvent(const MouseEvent& event) override;
    void onKeyEvent(const KeyEvent& event) override;
    
    // Custom functionality
    void customBehavior();
    
private:
    // Widget state
    State m_state;
};
```

### Best Practices
1. Keep widgets focused on a single responsibility
2. Implement proper event handling
3. Support themes by default
4. Include accessibility features
5. Document public APIs 