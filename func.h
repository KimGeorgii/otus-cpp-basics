// 212-Ким-calc
// объявление классов и функций

#pragma once

#include <iostream>
#include <memory>
#include <string>

// 212-Ким-Перечисление типов токенов
enum class Token {
    Number,
    Operator,
    End,
    Lbrace,
    Rbrace,
    Name,
};

// 212-Ким-Класс лексера: преобразует поток символов в токены
class Lexer {
public:
    explicit Lexer(std::istream& in);

    Lexer(const Lexer& other) = delete;
    Lexer& operator=(const Lexer& other) = delete;

    // 212-Ким-Возвращает следующий токен из потока
    Token next_token();

    int get_number() const { return number_; }
    std::string get_operator() const { return operator_; }
    std::string get_name() const { return name_; }

private:
    // 212-Ким-Состояния конечного автомата лексера
    enum class State {
        Empty,
        ReadNumber,
        ReadName,
        End,
    };

    char next_char();
    bool end() const;
    bool is_operator(char ch) const;

    State state_;
    std::string name_;
    int number_;
    std::string operator_;
    char ch_;
    std::istream& in_;
};

// 212-Ким-Базовый узел абстрактного синтаксического дерева
class ASTNode {
public:
    explicit ASTNode(const std::string& repr);
    ASTNode(const std::string& repr, ASTNode* lhs, ASTNode* rhs);

    ASTNode(const ASTNode& other) = delete;
    ASTNode& operator=(const ASTNode& other) = delete;

    virtual ~ASTNode();

    std::string repr() const { return repr_; }

    // 212-Ким-Печать дерева на экран
    void print(std::ostream& out) const;

private:
    void inner_print(std::ostream& out, size_t indent) const;

    std::string repr_;
    ASTNode* lhs_;
    ASTNode* rhs_;
};

// 212-Ким-Узел целочисленной константы
class Number : public ASTNode {
public:
    explicit Number(int val)
        : ASTNode(std::to_string(val))
        , val_(val) {}

    int value() const { return val_; }

private:
    int val_;
};

// 212-Ким-Узел имени переменной
class Variable : public ASTNode {
public:
    explicit Variable(const std::string& name)
        : ASTNode(name) {}
};

// 212-Ким-Узел бинарной операции
class BinaryOp : public ASTNode {
public:
    BinaryOp(const std::string& op, ASTNode* lhs, ASTNode* rhs)
        : ASTNode(op, lhs, rhs) {}
};

// 212-Ким-Узел операции сложения
class Add : public BinaryOp {
public:
    Add(ASTNode* lhs, ASTNode* rhs)
        : BinaryOp("+", lhs, rhs) {}
};

// 212-Ким-Узел операции вычитания
class Sub : public BinaryOp {
public:
    Sub(ASTNode* lhs, ASTNode* rhs)
        : BinaryOp("-", lhs, rhs) {}
};

// 212-Ким-Узел операции умножения
class Mul : public BinaryOp {
public:
    Mul(ASTNode* lhs, ASTNode* rhs)
        : BinaryOp("*", lhs, rhs) {}
};

// 212-Ким-Узел операции деления
class Div : public BinaryOp {
public:
    Div(ASTNode* lhs, ASTNode* rhs)
        : BinaryOp("/", lhs, rhs) {}
};

// 212-Ким-Класс синтаксического анализатора (рекурсивный спуск)
class Parser {
public:
    explicit Parser(Lexer& lexer);

    Parser(const Parser& other) = delete;
    Parser& operator=(const Parser& other) = delete;

    ~Parser() = default;

    // 212-Ким-Точка входа: разбирает всё выражение
    ASTNode* parse();

private:
    void next_token();

    ASTNode* expr();
    ASTNode* term();
    ASTNode* prim();

    Lexer& lexer_;
    Token tok_;
    bool has_error_;
};

// 212-Ким-Функция чтения одного выражения и печати дерева
// возвращает 0 при успехе и 1 при ошибке разбора
int run_calculator(std::istream& in, std::ostream& out);