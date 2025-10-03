#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct NullType {};

struct Value;
struct Scope;
struct Expression;
struct Statement;

struct ListObject {
  std::vector<Value> elements;
};

struct FuncObject {
  bool is_standart_func;
  std::function<Value(const std::vector<Value> &)> func;
  std::vector<std::string> params;
  std::vector<Statement *> body;
  std::string name;
  Scope *closure_scope;
  FuncObject() : is_standart_func(false), closure_scope(nullptr) {}
  ~FuncObject() {
    for (Statement *s : body)
      delete s;
    body.clear();
  }
};

struct Value {
  using VariantType =
      std::variant<NullType, double, bool, std::shared_ptr<std::string>,
                   std::shared_ptr<ListObject>, std::shared_ptr<FuncObject>>;
  VariantType data;
  Value() : data(NullType{}) {}
  Value(const NullType &) : data(NullType{}) {}
  Value(double num) : data(num) {}
  Value(bool b) : data(b) {}
  Value(const std::string &str) : data(std::make_shared<std::string>(str)) {}
  Value(std::shared_ptr<std::string> sp) : data(sp) {}
  Value(std::shared_ptr<ListObject> lp) : data(lp) {}
  Value(std::shared_ptr<FuncObject> fp) : data(fp) {}
};

struct Scope {
  Scope *parent;
  std::unordered_map<std::string, Value> variables;
  Scope(Scope *parentEnv = nullptr) : parent(parentEnv) {}
  Value *find(const std::string &name) {
    auto it = variables.find(name);
    if (it != variables.end())
      return &it->second;
    if (parent)
      return parent->find(name);
    return nullptr;
  }
};

struct ReturnException : public std::exception {
  Value value;
  ReturnException(const Value &val) : value(val) {}
};
struct BreakException : public std::exception {};
struct ContinueException : public std::exception {};

struct RuntimeError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct Expression {
  virtual ~Expression() = default;
  virtual Value eval(Scope *env) = 0;
};
struct Statement {
  virtual ~Statement() = default;
  virtual void exec(Scope *env) = 0;
};

static Scope *global_scope = nullptr;
static std::istream *input_stream = nullptr;
static std::ostream *output_stream = nullptr;
static std::vector<std::string> call_stack;

struct Token {
  enum Type {
    END_OF_FILE,
    NUMBER,
    STRING_LITERAL,
    TRUE,
    FALSE,
    NIL,
    IDENT,
    IF,
    THEN,
    ELSE,
    END,
    WHILE,
    FOR,
    IN,
    FUNCTION,
    RETURN,
    BREAK,
    CONTINUE,
    AND,
    OR,
    NOT,
    PLUS,
    MINUS,
    MULT,
    SLASH,
    PERCENT,
    CARET,
    EQEQ,
    NEQ,
    LT,
    RT,
    LE,
    RE,
    EQ,
    PLUS_EQ,
    MINUS_EQ,
    MULT_EQ,
    SLASH_EQ,
    PERCENT_EQ,
    CARET_EQ,
    LBRACKET,
    RBRACKET,
    LPAREN,
    RPAREN,
    COMMA,
    COLON,
    NEWLINE
  } type;
  std::string str;
  double num_value;
  Token(Type t = END_OF_FILE, std::string txt = "", double num = 0)
      : type(t), str(std::move(txt)), num_value(num) {}
};
