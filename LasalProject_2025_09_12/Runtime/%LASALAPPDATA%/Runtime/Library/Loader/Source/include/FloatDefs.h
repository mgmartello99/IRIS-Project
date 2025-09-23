#pragma once

// bit masks for masking parts of on floating value

// sign bit
#define MASK_SIGN         16#8000_0000
// exponent
#define MASK_EXPONENT     16#7F80_0000
// earlier mantissa
#define MASK_SIGNIFICAND  16#007F_FFFF

// mask significand for quit NaN
#define MASK_QUIET_NAN    16#0040_0000
