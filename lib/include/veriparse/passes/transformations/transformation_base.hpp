#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_TRANSFORMATION_BASE
#define VERIPARSE_PASSES_TRANSFORMATIONS_TRANSFORMATION_BASE

#include <veriparse/AST/nodes.hpp>
#include <string>
#include <set>
#include <map>
#include <algorithm>

namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class TransformationBase {
			public:
				/**
				 * @return zero on success
				 */
				virtual int run(AST::Node::Ptr node) {
					return process(node, nullptr);
				}

			protected:
				virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) = 0;

			protected:
				template<typename T>
				std::set<T> to_set(const std::vector<T> &v)
				{
					return std::set<T>(v.cbegin(), v.cend());
				}

				template<typename T>
				std::set<T> merge_set(const std::set<T> &s1, const std::set<T> &s2)
				{
					std::set<T> new_set;
					new_set.insert(s1.cbegin(), s1.cend());
					new_set.insert(s2.cbegin(), s2.cend());
					return new_set;
				}

				template<typename T_KEY, typename T_VAL>
				std::map<T_KEY, T_VAL> remove_keys(const std::map<T_KEY, T_VAL> &map, const std::set<T_KEY> &keys)
				{
					std::map<T_KEY, T_VAL> new_map;
					for(const auto &entry: map) {
						if(keys.count(entry.first) == 0) {
							new_map[entry.first] = entry.second;
						}
					}
					return new_map;
				}
			};

		}
	}
}

#endif
