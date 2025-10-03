#pragma once
#include "data.h"

static bool IsTrue(const Value &v) {
  if (std::holds_alternative<NullType>(v.data))
    return false;
  if (auto pd = std::get_if<double>(&v.data))
    return *pd != 0.0;
  if (auto pb = std::get_if<bool>(&v.data))
    return *pb;
  if (auto ps = std::get_if<std::shared_ptr<std::string>>(&v.data))
    return !(*ps)->empty();
  if (auto pl = std::get_if<std::shared_ptr<ListObject>>(&v.data))
    return !(*pl)->elements.empty();
  if (std::get_if<std::shared_ptr<FuncObject>>(&v.data))
    return true;
  return false;
}

struct NumberExpression : Expression {
  double value;
  NumberExpression(double v) : value(v) {}
  Value eval(Scope *env) override { return Value(value); }
};

struct BoolExpression : Expression {
  bool value;
  BoolExpression(bool v) : value(v) {}
  Value eval(Scope *env) override { return Value(value); }
};

struct StringExpression : Expression {
  std::string value;
  StringExpression(const std::string &v) : value(v) {}
  Value eval(Scope *env) override {
    return Value(std::make_shared<std::string>(value));
  }
};

struct NilExpression : Expression {
  Value eval(Scope *env) override { return Value(NullType{}); }
};

struct ListExpression : Expression {
  std::vector<Expression *> elements;
  ~ListExpression() {
    for (Expression *e : elements)
      delete e;
  }
  Value eval(Scope *env) override {
    auto listPtr = std::make_shared<ListObject>();
    for (Expression *e : elements)
      listPtr->elements.push_back(e->eval(env));
    return Value(listPtr);
  }
};

struct VarExpression : Expression {
  std::string name;
  VarExpression(const std::string &n) : name(n) {}
  Value eval(Scope *env) override {
    Value *vp = env->find(name);
    if (!vp)
      throw RuntimeError("Undefined variable: " + name);
    return *vp;
  }
};

struct BinaryExpression : Expression {
  enum Op { ADD, SUB, MUL, DIV, MOD, POW, EQ, NEQ, LT, RT, LE, RE, AND, OR } op;
  Expression *left;
  Expression *right;
  BinaryExpression(Op op, Expression *l, Expression *r)
      : op(op), left(l), right(r) {}
  ~BinaryExpression() {
    delete left;
    delete right;
  }
  Value eval(Scope *env) override {
    if (op == AND) {
      Value lv = left->eval(env);
      if (!IsTrue(lv))
        return Value(false);
      Value rv = right->eval(env);
      return Value(IsTrue(rv));
    }
    if (op == OR) {
      Value lv = left->eval(env);
      if (IsTrue(lv))
        return Value(true);
      Value rv = right->eval(env);
      return Value(IsTrue(rv));
    }

    Value lv = left->eval(env);
    Value rv = right->eval(env);
    auto getDouble = [&](const Value &v) -> double {
      if (auto pd = std::get_if<double>(&v.data))
        return *pd;
      if (auto pb = std::get_if<bool>(&v.data))
        return *pb ? 1.0 : 0.0;
      throw RuntimeError("Type error: expected number");
    };
    switch (op) {
    case ADD:
      if ((std::holds_alternative<double>(lv.data) ||
           std::holds_alternative<bool>(lv.data)) &&
          (std::holds_alternative<double>(rv.data) ||
           std::holds_alternative<bool>(rv.data))) {
        return Value(getDouble(lv) + getDouble(rv));
      }
      if (auto ls = std::get_if<std::shared_ptr<std::string>>(&lv.data)) {
        if (auto rs = std::get_if<std::shared_ptr<std::string>>(&rv.data)) {
          return Value(std::make_shared<std::string>(**ls + **rs));
        }
      }
      if (auto ll = std::get_if<std::shared_ptr<ListObject>>(&lv.data)) {
        if (auto rl = std::get_if<std::shared_ptr<ListObject>>(&rv.data)) {
          auto newList = std::make_shared<ListObject>();
          newList->elements = (*ll)->elements;
          newList->elements.insert(newList->elements.end(),
                                   (*rl)->elements.begin(),
                                   (*rl)->elements.end());
          return Value(newList);
        }
      }
      throw RuntimeError("Type error: cannot add these types");
    case SUB:
      if ((std::holds_alternative<double>(lv.data) ||
           std::holds_alternative<bool>(lv.data)) &&
          (std::holds_alternative<double>(rv.data) ||
           std::holds_alternative<bool>(rv.data))) {
        return Value(getDouble(lv) - getDouble(rv));
      }
      if (auto ls = std::get_if<std::shared_ptr<std::string>>(&lv.data)) {
        if (auto rs = std::get_if<std::shared_ptr<std::string>>(&rv.data)) {
          const std::string &main = **ls;
          const std::string &suffix = **rs;
          if (main.size() >= suffix.size() &&
              main.rfind(suffix) == main.size() - suffix.size()) {
            return Value(std::make_shared<std::string>(
                main.substr(0, main.size() - suffix.size())));
          } else {
            return Value(std::make_shared<std::string>(main));
          }
        }
      }
      throw RuntimeError("Type error: cannot subtract these types");
    case MUL:
      if ((std::holds_alternative<double>(lv.data) ||
           std::holds_alternative<bool>(lv.data)) &&
          (std::holds_alternative<double>(rv.data) ||
           std::holds_alternative<bool>(rv.data))) {
        return Value(getDouble(lv) * getDouble(rv));
      }
      if (auto ls = std::get_if<std::shared_ptr<std::string>>(&lv.data)) {
        if (std::holds_alternative<double>(rv.data) ||
            std::holds_alternative<bool>(rv.data)) {
          int times = (int)std::floor(getDouble(rv));
          if (times < 0)
            times = 0;
          std::string res;
          res.reserve((*ls)->size() * (times > 0 ? times : 1));
          for (int i = 0; i < times; ++i)
            res += **ls;
          return Value(std::make_shared<std::string>(res));
        }
      }
      if (auto rs = std::get_if<std::shared_ptr<std::string>>(&rv.data)) {
        if (std::holds_alternative<double>(lv.data) ||
            std::holds_alternative<bool>(lv.data)) {
          int times = (int)std::floor(getDouble(lv));
          if (times < 0)
            times = 0;
          std::string res;
          res.reserve((*rs)->size() * (times > 0 ? times : 1));
          for (int i = 0; i < times; ++i)
            res += **rs;
          return Value(std::make_shared<std::string>(res));
        }
      }
      if (auto ll = std::get_if<std::shared_ptr<ListObject>>(&lv.data)) {
        if (std::holds_alternative<double>(rv.data) ||
            std::holds_alternative<bool>(rv.data)) {
          int times = (int)std::floor(getDouble(rv));
          if (times < 0)
            times = 0;
          auto newList = std::make_shared<ListObject>();
          for (int i = 0; i < times; ++i) {
            newList->elements.insert(newList->elements.end(),
                                     (*ll)->elements.begin(),
                                     (*ll)->elements.end());
          }
          return Value(newList);
        }
      }
      if (auto rl = std::get_if<std::shared_ptr<ListObject>>(&rv.data)) {
        if (std::holds_alternative<double>(lv.data) ||
            std::holds_alternative<bool>(lv.data)) {
          int times = (int)std::floor(getDouble(lv));
          if (times < 0)
            times = 0;
          auto newList = std::make_shared<ListObject>();
          for (int i = 0; i < times; ++i) {
            newList->elements.insert(newList->elements.end(),
                                     (*rl)->elements.begin(),
                                     (*rl)->elements.end());
          }
          return Value(newList);
        }
      }
      throw RuntimeError("Type error: cannot multiply these types");
    case DIV: {
      double d1 = getDouble(lv);
      double d2 = getDouble(rv);
      if (d2 == 0.0)
        throw RuntimeError("Division by zero");
      return Value(d1 / d2);
    }
    case MOD: {
      double d1 = getDouble(lv);
      double d2 = getDouble(rv);
      if (d2 == 0.0)
        throw RuntimeError("Modulo by zero");
      return Value(std::fmod(d1, d2));
    }
    case POW:
      return Value(std::pow(getDouble(lv), getDouble(rv)));
    case EQ:
      if (std::holds_alternative<NullType>(lv.data) ||
          std::holds_alternative<NullType>(rv.data)) {
        return Value(std::holds_alternative<NullType>(lv.data) &&
                     std::holds_alternative<NullType>(rv.data));
      }
      if ((std::holds_alternative<double>(lv.data) ||
           std::holds_alternative<bool>(lv.data)) &&
          (std::holds_alternative<double>(rv.data) ||
           std::holds_alternative<bool>(rv.data))) {
        return Value(getDouble(lv) == getDouble(rv));
      }
      if (auto ls = std::get_if<std::shared_ptr<std::string>>(&lv.data)) {
        if (auto rs = std::get_if<std::shared_ptr<std::string>>(&rv.data)) {
          return Value(**ls == **rs);
        }
      }
      if (auto ll = std::get_if<std::shared_ptr<ListObject>>(&lv.data)) {
        if (auto rl = std::get_if<std::shared_ptr<ListObject>>(&rv.data)) {
          if ((*ll)->elements.size() != (*rl)->elements.size())
            return Value(false);
          for (size_t i = 0; i < (*ll)->elements.size(); ++i) {
            Value eqElem = BinaryExpression(EQ, new NumberExpression(0),
                                            new NumberExpression(0))
                               .eval(env);
          }
        }
      }
      if (auto lf = std::get_if<std::shared_ptr<FuncObject>>(&lv.data)) {
        if (auto rf = std::get_if<std::shared_ptr<FuncObject>>(&rv.data)) {
          return Value(lf->get() == rf->get());
        }
      }
      return Value(false);
    case NEQ: {
      Value eqVal = BinaryExpression(EQ, left, right).eval(env);
      return Value(!IsTrue(eqVal));
    }
    case LT:
      if ((std::holds_alternative<double>(lv.data) ||
           std::holds_alternative<bool>(lv.data)) &&
          (std::holds_alternative<double>(rv.data) ||
           std::holds_alternative<bool>(rv.data))) {
        return Value(getDouble(lv) < getDouble(rv));
      }
      if (auto ls = std::get_if<std::shared_ptr<std::string>>(&lv.data)) {
        if (auto rs = std::get_if<std::shared_ptr<std::string>>(&rv.data)) {
          return Value(**ls < **rs);
        }
      }
      throw RuntimeError("Type error: cannot compare these types");
    case RT:
      if ((std::holds_alternative<double>(lv.data) ||
           std::holds_alternative<bool>(lv.data)) &&
          (std::holds_alternative<double>(rv.data) ||
           std::holds_alternative<bool>(rv.data))) {
        return Value(getDouble(lv) > getDouble(rv));
      }
      if (auto ls = std::get_if<std::shared_ptr<std::string>>(&lv.data)) {
        if (auto rs = std::get_if<std::shared_ptr<std::string>>(&rv.data)) {
          return Value(**ls > **rs);
        }
      }
      throw RuntimeError("Type error: cannot compare these types");
    case LE:
      if ((std::holds_alternative<double>(lv.data) ||
           std::holds_alternative<bool>(lv.data)) &&
          (std::holds_alternative<double>(rv.data) ||
           std::holds_alternative<bool>(rv.data))) {
        return Value(getDouble(lv) <= getDouble(rv));
      }
      if (auto ls = std::get_if<std::shared_ptr<std::string>>(&lv.data)) {
        if (auto rs = std::get_if<std::shared_ptr<std::string>>(&rv.data)) {
          return Value(**ls <= **rs);
        }
      }
      throw RuntimeError("Type error: cannot compare these types");
    case RE:
      if ((std::holds_alternative<double>(lv.data) ||
           std::holds_alternative<bool>(lv.data)) &&
          (std::holds_alternative<double>(rv.data) ||
           std::holds_alternative<bool>(rv.data))) {
        return Value(getDouble(lv) >= getDouble(rv));
      }
      if (auto ls = std::get_if<std::shared_ptr<std::string>>(&lv.data)) {
        if (auto rs = std::get_if<std::shared_ptr<std::string>>(&rv.data)) {
          return Value(**ls >= **rs);
        }
      }
      throw RuntimeError("Type error: cannot compare these types");
    default:
      throw RuntimeError("Unknown binary operator");
    }
  }
};

struct UnaryExpression : Expression {
  enum Op { NEG, POS, NOT } op;
  Expression *expr;
  UnaryExpression(Op op, Expression *e) : op(op), expr(e) {}
  ~UnaryExpression() { delete expr; }
  Value eval(Scope *env) override {
    Value val = expr->eval(env);
    switch (op) {
    case NEG:
      if (auto pd = std::get_if<double>(&val.data))
        return Value(-*pd);
      if (auto pb = std::get_if<bool>(&val.data))
        return Value(-(*pb ? 1.0 : 0.0));
      throw RuntimeError("Type error: unary '-'");
    case POS:
      if (auto pd = std::get_if<double>(&val.data))
        return Value(*pd);
      if (auto pb = std::get_if<bool>(&val.data))
        return Value(*pb);
      throw RuntimeError("Type error: unary '+'");
    case NOT:
      return Value(!IsTrue(val));
    }
    throw RuntimeError("Unknown unary operator");
  }
};

struct IndexExpression : Expression {
  Expression *target;
  Expression *index;
  Expression *indexEnd;
  Expression *indexStep;
  bool isSlice;
  IndexExpression(Expression *target, Expression *idx, Expression *endIdx,
                  Expression *step, bool slice)
      : target(target), index(idx), indexEnd(endIdx), indexStep(step),
        isSlice(slice) {}
  ~IndexExpression() {
    delete target;
    if (index)
      delete index;
    if (indexEnd)
      delete indexEnd;
    if (indexStep)
      delete indexStep;
  }
  Value eval(Scope *env) override {
    Value tar = target->eval(env);

    if (auto sp = std::get_if<std::shared_ptr<std::string>>(&tar.data)) {
      const std::string &src = **sp;
      if (!isSlice) {
        Value idxV = index->eval(env);
        int idx = 0;
        if (auto pd = std::get_if<double>(&idxV.data))
          idx = (int)std::floor(*pd);
        else if (auto pb = std::get_if<bool>(&idxV.data))
          idx = *pb ? 1 : 0;
        else
          throw RuntimeError("Index must be number");
        if (idx < 0)
          idx += src.size();
        if (idx < 0 || idx >= (int)src.size())
          throw RuntimeError("String index out of range");
        return Value(std::make_shared<std::string>(src.substr(idx, 1)));
      } else {
        int start = 0, end = (int)src.size(), step = 1;
        if (index) {
          Value sv = index->eval(env);
          if (!std::holds_alternative<NullType>(sv.data))
            start = (int)std::floor(std::get<double>(sv.data));
        }
        if (indexEnd) {
          Value ev = indexEnd->eval(env);
          if (!std::holds_alternative<NullType>(ev.data))
            end = (int)std::floor(std::get<double>(ev.data));
        }
        if (indexStep) {
          Value pv = indexStep->eval(env);
          step = (int)std::floor(std::get<double>(pv.data));
          if (step == 0)
            throw RuntimeError("Slice step cannot be 0");
        }
        if (start < 0)
          start += src.size();
        if (end < 0)
          end += src.size();
        if (start < 0)
          start = 0;
        if (end > (int)src.size())
          end = src.size();
        std::string result;
        if (step > 0) {
          for (int i = start; i < end; i += step) {
            if (i >= 0 && i < (int)src.size())
              result.push_back(src[i]);
          }
        } else {
          for (int i = start; i > end; i += step) {
            if (i >= 0 && i < (int)src.size())
              result.push_back(src[i]);
          }
        }
        return Value(std::make_shared<std::string>(result));
      }
    }

    if (auto lp = std::get_if<std::shared_ptr<ListObject>>(&tar.data)) {
      if (!isSlice) {
        Value idxV = index->eval(env);
        int idx = 0;
        if (auto pd = std::get_if<double>(&idxV.data))
          idx = (int)std::floor(*pd);
        else if (auto pb = std::get_if<bool>(&idxV.data))
          idx = *pb ? 1 : 0;
        else
          throw RuntimeError("Index must be number");
        if (idx < 0)
          idx += (*lp)->elements.size();
        if (idx < 0 || idx >= (int)(*lp)->elements.size())
          throw RuntimeError("List index out of range");
        return (*lp)->elements[idx];
      } else {
        int start = 0, end = (int)(*lp)->elements.size(), step = 1;
        if (index) {
          Value sv = index->eval(env);
          if (!std::holds_alternative<NullType>(sv.data))
            start = (int)std::floor(std::get<double>(sv.data));
        }
        if (indexEnd) {
          Value ev = indexEnd->eval(env);
          if (!std::holds_alternative<NullType>(ev.data))
            end = (int)std::floor(std::get<double>(ev.data));
        }
        if (indexStep) {
          Value pv = indexStep->eval(env);
          step = (int)std::floor(std::get<double>(pv.data));
          if (step == 0)
            throw RuntimeError("Slice step cannot be 0");
        }
        if (start < 0)
          start += (*lp)->elements.size();
        if (end < 0)
          end += (*lp)->elements.size();
        if (start < 0)
          start = 0;
        if (end > (int)(*lp)->elements.size())
          end = (*lp)->elements.size();
        auto newList = std::make_shared<ListObject>();
        if (step > 0) {
          for (int i = start; i < end; i += step) {
            if (i >= 0 && i < (int)(*lp)->elements.size())
              newList->elements.push_back((*lp)->elements[i]);
          }
        } else {
          for (int i = start; i > end; i += step) {
            if (i >= 0 && i < (int)(*lp)->elements.size())
              newList->elements.push_back((*lp)->elements[i]);
          }
        }
        return Value(newList);
      }
    }
    throw RuntimeError("Type error: cannot index this type");
  }
};

struct CallExpression : Expression {
  Expression *funcExpr;
  std::vector<Expression *> args;
  CallExpression(Expression *func) : funcExpr(func) {}
  ~CallExpression() {
    delete funcExpr;
    for (Expression *e : args)
      delete e;
  }
  Value eval(Scope *env) override {
    Value funcVal = funcExpr->eval(env);
    auto fptr = std::get_if<std::shared_ptr<FuncObject>>(&funcVal.data);
    if (!fptr)
      throw RuntimeError("Attempt to call a non-function");
    std::shared_ptr<FuncObject> funcObj = *fptr;
    std::vector<Value> argVals;
    for (Expression *e : args)
      argVals.push_back(e->eval(env));
    if (funcObj->is_standart_func) {
      return funcObj->func(argVals);
    } else {
      if (argVals.size() != funcObj->params.size()) {
        throw RuntimeError("Function argument count mismatch");
      }
      Scope localEnv(funcObj->closure_scope);
      for (size_t i = 0; i < funcObj->params.size(); ++i) {
        localEnv.variables[funcObj->params[i]] = argVals[i];
      }
      call_stack.push_back(funcObj->name.empty() ? "<anon>" : funcObj->name);
      Value retVal = Value(NullType{});
      try {
        for (Statement *s : funcObj->body)
          s->exec(&localEnv);
      } catch (const ReturnException &re) {
        retVal = re.value;
      }
      call_stack.pop_back();
      return retVal;
    }
  }
};

struct FunctionExpression : Expression {
  std::vector<std::string> params;
  std::vector<Statement *> body;
  FunctionExpression(const std::vector<std::string> &params,
                     std::vector<Statement *> bodyStmts)
      : params(params), body(bodyStmts) {}
  Value eval(Scope *env) override {
    auto func = std::make_shared<FuncObject>();
    func->is_standart_func = false;
    func->params = params;
    func->body = body;
    func->closure_scope = global_scope;
    func->name = "";
    return Value(func);
  }
};
