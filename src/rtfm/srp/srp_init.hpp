
#pragma once

#include <initializer_list>
#include <type_traits>
#include <cstdint>

#include "kvasir/mpl/mpl.hpp"
#include "rtfm/rtfm_utils.hpp"

namespace rtfm
{
namespace srp
{
namespace details
{

/**
 * @brief Type wrapper for the for-each implementation.
 */
template <typename T>
struct _wrapper {};

/**
 * @brief For-each implementation, executes over every type in a list.
 *
 * @tparam Fun  Functor for which each type will be executed in.
 * @tparam Ts   Pack of types in the list.
 */
template <typename Fun, typename... Ts>
constexpr void for_each_impl(kvasir::mpl::list<Ts...>)
{
  std::initializer_list<int> {
    ( Fun{}( details::_wrapper<Ts>{} ), 0)...
  };
}

}

/**
 * @brief For-each interface, executes over every type in a list.
 *
 * @tparam Fun    Functor for which each type will be executed in.
 * @tparam List   List of types.
 */
template <typename Fun, typename List>
constexpr void for_each(void)
{
  details::for_each_impl<Fun>(List{});
}

/**
 * @brief Code generator for the NVIC initialization code.
 */
struct job_to_nvic_printer
{
  template <typename Job>
  void operator()(details::_wrapper<Job>) const
  {
    using ISRn = typename Job::ISR::index;
    using Prio = typename Job::P;


    if (ISRn{} < 0)
    {
      constexpr auto index = (static_cast<uint32_t>(ISRn{}) & 0xFUL) - 4UL;

      /* Enable system interrupt. */
      SCB->SHP[index] = util::priority_to_NVIC_priority(Prio{});
    }
    else
    {
      /* Enable interrupt. */
      NVIC->ISER[ISRn{} >> 5UL] = (1UL << (ISRn{} & 0x1FUL));

      /* Set priority */
      NVIC->IP[ISRn{}] = util::priority_to_NVIC_priority(Prio{});
    }
  }
};

/**
 * @brief Initialize the NVIC based on a list of RTFM jobs.
 *
 * @tparam JobList    The system's job list.
 */
template <typename JobList>
constexpr void initialize_jobs_impl()
{
  /* Enable DWT */
  DWT->CTRL |= (1 << DWT_CTRL_CYCCNTENA_Pos);

  /* Fill the ISR settings. */
  for_each<job_to_nvic_printer, JobList>();
}


} /* END namespace srp */
} /* END namespace rtfm */