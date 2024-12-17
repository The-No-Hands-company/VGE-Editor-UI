#include "Property/UIPropertyExpression.h"
#include <cctype>
#include <sstream>
#include <algorithm>

namespace VGE {
namespace UI {

// Node implementations
std::any VariableNode::Evaluate(const std::unordered_map<std::string, std::any>& variables) const {
    auto it = variables.find(m_Name);
    if (it == variables.end()) {
        throw std::runtime_error("Variable not found: " + m_Name);
    }
    return it->second;
}

std::any UnaryOpNode::Evaluate(const std::unordered_map<std::string, std::any>& variables) const {
    auto operand = m_Operand->Evaluate(variables);
    
    if (m_Operator == "-") {
        if (operand.type() == typeid(int)) return -std::any_cast<int>(operand);
        if (operand.type() == typeid(float)) return -std::any_cast<float>(operand);
        if (operand.type() == typeid(double)) return -std::any_cast<double>(operand);
    }
    else if (m_Operator == "!") {
        if (operand.type() == typeid(bool)) return !std::any_cast<bool>(operand);
    }
    
    throw std::runtime_error("Invalid unary operator: " + m_Operator);
}

std::any BinaryOpNode::Evaluate(const std::unordered_map<std::string, std::any>& variables) const {
    auto left = m_Left->Evaluate(variables);
    auto right = m_Right->Evaluate(variables);
    
    // Arithmetic operators
    if (m_Operator == "+") {
        if (left.type() == typeid(int) && right.type() == typeid(int))
            return std::any_cast<int>(left) + std::any_cast<int>(right);
        if (left.type() == typeid(float) || right.type() == typeid(float))
            return std::any_cast<float>(left) + std::any_cast<float>(right);
        if (left.type() == typeid(std::string) || right.type() == typeid(std::string))
            return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
    }
    else if (m_Operator == "-") {
        if (left.type() == typeid(int) && right.type() == typeid(int))
            return std::any_cast<int>(left) - std::any_cast<int>(right);
        if (left.type() == typeid(float) || right.type() == typeid(float))
            return std::any_cast<float>(left) - std::any_cast<float>(right);
    }
    else if (m_Operator == "*") {
        if (left.type() == typeid(int) && right.type() == typeid(int))
            return std::any_cast<int>(left) * std::any_cast<int>(right);
        if (left.type() == typeid(float) || right.type() == typeid(float))
            return std::any_cast<float>(left) * std::any_cast<float>(right);
    }
    else if (m_Operator == "/") {
        if (left.type() == typeid(int) && right.type() == typeid(int))
            return std::any_cast<int>(left) / std::any_cast<int>(right);
        if (left.type() == typeid(float) || right.type() == typeid(float))
            return std::any_cast<float>(left) / std::any_cast<float>(right);
    }
    
    // Comparison operators
    else if (m_Operator == "==") {
        if (left.type() != right.type()) return false;
        if (left.type() == typeid(int)) return std::any_cast<int>(left) == std::any_cast<int>(right);
        if (left.type() == typeid(float)) return std::any_cast<float>(left) == std::any_cast<float>(right);
        if (left.type() == typeid(bool)) return std::any_cast<bool>(left) == std::any_cast<bool>(right);
        if (left.type() == typeid(std::string)) return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);
    }
    else if (m_Operator == "!=") {
        if (left.type() != right.type()) return true;
        if (left.type() == typeid(int)) return std::any_cast<int>(left) != std::any_cast<int>(right);
        if (left.type() == typeid(float)) return std::any_cast<float>(left) != std::any_cast<float>(right);
        if (left.type() == typeid(bool)) return std::any_cast<bool>(left) != std::any_cast<bool>(right);
        if (left.type() == typeid(std::string)) return std::any_cast<std::string>(left) != std::any_cast<std::string>(right);
    }
    else if (m_Operator == "<") {
        if (left.type() == typeid(int) && right.type() == typeid(int))
            return std::any_cast<int>(left) < std::any_cast<int>(right);
        if (left.type() == typeid(float) || right.type() == typeid(float))
            return std::any_cast<float>(left) < std::any_cast<float>(right);
    }
    else if (m_Operator == "<=") {
        if (left.type() == typeid(int) && right.type() == typeid(int))
            return std::any_cast<int>(left) <= std::any_cast<int>(right);
        if (left.type() == typeid(float) || right.type() == typeid(float))
            return std::any_cast<float>(left) <= std::any_cast<float>(right);
    }
    else if (m_Operator == ">") {
        if (left.type() == typeid(int) && right.type() == typeid(int))
            return std::any_cast<int>(left) > std::any_cast<int>(right);
        if (left.type() == typeid(float) || right.type() == typeid(float))
            return std::any_cast<float>(left) > std::any_cast<float>(right);
    }
    else if (m_Operator == ">=") {
        if (left.type() == typeid(int) && right.type() == typeid(int))
            return std::any_cast<int>(left) >= std::any_cast<int>(right);
        if (left.type() == typeid(float) || right.type() == typeid(float))
            return std::any_cast<float>(left) >= std::any_cast<float>(right);
    }
    
    // Logical operators
    else if (m_Operator == "&&") {
        return std::any_cast<bool>(left) && std::any_cast<bool>(right);
    }
    else if (m_Operator == "||") {
        return std::any_cast<bool>(left) || std::any_cast<bool>(right);
    }
    
    throw std::runtime_error("Invalid binary operator: " + m_Operator);
}

std::any FunctionCallNode::Evaluate(const std::unordered_map<std::string, std::any>& variables) const {
    // Evaluate arguments
    std::vector<std::any> args;
    for (const auto& arg : m_Arguments) {
        args.push_back(arg->Evaluate(variables));
    }
    
    // Look up function in variables
    auto it = variables.find(m_Name);
    if (it != variables.end() && it->second.type() == typeid(std::function<std::any(const std::vector<std::any>&)>)) {
        auto& func = std::any_cast<const std::function<std::any(const std::vector<std::any>&)>&>(it->second);
        return func(args);
    }
    
    throw std::runtime_error("Function not found: " + m_Name);
}

std::any PropertyAccessNode::Evaluate(const std::unordered_map<std::string, std::any>& variables) const {
    auto object = m_Object->Evaluate(variables);
    // TODO: Implement property access using reflection system
    throw std::runtime_error("Property access not implemented");
}

// UIExpressionParser implementation
UIExpressionParser::UIExpressionParser() : m_Position(0) {
    // Register built-in functions
    RegisterFunction("min", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() != 2) throw std::runtime_error("min requires 2 arguments");
        if (args[0].type() == typeid(int) && args[1].type() == typeid(int))
            return std::min(std::any_cast<int>(args[0]), std::any_cast<int>(args[1]));
        if (args[0].type() == typeid(float) || args[1].type() == typeid(float))
            return std::min(std::any_cast<float>(args[0]), std::any_cast<float>(args[1]));
        throw std::runtime_error("Invalid argument types for min");
    });
    
    RegisterFunction("max", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() != 2) throw std::runtime_error("max requires 2 arguments");
        if (args[0].type() == typeid(int) && args[1].type() == typeid(int))
            return std::max(std::any_cast<int>(args[0]), std::any_cast<int>(args[1]));
        if (args[0].type() == typeid(float) || args[1].type() == typeid(float))
            return std::max(std::any_cast<float>(args[0]), std::any_cast<float>(args[1]));
        throw std::runtime_error("Invalid argument types for max");
    });
    
    RegisterFunction("clamp", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() != 3) throw std::runtime_error("clamp requires 3 arguments");
        if (args[0].type() == typeid(int) && args[1].type() == typeid(int) && args[2].type() == typeid(int)) {
            auto value = std::any_cast<int>(args[0]);
            auto min = std::any_cast<int>(args[1]);
            auto max = std::any_cast<int>(args[2]);
            return std::clamp(value, min, max);
        }
        if (args[0].type() == typeid(float) || args[1].type() == typeid(float) || args[2].type() == typeid(float)) {
            auto value = std::any_cast<float>(args[0]);
            auto min = std::any_cast<float>(args[1]);
            auto max = std::any_cast<float>(args[2]);
            return std::clamp(value, min, max);
        }
        throw std::runtime_error("Invalid argument types for clamp");
    });
}

std::shared_ptr<ExpressionNode> UIExpressionParser::Parse(const std::string& expression) {
    m_Expression = expression;
    m_Position = 0;
    m_Error.clear();
    
    try {
        auto result = ParseExpression();
        if (GetNextToken().type != TokenType::EndOfExpression) {
            m_Error = "Unexpected tokens after expression";
            return nullptr;
        }
        return result;
    }
    catch (const std::exception& e) {
        m_Error = e.what();
        return nullptr;
    }
}

void UIExpressionParser::RegisterFunction(const std::string& name, FunctionType function) {
    m_Functions[name] = function;
}

std::vector<Token> UIExpressionParser::Tokenize(const std::string& expression) {
    std::vector<Token> tokens;
    size_t pos = 0;
    
    while (pos < expression.length()) {
        // Skip whitespace
        while (pos < expression.length() && std::isspace(expression[pos])) pos++;
        if (pos >= expression.length()) break;
        
        Token token;
        token.position = pos;
        
        // Numbers
        if (std::isdigit(expression[pos]) || expression[pos] == '.') {
            token.type = TokenType::Number;
            size_t end;
            std::stod(expression.substr(pos), &end);
            token.value = expression.substr(pos, end);
            pos += end;
        }
        // Identifiers
        else if (std::isalpha(expression[pos]) || expression[pos] == '_') {
            token.type = TokenType::Identifier;
            size_t start = pos;
            while (pos < expression.length() && (std::isalnum(expression[pos]) || expression[pos] == '_')) pos++;
            token.value = expression.substr(start, pos - start);
        }
        // Strings
        else if (expression[pos] == '"' || expression[pos] == '\'') {
            char quote = expression[pos++];
            token.type = TokenType::String;
            size_t start = pos;
            while (pos < expression.length() && expression[pos] != quote) pos++;
            if (pos >= expression.length()) throw std::runtime_error("Unterminated string");
            token.value = expression.substr(start, pos - start);
            pos++;
        }
        // Operators and punctuation
        else {
            switch (expression[pos]) {
                case '(': token.type = TokenType::LeftParen; token.value = "("; break;
                case ')': token.type = TokenType::RightParen; token.value = ")"; break;
                case ',': token.type = TokenType::Comma; token.value = ","; break;
                case '.': token.type = TokenType::Dot; token.value = "."; break;
                default:
                    // Multi-character operators
                    static const std::string operators[] = {
                        "==", "!=", "<=", ">=", "&&", "||", "+", "-", "*", "/", "<", ">"
                    };
                    bool found = false;
                    for (const auto& op : operators) {
                        if (expression.substr(pos, op.length()) == op) {
                            token.type = TokenType::Operator;
                            token.value = op;
                            pos += op.length();
                            found = true;
                            break;
                        }
                    }
                    if (!found) throw std::runtime_error("Invalid character in expression");
            }
            if (token.value.empty()) pos++;
        }
        
        tokens.push_back(token);
    }
    
    // Add end of expression token
    tokens.push_back({TokenType::EndOfExpression, "", expression.length()});
    
    return tokens;
}

Token UIExpressionParser::GetNextToken() {
    static std::vector<Token> tokens;
    static size_t tokenPos = 0;
    
    if (m_Position == 0) {
        tokens = Tokenize(m_Expression);
        tokenPos = 0;
    }
    
    if (tokenPos < tokens.size()) {
        m_Position = tokens[tokenPos].position;
        return tokens[tokenPos++];
    }
    
    return {TokenType::EndOfExpression, "", m_Expression.length()};
}

std::shared_ptr<ExpressionNode> UIExpressionParser::ParseExpression() {
    return ParseBinary(0);
}

std::shared_ptr<ExpressionNode> UIExpressionParser::ParsePrimary() {
    Token token = GetNextToken();
    
    switch (token.type) {
        case TokenType::Number: {
            if (token.value.find('.') == std::string::npos)
                return std::make_shared<LiteralNode>(std::stoi(token.value));
            else
                return std::make_shared<LiteralNode>(std::stof(token.value));
        }
        
        case TokenType::String:
            return std::make_shared<LiteralNode>(token.value);
        
        case TokenType::Identifier: {
            Token next = GetNextToken();
            if (next.type == TokenType::LeftParen) {
                auto node = ParseFunctionCall(token.value);
                if (GetNextToken().type != TokenType::RightParen)
                    throw std::runtime_error("Expected ')'");
                return node;
            }
            else if (next.type == TokenType::Dot) {
                auto object = std::make_shared<VariableNode>(token.value);
                return ParsePropertyAccess(object);
            }
            else {
                // Put back the next token
                m_Position--;
                return std::make_shared<VariableNode>(token.value);
            }
        }
        
        case TokenType::LeftParen: {
            auto node = ParseExpression();
            if (GetNextToken().type != TokenType::RightParen)
                throw std::runtime_error("Expected ')'");
            return node;
        }
        
        default:
            throw std::runtime_error("Unexpected token");
    }
}

std::shared_ptr<ExpressionNode> UIExpressionParser::ParseUnary() {
    Token token = GetNextToken();
    
    if (token.type == TokenType::Operator && IsUnaryOperator(token.value)) {
        auto operand = ParseUnary();
        return std::make_shared<UnaryOpNode>(token.value, operand);
    }
    
    // Put back the token
    m_Position--;
    return ParsePrimary();
}

std::shared_ptr<ExpressionNode> UIExpressionParser::ParseBinary(int precedence) {
    auto left = ParseUnary();
    
    while (true) {
        Token token = GetNextToken();
        if (token.type != TokenType::Operator || GetOperatorPrecedence(token.value) < precedence) {
            // Put back the token
            m_Position--;
            return left;
        }
        
        auto right = ParseBinary(GetOperatorPrecedence(token.value) + 1);
        left = std::make_shared<BinaryOpNode>(token.value, left, right);
    }
}

std::shared_ptr<ExpressionNode> UIExpressionParser::ParseFunctionCall(const std::string& name) {
    std::vector<std::shared_ptr<ExpressionNode>> arguments;
    
    Token token = GetNextToken();
    if (token.type != TokenType::RightParen) {
        m_Position--;
        do {
            arguments.push_back(ParseExpression());
            token = GetNextToken();
        } while (token.type == TokenType::Comma);
        m_Position--;
    }
    else {
        m_Position--;
    }
    
    return std::make_shared<FunctionCallNode>(name, arguments);
}

std::shared_ptr<ExpressionNode> UIExpressionParser::ParsePropertyAccess(std::shared_ptr<ExpressionNode> object) {
    Token token = GetNextToken();
    if (token.type != TokenType::Identifier)
        throw std::runtime_error("Expected property name");
    
    auto node = std::make_shared<PropertyAccessNode>(object, token.value);
    
    token = GetNextToken();
    if (token.type == TokenType::Dot)
        return ParsePropertyAccess(node);
    
    // Put back the token
    m_Position--;
    return node;
}

bool UIExpressionParser::IsOperator(const std::string& token) const {
    static const std::string operators[] = {
        "+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=", "&&", "||"
    };
    return std::find(std::begin(operators), std::end(operators), token) != std::end(operators);
}

int UIExpressionParser::GetOperatorPrecedence(const std::string& op) const {
    if (op == "||") return 1;
    if (op == "&&") return 2;
    if (op == "==" || op == "!=") return 3;
    if (op == "<" || op == ">" || op == "<=" || op == ">=") return 4;
    if (op == "+" || op == "-") return 5;
    if (op == "*" || op == "/") return 6;
    return 0;
}

bool UIExpressionParser::IsUnaryOperator(const std::string& op) const {
    return op == "-" || op == "!";
}

}} // namespace VGE::UI
  </rewritten_file> 