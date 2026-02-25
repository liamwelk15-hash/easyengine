#pragma once

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace EasyScript {

class VM;

using Value = std::variant<
    std::monostate,
    bool,
    int,
    float,
    std::string
>;

using NativeFunction = std::function<Value(VM*, const std::vector<Value>&)>;
using ScriptFunction = std::shared_ptr<class Function>;

enum class TokenType {
    ENDFILE, ERROR,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_PAREN, RIGHT_PAREN,
    COMMA, DOT, SEMICOLON,
    PLUS, MINUS, STAR, SLASH, PERCENT,
    EQUAL, EQUAL_EQUAL, BANG, BANG_EQUAL,
    LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
    VAR, FUN, CLASS, IF, ELSE, FOR, WHILE, RETURN,
    TRUE, FALSE, NULLPTR, THIS, PRINT, NEW, SUPER,
    IDENTIFIER, NUMBER, STRING
};

struct Token {
    TokenType type;
    std::string lexeme;
    Value literal;
    int line;
};

class Scanner {
public:
    Scanner(const std::string& source);
    std::vector<Token> scanTokens();
    
private:
    std::string source;
    int start = 0;
    int current = 0;
    int line = 1;
    
    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const Value& literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void stringToken();
    void number();
    void identifier();
    TokenType identifierType();
    void scanToken();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
};

struct Upvalue {
    int index;
    bool isLocal;
};

class Closure;
class Class;
class Instance;

struct Slot {
    Value value;
    Upvalue* upvalue = nullptr;
};

class VM {
public:
    VM();
    ~VM();
    
    Value interpret(const std::string& source);
    Value call(ScriptFunction function, const std::vector<Value>& args);
    void setGlobal(const std::string& name, const Value& value);
    Value getGlobal(const std::string& name);
    void defineNative(const std::string& name, NativeFunction fn);
    
    std::vector<Slot>& getStack() { return stack; }
    Value peek(int distance);
    void push(const Value& value);
    Value pop();
    
private:
    std::vector<Slot> stack;
    std::unordered_map<std::string, Value> globals;
    std::unordered_map<std::string, NativeFunction> natives;
    std::vector<std::shared_ptr<Closure>> closures;
    
    bool isTruthy(const Value& value);
    bool isEqual(const Value& a, const Value& b);
    std::string valueToString(const Value& value);
};

struct Expr;
struct Stmt;

class Parser {
public:
    Parser(VM* vm);
    std::vector<std::shared_ptr<Stmt>> parse(const std::string& source);
    
private:
    VM* vm;
    std::vector<Token> tokens;
    int current = 0;
    
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> varDeclaration();
    std::shared_ptr<Stmt> functionDeclaration(const std::string& kind);
    std::shared_ptr<Stmt> classDeclaration();
    std::shared_ptr<Stmt> ifStatement();
    std::shared_ptr<Stmt> whileStatement();
    std::shared_ptr<Stmt> forStatement();
    std::shared_ptr<Stmt> returnStatement();
    std::shared_ptr<Stmt> expressionStatement();
    std::shared_ptr<Stmt> block();
    
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> orExpr();
    std::shared_ptr<Expr> andExpr();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> primary();
    
    bool check(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token& advance();
    Token& peek();
    Token& previous();
    bool isAtEnd();
    
    void error(const std::string& message);
};

} // namespace EasyScript
