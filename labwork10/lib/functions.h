#pragma once

#include "data.h"

namespace SystemFunctions {

static Value Print(const std::vector<Value> &args) {
  for (size_t i = 0; i < args.size(); ++i) {
    if (std::holds_alternative<NullType>(args[i].data)) {
      *output_stream << "nil";
    }

    else if (auto pd = std::get_if<double>(&args[i].data)) {
      double val = *pd;
      if (std::fabs(val - std::floor(val)) < 1e-9) {
        *output_stream << (long long)val;
      } else {
        *output_stream << val;
      }
    }

    else if (auto pb = std::get_if<bool>(&args[i].data)) {
      *output_stream << (*pb ? "true" : "false");
    }

    else if (auto ps =
                 std::get_if<std::shared_ptr<std::string>>(&args[i].data)) {
      *output_stream << ((**ps).find_first_of(" \t") != std::string::npos ? "\""
                                                                          : "")
                     << **ps
                     << ((**ps).find_first_of(" \t") != std::string::npos ? "\""
                                                                          : "");
    }

    else if (auto pl =
                 std::get_if<std::shared_ptr<ListObject>>(&args[i].data)) {
      *output_stream << "[";
      for (size_t j = 0; j < (*pl)->elements.size(); ++j) {
        std::vector<Value> elemArg{(*pl)->elements[j]};
        SystemFunctions::Print(elemArg);
        if (j < (*pl)->elements.size() - 1) {
          *output_stream << ", ";
        }
      }
      *output_stream << "]";
    }

    else if (std::get_if<std::shared_ptr<FuncObject>>(&args[i].data)) {
      *output_stream << "<function>";
    }
  }

  return Value(NullType{});
}

static Value PrintLN(const std::vector<Value> &args) {
  SystemFunctions::Print(args);
  *output_stream << "\n";

  return Value(NullType{});
}

static Value Read(const std::vector<Value> &args) {
  if (args.size() != 0) {
    throw RuntimeError(
        "Function Read() argument count mismatch. Need - 0, Your - " +
        std::to_string(args.size()));
  }
  std::string line;
  if (!std::getline(*input_stream, line)) {
    return Value(NullType{});
  }
  return Value(std::make_shared<std::string>(line));
}

static Value StackTrace(const std::vector<Value> &args) {
  if (args.size() != 0) {
    throw RuntimeError(
        "Function StackTrace() argument count mismatch. Need - 0, Your - " +
        std::to_string(args.size()));
  }
  std::ostringstream oss;
  oss << "StackTrace: [";
  for (size_t i = 0; i < call_stack.size(); ++i) {
    if (i < call_stack.size() - 1)
      oss << call_stack[i] << ", ";
    else
      oss << call_stack[i];
  }
  oss << "]";
  return Value(std::make_shared<std::string>(oss.str()));
}

} // namespace SystemFunctions

namespace NumericalFunctions {

static Value Abs(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Abs() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  return Value(std::fabs(std::get<double>(args[0].data)));
}

static Value Ceil(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Ceil() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  return Value(std::ceil(std::get<double>(args[0].data)));
}

static Value Floor(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Floor() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  return Value(std::floor(std::get<double>(args[0].data)));
}

static Value Round(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Round() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  return Value(
      static_cast<double>(std::llround(std::get<double>(args[0].data))));
}

static Value Sqrt(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Sqrt() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  double x = std::get<double>(args[0].data);
  return x < 0 ? Value(NullType{}) : Value(std::sqrt(x));
}

static Value Rnd(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Rnd() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  static std::mt19937 generator((unsigned)std::random_device{}());

  double x = std::get<double>(args[0].data);
  if (x <= 0) {
    return Value(0.0);
  }

  std::uniform_int_distribution<long long> dist(0,
                                                (long long)std::floor(x) - 1);
  return Value((double)dist(generator));
}

static Value ParseNum(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function ParseNum() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  auto p = std::get<std::shared_ptr<std::string>>(args[0].data);
  try {
    return Value(std::stod(*p));
  } catch (...) {
    return Value(NullType{});
  }
}

static Value ToString(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function ToString() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  std::ostringstream a;
  a << std::get<double>(args[0].data);
  return Value(std::make_shared<std::string>(a.str()));
}

} // namespace NumericalFunctions

namespace StringFunctions {

static Value Len(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Len() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  if (auto ps = std::get_if<std::shared_ptr<std::string>>(&args[0].data)) {
    return Value((double)(*ps)->size());
  }
  if (auto pl = std::get_if<std::shared_ptr<ListObject>>(&args[0].data)) {
    return Value((double)(*pl)->elements.size());
  }
  throw RuntimeError("The len() argument must be string or list");
}

static Value Lower(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Lower() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  std::string res = *(std::get<std::shared_ptr<std::string>>(args[0].data));
  std::transform(res.begin(), res.end(), res.begin(), ::tolower);
  return Value(std::make_shared<std::string>(res));
}

static Value Upper(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Upper() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  std::string res = *(std::get<std::shared_ptr<std::string>>(args[0].data));
  std::transform(res.begin(), res.end(), res.begin(), ::toupper);
  return Value(std::make_shared<std::string>(res));
}

static Value Split(const std::vector<Value> &args) {
  if (args.size() != 2) {
    throw RuntimeError(
        "Function Split() argument count mismatch. Need - 2, Your - " +
        std::to_string(args.size()));
  }
  auto s = std::get<std::shared_ptr<std::string>>(args[0].data);
  auto delim = std::get<std::shared_ptr<std::string>>(args[1].data);
  std::vector<std::string> parts;
  std::string str = *s;
  std::string d = *delim;
  if (d.empty()) {
    for (char ch : str)
      parts.emplace_back(1, ch);
  } else {
    size_t pos = 0, found;
    while ((found = str.find(d, pos)) != std::string::npos) {
      parts.push_back(str.substr(pos, found - pos));
      pos = found + d.size();
    }
    parts.push_back(str.substr(pos));
  }
  auto listPtr = std::make_shared<ListObject>();
  for (auto &part : parts) {
    listPtr->elements.emplace_back(std::make_shared<std::string>(part));
  }
  return Value(listPtr);
}

static Value Join(const std::vector<Value> &args) {
  if (args.size() != 2) {
    throw RuntimeError(
        "Function Join() argument count mismatch. Need - 2, Your - " +
        std::to_string(args.size()));
  }
  auto listPtr = std::get<std::shared_ptr<ListObject>>(args[0].data);
  auto delim = std::get<std::shared_ptr<std::string>>(args[1].data);
  std::string result;
  for (size_t i = 0; i < listPtr->elements.size(); ++i) {
    Value elem = listPtr->elements[i];
    if (auto ps = std::get_if<std::shared_ptr<std::string>>(&elem.data)) {
      result += **ps;
    } else {
      throw RuntimeError("join() list elements must be strings");
    }
    if (i < listPtr->elements.size() - 1)
      result += *delim;
  }
  return Value(std::make_shared<std::string>(result));
}

static Value Replace(const std::vector<Value> &args) {
  if (args.size() != 3) {
    throw RuntimeError(
        "Function Replace() argument count mismatch. Need - 3, Your - " +
        std::to_string(args.size()));
  }
  auto s = std::get<std::shared_ptr<std::string>>(args[0].data);
  auto oldSub = std::get<std::shared_ptr<std::string>>(args[1].data);
  auto newSub = std::get<std::shared_ptr<std::string>>(args[2].data);
  std::string str = *s;
  std::string oldS = *oldSub;
  std::string newS = *newSub;
  if (oldS.empty())
    return Value(std::make_shared<std::string>(str));
  size_t pos = 0;
  while ((pos = str.find(oldS, pos)) != std::string::npos) {
    str.replace(pos, oldS.size(), newS);
    pos += newS.size();
  }
  return Value(std::make_shared<std::string>(str));
}

} // namespace StringFunctions

namespace ListFunctions {

static Value Range(const std::vector<Value> &args) {
  if (0 == args.size() || args.size() > 3) {
    throw RuntimeError("Function Range() argument count mismatch. Need - from "
                       "1 to 3, Your - " +
                       std::to_string(args.size()));
  }
  double start = 0, end = 0, step = 1;
  if (args.size() == 1) {
    start = 0;
    end = std::floor(std::get<double>(args[0].data));
    step = 1;
  } else if (args.size() == 2) {
    start = std::floor(std::get<double>(args[0].data));
    end = std::floor(std::get<double>(args[1].data));
    step = 1;
  } else if (args.size() == 3) {
    start = std::floor(std::get<double>(args[0].data));
    end = std::floor(std::get<double>(args[1].data));
    step = std::floor(std::get<double>(args[2].data));
  }
  auto listPtr = std::make_shared<ListObject>();
  if (step == 0)
    return Value(listPtr);
  if (step > 0) {
    for (double x = start; x < end; x += step)
      listPtr->elements.emplace_back(x);
  } else {
    for (double x = start; x > end; x += step)
      listPtr->elements.emplace_back(x);
  }
  return Value(listPtr);
}

static Value Push(const std::vector<Value> &args) {
  if (args.size() != 2) {
    throw RuntimeError(
        "Function Push() argument count mismatch. Need - 2, Your - " +
        std::to_string(args.size()));
  }
  auto listPtr = std::get<std::shared_ptr<ListObject>>(args[0].data);
  listPtr->elements.push_back(args[1]);
  return Value(NullType{});
}

static Value Pop(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Pop() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  auto listPtr = std::get<std::shared_ptr<ListObject>>(args[0].data);
  if (listPtr->elements.empty())
    return Value(NullType{});
  Value val = listPtr->elements.back();
  listPtr->elements.pop_back();
  return val;
}

static Value Insert(const std::vector<Value> &args) {
  if (args.size() != 3) {
    throw RuntimeError(
        "Function Insert() argument count mismatch. Need - 3, Your - " +
        std::to_string(args.size()));
  }
  auto listPtr = std::get<std::shared_ptr<ListObject>>(args[0].data);
  double idxd = std::get<double>(args[1].data);
  int idx = (int)std::floor(idxd);
  if (idx < 0 || idx > (int)listPtr->elements.size())
    return Value(NullType{});
  listPtr->elements.insert(listPtr->elements.begin() + idx, args[2]);
  return Value(NullType{});
}

static Value Remove(const std::vector<Value> &args) {
  if (args.size() != 2) {
    throw RuntimeError(
        "Function Remove() argument count mismatch. Need - 2, Your - " +
        std::to_string(args.size()));
  }
  auto listPtr = std::get<std::shared_ptr<ListObject>>(args[0].data);
  double idxd = std::get<double>(args[1].data);
  int idx = (int)std::floor(idxd);
  if (idx < 0 || idx >= (int)listPtr->elements.size()) {
    throw RuntimeError("remove(): index out of range");
  }
  listPtr->elements.erase(listPtr->elements.begin() + idx);
  return Value(NullType{});
}

static Value Sort(const std::vector<Value> &args) {
  if (args.size() != 1) {
    throw RuntimeError(
        "Function Sort() argument count mismatch. Need - 1, Your - " +
        std::to_string(args.size()));
  }
  auto listPtr = std::get<std::shared_ptr<ListObject>>(args[0].data);
  std::sort(
      listPtr->elements.begin(), listPtr->elements.end(),
      [](const Value &a, const Value &b) {
        if (a.data.index() != b.data.index())
          return a.data.index() < b.data.index();
        switch (a.data.index()) {
        case 1:
          return std::get<double>(a.data) < std::get<double>(b.data);
        case 2:
          return std::get<bool>(a.data) < std::get<bool>(b.data);
        case 3:
          return *std::get<std::shared_ptr<std::string>>(a.data) <
                 *std::get<std::shared_ptr<std::string>>(b.data);
        case 4:
          return std::get<std::shared_ptr<ListObject>>(a.data)
                     ->elements.size() <
                 std::get<std::shared_ptr<ListObject>>(b.data)->elements.size();
        case 5:
          return std::get<std::shared_ptr<FuncObject>>(a.data).get() <
                 std::get<std::shared_ptr<FuncObject>>(b.data).get();
        }
        return false;
      });
  return Value(NullType{});
}

} // namespace ListFunctions
