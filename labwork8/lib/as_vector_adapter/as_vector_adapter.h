#pragma once

#include "../all_includes.h"


class AsVector {

public:

    AsVector() {}

};


template <class Flow>
auto operator|(Flow&& obj1, AsVector&&) {
    using ValueType = decltype(*std::begin(obj1));
	std::vector<std::decay_t<ValueType>> result;
	for (auto&& item : obj1) {
		result.emplace_back(item);
	}
	return result;
}