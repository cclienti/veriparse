#ifndef VERIPARSE_PASSES_ANALYSIS_DIMENSION
#define VERIPARSE_PASSES_ANALYSIS_DIMENSION

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/module.hpp>


namespace Veriparse {
namespace Passes {
namespace Analysis {


class Dimensions
{
public:
	enum class Packing {packed, unpacked};

	/**
	 * @brief gather array information of signals.
	 */
	struct DimInfo
	{
		std::int64_t msb;  /**< Index of the dimension's Most Significant Bit. */
		std::int64_t lsb;  /**< Index of the dimension's Least Significant Bit. */
		std::size_t width; /**< Width of the array. */
		bool is_big;       /**< True if MSB > LSB, not relevant if MSB is equal to LSB. */
		bool is_packed;    /**< True if the dimension is packed. */

		/**
		 * @brief check if all members are equal.
		 */
		bool operator==(const DimInfo &dim) const;

		/**
		 * @brief check if a member is different.
		 */
		bool operator!=(const DimInfo &dim) const;
	};

	/**
	 * @brief List of Dimension.
	 *
	 * For a specific signal, each element corresponds to one array
	 * dimension. The first list element is the outer array dimension.
	 *
	 * Example:
	 *
	 * @code
	 *   reg [3:0] [4:0] [5:0] a [2:0] [1:0];
	 * @endcode
	 *
	 * The front element in the list will correspond to the dimension
	 * [2:0], the next for [1:0], then [3:0], ... until the last
	 * element (back) for [5:0].
	 *
	 * Note that an empty DimList::list means a dimension of 1-bit.
	 */
	struct DimList
	{
		enum class Decl {var, io, both};

		Decl decl;
		std::list<DimInfo> list;

		/**
		 * @brief Check if all members are equal.
		 *
		 * To check only list, use std::list::operator==().
		 */
		bool operator==(const DimList& dims) const;

		/**
		 * @brief Return true if all dimensions are packed.
		 */
		bool is_fully_packed() const;

		/**
		 * @brief Return the merged width of all packed dimensions.
		 */
		std::size_t packed_width() const;
	};

	/**
	 * @brief Map associating a signal name and the list of Dimension
	 */
	using DimMap = std::map<std::string, DimList>;

	friend std::ostream &operator<<(std::ostream &os, const DimInfo &dim);
	friend std::ostream &operator<<(std::ostream &os, const DimList::Decl decl);
	friend std::ostream &operator<<(std::ostream &os, const std::list<DimInfo> &list);
	friend std::ostream &operator<<(std::ostream &os, const DimList &dims);
	friend std::ostream &operator<<(std::ostream &os, const DimMap::value_type &dim_pair);
	friend std::ostream &operator<<(std::ostream &os, const DimMap &dim_map);

public:
	/**
	 * @brief Analyze a AST::Length::Ptr or AST::Width::Ptr.
	 *
	 * Fill a DimInfo structure with all gathered dimensions information.
	 *
	 * @param[in] array (AST::Length::Ptr or AST::Width::Ptr)
	 * @param[in] packing
	 * @param[out] dim
	 * @return true if the dimensions are correctly resolved
	 */
	template<typename TArray>
	static bool extract_array(const TArray &array, Packing packing, DimInfo &dim);

	/**
	 * @brief Analyze a list of AST::Length::Ptr or AST::Width::Ptr.
	 *
	 * Fill a DimList structure with all gathered dimensions information.
	 */
	template<typename TArrays>
	static bool extract_arrays(const TArrays &arrays, Packing packing, DimList &dims);

	/**
	 * @brief analyse all I/O and variables of a module and fill a map
	 * where name are keys and values are DimList.
	 *
	 * @return zero on success
	 */
	static int analyze_decls(const AST::Node::Ptr &node, DimMap &dim_map);

	/**
	 * @brief analyse the dimension of an expression.
	 *
	 * @param[in] node: expression to analyze.
	 * @param[in] dim_map: map of the analyzed declarations.
	 * @param[out] dims: result of the analysis.
	 * @return zero on success.
	 */
	static int analyze_expr(const AST::Node::Ptr &node, const DimMap &dim_map, DimList &dims);

	/**
	 * @brief generate a declaration that corresponds to the given
	 * dimension list.
	 *
	 * @param[in] name: name of the declaration
	 * @param[in] node_type: type of the declaration (wire, reg, ...)
	 * @param[in] dims: dimensions list
	 * @return the declaration AST else nullptr
	 */
	static AST::Node::Ptr generate_decl(const std::string &name, const AST::NodeType node_type, const DimList &dims,
	                                    const std::string &filename="", std::uint32_t line=0);
};



}
}
}


#endif
