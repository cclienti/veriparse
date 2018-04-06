#ifndef VERIPARSE_AST_NODE_CAST_HPP
#define VERIPARSE_AST_NODE_CAST_HPP


#include <veriparse/AST/node_type.hpp>
#include <veriparse/AST/node_to_node_type.hpp>
#include <veriparse/AST/node_pointers.hpp>
#include <veriparse/AST/node.hpp>
#include <veriparse/logger/logger.hpp>

#include <memory>
#include <list>


namespace Veriparse {
	namespace AST {

		template<class TDest>
		struct CastTo {
			using Ptr = typename NodePointers<TDest>::Ptr;
			using List = typename NodePointers<TDest>::List;
			using ListPtr = typename NodePointers<TDest>::ListPtr;

			CastTo(void) = delete;

			/**
			 * Cast a node pointer to another node type pointer
			 */
			template<class TSrcPtr>
			static Ptr ptr(TSrcPtr node) {
				constexpr NodeType dest_node_type = get_node_type<TDest>();

				if (node) {
					if ((node->is_node_type(dest_node_type)) ||
					    (node->is_node_category(dest_node_type))) {
						return std::static_pointer_cast<TDest>(node);
					}
					else {
						LOG_ERROR << "node type \"" << node->get_node_type()
						          << "\" cannot be casted to \"" << dest_node_type << "\"";
						return nullptr;
					}
				}
				else {
					return nullptr;
				}
			}

			/**
			 * Cast a pointer to a list of node pointer to another node
			 * type.
			 */
			template<class TSrcListPtr>
			static ListPtr list_ptr(TSrcListPtr nodes) {
				ListPtr list = std::make_shared<List>();
				if(nodes) {
					for(auto p: *nodes) {
						list->push_back(CastTo<TDest>::ptr(p));
					}
				}
				return list;
			}

		};


		/**
		 * Convenient function to cast to any node list pointer type
		 */
		template<class TDest, class TSrcListPtr>
		inline typename CastTo<TDest>::ListPtr cast_list_to(TSrcListPtr node) {
			return CastTo<TDest>::list_ptr(node);
		}


		/**
		 * Convenient function to cast to any node pointer type
		 */
		template<class TDest, class TSrcPtr>
		inline typename CastTo<TDest>::Ptr cast_to(TSrcPtr node) {
			return CastTo<TDest>::ptr(node);
		}

		/**
		 * Convenient function to cast to any node pointer type
		 */
		template<class TDest>
		inline typename CastTo<TDest>::Ptr cast_to(AST::Node::Ptr node) {
			return CastTo<TDest>::ptr(node);
		}


		/**
		 * Convenient function to cast a node pointer to the base node.
		 */
		template<class TSrcPtr>
		inline Node::Ptr to_node(TSrcPtr node) {
			return CastTo<AST::Node>::ptr(node);
		}


		/**
		 * Convenient function to cast a node list pointer to a pointer
		 * to the base node type list.
		 */
		template<class TSrcListPtr>
		inline Node::ListPtr to_node_list(TSrcListPtr nodes) {
			return CastTo<AST::Node>::list_ptr(nodes);
		}

	}
}


#endif
