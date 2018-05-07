# Stuff

## Notation

> All indicies are zero-based.

```
0Q2, 2Q7    Shift register output    REGISTER_INDEX "Q" OUTPUT_INDEX
1B, 4G      Display segment          DIGIT_INDEX SEGMENT
```

### Segments

```
  A
F   B
  G
E   C
  D
```

## Display Pinout

> Note that there is no 0F because the first digit in a clock will only show a 1 or 2 in all cases.
> So, they cheaped out on exactly 1 LED, making using the first two digits as a temperature display
> impossible. Jerks.

```
Vcc N/A AM  N/A 0A 0D MON 1F N/A 1E 1G 1D DP0    DP1 THU FRI 2A 2B 2C 3E N/A 3F N/A 3D 3G SUN N/A
N/A Vdd N/A AL1 0G 0E TUE 1A 0B  0C 1B 1C N/A    N/A WED N/A 2F 2G 2D 2E SAT 3A BAT 3C 3B N/A N/A
          \ /                              \      /
        Combined                           Combined
```

## Protocol

> Confusing, I know. But, software is easier to change than hardware.  
> Swapping 0Q1 and 0Q7 would be nice, so that the two power bits are the first two bits, instead of first and last =[.

To power bank 0, set bit 7 high. For power bank 1, set bit 0 high.  
Then, set the corresponding segment's bit low and all other segment bits high.

For example, to light up segment FRI: `01111111 01111111 1111111`

```
Bit             0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23
Bit Offset      23  22  21  20  19  18  17  16  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
Shift Register  0Q0 0Q1 0Q2 0Q3 0Q4 0Q5 0Q6 0Q7 1Q0 1Q1 1Q2 1Q3 1Q4 1Q5 1Q6 1Q7 2Q0 2Q1 2Q2 2Q3 2Q4 2Q5 2Q6 2Q7
Bank 0          N/A N/A 1F  MON 0D  0A  AM  Vcc 1E  2B  2A  FRI THU DP  1D  1G  2C  SUN 3G  3D  N/A 3F  N/A 3E
Bank 1          Vdd 0B  1A  TUE 0E  0G  AL1 N/A 0C  2G  2F  N/A WED N/A 1C  1B  2D  N/A 3B  3C  BAT 3A  SAT 2E
```

