       identification division.
       program-id. aoc4.

       data division.
       working-storage section.
       01 the-number    pic 9(6).
       01 number-string redefines the-number.
          05 digits pic 9 occurs 6 times.

       77 lower-bound   pic 9(6) value 367479.
       77 upper-bound   pic 9(6) value 893698.

       77 idx           pic 9.
       77 digit         pic 9.

       77 good-count    pic 9(6) value 0.
       77 good-count-disp pic Z(6).

       77 number-status pic 9.
          88 good value 1.
          88 bad  value 0.

       procedure division.
       solve.
           perform varying the-number 
             from lower-bound by 1
             until the-number >= upper-bound
               
               perform check-increasing
               perform check-repeating

               if good and the-number < upper-bound then
                   add 1 to good-count
               end-if
           end-perform.

           move good-count to good-count-disp.
           display 'Found ' good-count-disp ' potential passwords'.

           stop run.

       check-increasing.
           perform varying idx from 2 by 1 until idx > 6
               if digits(idx) < digits(idx - 1) then
                   perform fixup
               end-if
           end-perform.

       fixup.
           move digits(idx - 1) to digit.
           perform until idx > 6
               move digit to digits(idx)
               add 1 to idx
           end-perform.

       check-repeating.
           move 0 to number-status.

           perform varying idx from 2 by 1 until idx > 6
               if digits(idx) = digits(idx - 1) then
                   move 1 to number-status
                   exit paragraph
               end-if
           end-perform.
           