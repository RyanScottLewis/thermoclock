#pragma once

#define THERMOCLOCK_BANK_0 (1 << 16)
#define THERMOCLOCK_BANK_1 (1 << 23)

// The complement of the bit offset for the segment is XOR'd with the bank the segment is NOT in.

#define THERMOCLOCK_0A ( ~(1 << 18) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_0B ( ~(1 << 22) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_0C ( ~(1 << 15) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_0D ( ~(1 << 19) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_0E ( ~(1 << 19) ^ THERMOCLOCK_BANK_0 )
// #define THERMOCLOCK_0F ( ~(1 << UNSET) ^ THERMOCLOCK_BANK_UNSET )
#define THERMOCLOCK_0G ( ~(1 << 18) ^ THERMOCLOCK_BANK_0 )

#define THERMOCLOCK_1A ( ~(1 << 21) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_1B ( ~(1 << 8 ) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_1C ( ~(1 << 9 ) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_1D ( ~(1 << 9 ) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_1E ( ~(1 << 15) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_1F ( ~(1 << 21) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_1G ( ~(1 << 15) ^ THERMOCLOCK_BANK_1 )

#define THERMOCLOCK_2A ( ~(1 << 13) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_2B ( ~(1 << 14) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_2C ( ~(1 << 7 ) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_2D ( ~(1 << 7 ) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_2E ( ~(1 << 0 ) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_2F ( ~(1 << 13) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_2G ( ~(1 << 14) ^ THERMOCLOCK_BANK_0 )

#define THERMOCLOCK_3A ( ~(1 << 2 ) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_3B ( ~(1 << 5 ) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_3C ( ~(1 << 4 ) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_3D ( ~(1 << 4 ) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_3E ( ~(1 << 0 ) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_3F ( ~(1 << 2 ) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_3G ( ~(1 << 5 ) ^ THERMOCLOCK_BANK_1 )

#define THERMOCLOCK_MON ( ~(1 << 20) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_TUE ( ~(1 << 20) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_WED ( ~(1 << 11) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_THU ( ~(1 << 11) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_FRI ( ~(1 << 12) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_SAT ( ~(1 << 1 ) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_SUN ( ~(1 << 6 ) ^ THERMOCLOCK_BANK_1 )

#define THERMOCLOCK_AM  ( ~(1 << 17) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_DP  ( ~(1 << 10) ^ THERMOCLOCK_BANK_0 )
#define THERMOCLOCK_AL1 ( ~(1 << 17) ^ THERMOCLOCK_BANK_1 )
#define THERMOCLOCK_BAT ( ~(1 << 3 ) ^ THERMOCLOCK_BANK_1 )

// Not using typedef because Linux conventions
struct thermoclock_segment {
  const char *name;
  const u32  value;
};

#define THERMOCLOCK_SEGMENT(NAME) { #NAME, THERMOCLOCK_##NAME },

struct thermoclock_segment thermoclock_segments[] = {
  THERMOCLOCK_SEGMENT(0A)
  THERMOCLOCK_SEGMENT(0B)
  THERMOCLOCK_SEGMENT(0C)
  THERMOCLOCK_SEGMENT(0D)
  THERMOCLOCK_SEGMENT(0E)
  //THERMOCLOCK_SEGMENT(0F)
  THERMOCLOCK_SEGMENT(0G)

  THERMOCLOCK_SEGMENT(1A)
  THERMOCLOCK_SEGMENT(1B)
  THERMOCLOCK_SEGMENT(1C)
  THERMOCLOCK_SEGMENT(1D)
  THERMOCLOCK_SEGMENT(1E)
  THERMOCLOCK_SEGMENT(1F)
  THERMOCLOCK_SEGMENT(1G)

  THERMOCLOCK_SEGMENT(2A)
  THERMOCLOCK_SEGMENT(2B)
  THERMOCLOCK_SEGMENT(2C)
  THERMOCLOCK_SEGMENT(2D)
  THERMOCLOCK_SEGMENT(2E)
  THERMOCLOCK_SEGMENT(2F)
  THERMOCLOCK_SEGMENT(2G)

  THERMOCLOCK_SEGMENT(3A)
  THERMOCLOCK_SEGMENT(3B)
  THERMOCLOCK_SEGMENT(3C)
  THERMOCLOCK_SEGMENT(3D)
  THERMOCLOCK_SEGMENT(3E)
  THERMOCLOCK_SEGMENT(3F)
  THERMOCLOCK_SEGMENT(3G)

  THERMOCLOCK_SEGMENT(MON)
  THERMOCLOCK_SEGMENT(TUE)
  THERMOCLOCK_SEGMENT(WED)
  THERMOCLOCK_SEGMENT(THU)
  THERMOCLOCK_SEGMENT(FRI)
  THERMOCLOCK_SEGMENT(SAT)
  THERMOCLOCK_SEGMENT(SUN)

  THERMOCLOCK_SEGMENT(AM)
  THERMOCLOCK_SEGMENT(DP)
  THERMOCLOCK_SEGMENT(AL1)
  THERMOCLOCK_SEGMENT(BAT)

  { NULL, 0 }
};
