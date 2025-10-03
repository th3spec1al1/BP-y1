#pragma once
#include "expressions.h"

struct ExprStmt : Statement {
  Expression *expr;
  ExprStmt(Expression *e) : expr(e) {}
  ~ExprStmt() { delete expr; }
  void exec(Scope *env) override { expr->eval(env); }
};

struct AssignStmt : Statement {
  enum Op { EQ, ADD_EQ, SUB_EQ, MUL_EQ, DIV_EQ, MOD_EQ, POW_EQ } op;
  Expression *target;
  Expression *valueExpr;
  AssignStmt(Op op, Expression *target, Expression *val)
      : op(op), target(target), valueExpr(val) {}
  ~AssignStmt() {
    delete target;
    delete valueExpr;
  }
  void exec(Scope *env) override {
    if (auto var = dynamic_cast<VarExpression *>(target)) {
      std::string name = var->name;
      if (op == EQ) {
        Value val = valueExpr->eval(env);
        env->variables[name] = val;
        if (auto f = std::get_if<std::shared_ptr<FuncObject>>(&val.data)) {
          if (*f && !(*f)->is_standart_func)
            (*f)->name = name;
        }
      } else {
        Value *curPtr = env->find(name);
        if (!curPtr)
          throw RuntimeError("Undefined variable: " + name);
        Value curVal = *curPtr;
        Value rightVal = valueExpr->eval(env);
        BinaryExpression::Op opType;
        switch (op) {
        case ADD_EQ:
          opType = BinaryExpression::ADD;
          break;
        case SUB_EQ:
          opType = BinaryExpression::SUB;
          break;
        case MUL_EQ:
          opType = BinaryExpression::MUL;
          break;
        case DIV_EQ:
          opType = BinaryExpression::DIV;
          break;
        case MOD_EQ:
          opType = BinaryExpression::MOD;
          break;
        case POW_EQ:
          opType = BinaryExpression::POW;
          break;
        default:
          opType = BinaryExpression::ADD;
        }
        Expression *lExpr;
        if (std::holds_alternative<double>(curVal.data))
          lExpr = new NumberExpression(std::get<double>(curVal.data));
        else if (std::holds_alternative<bool>(curVal.data))
          lExpr = new BoolExpression(std::get<bool>(curVal.data));
        else if (auto sp =
                     std::get_if<std::shared_ptr<std::string>>(&curVal.data))
          lExpr = new StringExpression(**sp);
        else if (std::holds_alternative<NullType>(curVal.data))
          lExpr = new NilExpression();
        else if (auto sp =
                     std::get_if<std::shared_ptr<ListObject>>(&curVal.data)) {
          ListExpression *le = new ListExpression();
          for (auto &elem : (*sp)->elements) {
            if (std::holds_alternative<double>(elem.data))
              le->elements.push_back(
                  new NumberExpression(std::get<double>(elem.data)));
            else if (std::holds_alternative<bool>(elem.data))
              le->elements.push_back(
                  new BoolExpression(std::get<bool>(elem.data)));
            else if (auto p =
                         std::get_if<std::shared_ptr<std::string>>(&elem.data))
              le->elements.push_back(new StringExpression(**p));
            else if (std::holds_alternative<NullType>(elem.data))
              le->elements.push_back(new NilExpression());
            else
              le->elements.push_back(new NilExpression());
          }
          lExpr = le;
        } else {
          lExpr = new NilExpression();
        }
        Expression *rExpr;
        if (std::holds_alternative<double>(rightVal.data))
          rExpr = new NumberExpression(std::get<double>(rightVal.data));
        else if (std::holds_alternative<bool>(rightVal.data))
          rExpr = new BoolExpression(std::get<bool>(rightVal.data));
        else if (auto sp =
                     std::get_if<std::shared_ptr<std::string>>(&rightVal.data))
          rExpr = new StringExpression(**sp);
        else if (std::holds_alternative<NullType>(rightVal.data))
          rExpr = new NilExpression();
        else if (auto sp =
                     std::get_if<std::shared_ptr<ListObject>>(&rightVal.data)) {
          ListExpression *re = new ListExpression();
          for (auto &elem : (*sp)->elements) {
            if (std::holds_alternative<double>(elem.data))
              re->elements.push_back(
                  new NumberExpression(std::get<double>(elem.data)));
            else if (std::holds_alternative<bool>(elem.data))
              re->elements.push_back(
                  new BoolExpression(std::get<bool>(elem.data)));
            else if (auto p =
                         std::get_if<std::shared_ptr<std::string>>(&elem.data))
              re->elements.push_back(new StringExpression(**p));
            else if (std::holds_alternative<NullType>(elem.data))
              re->elements.push_back(new NilExpression());
            else
              re->elements.push_back(new NilExpression());
          }
          rExpr = re;
        } else {
          rExpr = new NilExpression();
        }
        Value newVal = BinaryExpression(opType, lExpr, rExpr).eval(env);
        env->variables[name] = newVal;
      }
      return;
    }
    if (auto idxExp = dynamic_cast<IndexExpression *>(target)) {
      Value tar = idxExp->target->eval(env);
      auto listPtrPtr = std::get_if<std::shared_ptr<ListObject>>(&tar.data);
      if (!listPtrPtr)
        throw RuntimeError("Indexed assignment target is not a list");
      std::shared_ptr<ListObject> listObj = *listPtrPtr;
      if (idxExp->isSlice) {
        throw RuntimeError("Slice assignment not supported");
      }
      Value idxVal = idxExp->index->eval(env);
      int idx = 0;
      if (auto pd = std::get_if<double>(&idxVal.data))
        idx = (int)std::floor(*pd);
      else if (auto pb = std::get_if<bool>(&idxVal.data))
        idx = *pb ? 1 : 0;
      else
        throw RuntimeError("Index must be number for assignment");
      if (idx < 0)
        idx += listObj->elements.size();
      if (idx < 0 || idx >= (int)listObj->elements.size())
        throw RuntimeError("List index out of range");
      if (op == EQ) {
        listObj->elements[idx] = valueExpr->eval(env);
      } else {
        Value curVal = listObj->elements[idx];
        Value rightVal = valueExpr->eval(env);
        BinaryExpression::Op opType;
        switch (op) {
        case ADD_EQ:
          opType = BinaryExpression::ADD;
          break;
        case SUB_EQ:
          opType = BinaryExpression::SUB;
          break;
        case MUL_EQ:
          opType = BinaryExpression::MUL;
          break;
        case DIV_EQ:
          opType = BinaryExpression::DIV;
          break;
        case MOD_EQ:
          opType = BinaryExpression::MOD;
          break;
        case POW_EQ:
          opType = BinaryExpression::POW;
          break;
        default:
          opType = BinaryExpression::ADD;
        }
        Expression *lExpr =
            std::holds_alternative<double>(curVal.data)
                ? (Expression *)new NumberExpression(
                      std::get<double>(curVal.data))
            : std::holds_alternative<bool>(curVal.data)
                ? (Expression *)new BoolExpression(std::get<bool>(curVal.data))
            : std::holds_alternative<std::shared_ptr<std::string>>(curVal.data)
                ? (Expression *)new StringExpression(
                      *std::get<std::shared_ptr<std::string>>(curVal.data))
            : std::holds_alternative<NullType>(curVal.data)
                ? (Expression *)new NilExpression()
                : (Expression *)new NilExpression();
        Expression *rExpr =
            std::holds_alternative<double>(rightVal.data)
                ? (Expression *)new NumberExpression(
                      std::get<double>(rightVal.data))
            : std::holds_alternative<bool>(rightVal.data)
                ? (Expression *)new BoolExpression(
                      std::get<bool>(rightVal.data))
            : std::holds_alternative<std::shared_ptr<std::string>>(
                  rightVal.data)
                ? (Expression *)new StringExpression(
                      *std::get<std::shared_ptr<std::string>>(rightVal.data))
            : std::holds_alternative<NullType>(rightVal.data)
                ? (Expression *)new NilExpression()
                : (Expression *)new NilExpression();
        Value newVal = BinaryExpression(opType, lExpr, rExpr).eval(env);
        listObj->elements[idx] = newVal;
      }
      return;
    }
    throw RuntimeError("Invalid assignment target");
  }
};

struct IfStmt : Statement {
  struct Branch {
    Expression *cond;
    std::vector<Statement *> stmts;
  };
  std::vector<Branch> ifBranches;
  std::vector<Statement *> elseBranch;
  ~IfStmt() {
    for (auto &br : ifBranches) {
      delete br.cond;
      for (Statement *s : br.stmts)
        delete s;
    }
    for (Statement *s : elseBranch)
      delete s;
  }
  void exec(Scope *env) override {
    bool executed = false;
    for (auto &br : ifBranches) {
      Value condVal = br.cond->eval(env);
      if (IsTrue(condVal)) {
        executed = true;
        for (Statement *s : br.stmts)
          s->exec(env);
        break;
      }
    }
    if (!executed && !elseBranch.empty()) {
      for (Statement *s : elseBranch)
        s->exec(env);
    }
  }
};

struct WhileStmt : Statement {
  Expression *cond;
  std::vector<Statement *> body;
  WhileStmt(Expression *c) : cond(c) {}
  ~WhileStmt() {
    delete cond;
    for (Statement *s : body)
      delete s;
  }
  void exec(Scope *env) override {
    while (true) {
      Value condVal = cond->eval(env);
      if (!IsTrue(condVal))
        break;
      try {
        for (Statement *s : body)
          s->exec(env);
      } catch (const ContinueException &) {
        continue;
      } catch (const BreakException &) {
        break;
      }
    }
  }
};

struct ForStmt : Statement {
  std::string varName;
  Expression *iterable;
  std::vector<Statement *> body;
  ForStmt(const std::string &name, Expression *iter)
      : varName(name), iterable(iter) {}
  ~ForStmt() {
    delete iterable;
    for (Statement *s : body)
      delete s;
  }
  void exec(Scope *env) override {
    Value iterVal = iterable->eval(env);
    if (auto listPtr =
            std::get_if<std::shared_ptr<ListObject>>(&iterVal.data)) {
      for (size_t i = 0; i < (*listPtr)->elements.size(); ++i) {
        env->variables[varName] = (*listPtr)->elements[i];
        try {
          for (Statement *s : body)
            s->exec(env);
        } catch (const ContinueException &) {
          continue;
        } catch (const BreakException &) {
          goto loop_end;
        }
      }
    } else if (auto strPtr =
                   std::get_if<std::shared_ptr<std::string>>(&iterVal.data)) {
      const std::string &str = **strPtr;
      for (size_t i = 0; i < str.size(); ++i) {
        env->variables[varName] =
            Value(std::make_shared<std::string>(std::string(1, str[i])));
        try {
          for (Statement *s : body)
            s->exec(env);
        } catch (const ContinueException &) {
          continue;
        } catch (const BreakException &) {
          goto loop_end;
        }
      }
    } else {
      throw RuntimeError("Cannot iterate over this type");
    }
  loop_end:;
  }
};

struct BreakStmt : Statement {
  void exec(Scope *env) override { throw BreakException(); }
};

struct ContinueStmt : Statement {
  void exec(Scope *env) override { throw ContinueException(); }
};

struct ReturnStmt : Statement {
  Expression *valueExpr;
  ReturnStmt(Expression *expr) : valueExpr(expr) {}
  ~ReturnStmt() {
    if (valueExpr)
      delete valueExpr;
  }
  void exec(Scope *env) override {
    Value val = valueExpr ? valueExpr->eval(env) : Value(NullType{});
    throw ReturnException(val);
  }
};
