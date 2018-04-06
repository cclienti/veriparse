#include <veriparse/passes/transformations/expression_operators.hpp>
#include <veriparse/logger/logger.hpp>

#include <memory>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			template<>
			AST::IntConstN::Ptr unsigned_fct<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();
					mpz_class max;

					if(size_x < 0) {
						LOG_WARNING << *x << ", " << "truncating to 32-bit";
						size_x = 32;
						max = (mpz_class(1) << size_x);
						val_x = val_x % max;
					}
					else {
						max = (mpz_class(1) << size_x);
					}

					int base_res = base_x;
					int size_res = size_x;
					bool sign_res = false;
					mpz_class res = (val_x < 0) ? max + val_x : val_x;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr signed_fct<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();
					mpz_class max;

					if(size_x < 0) {
						LOG_WARNING << *x << ", " << "truncating to 32-bit";
						size_x = 32;
						max = (mpz_class(1) << size_x);
						val_x = val_x % max;
					}
					else {
						max = (mpz_class(1) << size_x);
					}

					mpz_class mid = mpz_class(1) << (size_x-1);

					int base_res = base_x;
					int size_res = size_x;
					bool sign_res = true;
					mpz_class res = (val_x >= mid) ? val_x - max : val_x;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr clog2<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					int base_res = base_x;
					int size_res = size_x;
					bool sign_res = false;
					mpz_class res = clog2<mpz_class>()(val_x);

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr uplus<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					int base_res = base_x;
					int size_res = size_x;
					bool sign_res = sign_x;
					mpz_class res = val_x;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr uminus<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					//bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					int base_res = base_x;
					int size_res = size_x;
					bool sign_res = true;
					mpz_class res = -val_x;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr ulnot<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					int base_res = base_x;
					int size_res = size_x;
					bool sign_res = sign_x;
					mpz_class res = (val_x != 0) ? mpz_class(0) : mpz_class(1);

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr unot<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					int base_res = base_x;
					int size_res = size_x;
					bool sign_res = sign_x;
					mpz_class res = ~val_x;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr uand<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res;

					if (size_x == -1) {
						if (!sign_x) {
							LOG_WARNING << *x << ", " << "non-sized integer should be signed";
						}
						res = (val_x == -1) ? mpz_class(1) : mpz_class(0);
					}
					else {
						if (sign_x) {
							res = (val_x == -1) ? mpz_class(1) : mpz_class(0);
						}
						else {
							mpz_class mpz_max = (mpz_class(1) << size_x) - 1;

							res = (val_x == mpz_max) ? mpz_class(1) : mpz_class(0);
						}
					}

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr unand<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					uand<AST::IntConstN::Ptr> op;
					AST::IntConstN::Ptr z = op(x);
					mpz_class res = (z->get_value() == 0) ? mpz_class(1) : mpz_class(0);
					z->set_value(res);
					return z;
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr uor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					mpz_class val_x = x->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = (val_x == 0) ? 0 : 1;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr unor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					uor<AST::IntConstN::Ptr> op;
					AST::IntConstN::Ptr z = op(x);
					mpz_class res = (z->get_value() == 0) ? mpz_class(1) : mpz_class(0);
					z->set_value(res);
					return z;
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr uxor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					int size_x = x->get_size();
					mpz_class val_x = x->get_value();

					if (size_x == -1) {
						LOG_WARNING << x << ", truncating value to 32-bits signed";
						size_x = 32;
					}
					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res;

					const mpz_class max = mpz_class(1) << (size_x-1);
					int count = 0;

					for(int i=0; i<size_x; i++) {
						count = (val_x & max) == 0 ? count : count + 1;
						val_x <<= 1;
					}

					res = mpz_class(count % 2);

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr uxnor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x) const {
				if (x) {
					uxor<AST::IntConstN::Ptr> op;
					AST::IntConstN::Ptr z = op(x);
					mpz_class res = (z->get_value() == 0) ? mpz_class(1) : mpz_class(0);
					z->set_value(res);
					return z;
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr pointer<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                             const AST::IntConstN::Ptr y) const {
				if (x && y) {
					mpz_class val_x = x->get_value();
					mpz_class val_y = y->get_value();
					power<mpz_class> op;
					mpz_class mask = op(mpz_class(2), val_y);
					mpz_class res = ((val_x & mask) != 0) ? mpz_class(1) : mpz_class(0);
					return std::make_shared<AST::IntConstN>(2, 1, false, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr power<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                           const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					int size_y = y->get_size();
					bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = base_x;
					int size_res = ((size_x==-1) || (size_y==-1)) ? -1: size_x*size_y;
					bool sign_res = sign_x || sign_y;
					mpz_class res;

					if ((val_y < 0) || (val_x == 0)) {
						res = 0;
					}
					else {
						res = 1;
						while (val_y) {
							if ((val_y & mpz_class(1))==1) res *= val_x;
							val_y >>= 1;
							val_x *= val_x;
						}
					}

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr times<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                           const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					int size_y = y->get_size();
					bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = base_x;
					int size_res = ((size_x==-1) || (size_y==-1)) ? -1: size_x+size_y;
					bool sign_res = sign_x || sign_y;
					mpz_class res = val_x * val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr divide<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                            const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = base_x;
					int size_res = (size_x==-1) ? -1: size_x;
					bool sign_res = sign_x || sign_y;
					mpz_class res = val_x / val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr mod<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                         const AST::IntConstN::Ptr y) const {
				if (x && y) {
					//int base_x = x->get_base();
					//int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					int base_y = y->get_base();
					int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = base_y;
					int size_res = (size_y==-1) ? -1: size_y;
					bool sign_res = sign_x;
					mpz_class res = val_x % val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr plus<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                          const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					int size_y = y->get_size();
					bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = base_x;
					int size_res = ((size_x==-1) || (size_y==-1)) ? -1: std::max(size_x, size_y)+1;
					bool sign_res = sign_x || sign_y;
					mpz_class res = val_x + val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr minus<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                           const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					int size_y = y->get_size();
					bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = base_x;
					int size_res = ((size_x==-1) || (size_y==-1)) ? -1: std::max(size_x,size_y)+1;
					bool sign_res = sign_x || sign_y;
					mpz_class res = val_x - val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr sll<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                         const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();
					uint64_t shift = val_y.get_ui();

					int base_res = base_x;
					int size_res = (size_x==-1) ? -1: size_x+shift;
					bool sign_res = sign_x;
					mpz_class res = val_x << shift;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr srl<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                         const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();
					uint64_t shift = val_y.get_ui();

					int base_res = base_x;
					int size_res = (size_x==-1) ? -1: size_x-shift;
					bool sign_res = sign_x;
					mpz_class res = val_x;

					if (res < 0) {
						// mpz_t variables represent integers using sign
						// and magnitude. We need to "cast" the signed
						// number into an unsigned number by applying an
						// offset (mpz_t does not use the two's
						// complement number format).
						size_t s = mpz_sizeinbase(val_x.get_mpz_t(), 2);
						mpz_class max = mpz_class(1) << (s+1);
						res = max + res;
					}
					res = res >> shift;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr sra<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                         const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();
					uint64_t shift = val_y.get_ui();

					int base_res = base_x;
					int size_res = (size_x==-1) ? -1: size_x-shift;
					bool sign_res = sign_x;
					mpz_class res = val_x >> shift;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr less_than<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                               const AST::IntConstN::Ptr y) const {
				if (x && y) {
					// int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = val_x < val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr greater_than<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                  const AST::IntConstN::Ptr y) const {
				if (x && y) {
					// int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = val_x > val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr less_equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                const AST::IntConstN::Ptr y) const {
				if (x && y) {
					// int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = val_x <= val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr greater_equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                   const AST::IntConstN::Ptr y) const {
				if (x && y) {
					// int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = val_x >= val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                           const AST::IntConstN::Ptr y) const {
				if (x && y) {
					// int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = val_x == val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr not_equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                               const AST::IntConstN::Ptr y) const {
				if (x && y) {
					// int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = val_x != val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr case_equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                const AST::IntConstN::Ptr y) const {
				if (x && y) {
					// int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = val_x == val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}



			template<>
			AST::IntConstN::Ptr case_not_equal<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                    const AST::IntConstN::Ptr y) const {
				if (x && y) {
					// int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = val_x != val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


			template<>
			AST::IntConstN::Ptr bitwise_and<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                 const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					int size_y = y->get_size();
					bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = base_x;
					int size_res = ((size_x==-1) || (size_y==-1)) ? -1: std::max(size_x, size_y);
					bool sign_res = sign_x || sign_y;
					mpz_class res = val_x & val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr bitwise_xor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                 const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					int size_y = y->get_size();
					bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = base_x;
					int size_res = ((size_x==-1) || (size_y==-1)) ? -1: std::max(size_x, size_y);
					bool sign_res = sign_x || sign_y;
					mpz_class res = val_x ^ val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr bitwise_xnor<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                  const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					int size_y = y->get_size();
					bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = base_x;
					int size_res = ((size_x==-1) || (size_y==-1)) ? -1: std::max(size_x, size_y);
					bool sign_res = sign_x || sign_y;
					mpz_class res = ~(val_x ^ val_y);

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr bitwise_or<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                const AST::IntConstN::Ptr y) const {
				if (x && y) {
					int base_x = x->get_base();
					int size_x = x->get_size();
					bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					int size_y = y->get_size();
					bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = base_x;
					int size_res = ((size_x==-1) || (size_y==-1)) ? -1: std::max(size_x, size_y);
					bool sign_res = sign_x || sign_y;
					mpz_class res = val_x | val_y;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr logical_and<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                 const AST::IntConstN::Ptr y) const {
				if (x && y) {
					//int base_x = x->get_base();
					//int size_x = x->get_size();
					//bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = (val_x != 0) && (val_y != 0);

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr logical_or<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                const AST::IntConstN::Ptr y) const {
				if (x && y) {
					// int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					//int base_y = y->get_base();
					//int size_y = y->get_size();
					//bool sign_y = y->get_sign();
					mpz_class val_y = y->get_value();

					int base_res = 2;
					int size_res = 1;
					bool sign_res = false;
					mpz_class res = (val_x != 0) || (val_y != 0);

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr partselect<AST::IntConstN::Ptr>::operator()(const AST::IntConstN::Ptr x,
			                                                                const AST::IntConstN::Ptr msb,
			                                                                const AST::IntConstN::Ptr lsb) const {
				if (x && msb && lsb) {
					int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					int val_msb = msb->get_value().get_ui();
					int val_lsb = lsb->get_value().get_ui();

					int base_res = base_x;
					int size_res = val_msb - val_lsb + 1;
					bool sign_res = false;
					mpz_class mask = (mpz_class(1)<<(val_msb+1))-1;
					mpz_class res = (val_x & mask) >> val_lsb;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr partselect_minus<AST::IntConstN::Ptr>
			::operator()(const AST::IntConstN::Ptr x,
			             const AST::IntConstN::Ptr msb, const AST::IntConstN::Ptr index) const {
				if (x && msb && index) {
					int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					int val_msb = msb->get_value().get_ui();
					int val_index = index->get_value().get_ui();
					int val_lsb = val_msb-val_index+1;

					int base_res = base_x;
					int size_res = val_index;
					bool sign_res = false;
					mpz_class mask = (mpz_class(1)<<(val_msb+1))-1;
					mpz_class res = (val_x & mask) >> val_lsb;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}

			template<>
			AST::IntConstN::Ptr partselect_plus<AST::IntConstN::Ptr>
			::operator()(const AST::IntConstN::Ptr x,
			             const AST::IntConstN::Ptr lsb, const AST::IntConstN::Ptr index) const {
				if (x && lsb && index) {
					int base_x = x->get_base();
					// int size_x = x->get_size();
					// bool sign_x = x->get_sign();
					mpz_class val_x = x->get_value();

					int val_lsb = lsb->get_value().get_ui();
					int val_index = index->get_value().get_ui();
					int val_msb = val_lsb+val_index-1;

					int base_res = base_x;
					int size_res = val_index;
					bool sign_res = false;
					mpz_class mask = (mpz_class(1)<<(val_msb+1))-1;
					mpz_class res = (val_x & mask) >> val_lsb;

					return std::make_shared<AST::IntConstN>(base_res, size_res, sign_res, res);
				}
				else {
					return AST::IntConstN::Ptr();
				}
			}


		}
	}
}
