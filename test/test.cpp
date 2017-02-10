
#include "rtfm/rtfm_srp.hpp"
#include "util/print_types.hpp"

/* TODO: Implement "get vector table" from job list. */


//void test_rtfm()
//{
//  rtfm::srp::lock< R4 > lock;
//  /* Lock */
//
//  asm volatile("nop");
//  asm volatile("nop");
//  asm volatile("nop");
//  asm volatile("nop");
//
//  /* Automatic unlock via RAII */
//}

namespace rtfm
{
namespace srp
{

template <typename Job>
static inline void pend()
{
  using ISRn = typename Job::ISR::index;
  NVIC->ISPR[ISRn::value >> 5UL] = (1UL << (ISRn::value & 0x1FUL));
}

template <typename Job>
static inline void clear()
{
  using ISRn = typename Job::ISR::index;
  NVIC->ICPR[ISRn::value >> 5UL] = (1UL << (ISRn::value & 0x1FUL));
}

}
}

int main()
{
  print_list<rtfm::system_job_list>("System Jobs");

  print_list<rtfm::details::resource_tree>("Resource tree");

  rtfm::srp::pend<J1>();
  rtfm::srp::pend<J2>();
  rtfm::srp::pend<J3>();

  rtfm::srp::clear<J1>();
  rtfm::srp::clear<J2>();
  rtfm::srp::clear<J3>();

  //using isr = find_job_isr< find_job_isr_impl< brigand::integral_constant<int, 10> >::type >::isr;
  //std::cout << "\nisr: " << type_name< isr >() << std::endl;

  return 0;
}