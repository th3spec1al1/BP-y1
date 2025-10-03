#pragma once

#include "../all_includes.h"


class Out {
public:
    Out(std::ostream& output_stream = std::cout) : output_stream_(output_stream) {}

    std::ostream& get_os() const { return output_stream_; }

private:
    std::ostream& output_stream_;
};


template <typename Adapter>
class OutAdapter {
public:
    OutAdapter(const Adapter& adapter, std::ostream& output_stream) : adapter_(adapter), output_stream_(output_stream) {
        if constexpr (std::is_same_v<typename std::iterator_traits<typename Adapter::iterator>::value_type, std::ifstream>) {
            for (auto&& file_stream : adapter_) {
                std::string line;
                while (std::getline(file_stream, line)) {
                    output_stream_ << line << std::endl;
                }
            }
        } 
        else {
            for (auto&& item : adapter_) {
                output_stream_ << item << std::endl;
            }
        }
    }

private:
    const Adapter& adapter_;
    std::ostream& output_stream_;
};


template <class Flow>
auto operator|(Flow&& flow, Out&& out) {
    return OutAdapter<Flow>(flow, out.get_os());
}