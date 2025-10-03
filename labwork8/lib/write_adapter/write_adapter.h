#pragma once

#include "../all_includes.h"

template<class Flow>
class WriteAdapter {

public:

    WriteAdapter(Flow& input_flow, std::ostream& output_flow, char& delimiter) 
    : input_flow_(input_flow), output_flow_(output_flow), delimiter_(delimiter) {
        for (auto&& it : input_flow_) {
            output_flow_ << it << delimiter_;
        }
    }

private:

    Flow& input_flow_;
    std::ostream& output_flow_;
    char& delimiter_;

};


class Write {

public:

    Write(std::ostream& flow, char&& delimiter) : output_flow_(flow), delimiter_(delimiter) {}

    std::ostream& get_of() const { return output_flow_; }

    char& get_del() const { return delimiter_; }

private:

    std::ostream& output_flow_;
    char& delimiter_;

};


template<class Flow>
auto operator|(Flow&& flow, Write&& write) {
    return WriteAdapter<Flow>(flow, write.get_of(), write.get_del());
}

