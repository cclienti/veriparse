// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_EXPRESSION_OPERATORS
#define VERIPARSE_PASSES_TRANSFORMATIONS_EXPRESSION_OPERATORS

#include <veriparse/AST/nodes.hpp>
#include <limits>
#include <type_traits>

// References:
// http://www.asic-world.com/verilog/operators1.html
// http://www.asic-world.com/verilog/operators2.html

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * Base class unary operator
 */
template <class T> struct unary_operator
{
    virtual T operator()(const T x) const = 0;
};

/**
 * Unary $unsigned function
 */
template <class T> struct unsigned_fct : public unary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");
    T operator()(const T x) const { return x; }
};
template <> struct unsigned_fct<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary $signed function
 */
template <class T> struct signed_fct : public unary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");
    T operator()(const T x) const { return x; }
};
template <> struct signed_fct<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

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
template <class T> struct clog2 : public unary_operator<T>
{
    T operator()(const T x) const
    {
        if(x < 2) {
            return 0;
        } else {
            return (operator()((x + 1) / 2) + 1);
        }
    }
};
template <> struct clog2<double> : public unary_operator<double>
{
    double operator()(const double x) const override;
};
template <> struct clog2<AST::FloatConst::Ptr> : public unary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(const AST::FloatConst::Ptr x) const override;
};
template <> struct clog2<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary plus
 */
template <class T> struct uplus : public unary_operator<T>
{
    T operator()(const T x) const { return x; }
};
template <> struct uplus<AST::FloatConst::Ptr> : public unary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(const AST::FloatConst::Ptr x) const override;
};
template <> struct uplus<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary minus
 */
template <class T> struct uminus : public unary_operator<T>
{
    T operator()(const T x) const { return -x; }
};
template <> struct uminus<AST::FloatConst::Ptr> : public unary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(const AST::FloatConst::Ptr x) const override;
};
template <> struct uminus<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary logical not
 */
template <class T> struct ulnot : public unary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x) const { return (x != 0) ? T{0} : T{1}; }
};
template <> struct ulnot<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary bit-wise not
 */
template <class T> struct unot : public unary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x) const { return ~x; }
};
template <> struct unot<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary and reduction
 */
template <class T> struct uand : public unary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    typedef std::numeric_limits<T> lim;
    T operator()(const T x) const
    {
        return (lim::is_signed ? static_cast<T>(-1) : lim::max()) == x ? 1 : 0;
    }
};
template <> struct uand<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary nand reduction
 */
template <class T> struct unand : public unary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    typedef std::numeric_limits<T> lim;
    T operator()(const T x) const
    {
        return (lim::is_signed ? static_cast<T>(-1) : lim::max()) == x ? 0 : 1;
    }
};
template <> struct unand<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary or reduction
 */
template <class T> struct uor : public unary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x) const { return x == 0 ? 0 : 1; }
};
template <> struct uor<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary nor reduction
 */
template <class T> struct unor : public unary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x) const { return x == 0 ? 1 : 0; }
};
template <> struct unor<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary xor reduction, return 1 if there is an odd number of bit set.
 */
template <class T> struct uxor : public unary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x) const
    {
        T _x = x;
        const T width = sizeof(T) * 8;
        const T max = 1 << (width - 1);
        T count = 0;

        for(T i = 0; i < width; i++) {
            count = (_x & max) == 0 ? count : count + 1;
            _x <<= 1;
        }

        return count % 2;
    }
};
template <> struct uxor<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Unary xnor reduction, return 1 if there is an even number of bit set.
 */
template <class T> struct uxnor : public unary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x) const { return uxor<T>()(x) ? 0 : 1; }
};
template <> struct uxnor<AST::IntConstN::Ptr> : public unary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x) const override;
};

/**
 * Base class binary operator
 */
template <class T> struct binary_operator
{
    virtual T operator()(T x, T y) const = 0;
};

/**
 * Binary Pointer (bit read)
 */
template <class T> struct pointer : public binary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");
    T operator()(const T x, const T y) const { return (x & (1 << y)) ? 1 : 0; }
};
template <> struct pointer<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary power operator x^y
 */
template <class T> struct power : public binary_operator<T>
{
    T operator()(const T x, const T y) const
    {
        T _x{x}, _y{y};
        if(_y < T{0}) {
            return 0;
        }
        if(_x == T{0}) {
            return 0;
        }

        T result{1};
        while(_y != T{0}) {
            if((_y & T{1}) != T{0}) {
                result *= _x;
            }
            _y >>= 1;
            _x *= _x;
        }

        return result;
    }
};
template <> struct power<double> : public binary_operator<double>
{
    double operator()(double x, double y) const override;
};
template <> struct power<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};
template <> struct power<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary multiplication operator x*y
 */
template <class T> struct times : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x * y; }
};
template <> struct times<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};
template <> struct times<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary division operator x/y
 */
template <class T> struct divide : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x / y; }
};
template <> struct divide<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};
template <> struct divide<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary modulus operator x%y
 */
template <class T> struct mod : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x % y; }
};
template <> struct mod<double> : public binary_operator<double>
{
    double operator()(double x, double y) const override;
};
template <> struct mod<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};
template <> struct mod<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary plus operator x+y
 */
template <class T> struct plus : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x + y; }
};
template <> struct plus<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};
template <> struct plus<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary plus operator x-y
 */
template <class T> struct minus : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x - y; }
};
template <> struct minus<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};
template <> struct minus<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary shift left operator x<<y
 */
template <class T> struct sll : public binary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x, const T y) const { return x << y; }
};
template <> struct sll<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary logical shift right operator x>>y
 */
template <class T> struct srl : public binary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x, const T y) const
    {
        typedef std::numeric_limits<T> lim;
        T _x = x;
        for(T i = 0; i < y; i++) {
            _x = (_x >> 1) & lim::max();
        }
        return _x;
    }
};
template <> struct srl<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary arithmetical shift right operator x>>y
 */
template <class T> struct sra : public binary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x, const T y) const { return x >> y; }
};
template <> struct sra<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary less than operator x<y
 */
template <class T> struct less_than : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x < y ? 1 : 0; }
};
template <> struct less_than<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};
template <> struct less_than<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary greater than operator x>y
 */
template <class T> struct greater_than : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x > y ? 1 : 0; }
};
template <> struct greater_than<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};
template <> struct greater_than<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary less or equal operator x<=y
 */
template <class T> struct less_equal : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x <= y ? 1 : 0; }
};
template <> struct less_equal<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};
template <> struct less_equal<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary greater or equal operator x>=y
 */
template <class T> struct greater_equal : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x >= y ? 1 : 0; }
};
template <>
struct greater_equal<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};
template <> struct greater_equal<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary equal operator x==y
 */
template <class T> struct equal : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x == y ? 1 : 0; }
};
template <> struct equal<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};
template <> struct equal<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};

/**
 * Binary not equal operator x!=y
 */
template <class T> struct not_equal : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return x != y ? 1 : 0; }
};
template <> struct not_equal<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};
template <> struct not_equal<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};

/**
 * Binary case equal operator x===y
 */
template <class T> struct case_equal : public binary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x, const T y) const { return x == y ? 1 : 0; }
};
template <> struct case_equal<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary case not equal operator x!==y
 */
template <class T> struct case_not_equal : public binary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x, const T y) const { return x != y ? 1 : 0; }
};
template <> struct case_not_equal<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary bitwise and operator x&y
 */
template <class T> struct bitwise_and : public binary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x, const T y) const { return x & y; }
};
template <> struct bitwise_and<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary bitwise xor operator x^y
 */
template <class T> struct bitwise_xor : public binary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x, const T y) const { return x ^ y; }
};
template <> struct bitwise_xor<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary bitwise xnor operator x~^y or x^~y
 */
template <class T> struct bitwise_xnor : public binary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x, const T y) const { return ~(x ^ y); }
};
template <> struct bitwise_xnor<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary bitwise or operator x|y
 */
template <class T> struct bitwise_or : public binary_operator<T>
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");

    T operator()(const T x, const T y) const { return x | y; }
};
template <> struct bitwise_or<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};

/**
 * Binary logical and operator x&&y
 */
template <class T> struct logical_and : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return (x != 0) && (y != 0); }
};
template <> struct logical_and<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};
template <> struct logical_and<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};

/**
 * Binary logical and operator x||y
 */
template <class T> struct logical_or : public binary_operator<T>
{
    T operator()(const T x, const T y) const { return (x != 0) || (y != 0); }
};
template <> struct logical_or<AST::IntConstN::Ptr> : public binary_operator<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(AST::IntConstN::Ptr x, AST::IntConstN::Ptr y) const override;
};
template <> struct logical_or<AST::FloatConst::Ptr> : public binary_operator<AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(AST::FloatConst::Ptr x, AST::FloatConst::Ptr y) const override;
};

/**
 * Ternary operator
 */
template <class T_TEST, class T> struct ternary
{
    T operator()(const T_TEST x, const T y, const T z) const { return x ? y : z; }
};
template <> struct ternary<AST::IntConstN::Ptr, AST::Node::Ptr>
{
    AST::Node::Ptr operator()(const AST::IntConstN::Ptr x, const AST::Node::Ptr y,
                              const AST::Node::Ptr z) const;
};
template <> struct ternary<AST::FloatConst::Ptr, AST::Node::Ptr>
{
    AST::Node::Ptr operator()(const AST::FloatConst::Ptr x, const AST::Node::Ptr y,
                              const AST::Node::Ptr z) const;
};
template <> struct ternary<AST::IntConstN::Ptr, AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x, const AST::IntConstN::Ptr y,
                                   const AST::IntConstN::Ptr z) const;
};
template <> struct ternary<AST::IntConstN::Ptr, AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(const AST::IntConstN::Ptr x, const AST::FloatConst::Ptr y,
                                    const AST::FloatConst::Ptr z) const;
};
template <> struct ternary<AST::FloatConst::Ptr, AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::FloatConst::Ptr x, const AST::IntConstN::Ptr y,
                                   const AST::IntConstN::Ptr z) const;
};
template <> struct ternary<AST::FloatConst::Ptr, AST::FloatConst::Ptr>
{
    AST::FloatConst::Ptr operator()(const AST::FloatConst::Ptr x, const AST::FloatConst::Ptr y,
                                    const AST::FloatConst::Ptr z) const;
};

/**
 * Partselect (bits read)
 *
 * example: var[31 : 24]
 */
template <class T> struct partselect
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");
    T operator()(const T x, const T msb, const T lsb) const
    {
        const T msb1 = msb + 1;
        if(msb1 < (sizeof(T) * 8)) {
            T mask = (1 << msb1) - 1;
            return (x & mask) >> lsb;
        } else {
            return x >> lsb;
        }
    }
};
template <> struct partselect<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x, const AST::IntConstN::Ptr msb,
                                   const AST::IntConstN::Ptr lsb) const;
};

/**
 * Partselect minus indexed
 *
 * example: var[31 -: 8]  (equivalent to var[24 +: 8])
 */
template <class T> struct partselect_minus
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");
    T operator()(const T x, const T msb, const T index) const
    {
        const partselect<T> op;
        return op(x, msb, msb - index + 1);
    }
};
template <> struct partselect_minus<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x, const AST::IntConstN::Ptr msb,
                                   const AST::IntConstN::Ptr index) const;
};

/**
 * Partselect minus indexed
 *
 * example: var[24 +: 8]
 */
template <class T> struct partselect_plus
{
    static_assert(!std::is_floating_point<T>::value,
                  "operator may not have a floating point operand");
    T operator()(const T x, const T lsb, const T index) const
    {
        const partselect<T> op;
        return op(x, lsb + index - 1, lsb);
    }
};
template <> struct partselect_plus<AST::IntConstN::Ptr>
{
    AST::IntConstN::Ptr operator()(const AST::IntConstN::Ptr x, const AST::IntConstN::Ptr lsb,
                                   const AST::IntConstN::Ptr index) const;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
