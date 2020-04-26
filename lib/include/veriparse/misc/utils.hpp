#ifndef VERIPARSE_MISC_UTILS
#define VERIPARSE_MISC_UTILS

#include <type_traits>
#include <utility>
#include <iterator>

namespace Veriparse {
namespace Misc {
namespace Utils {

template <typename Container>
struct VIteratorDrop {
	using const_iterator = typename std::remove_reference_t<Container>::const_iterator;

	explicit VIteratorDrop(Container&& container, bool drop_first, bool drop_last) :
		m_container(std::forward<Container>(container)),
		m_drop_first(drop_first),
		m_drop_last(drop_last)
	{}

	const_iterator begin() {
		if (m_drop_first) {
			return m_container.empty() ? m_container.begin() : std::next(m_container.begin());
		}
		else {
			return m_container.begin();
		}
	}

	const_iterator end() {
		if (m_drop_last) {
			return m_container.empty() ? m_container.end() : std::prev(m_container.end());
		}
		else {
			return m_container.end();
		}
	}

private:
	const Container m_container;
	const bool m_drop_first;
	const bool m_drop_last;
};

template <typename Container>
inline auto viter_drop(Container&& container, bool drop_first = false, bool drop_last = false) {
	return VIteratorDrop<Container>(std::forward<Container>(container), drop_first, drop_last);
}

template <typename Container>
inline auto viter_drop_last(Container&& container) {
	return VIteratorDrop<Container>(std::forward<Container>(container), false, true);
}

template <typename Container>
inline auto viter_drop_first(Container&& container) {
	return VIteratorDrop<Container>(std::forward<Container>(container), true, false);
}

template <typename Container>
inline auto viter_drop_both(Container&& container) {
	return VIteratorDrop<Container>(std::forward<Container>(container), true, true);
}

}
}
}

#endif
