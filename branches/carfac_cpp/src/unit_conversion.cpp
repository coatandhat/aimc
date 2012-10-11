#include "unit_conversion.h"

float ERB_Hz(float cf_hz, float erb_break_freq, float erb_q){
  return (erb_break_freq + cf_hz) / erb_q;
}
