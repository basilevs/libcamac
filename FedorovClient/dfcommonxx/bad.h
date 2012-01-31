#ifndef DFCOMMONXX_BAD_H__
#define DFCOMMONXX_BAD_H__
#include <limits>

template<typename T>
T bad(const T &) {
	return std::numeric_limits<T>::max();
}

template<typename T>
bool is_bad(const T & value) {
	return value == bad(value);
}

#endif
