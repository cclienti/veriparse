#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_INSTANCE_NORMALIZER
#define VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_INSTANCE_NORMALIZER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <string>
#include <map>
#include <list>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class ModuleInstanceNormalizer: public TransformationBase
			{
			private:
				/**
				 * @return zero on success
				 */
				virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

				/**
				 * @brief split instance lists
				 */
				int split_lists(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

				/**
				 * @brief split instance's array
				 */
				int split_array(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

				/**
				 * @brief split instance's array
				 */
				int analyze_signals(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

			private:

				/**
				 * @brief gather array information of signals
				 */
				struct Dimension
				{
					std::size_t msb;   /**< Index of the dimension's Most Significant Bit */
					std::size_t lsb;   /**< Index of the dimension's Least Significant Bit */
					std::size_t width; /**< Width of the array */
					bool is_big;       /**< True if MSB > LSB, not relevant if MSB is equal to LSB */
					bool is_packed;    /**< True if the dimension is packed. */

					bool operator==(const Dimension &dim)
					{
						return ((msb == dim.msb) && (lsb == dim.lsb) && (width == dim.width) &&
						        (is_big == dim.is_big) && (is_packed == dim.is_packed));
					}
				};

				/**
				 * @brief List of Dimension.
				 *
				 * For a specific signal, each element corresponds to one
				 * array dimension. The first list element is the outer
				 * array dimension.
				 *
				 * Example:
				 *
				 * @code
				 *   reg [3:0] [7:0] [31:0] a [1:0];
				 * @endcode
				 *
				 * The front element in the list will correspond to the
				 * dimension [1:0], the next for [3:0] and the last
				 * element (back) will be for [31:0].
				 */
				using DimensionList = std::list<Dimension>;

				/**
				 * @brief Map associating a signal name and the list of Dimension
				 */
				using DimensionMap = std::map<std::string, DimensionList>;

				DimensionMap m_signals;
			};

		}
	}
}

#endif
