#pragma once
#include "data.h"

namespace Lexer {

static void Lexer(std::vector<Token> &tokens, const std::string &code) {
  size_t i = 0;
  size_t line = 1;

  while (i < code.size()) {
    char c = code[i];

    if (c == '\n') {
      tokens.push_back(tokens.emplace_back(Token::NEWLINE, "\\n"));
      ++i;
      ++line;
    }

    else if (c == ' ' || c == '\t' || c == '\r') {
      ++i;
    }

    else if (c == '/' && i + 1 < code.size() && code[i + 1] == '/') {
      while (i < code.size() && code[i] != '\n') {
        ++i;
      }
    }

    else if (c == '\"') {
      ++i;
      std::string res;
      bool closed = false;

      while (i < code.size()) {
        char cs = code[i];
        ++i;
        if (cs == '\"') {
          closed = true;
          break;
        }
        if (cs == '\\') {
          if (i >= code.size()) {
            break;
          }
          char next = code[i];
          ++i;
          if (next == 'n') {
            res += '\n';
          } else if (next == 't') {
            res += '\t';
          } else if (next == 'r') {
            res += '\r';
          } else if (next == '\\') {
            res += '\\';
          } else if (next == '\"') {
            res += '\"';
          } else {
            res += next;
          }
        } else {
          res += cs;
        }
      }

      if (!closed) {
        throw RuntimeError("The string type in the line number " +
                           std::to_string(line) +
                           " is incorrect (missing quotes)");
      }
      tokens.emplace_back(Token::STRING_LITERAL, res);
    }

    else if (std::isdigit(c) ||
             (c == '.' && code.size() > i + 1 && std::isdigit(code[i + 1]))) {
      size_t st = i;
      bool dot = false, exp = false;

      if (c == '.') {
        dot = true;
        i++;
      }

      while (i < code.size()) {
        char ch = code[i];
        if (std::isdigit(ch)) {
          ++i;
          continue;
        }
        if (ch == '.') {
          if (dot) {
            break;
          }
          dot = true;
          ++i;
          continue;
        }
        if (ch == 'e' || ch == 'E') {
          if (exp) {
            break;
          }
          exp = true;
          ++i;
          if (i < code.size() && (code[i] == '+' || code[i] == '-')) {
            ++i;
          }
          continue;
        }
        break;
      }

      std::string num = code.substr(st, i - st);
      double value = 0;
      try {
        value = std::stod(num);
      } catch (...) {
        throw RuntimeError("The numeric type in the line number " +
                           std::to_string(line) +
                           " is incorrect. In your code: " + num);
      }
      tokens.emplace_back(Token::NUMBER, num, value);
    }

    else if (std::isalpha(c) || c == '_') {
      size_t st = i;

      while (i < code.size() && (std::isalnum(code[i]) || code[i] == '_')) {
        ++i;
      }

      std::string str = code.substr(st, i - st);
      if (str == "if")
        tokens.emplace_back(Token::IF, str);
      else if (str == "then")
        tokens.emplace_back(Token::THEN, str);
      else if (str == "else")
        tokens.emplace_back(Token::ELSE, str);
      else if (str == "end")
        tokens.emplace_back(Token::END, str);
      else if (str == "while")
        tokens.emplace_back(Token::WHILE, str);
      else if (str == "for")
        tokens.emplace_back(Token::FOR, str);
      else if (str == "in")
        tokens.emplace_back(Token::IN, str);
      else if (str == "function")
        tokens.emplace_back(Token::FUNCTION, str);
      else if (str == "return")
        tokens.emplace_back(Token::RETURN, str);
      else if (str == "break")
        tokens.emplace_back(Token::BREAK, str);
      else if (str == "continue")
        tokens.emplace_back(Token::CONTINUE, str);
      else if (str == "true")
        tokens.emplace_back(Token::TRUE, str);
      else if (str == "false")
        tokens.emplace_back(Token::FALSE, str);
      else if (str == "nil")
        tokens.emplace_back(Token::NIL, str);
      else if (str == "and")
        tokens.emplace_back(Token::AND, str);
      else if (str == "or")
        tokens.emplace_back(Token::OR, str);
      else if (str == "not")
        tokens.emplace_back(Token::NOT, str);
      else
        tokens.emplace_back(Token::IDENT, str);
    }

    else if (c == '+') {
      if (i + 1 < code.size() && code[i + 1] == '=') {
        tokens.emplace_back(Token::PLUS_EQ, "+=");
        i += 2;
      } else {
        tokens.emplace_back(Token::PLUS, "+");
        ++i;
      }
    }

    else if (c == '-') {
      if (i + 1 < code.size() && code[i + 1] == '=') {
        tokens.emplace_back(Token::MINUS_EQ, "-=");
        i += 2;
      } else {
        tokens.emplace_back(Token::MINUS, "-");
        ++i;
      }
    }

    else if (c == '*') {
      if (i + 1 < code.size() && code[i + 1] == '=') {
        tokens.emplace_back(Token::MULT_EQ, "*=");
        i += 2;
      } else {
        tokens.emplace_back(Token::MULT, "*");
        ++i;
      }
    }

    else if (c == '/') {
      if (i + 1 < code.size() && code[i + 1] == '=') {
        tokens.emplace_back(Token::SLASH_EQ, "/=");
        i += 2;
      } else {
        tokens.emplace_back(Token::SLASH, "/");
        ++i;
      }
    }

    else if (c == '%') {
      if (i + 1 < code.size() && code[i + 1] == '=') {
        tokens.emplace_back(Token::PERCENT_EQ, "%=");
        i += 2;
      } else {
        tokens.emplace_back(Token::PERCENT, "%");
        ++i;
      }
    }

    else if (c == '^') {
      if (i + 1 < code.size() && code[i + 1] == '=') {
        tokens.emplace_back(Token::CARET_EQ, "^=");
        i += 2;
      } else {
        tokens.emplace_back(Token::CARET, "^");
        ++i;
      }
    }

    else if (c == '=') {
      if (i + 1 < code.size() && code[i + 1] == '=') {
        tokens.emplace_back(Token::EQEQ, "==");
        i += 2;
      } else {
        tokens.emplace_back(Token::EQ, "=");
        ++i;
      }
    }

    else if (c == '!') {
      if (i + 1 < code.size() && code[i + 1] == '=') {
        tokens.emplace_back(Token::NEQ, "!=");
        i += 2;
      } else {
        throw RuntimeError("Unknown character '!' in line number: " +
                           std::to_string(line) + ".");
      }
    }

    else if (c == '<') {
      if (i + 1 < code.size() && code[i + 1] == '=') {
        tokens.emplace_back(Token::LE, "<=");
        i += 2;
      } else {
        tokens.emplace_back(Token::LT, "<");
        ++i;
      }
    }

    else if (c == '>') {
      if (i + 1 < code.size() && code[i + 1] == '=') {
        tokens.emplace_back(Token::RE, ">=");
        i += 2;
      } else {
        tokens.emplace_back(Token::RT, ">");
        ++i;
      }
    }

    else if (c == '[') {
      tokens.emplace_back(Token::LBRACKET, "[");
      ++i;
    }

    else if (c == ']') {
      tokens.emplace_back(Token::RBRACKET, "[");
      ++i;
    }

    else if (c == '(') {
      tokens.emplace_back(Token::LPAREN, "(");
      ++i;
    }

    else if (c == ')') {
      tokens.emplace_back(Token::RPAREN, ")");
      ++i;
    }

    else if (c == ',') {
      tokens.emplace_back(Token::COMMA, ",");
      ++i;
    }

    else if (c == ':') {
      tokens.emplace_back(Token::COLON, ":");
      ++i;
    }

    else {
      throw RuntimeError(
          "Unknown character in line number: " + std::to_string(line) +
          ". In your code: " + std::to_string(c));
    }
  }

  tokens.emplace_back(Token::END_OF_FILE);
}

} // namespace Lexer