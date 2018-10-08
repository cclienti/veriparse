#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_EXPRESSION_OPERATORS
#define VERIPARSE_PASSES_TRANSFORMATIONS_EXPRESSION_OPERATORS

#include <veriparse/AST/nodes.hpp>
#include <limits>
#include <type_traits>

// References:
// http://www.asic-world.com/verilog/operators1.html
// http://www.asic-world.com/verilog/operators2.html

namespace Veriparse {
namespace Passes {
namespace Transformations {

/**
 * Base class unary operator
 */
template<class T>
struct unary_operator {
	virtual T operator()(const T x) const = 0;
};

/**
 * Unary $unsigned function
 */
template<class T>
struct unsigned_fct: public unary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");
	T operator()(const T x) const {
		return x;
	}
};
template<>
AST::IntConstN::Ptr unsigned_fct<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;

/**
 * Unary $signed function
 */
template<class T>
struct signed_fct: public unary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");
	T operator()(const T x) const {
		return x;
	}
};
template<>
AST::IntConstN::Ptr signed_fct<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;

/**
 * Unary clog2. It represents the number of bits to store "x" states.
 *
 * clog2(0) = 0
 * clog2(1) = 0
 * clog2(2) = 1
 * clog2(3) = 2
 * clog2(4) = 2
 * clog2(5) = 3
 * clog2(6) = 3
 * clog2(7) = 3
 * clog2(8) = 3
 */
template<class T>
struct clog2: public unary_operator<T> {
	T operator()(const T x) const {
		if (x < 2) return 0;
		else return (operator()((x+1)/2)+1);
	}
};
template<>
double clog2<double>::operator()(const double x) const;
template<>
AST::FloatConst::Ptr clog2<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x) const;
template<>
AST::IntConstN::Ptr clog2<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;

/**
 * Unary plus
 */
template<class T>
struct uplus: public unary_operator<T> {
	T operator()(const T x) const {
		return x;
	}
};
template<>
AST::FloatConst::Ptr uplus<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x) const;
template<>
AST::IntConstN::Ptr uplus<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;


/**
 * Unary minus
 */
template<class T>
struct uminus: public unary_operator<T> {
	T operator()(const T x) const {
		return -x;
	}
};
template<>
AST::FloatConst::Ptr uminus<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x) const;
template<>
AST::IntConstN::Ptr uminus<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;


/**
 * Unary logical not
 */
template<class T>
struct ulnot: public unary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x) const {
		return (x != 0) ? T{0} : T{1};
	}
};
template<>
AST::IntConstN::Ptr ulnot<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;


/**
 * Unary bit-wise not
 */
template<class T>
struct unot: public unary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x) const {
		return ~x;
	}
};
template<>
AST::IntConstN::Ptr unot<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;


/**
 * Unary and reduction
 */
template<class T>
struct uand: public unary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	typedef std::numeric_limits<T> lim;
	T operator()(const T x) const {
		return (lim::is_signed ? static_cast<T>(-1) : lim::max()) == x ? 1 : 0;
	}
};
template<>
AST::IntConstN::Ptr uand<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;


/**
 * Unary nand reduction
 */
template<class T>
struct unand: public unary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	typedef std::numeric_limits<T> lim;
	T operator()(const T x) const {
		return (lim::is_signed ? static_cast<T>(-1) : lim::max()) == x ? 0 : 1;
	}
};
template<>
AST::IntConstN::Ptr unand<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;


/**
 * Unary or reduction
 */
template<class T>
struct uor: public unary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x) const {
		return x == 0 ? 0 : 1;
	}
};
template<>
AST::IntConstN::Ptr uor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;


/**
 * Unary nor reduction
 */
template<class T>
struct unor: public unary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x) const {
		return x == 0 ? 1 : 0;
	}
};
template<>
AST::IntConstN::Ptr unor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;


/**
 * Unary xor reduction, return 1 if there is an odd number of bit set.
 */
template<class T>
struct uxor: public unary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x) const {
		T _x = x;
		const T width = sizeof(T)*8;
		const T max = 1 << (width-1);
		T count = 0;

		for(T i=0; i<width; i++) {
			count = (_x & max) == 0 ? count : count + 1;
			_x <<= 1;
		}

		return count % 2;
	}
};
template<>
AST::IntConstN::Ptr uxor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;


/**
 * Unary xnor reduction, return 1 if there is an even number of bit set.
 */
template<class T>
struct uxnor: public unary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x) const {
		return uxor<T>()(x) ? 0 : 1;
	}
};
template<>
AST::IntConstN::Ptr uxnor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const;



/**
 * Base class binary operator
 */
template<class T>
struct binary_operator {
	virtual T operator()(T x, T y) const = 0;
};

/**
 * Binary Pointer (bit read)
 */
template<class T>
struct pointer: public binary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");
	T operator()(const T x, const T y) const {
		return (x & (1<<y)) ? 1 : 0;
	}
};
template<>
AST::IntConstN::Ptr pointer<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                             const AST::IntConstN::Ptr y) const;

/**
 * Binary power operator x^y
 */
template<class T>
struct power: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		T _x{x}, _y{y};
		if (_y < T{0}) return 0;
		if (_x == T{0}) return 0;

		T result{1};
		while (_y) {
			if ((_y & T{1}) != T{0}) result *= _x;
			_y >>= 1;
			_x *= _x;
		}

		return result;
	}
};
template<>
double power<double>::operator()(const double x, const double y) const;
template<>
AST::FloatConst::Ptr power<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                             const AST::FloatConst::Ptr y) const;
template<>
AST::IntConstN::Ptr power<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                           const AST::IntConstN::Ptr y) const;


/**
 * Binary multiplication operator x*y
 */
template<class T>
struct times: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x*y;
	}
};
template<>
AST::FloatConst::Ptr times<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                             const AST::FloatConst::Ptr y) const;
template<>
AST::IntConstN::Ptr times<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                           const AST::IntConstN::Ptr y) const;


/**
 * Binary division operator x/y
 */
template<class T>
struct divide: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x/y;
	}
};
template<>
AST::FloatConst::Ptr divide<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                              const AST::FloatConst::Ptr y) const;
template<>
AST::IntConstN::Ptr divide<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                            const AST::IntConstN::Ptr y) const;


/**
 * Binary modulus operator x%y
 */
template<class T>
struct mod: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x%y;
	}
};
template<>
double mod<double>::operator()(const double x, const double y) const;
template<>
AST::FloatConst::Ptr mod<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                           const AST::FloatConst::Ptr y) const;
template<>
AST::IntConstN::Ptr mod<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                         const AST::IntConstN::Ptr y) const;


/**
 * Binary plus operator x+y
 */
template<class T>
struct plus: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x+y;
	}
};
template<>
AST::FloatConst::Ptr plus<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                            const AST::FloatConst::Ptr y) const;
template<>
AST::IntConstN::Ptr plus<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                          const AST::IntConstN::Ptr y) const;


/**
 * Binary plus operator x-y
 */
template<class T>
struct minus: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x-y;
	}
};
template<>
AST::FloatConst::Ptr minus<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                             const AST::FloatConst::Ptr y) const;
template<>
AST::IntConstN::Ptr minus<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                           const AST::IntConstN::Ptr y) const;


/**
 * Binary shift left operator x<<y
 */
template<class T>
struct sll: public binary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x, const T y) const {
		return x<<y;
	}
};
template<>
AST::IntConstN::Ptr sll<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                         const AST::IntConstN::Ptr y) const;


/**
 * Binary logical shift right operator x>>y
 */
template<class T>
struct srl: public binary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x, const T y) const {
		typedef std::numeric_limits<T> lim;
		T _x=x;
		for(T i=0; i<y; i++) {
			_x = (_x>>1) & lim::max();
		}
		return _x;
	}
};
template<>
AST::IntConstN::Ptr srl<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                         const AST::IntConstN::Ptr y) const;


/**
 * Binary arithmetical shift right operator x>>y
 */
template<class T>
struct sra: public binary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x, const T y) const {
		return x>>y;
	}
};
template<>
AST::IntConstN::Ptr sra<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                         const AST::IntConstN::Ptr y) const;


/**
 * Binary less than operator x<y
 */
template<class T>
struct less_than: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x<y ? 1 : 0;
	}
};
template<>
AST::FloatConst::Ptr less_than<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                                 const AST::FloatConst::Ptr y) const;
template<>
AST::IntConstN::Ptr less_than<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                               const AST::IntConstN::Ptr y) const;


/**
 * Binary greater than operator x>y
 */
template<class T>
struct greater_than: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x>y ? 1 : 0;
	}
};
template<>
AST::FloatConst::Ptr greater_than<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                                    const AST::FloatConst::Ptr y) const;
template<>
AST::IntConstN::Ptr greater_than<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                  const AST::IntConstN::Ptr y) const;


/**
 * Binary less or equal operator x<=y
 */
template<class T>
struct less_equal: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x<=y ? 1 : 0;
	}
};
template<>
AST::FloatConst::Ptr less_equal<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                                  const AST::FloatConst::Ptr y) const;
template<>
AST::IntConstN::Ptr less_equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                const AST::IntConstN::Ptr y) const;


/**
 * Binary greater or equal operator x>=y
 */
template<class T>
struct greater_equal: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x>=y ? 1 : 0;
	}
};
template<>
AST::FloatConst::Ptr greater_equal<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                                     const AST::FloatConst::Ptr y) const;
template<>
AST::IntConstN::Ptr greater_equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                   const AST::IntConstN::Ptr y) const;


/**
 * Binary equal operator x==y
 */
template<class T>
struct equal: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x==y ? 1 : 0;
	}
};
template<>
AST::IntConstN::Ptr equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                           const AST::IntConstN::Ptr y) const;
template<>
AST::FloatConst::Ptr equal<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                             const AST::FloatConst::Ptr y) const;


/**
 * Binary not equal operator x!=y
 */
template<class T>
struct not_equal: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return x!=y ? 1 : 0;
	}
};
template<>
AST::IntConstN::Ptr not_equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                               const AST::IntConstN::Ptr y) const;
template<>
AST::FloatConst::Ptr not_equal<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                                 const AST::FloatConst::Ptr y) const;


/**
 * Binary case equal operator x===y
 */
template<class T>
struct case_equal: public binary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x, const T y) const {
		return x==y ? 1 : 0;
	}
};
template<>
AST::IntConstN::Ptr case_equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                const AST::IntConstN::Ptr y) const;


/**
 * Binary case not equal operator x!==y
 */
template<class T>
struct case_not_equal: public binary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x, const T y) const {
		return x!=y ? 1 : 0;
	}
};
template<>
AST::IntConstN::Ptr case_not_equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                    const AST::IntConstN::Ptr y) const;

/**
 * Binary bitwise and operator x&y
 */
template<class T>
struct bitwise_and: public binary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x, const T y) const {
		return x&y;
	}
};
template<>
AST::IntConstN::Ptr bitwise_and<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                 const AST::IntConstN::Ptr y) const;

/**
 * Binary bitwise xor operator x^y
 */
template<class T>
struct bitwise_xor: public binary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x, const T y) const {
		return x^y;
	}
};
template<>
AST::IntConstN::Ptr bitwise_xor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                 const AST::IntConstN::Ptr y) const;

/**
 * Binary bitwise xnor operator x~^y or x^~y
 */
template<class T>
struct bitwise_xnor: public binary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x, const T y) const {
		return ~(x^y);
	}
};
template<>
AST::IntConstN::Ptr bitwise_xnor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                  const AST::IntConstN::Ptr y) const;

/**
 * Binary bitwise or operator x|y
 */
template<class T>
struct bitwise_or: public binary_operator<T> {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");

	T operator()(const T x, const T y) const {
		return x|y;
	}
};
template<>
AST::IntConstN::Ptr bitwise_or<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                const AST::IntConstN::Ptr y) const;

/**
 * Binary logical and operator x&&y
 */
template<class T>
struct logical_and: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return (x!=0) && (y!=0);
	}
};
template<>
AST::IntConstN::Ptr logical_and<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                 const AST::IntConstN::Ptr y) const;
template<>
AST::FloatConst::Ptr logical_and<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                                   const AST::FloatConst::Ptr y) const;

/**
 * Binary logical and operator x||y
 */
template<class T>
struct logical_or: public binary_operator<T> {
	T operator()(const T x, const T y) const {
		return (x!=0) || (y!=0);
	}
};
template<>
AST::IntConstN::Ptr logical_or<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                const AST::IntConstN::Ptr y) const;
template<>
AST::FloatConst::Ptr logical_or<AST::FloatConst::Ptr>::operator()(const AST::FloatConst::Ptr x,
                                                                  const AST::FloatConst::Ptr y) const;

/**
 * Ternary operator
 */
template<class T_TEST, class T>
struct ternary {
	T operator()(const T_TEST x, const T y, const T z) const {
		return x ? y : z;
	}
};

template<>
AST::Node::Ptr ternary<AST::IntConstN::Ptr, AST::Node::Ptr>::operator()
	(const AST::IntConstN::Ptr x, const AST::Node::Ptr y, const AST::Node::Ptr z) const;

template<>
AST::Node::Ptr ternary<AST::FloatConst::Ptr, AST::Node::Ptr>::operator()
	(const AST::FloatConst::Ptr x, const AST::Node::Ptr y, const AST::Node::Ptr z) const;

template<>
AST::IntConstN::Ptr ternary<AST::IntConstN::Ptr, AST::IntConstN::Ptr>::operator()
	(const AST::IntConstN::Ptr x, const AST::IntConstN::Ptr y, const AST::IntConstN::Ptr z) const;

template<>
AST::FloatConst::Ptr ternary<AST::IntConstN::Ptr, AST::FloatConst::Ptr>::operator()
	(const AST::IntConstN::Ptr x, const AST::FloatConst::Ptr y, const AST::FloatConst::Ptr z) const;

template<>
AST::IntConstN::Ptr ternary<AST::FloatConst::Ptr, AST::IntConstN::Ptr>::operator()
	(const AST::FloatConst::Ptr x, const AST::IntConstN::Ptr y, const AST::IntConstN::Ptr z) const;

template<>
AST::FloatConst::Ptr ternary<AST::FloatConst::Ptr, AST::FloatConst::Ptr>::operator()
	(const AST::FloatConst::Ptr x, const AST::FloatConst::Ptr y, const AST::FloatConst::Ptr z) const;

/**
 * Partselect (bits read)
 *
 * example: var[31 : 24]
 */
template<class T>
struct partselect {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");
	T operator()(const T x, const T msb, const T lsb) const {
		const T msb1 = msb+1;
		if (msb1 < (sizeof(T)*8)) {
			T mask = (1<<msb1)-1;
			return (x & mask) >> lsb;
		}
		else {
			return x >> lsb;
		}
	}
};
template<>
AST::IntConstN::Ptr partselect<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
                                                                const AST::IntConstN::Ptr msb,
                                                                const AST::IntConstN::Ptr lsb) const;

/**
 * Partselect minus indexed
 *
 * example: var[31 -: 8]  (equivalent to var[24 +: 8])
 */
template<class T>
struct partselect_minus {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");
	T operator()(const T x, const T msb, const T index) const {
		const partselect<T> op;
		return op(x, msb, msb-index+1);
	}
};
template<>
AST::IntConstN::Ptr partselect_minus<AST::IntConstN::Ptr>::operator()
	(const AST::IntConstN::Ptr x,
	 const AST::IntConstN::Ptr msb, const AST::IntConstN::Ptr index) const;

/**
 * Partselect minus indexed
 *
 * example: var[24 +: 8]
 */
template<class T>
struct partselect_plus {
	static_assert(!std::is_floating_point<T>::value, "operator may not have a floating point operand");
	T operator()(const T x, const T lsb, const T index) const {
		const partselect<T> op;
		return op(x, lsb+index-1, lsb);
	}
};
template<>
AST::IntConstN::Ptr partselect_plus<AST::IntConstN::Ptr>::operator()
	(const AST::IntConstN::Ptr x,
	 const AST::IntConstN::Ptr lsb, const AST::IntConstN::Ptr index) const;

}
}
}


#endif
