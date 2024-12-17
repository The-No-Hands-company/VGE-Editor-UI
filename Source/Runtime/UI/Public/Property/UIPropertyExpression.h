#pragma once

#include "Runtime/Core/Public/Core/UICore.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <any>

namespace VGE {
namespace UI {

/**
 * @brief Token types for expression parsing
 */
enum class TokenType {
    Number,
    String,
    Identifier,
    Operator,
    LeftParen,
    RightParen,
    Comma,
    Dot,
    EndOfExpression
};

/**
 * @brief Token structure for lexical analysis
 */
struct Token {
    TokenType type;
    std::string value;
    size_t position;
};

/**
 * @brief Expression node types
 */
enum class NodeType {
    Literal,
    Variable,
    UnaryOp,
    BinaryOp,
    FunctionCall,
    PropertyAccess
};

/**
 * @brief Base class for expression nodes
 */
class ExpressionNode {
public:
    virtual ~ExpressionNode() = default;
    virtual NodeType GetType() const = 0;
    virtual std::any Evaluate(const std::unordered_map<std::string, std::any>& variables) const = 0;
};

/**
 * @brief Literal value node
 */
class LiteralNode : public ExpressionNode {
public:
    LiteralNode(const std::any& value) : m_Value(value) {}
    NodeType GetType() const override { return NodeType::Literal; }
    std::any Evaluate(const std::unordered_map<std::string, std::any>&) const override { return m_Value; }

private:
    std::any m_Value;
};

/**
 * @brief Variable reference node
 */
class VariableNode : public ExpressionNode {
public:
    VariableNode(const std::string& name) : m_Name(name) {}
    NodeType GetType() const override { return NodeType::Variable; }
    std::any Evaluate(const std::unordered_map<std::string, std::any>& variables) const override;

private:
    std::string m_Name;
};

/**
 * @brief Unary operator node
 */
class UnaryOpNode : public ExpressionNode {
public:
    UnaryOpNode(const std::string& op, std::shared_ptr<ExpressionNode> operand)
        : m_Operator(op), m_Operand(operand) {}
    NodeType GetType() const override { return NodeType::UnaryOp; }
    std::any Evaluate(const std::unordered_map<std::string, std::any>& variables) const override;

private:
    std::string m_Operator;
    std::shared_ptr<ExpressionNode> m_Operand;
};

/**
 * @brief Binary operator node
 */
class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(const std::string& op, std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        : m_Operator(op), m_Left(left), m_Right(right) {}
    NodeType GetType() const override { return NodeType::BinaryOp; }
    std::any Evaluate(const std::unordered_map<std::string, std::any>& variables) const override;

private:
    std::string m_Operator;
    std::shared_ptr<ExpressionNode> m_Left;
    std::shared_ptr<ExpressionNode> m_Right;
};

/**
 * @brief Function call node
 */
class FunctionCallNode : public ExpressionNode {
public:
    FunctionCallNode(const std::string& name, std::vector<std::shared_ptr<ExpressionNode>> args)
        : m_Name(name), m_Arguments(std::move(args)) {}
    NodeType GetType() const override { return NodeType::FunctionCall; }
    std::any Evaluate(const std::unordered_map<std::string, std::any>& variables) const override;

private:
    std::string m_Name;
    std::vector<std::shared_ptr<ExpressionNode>> m_Arguments;
};

/**
 * @brief Property access node
 */
class PropertyAccessNode : public ExpressionNode {
public:
    PropertyAccessNode(std::shared_ptr<ExpressionNode> object, const std::string& property)
        : m_Object(object), m_Property(property) {}
    NodeType GetType() const override { return NodeType::PropertyAccess; }
    std::any Evaluate(const std::unordered_map<std::string, std::any>& variables) const override;

private:
    std::shared_ptr<ExpressionNode> m_Object;
    std::string m_Property;
};

/**
 * @brief Expression parser class
 */
class UI_API UIExpressionParser {
public:
    UIExpressionParser();
    
    // Parse an expression string into an expression tree
    std::shared_ptr<ExpressionNode> Parse(const std::string& expression);
    
    // Register a custom function
    using FunctionType = std::function<std::any(const std::vector<std::any>&)>;
    void RegisterFunction(const std::string& name, FunctionType function);
    
    // Error handling
    bool HasError() const { return !m_Error.empty(); }
    const std::string& GetError() const { return m_Error; }

private:
    // Lexical analysis
    std::vector<Token> Tokenize(const std::string& expression);
    Token GetNextToken();
    
    // Parsing methods
    std::shared_ptr<ExpressionNode> ParseExpression();
    std::shared_ptr<ExpressionNode> ParsePrimary();
    std::shared_ptr<ExpressionNode> ParseUnary();
    std::shared_ptr<ExpressionNode> ParseBinary(int precedence);
    std::shared_ptr<ExpressionNode> ParseFunctionCall(const std::string& name);
    std::shared_ptr<ExpressionNode> ParsePropertyAccess(std::shared_ptr<ExpressionNode> object);
    
    // Helper methods
    bool IsOperator(const std::string& token) const;
    int GetOperatorPrecedence(const std::string& op) const;
    bool IsUnaryOperator(const std::string& op) const;
    
private:
    std::string m_Expression;
    size_t m_Position;
    std::string m_Error;
    std::unordered_map<std::string, FunctionType> m_Functions;
};

}} // namespace VGE::UI 