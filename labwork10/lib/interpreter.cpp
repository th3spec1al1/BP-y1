#include "interpreter.h"

static void ReadInput(std::istream &input, std::string &code) {
  std::string line;
  while (std::getline(input, line)) {
    code += line + '\n';
  }
}

static void AddFunction(const std::string &name,
                        std::function<Value(const std::vector<Value> &)> func) {
  auto fobj = std::make_shared<FuncObject>();
  fobj->is_standart_func = true;
  fobj->func = func;
  fobj->name = name;
  global_scope->variables[name] = Value(fobj);
}

static void InitialData(std::istream &input, std::ostream &output) {
  global_scope = new Scope(nullptr);
  input_stream = &input;
  output_stream = &output;
  call_stack.clear();

  AddFunction("print", SystemFunctions::Print);
  AddFunction("println", SystemFunctions::PrintLN);
  AddFunction("read", SystemFunctions::Read);
  AddFunction("stacktrace", SystemFunctions::StackTrace);

  AddFunction("abs", NumericalFunctions::Abs);
  AddFunction("ceil", NumericalFunctions::Ceil);
  AddFunction("floor", NumericalFunctions::Floor);
  AddFunction("round", NumericalFunctions::Round);
  AddFunction("sqrt", NumericalFunctions::Sqrt);
  AddFunction("rnd", NumericalFunctions::Rnd);
  AddFunction("parse_num", NumericalFunctions::ParseNum);
  AddFunction("to_string", NumericalFunctions::ToString);

  AddFunction("len", StringFunctions::Len);
  AddFunction("lower", StringFunctions::Lower);
  AddFunction("upper", StringFunctions::Upper);
  AddFunction("split", StringFunctions::Split);
  AddFunction("join", StringFunctions::Join);
  AddFunction("replace", StringFunctions::Replace);

  AddFunction("range", ListFunctions::Range);
  AddFunction("push", ListFunctions::Push);
  AddFunction("pop", ListFunctions::Pop);
  AddFunction("insert", ListFunctions::Insert);
  AddFunction("remove", ListFunctions::Remove);
  AddFunction("sort", ListFunctions::Sort);
}

static bool Match(std::vector<Token> &tokens, size_t &pos, Token::Type t) {
  if (tokens[pos].type == t) {
    pos++;
    return true;
  }
  return false;
}

static void Expect(std::vector<Token> &tokens, size_t &pos, Token::Type t) {
  if (tokens[pos].type != t) {
    throw RuntimeError("Unexpected token " + tokens[pos].str);
  }
  pos++;
}

static void Clear(std::vector<Statement *> &programm) {
  for (Statement *s : programm) {
    delete s;
  }
  delete global_scope;
  global_scope = nullptr;
}

bool interpret(std::istream &input, std::ostream &output) {

  std::string code;
  ReadInput(input, code);

  std::vector<Token> tokens;
  Lexer::Lexer(tokens, code);

  InitialData(input, output);

  size_t pos = 0;

  std::function<Expression *()> ExpressionParse;
  std::function<Statement *()> StatementParse;

  std::function<Expression *()> PrimaryParce = [&]() -> Expression * {
    if (Match(tokens, pos, Token::NUMBER)) {
      double num = tokens[pos - 1].num_value;
      return new NumberExpression(num);
    }

    if (Match(tokens, pos, Token::TRUE)) {
      return new BoolExpression(true);
    }

    if (Match(tokens, pos, Token::FALSE)) {
      return new BoolExpression(false);
    }

    if (Match(tokens, pos, Token::NIL)) {
      return new NilExpression();
    }

    if (Match(tokens, pos, Token::STRING_LITERAL)) {
      std::string str = tokens[pos - 1].str;
      return new StringExpression(str);
    }

    if (Match(tokens, pos, Token::IDENT)) {
      return new VarExpression(tokens[pos - 1].str);
    }

    if (Match(tokens, pos, Token::FUNCTION)) {
      Expect(tokens, pos, Token::LPAREN);

      std::vector<std::string> params;
      while (true) {
        if (Match(tokens, pos, Token::NEWLINE)) {
          continue;
        }

        if (tokens[pos].type == Token::RPAREN) {
          break;
        }

        if (tokens[pos].type != Token::IDENT) {
          throw RuntimeError("Expected parameter name");
        }
        params.push_back(tokens[pos++].str);
        while (Match(tokens, pos, Token::NEWLINE)) {
        }
        if (!Match(tokens, pos, Token::COMMA)) {
          break;
        }
      }
      Expect(tokens, pos, Token::RPAREN);

      while (Match(tokens, pos, Token::NEWLINE)) {
      }

      std::vector<Statement *> body;
      while (!(tokens[pos].type == Token::END &&
               tokens[pos + 1].type == Token::FUNCTION)) {
        body.push_back(StatementParse());
        Match(tokens, pos, Token::NEWLINE);
        while (Match(tokens, pos, Token::NEWLINE)) {
        }
      }
      Expect(tokens, pos, Token::END);
      Expect(tokens, pos, Token::FUNCTION);

      return new FunctionExpression(params, body);
    }

    if (Match(tokens, pos, Token::LPAREN)) {
      Expression *expr = ExpressionParse();
      Expect(tokens, pos, Token::RPAREN);
      return expr;
    }

    if (Match(tokens, pos, Token::LBRACKET)) {
      ListExpression *list = new ListExpression();

      if (tokens[pos].type != Token::RBRACKET) {
        while (true) {
          while (Match(tokens, pos, Token::NEWLINE)) {
          }
          if (tokens[pos].type == Token::RBRACKET) {
            break;
          }

          list->elements.push_back(ExpressionParse());

          while (Match(tokens, pos, Token::NEWLINE)) {
          }
          if (!Match(tokens, pos, Token::COMMA)) {
            break;
          }
        }
      }

      Expect(tokens, pos, Token::RBRACKET);
      return list;
    }

    throw RuntimeError("Unexpected token: " + tokens[pos].str);
  };

  std::function<Expression *()> IndexAndCallParse = [&]() -> Expression * {
    Expression *expr = PrimaryParce();
    while (true) {
      if (Match(tokens, pos, Token::LBRACKET)) {
        Expression *startExpr = nullptr;
        Expression *endExpr = nullptr;
        Expression *stepExpr = nullptr;
        bool slice = false;
        if (tokens[pos].type != Token::COLON) {
          startExpr = ExpressionParse();
        }
        if (Match(tokens, pos, Token::COLON)) {
          slice = true;
          if (tokens[pos].type != Token::COLON &&
              tokens[pos].type != Token::RBRACKET) {
            endExpr = ExpressionParse();
          }
          if (Match(tokens, pos, Token::COLON)) {
            stepExpr = ExpressionParse();
          }
        }
        Expect(tokens, pos, Token::RBRACKET);
        expr = new IndexExpression(
            expr, startExpr ? startExpr : new NilExpression(),
            endExpr ? endExpr : new NilExpression(),
            stepExpr ? stepExpr : new NilExpression(), slice);
      }

      else if (Match(tokens, pos, Token::LPAREN)) {
        CallExpression *call = new CallExpression(expr);
        while (true) {
          if (Match(tokens, pos, Token::NEWLINE)) {
            continue;
          }

          if (tokens[pos].type == Token::RPAREN) {
            break;
          }

          call->args.push_back(ExpressionParse());
          while (Match(tokens, pos, Token::NEWLINE)) {
          }

          if (!Match(tokens, pos, Token::COMMA)) {
            break;
          }
        }
        Expect(tokens, pos, Token::RPAREN);
        expr = call;
      }

      else {
        break;
      }
    }
    return expr;
  };

  std::function<Expression *()> UnaryParse = [&]() -> Expression * {
    if (Match(tokens, pos, Token::NOT)) {
      return new UnaryExpression(UnaryExpression::NOT, UnaryParse());
    }

    if (Match(tokens, pos, Token::MINUS)) {
      return new UnaryExpression(UnaryExpression::NEG, UnaryParse());
    }

    if (Match(tokens, pos, Token::PLUS)) {
      return new UnaryExpression(UnaryExpression::POS, UnaryParse());
    }

    return IndexAndCallParse();
  };

  std::function<Expression *()> MulParse = [&]() -> Expression * {
    Expression *expr = UnaryParse();
    while (true) {
      if (Match(tokens, pos, Token::MULT)) {
        expr = new BinaryExpression(BinaryExpression::MUL, expr, UnaryParse());
      }

      else if (Match(tokens, pos, Token::SLASH)) {
        expr = new BinaryExpression(BinaryExpression::DIV, expr, UnaryParse());
      }

      else if (Match(tokens, pos, Token::PERCENT)) {
        expr = new BinaryExpression(BinaryExpression::MOD, expr, UnaryParse());
      }

      else if (Match(tokens, pos, Token::CARET)) {
        expr = new BinaryExpression(BinaryExpression::POW, expr, UnaryParse());
      }

      else {
        break;
      }
    }
    return expr;
  };

  std::function<Expression *()> AddParse = [&]() -> Expression * {
    Expression *expr = MulParse();
    while (true) {
      if (Match(tokens, pos, Token::PLUS)) {
        expr = new BinaryExpression(BinaryExpression::ADD, expr, MulParse());
      }

      else if (Match(tokens, pos, Token::MINUS)) {
        expr = new BinaryExpression(BinaryExpression::SUB, expr, MulParse());
      }

      else {
        break;
      }
    }
    return expr;
  };

  std::function<Expression *()> ComparisonParse = [&]() -> Expression * {
    Expression *expr = AddParse();
    while (true) {
      if (Match(tokens, pos, Token::LT)) {
        expr = new BinaryExpression(BinaryExpression::LT, expr, AddParse());
      }

      else if (Match(tokens, pos, Token::RT)) {
        expr = new BinaryExpression(BinaryExpression::RT, expr, AddParse());
      }

      else if (Match(tokens, pos, Token::LE)) {
        expr = new BinaryExpression(BinaryExpression::LE, expr, AddParse());
      }

      else if (Match(tokens, pos, Token::RE)) {
        expr = new BinaryExpression(BinaryExpression::RE, expr, AddParse());
      }

      else {
        break;
      }
    }
    return expr;
  };

  std::function<Expression *()> EqualityParse = [&]() -> Expression * {
    Expression *expr = ComparisonParse();
    while (true) {
      if (Match(tokens, pos, Token::EQEQ)) {
        expr =
            new BinaryExpression(BinaryExpression::EQ, expr, ComparisonParse());
      }

      else if (Match(tokens, pos, Token::NEQ)) {
        expr = new BinaryExpression(BinaryExpression::NEQ, expr,
                                    ComparisonParse());
      }

      else {
        break;
      }
    }
    return expr;
  };

  std::function<Expression *()> AndParse = [&]() -> Expression * {
    Expression *expr = EqualityParse();
    while (Match(tokens, pos, Token::AND)) {
      expr = new BinaryExpression(BinaryExpression::AND, expr, EqualityParse());
    }
    return expr;
  };

  std::function<Expression *()> OrParse = [&]() -> Expression * {
    Expression *expr = AndParse();
    while (Match(tokens, pos, Token::OR)) {
      expr = new BinaryExpression(BinaryExpression::OR, expr, AndParse());
    }
    return expr;
  };

  ExpressionParse = [&]() -> Expression * { return OrParse(); };

  StatementParse = [&]() -> Statement * {
    if (Match(tokens, pos, Token::IF)) {
      Expression *cond = ExpressionParse();
      Expect(tokens, pos, Token::THEN);
      IfStmt *ifs = new IfStmt();

      IfStmt::Branch mainBr;
      mainBr.cond = cond;
      std::vector<Statement *> block;

      if (Match(tokens, pos, Token::NEWLINE)) {
        while (true) {
          if (tokens[pos].type == Token::END || tokens[pos].type == Token::ELSE)
            break;
          if (Match(tokens, pos, Token::NEWLINE))
            continue;
          block.push_back(StatementParse());
          Match(tokens, pos, Token::NEWLINE);
        }
      }

      else {
        block.push_back(StatementParse());
      }
      mainBr.stmts = block;
      ifs->ifBranches.push_back(mainBr);

      while (Match(tokens, pos, Token::ELSE)) {
        if (Match(tokens, pos, Token::IF)) {
          Expression *cond2 = ExpressionParse();
          Expect(tokens, pos, Token::THEN);
          IfStmt::Branch elifBr;
          elifBr.cond = cond2;
          std::vector<Statement *> elifBlock;

          if (Match(tokens, pos, Token::NEWLINE)) {
            while (true) {
              if (tokens[pos].type == Token::END ||
                  tokens[pos].type == Token::ELSE)
                break;
              if (Match(tokens, pos, Token::NEWLINE))
                continue;
              elifBlock.push_back(StatementParse());
              Match(tokens, pos, Token::NEWLINE);
            }
          }

          else {
            elifBlock.push_back(StatementParse());
          }
          elifBr.stmts = elifBlock;
          ifs->ifBranches.push_back(elifBr);
        }

        else {
          std::vector<Statement *> elseBlock;

          if (Match(tokens, pos, Token::NEWLINE)) {
            while (true) {
              if (tokens[pos].type == Token::END)
                break;
              if (Match(tokens, pos, Token::NEWLINE))
                continue;
              elseBlock.push_back(StatementParse());
              Match(tokens, pos, Token::NEWLINE);
            }
          }

          else {
            elseBlock.push_back(StatementParse());
          }
          ifs->elseBranch = elseBlock;
          break;
        }
      }
      Expect(tokens, pos, Token::END);
      Expect(tokens, pos, Token::IF);
      return ifs;
    }

    if (Match(tokens, pos, Token::WHILE)) {
      Expression *cond = ExpressionParse();
      WhileStmt *ws = new WhileStmt(cond);

      if (Match(tokens, pos, Token::NEWLINE)) {
        while (true) {
          if (tokens[pos].type == Token::END) {
            break;
          }

          if (Match(tokens, pos, Token::NEWLINE)) {
            continue;
          }

          ws->body.push_back(StatementParse());
          Match(tokens, pos, Token::NEWLINE);
        }
      }

      else {
        ws->body.push_back(StatementParse());
      }
      Expect(tokens, pos, Token::END);
      Expect(tokens, pos, Token::WHILE);
      return ws;
    }

    if (Match(tokens, pos, Token::FOR)) {
      if (tokens[pos].type != Token::IDENT) {
        throw RuntimeError("Expected loop variable after 'for'");
      }

      std::string var = tokens[pos].str;
      pos++;
      Expect(tokens, pos, Token::IN);
      Expression *iterExpr = ExpressionParse();
      ForStmt *fs = new ForStmt(var, iterExpr);

      if (Match(tokens, pos, Token::NEWLINE)) {
        while (true) {
          if (tokens[pos].type == Token::END)
            break;
          if (Match(tokens, pos, Token::NEWLINE))
            continue;
          fs->body.push_back(StatementParse());
          Match(tokens, pos, Token::NEWLINE);
        }
      }

      else {
        fs->body.push_back(StatementParse());
      }
      Expect(tokens, pos, Token::END);
      Expect(tokens, pos, Token::FOR);
      return fs;
    }

    if (Match(tokens, pos, Token::BREAK)) {
      return new BreakStmt();
    }

    if (Match(tokens, pos, Token::CONTINUE)) {
      return new ContinueStmt();
    }

    if (Match(tokens, pos, Token::RETURN)) {
      Expression *retExpr = nullptr;

      if (tokens[pos].type != Token::NEWLINE &&
          tokens[pos].type != Token::END_OF_FILE) {
        retExpr = ExpressionParse();
      }
      return new ReturnStmt(retExpr);
    }

    Expression *expr = ExpressionParse();
    if (tokens[pos].type == Token::EQ || tokens[pos].type == Token::PLUS_EQ ||
        tokens[pos].type == Token::MINUS_EQ ||
        tokens[pos].type == Token::MULT_EQ ||
        tokens[pos].type == Token::SLASH_EQ ||
        tokens[pos].type == Token::PERCENT_EQ ||
        tokens[pos].type == Token::CARET_EQ) {
      Token::Type opToken = tokens[pos].type;
      pos++;
      Expression *valExpr = ExpressionParse();
      AssignStmt::Op op;
      switch (opToken) {
      case Token::EQ:
        op = AssignStmt::EQ;
        break;
      case Token::PLUS_EQ:
        op = AssignStmt::ADD_EQ;
        break;
      case Token::MINUS_EQ:
        op = AssignStmt::SUB_EQ;
        break;
      case Token::MULT_EQ:
        op = AssignStmt::MUL_EQ;
        break;
      case Token::SLASH_EQ:
        op = AssignStmt::DIV_EQ;
        break;
      case Token::PERCENT_EQ:
        op = AssignStmt::MOD_EQ;
        break;
      case Token::CARET_EQ:
        op = AssignStmt::POW_EQ;
        break;
      default:
        op = AssignStmt::EQ;
      }

      if (!(dynamic_cast<VarExpression *>(expr) ||
            dynamic_cast<IndexExpression *>(expr))) {
        throw RuntimeError("Invalid left-hand side in assignment");
      }

      return new AssignStmt(op, expr, valExpr);
    }

    else {
      return new ExprStmt(expr);
    }
  };

  std::vector<Statement *> programm;
  try {
    while (tokens[pos].type != Token::END_OF_FILE) {
      if (Match(tokens, pos, Token::NEWLINE)) {
        continue;
      }
      programm.push_back(StatementParse());
      Match(tokens, pos, Token::NEWLINE);
    }
  } catch (const RuntimeError &e) {
    Clear(programm);
  }

  bool compile = true;
  try {
    for (Statement *s : programm) {
      s->exec(global_scope);
    }
  } catch (const RuntimeError &e) {
    compile = false;
  } catch (const BreakException &e) {
    compile = false;
  } catch (const ContinueException &e) {
    compile = false;
  }

  Clear(programm);
  return compile;
}
