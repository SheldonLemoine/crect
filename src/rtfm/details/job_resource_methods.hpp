
#pragma once


#include "kvasir/mpl/mpl.hpp"
#include "rtfm/details/interrupts.hpp"
#include "rtfm/details/job_resource.hpp"
#include "rtfm/details/job_resource_details.hpp"

namespace rtfm
{

/**
 * @brief Interface to generate the resource tree.
 *
 * @tparam JobList   List of Jobs.
 */
template <typename JobList>
struct make_resource_tree
{
  /* Take all the job<resources> and transform to resource<jobs> */
  using j2r = kvasir::mpl::eager::flatten<
                kvasir::mpl::eager::transform<
                  JobList,
                  job_to_resource
                >
              >;

  using result = typename details::make_resource_tree_impl<j2r>::f;
};

} /* END namespace rtfm */
