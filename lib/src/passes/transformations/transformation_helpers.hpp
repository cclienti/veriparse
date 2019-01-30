#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_TRANSFORMATION_HELPERS_PRIVATE
#define VERIPARSE_PASSES_TRANSFORMATIONS_TRANSFORMATION_HELPERS_PRIVATE

#include <vector>
#include <set>
#include <map>
#include <algorithm>

namespace {

template<typename T>
inline std::set<T> to_set(const std::vector<T> &v)
{
	return std::set<T>(v.cbegin(), v.cend());
}

template<typename T>
inline std::set<T> merge_set(const std::set<T> &s1, const std::set<T> &s2)
{
	std::set<T> new_set;
	new_set.insert(s1.cbegin(), s1.cend());
	new_set.insert(s2.cbegin(), s2.cend());
	return new_set;
}

template<typename T_KEY, typename T_VAL>
inline std::map<T_KEY, T_VAL> remove_keys(const std::map<T_KEY, T_VAL> &map, const std::set<T_KEY> &keys)
{
	std::map<T_KEY, T_VAL> new_map;
	for(const auto &entry: map) {
		if(keys.count(entry.first) == 0) {
			new_map[entry.first] = entry.second;
		}
	}
	return new_map;
}

template<typename T_KEY>
inline std::set<T_KEY> remove_keys(const std::set<T_KEY> &set, const std::set<T_KEY> &keys)
{
	std::set<T_KEY> new_set;
	for(const auto &entry: set) {
		if(keys.count(entry) == 0) {
			new_set.insert(entry);
		}
	}
	return new_set;
}

}

#endif
