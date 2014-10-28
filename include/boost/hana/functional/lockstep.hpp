/*!
@file
Defines `boost::hana::lockstep`.

@copyright Louis Dionne 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_FUNCTIONAL_LOCKSTEP_HPP
#define BOOST_HANA_FUNCTIONAL_LOCKSTEP_HPP

#include <boost/hana/detail/closure.hpp>
#include <boost/hana/detail/create.hpp>
#include <boost/hana/detail/std/forward.hpp>
#include <boost/hana/detail/std/move.hpp>


namespace boost { namespace hana {
    //! @ingroup group-functional
    //! Invoke a function with the result of invoking other functions on its
    //! arguments, in lockstep.
    //!
    //! Specifically, `lockstep(f, g1, ..., gN)` is a function such that
    //! @code
    //!     lockstep(f, g1, ..., gN)(x1, ..., xN) == f(g1(x1), ..., gN(xN))
    //! @endcode
    //!
    //! Since each `g` is invoked on its corresponding argument in lockstep,
    //! the number of arguments must match the number of `g`s.
    //!
    //! ### Example
    //! @snippet example/functional/lockstep.cpp main
    //!
    //! @todo
    //! - I think this is equivalent to `<*>` for `((->) r)`.
    //! - Change the syntax to be the same as `demux`. Impossible right now
    //!   because Clang blows up.
#ifdef BOOST_HANA_DOXYGEN_INVOKED
    constexpr auto lockstep = [](auto&& f, auto&& ...g) {
        return [perfect-capture](auto&& ...x) -> decltype(auto) {
            return forwarded(f)(forwarded(g)(forwarded(x))...);
        };
    };
#else
    template <typename F, typename G>
    struct _lockstep;

    template <typename F, typename ...G>
    struct _lockstep<F, detail::closure<G...>> {
        F f;
        detail::closure<G...> g;

        template <typename ...X>
        constexpr decltype(auto) operator()(X&& ...x) const& {
            return f(static_cast<G const&>(g).get(detail::std::forward<X>(x))...);
        }

        template <typename ...X>
        constexpr decltype(auto) operator()(X&& ...x) & {
            return f(static_cast<G&>(g).get(detail::std::forward<X>(x))...);
        }

        template <typename ...X>
        constexpr decltype(auto) operator()(X&& ...x) && {
            return f(static_cast<G&&>(g).get(detail::std::forward<X>(x))...);
        }
    };

    struct _make_lockstep {
        template <typename F, typename ...G>
        constexpr decltype(auto) operator()(F&& f, G&& ...g) const {
            return detail::create<_lockstep>{}(
                detail::std::forward<F>(f),
                detail::create<detail::closure_t>{}(detail::std::forward<G>(g)...)
            );
        }
    };

    constexpr _make_lockstep lockstep{};
#endif
}} // end namespace boost::hana

#endif // !BOOST_HANA_FUNCTIONAL_LOCKSTEP_HPP
