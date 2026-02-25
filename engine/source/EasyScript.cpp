#include "EasyScript.h"
#include <iostream>
#include <cctype>
#include <sstream>

namespace EasyScript {

Scanner::Scanner(const std::string& source) : source(source) {}

bool Scanner::isAtEnd() {
    return current >= source.length();
}

char Scanner::advance() {
    return source[current++];
}

void Scanner::addToken(TokenType type) {
    addToken(type, std::monostate{});
}

void Scanner::addToken(TokenType type, const Value& literal) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, literal, line});
}

bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char Scanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Scanner::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Scanner::stringToken() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    
    if (isAtEnd()) {
        std::cerr << "Error: Unterminated string at line " << line << std::endl;
        return;
    }
    
    advance();
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(STRING, value);
}

void Scanner::number() {
    while (isDigit(peek())) advance();
    
    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    }
    
    std::string numStr = source.substr(start, current - start);
    try {
        if (numStr.find('.') != std::string::npos) {
            float val = std::stof(numStr);
            addToken(NUMBER, val);
        } else {
            int val = std::stoi(numStr);
            addToken(NUMBER, val);
        }
    } catch (...) {
        addToken(NUMBER, 0.0f);
    }
}

void Scanner::identifier() {
    while (isAlphaNumeric(peek())) advance();
    
    std::string text = source.substr(start, current - start);
    TokenType type = identifierType();
    addToken(type, std::monostate{});
}

TokenType Scanner::identifierType() {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"var", VAR}, {"fun", FUN}, {"klasse", CLASS},
        {"wenn", IF}, {"sonst", ELSE}, {"für", FOR},
        {"solange", WHILE}, {"zurück", RETURN},
        {"wahr", TRUE}, {"falsch", FALSE}, {"nichts", NULLPTR},
        {"dies", THIS}, {"drucke", PRINT}, {"neu", NEW},
        {"super", SUPER}
    };
    
    auto it = keywords.find(source.substr(start, current - start));
    if (it != keywords.end()) {
        return it->second;
    }
    return IDENTIFIER;
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void Scanner::scanToken() {
    char c = advance();
    
    switch (c) {
        case '{': addToken(LEFT_BRACE); break;
        case '}': addToken(RIGHT_BRACE); break;
        case '(': addToken(LEFT_PAREN); break;
        case ')': addToken(RIGHT_PAREN); break;
        case ',': addToken(COMMA); break;
        case '.': addToken(DOT); break;
        case ';': addToken(SEMICOLON); break;
        case '+': addToken(PLUS); break;
        case '-': addToken(MINUS); break;
        case '*': addToken(STAR); break;
        case '/': 
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
            } else {
                addToken(SLASH);
            }
            break;
        case '%': addToken(PERCENT); break;
        case '!': addToken(match('=') ? BANG_EQUAL : BANG); break;
        case '=': addToken(match('=') ? EQUAL_EQUAL : EQUAL); break;
        case '<': addToken(match('=') ? LESS_EQUAL : LESS); break;
        case '>': addToken(match('=') ? GREATER_EQUAL : GREATER); break;
        case '"': stringToken(); break;
        case '\n': line++; break;
        case ' ':
        case '\r':
        case '\t': break;
        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                std::cerr << "Error: Unexpected character '" << c << "' at line " << line << std::endl;
            }
            break;
    }
}

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    
    tokens.push_back({TokenType::ENDFILE, "", std::monostate{}, line});
    return tokens;
}

VM::VM() {}
VM::~VM() {}

bool VM::isTruthy(const Value& value) {
    if (std::holds_alternative<std::monostate>(value)) return false;
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
    return true;
}

bool VM::isEqual(const Value& a, const Value& b) {
    if (a.index() != b.index()) return false;
    if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) return true;
    if (std::holds_alternative<bool>(a)) return std::get<bool>(a) == std::get<bool>(b);
    if (std::holds_alternative<int>(a)) return std::get<int>(a) == std::get<int>(b);
    if (std::holds_alternative<float>(a)) return std::get<float>(a) == std::get<float>(b);
    if (std::holds_alternative<std::string>(a)) return std::get<std::string>(a) == std::get<std::string>(b);
    return false;
}

std::string VM::valueToString(const Value& value) {
    if (std::holds_alternative<std::monostate>(value)) return "nichts";
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "wahr" : "falsch";
    if (std::holds_alternative<int>(value)) return std::to_string(std::get<int>(value));
    if (std::holds_alternative<float>(value)) return std::to_string(std::get<float>(value));
    if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
    return "?";
}

void VM::push(const Value& value) {
    stack.push_back({value, nullptr});
}

Value VM::pop() {
    if (stack.empty()) return std::monostate{};
    Value val = stack.back().value;
    stack.pop_back();
    return val;
}

Value VM::peek(int distance) {
    if (stack.size() <= distance) return std::monostate{};
    return stack[stack.size() - 1 - distance].value;
}

void VM::setGlobal(const std::string& name, const Value& value) {
    globals[name] = value;
}

Value VM::getGlobal(const std::string& name) {
    auto it = globals.find(name);
    if (it != globals.end()) return it->second;
    return std::monostate{};
}

void VM::defineNative(const std::string& name, NativeFunction fn) {
    natives[name] = fn;
}

Value VM::call(ScriptFunction function, const std::vector<Value>& args) {
    if (natives.find(function->name) != natives.end()) {
        return natives[function->name](this, args);
    }
    return std::monostate{};
}

Value VM::interpret(const std::string& source) {
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();
    
    Parser parser(this);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse(source);
    
    return std::monostate{};
}

Parser::Parser(VM* vm) : vm(vm) {}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token& Parser::peek() {
    return tokens[current];
}

Token& Parser::previous() {
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::ENDFILE;
}

void Parser::error(const std::string& message) {
    std::cerr << "Parse error: " << message << std::endl;
}

std::vector<std::shared_ptr<Stmt>> Parser::parse(const std::string& source) {
    Scanner scanner(source);
    tokens = scanner.scanTokens();
    current = 0;
    
    std::vector<std::shared_ptr<Stmt>> statements;
    
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    
    return statements;
}

std::shared_ptr<Stmt> Parser::declaration() {
    if (match(VAR)) return varDeclaration();
    if (match(FUN)) return functionDeclaration("fun");
    if (match(CLASS)) return classDeclaration();
    return statement();
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match(IF)) return ifStatement();
    if (match(WHILE)) return whileStatement();
    if (match(FOR)) return forStatement();
    if (match(RETURN)) return returnStatement();
    if (match(LEFT_BRACE)) return block();
    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::varDeclaration() {
    Token name = peek();
    advance();
    
    std::shared_ptr<Expr> initializer = nullptr;
    if (match(EQUAL)) {
        initializer = expression();
    }
    
    match(SEMICOLON);
    return nullptr;
}

std::shared_ptr<Stmt> Parser::functionDeclaration(const std::string& kind) {
    Token name = peek();
    advance();
    
    match(LEFT_PAREN);
    match(RIGHT_PAREN);
    
    match(LEFT_BRACE);
    return nullptr;
}

std::shared_ptr<Stmt> Parser::classDeclaration() {
    Token name = peek();
    advance();
    
    match(LEFT_BRACE);
    while (!check(RIGHT_BRACE) && !isAtEnd()) {
        declaration();
    }
    match(RIGHT_BRACE);
    return nullptr;
}

std::shared_ptr<Stmt> Parser::ifStatement() {
    match(LEFT_PAREN);
    expression();
    match(RIGHT_PAREN);
    
    statement();
    if (match(ELSE)) {
        statement();
    }
    return nullptr;
}

std::shared_ptr<Stmt> Parser::whileStatement() {
    match(LEFT_PAREN);
    expression();
    match(RIGHT_PAREN);
    statement();
    return nullptr;
}

std::shared_ptr<Stmt> Parser::forStatement() {
    match(LEFT_PAREN);
    match(VAR);
    varDeclaration();
    match(SEMICOLON);
    expression();
    match(SEMICOLON);
    expression();
    match(RIGHT_PAREN);
    statement();
    return nullptr;
}

std::shared_ptr<Stmt> Parser::returnStatement() {
    Value value = std::monostate{};
    if (!check(SEMICOLON)) {
        value = std::get<Value>(*expression());
    }
    match(SEMICOLON);
    return nullptr;
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    expression();
    match(SEMICOLON);
    return nullptr;
}

std::shared_ptr<Stmt> Parser::block() {
    while (!check(RIGHT_BRACE) && !isAtEnd()) {
        declaration();
    }
    match(RIGHT_BRACE);
    return nullptr;
}

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    return orExpr();
}

std::shared_ptr<Expr> Parser::orExpr() {
    return andExpr();
}

std::shared_ptr<Expr> Parser::andExpr() {
    return equality();
}

std::shared_ptr<Expr> Parser::equality() {
    return comparison();
}

std::shared_ptr<Expr> Parser::comparison() {
    return term();
}

std::shared_ptr<Expr> Parser::term() {
    return factor();
}

std::shared_ptr<Expr> Parser::factor() {
    return unary();
}

std::shared_ptr<Expr> Parser::unary() {
    return call();
}

std::shared_ptr<Expr> Parser::call() {
    return primary();
}

std::shared_ptr<Expr> Parser::primary() {
    if (match(FALSE)) return nullptr;
    if (match(TRUE)) return nullptr;
    if (match(NULLPTR)) return nullptr;
    if (match(NUMBER)) return nullptr;
    if (match(STRING)) return nullptr;
    if (match(IDENTIFIER)) return nullptr;
    if (match(LEFT_PAREN)) {
        auto expr = expression();
        match(RIGHT_PAREN);
        return expr;
    }
    return nullptr;
}

} // namespace EasyScript
