#pragma once
#include "data.h"
#include "expressions.h"
#include "functions.h"
#include "lexer.h"
#include "statements.h"

bool interpret(std::istream &input, std::ostream &output);

template <class T>
std::istringstream &operator<<(std::istringstream &in, const T &val) {
  std::ostringstream tmp;
  tmp << val;
  in.str(in.str() + tmp.str());
  in.clear();
  in.seekg(0);
  return in;
}
