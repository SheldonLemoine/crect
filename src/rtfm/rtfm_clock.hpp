
#pragma once

#include <cstdint>
#include <chrono>
#include "kvasir/mpl/mpl.hpp"
#include "rtfm/details/job_resource.hpp"
#include "rtfm/srp/srp_locks.hpp"

namespace rtfm
{
namespace time
{

/**
 * @brief     The definition of the system clock.
 * @details   It is based on the DWT cycle counter for calculating time, this
 *            means that system_clock::now() needs to be called at least once
 *            per overflow of the DWT, else the time will not be correct.
 *            It is also possible to replace this function with another timer
 *            implementation.
 */
struct system_clock
{
  /** @brief  Definition of the duration of timer ticks running at MCU clock. */
  using duration = std::chrono::duration<int64_t, std::ratio<1, __F_CPU>>;
  using small_duration = std::chrono::duration<int32_t, std::ratio<1, __F_CPU>>;
  using rep = duration::rep;
  using period = duration::period;
  using time_point = std::chrono::time_point<system_clock>;
  static const bool is_steady = false;

  /**
   * @brief   Extraction of the current time in clock ticks using the DWT.
   *
   * @note    This function MUST be called more often than the time for an
   *          overflow of the DWT counter, else the system time will be
   *          incorrect.
   *
   * @note    This is a shared resouce, must be accessed from within a lock.
   *
   * @return  Returns the current time.
   */
  static time_point now() noexcept
  {
    /* Holds the current offset from start time due to DWT overflows. */
    static uint32_t base = 0;

    /* Holds the old DWT value to check for overflows. */
    static uint32_t old_dwt = 0;

    uint32_t dwt = DWT->CYCCNT;

    /* If the DWT has overflowed, update the base. */
    if (old_dwt > dwt)
      base += 1;

    /* Save old DWT. */
    old_dwt = dwt;

    return time_point(duration(
      (static_cast<uint64_t>(base) << 32U) + static_cast<uint64_t>(dwt)
    ));
  }
};


} /* END namespace time */

/**
 * @brief   Convenience definition of the clock resource.
 */
using Rsystem_clock =
    resource<time::system_clock,
             kvasir::mpl::integral_constant<
                decltype(&time::system_clock::now),
                &time::system_clock::now
             >,
             false>;


} /* END namespace rtfm */
