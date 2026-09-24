// 212-Ким-calc
// реализации классов и функций

#include "func.h"

#include <cctype>

// 212-Ким-Конструктор лексера: сразу читает первый символ
Lexer::Lexer(std::istream& in)
    : state_(State::Empty)
    , number_(0)
    , operator_("")
    , ch_(0)
    , in_(in) {
    next_char();
}

// 212-Ким-Чтение очередного символа из потока
char Lexer::next_char() {
    in_.get(ch_);
    return ch_;
}

// 212-Ким-Проверка конца входного потока
bool Lexer::end() const {
    return in_.eof() || ch_ == '\n';
}

// 212-Ким-Проверка, является ли символ оператором
bool Lexer::is_operator(char ch) const {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

// 212-Ким-Логика конечного автомата: разбор одного токена
Token Lexer::next_token() {
    for (;;) {
        switch (state_) {
        case State::End:
            return Token::End;

        case State::ReadNumber:
            if (end()) {
                state_ = State::End;
                return Token::Number;
            }
            if (std::isdigit(ch_)) {
                number_ = 10 * number_ + (ch_ - '0');
                next_char();
                break;
            }
            state_ = State::Empty;
            return Token::Number;

        case State::ReadName:
            if (end()) {
                state_ = State::End;
                return Token::Name;
            }
            if (std::isalpha(ch_) || std::isdigit(ch_)) {
                name_ += ch_;
                next_char();
                break;
            }
            state_ = State::Empty;
            return Token::Name;

        case State::Empty:
            if (end()) {
                state_ = State::End;
                return Token::End;
            }
            if (std::isspace(ch_)) {
                next_char();
                break;
            }
            if (is_operator(ch_)) {
                operator_ = ch_;
                next_char();
                return Token::Operator;
            }
            if (ch_ == '(') {
                next_char();
                return Token::Lbrace;
            }
            if (ch_ == ')') {
                next_char();
                return Token::Rbrace;
            }
            if (std::isdigit(ch_)) {
                number_ = ch_ - '0';
                state_ = State::ReadNumber;
                next_char();
                break;
            }
            if (std::isalpha(ch_)) {
                name_ = ch_;
                state_ = State::ReadName;
                next_char();
                break;
            }
            // 212-Ким-Неизвестный символ: пропускаем, чтобы не зациклиться
            next_char();
            break;
        }
    }
}

// 212-Ким-Конструктор узла без потомков
ASTNode::ASTNode(const std::string& repr)
    : repr_(repr)
    , lhs_(nullptr)
    , rhs_(nullptr) {}

// 212-Ким-Конструктор узла с потомками
ASTNode::ASTNode(const std::string& repr, ASTNode* lhs, ASTNode* rhs)
    : repr_(repr)
    , lhs_(lhs)
    , rhs_(rhs) {}

// 212-Ким-Деструктор: освобождает динамическую память потомков
ASTNode::~ASTNode() {
    delete lhs_;
    delete rhs_;
}

// 212-Ким-Печать дерева (обёртка)
void ASTNode::print(std::ostream& out) const {
    inner_print(out, 0);
}

// 212-Ким-Рекурсивная печать дерева, уложенного набок
void ASTNode::inner_print(std::ostream& out, size_t indent) const {
    if (lhs_) {
        lhs_->inner_print(out, indent + 1);
    }
    for (size_t i = 0; i < indent; ++i) {
        out << "    ";
    }
    out << repr_ << '\n';
    if (rhs_) {
        rhs_->inner_print(out, indent + 1);
    }
}

// 212-Ким-Конструктор парсера
Parser::Parser(Lexer& lexer)
    : lexer_(lexer)
    , tok_(Token::End)
    , has_error_(false) {}

// 212-Ким-Запрос следующего токена у лексера
void Parser::next_token() {
    tok_ = lexer_.next_token();
}

// 212-Ким-Разбор выражения: сложение и вычитание
ASTNode* Parser::expr() {
    ASTNode* root = term();
    for (;;) {
        if (tok_ != Token::Operator) {
            return root;
        }
        std::string op = lexer_.get_operator();
        if (op.front() == '+') {
            root = new Add(root, term());
        }
        else if (op.front() == '-') {
            root = new Sub(root, term());
        }
        else {
            return root;
        }
    }
}

// 212-Ким-Разбор терма: умножение и деление
ASTNode* Parser::term() {
    ASTNode* root = prim();
    for (;;) {
        if (tok_ != Token::Operator) {
            return root;
        }
        std::string op = lexer_.get_operator();
        if (op.front() == '*') {
            root = new Mul(root, prim());
        }
        else if (op.front() == '/') {
            root = new Div(root, prim());
        }
        else {
            return root;
        }
    }
}

// 212-Ким-Разбор первичного выражения: число, имя или скобки
ASTNode* Parser::prim() {
    ASTNode* node = nullptr;
    next_token();

    switch (tok_) {
    case Token::Number:
        node = new Number(lexer_.get_number());
        next_token();
        return node;

    case Token::Name:
        node = new Variable(lexer_.get_name());
        next_token();
        return node;

    case Token::Lbrace: {
        ASTNode* inner = expr();
        if (tok_ != Token::Rbrace) {
            has_error_ = true;
            delete inner;
            return nullptr;
        }
        next_token();
        return inner;
    }

    default:
        has_error_ = true;
        return nullptr;
    }
}

// 212-Ким-Точка входа парсера
ASTNode* Parser::parse() {
    has_error_ = false;
    ASTNode* ast = expr();
    if (has_error_) {
        delete ast;
        return nullptr;
    }
    if (tok_ != Token::End) {
        delete ast;
        return nullptr;
    }
    return ast;
}

// 212-Ким-Чтение одного выражения и печать дерева
int run_calculator(std::istream& in, std::ostream& out) {
    Lexer lexer(in);
    Parser parser(lexer);

    ASTNode* ast = parser.parse();
    if (!ast) {
        out << "Error: invalid expression" << std::endl;
        return 1;
    }

    ast->print(out);
    delete ast;
    return 0;
}