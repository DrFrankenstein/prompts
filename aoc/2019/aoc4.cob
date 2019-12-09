       identification division.
       program-id. aoc4.

      * We're going to use an interesting property of COBOL, which is
      * that it encodes numbers in decimal by default (instead of
      * binary). Because of that, we can redefine our 'the-number' 
      * variable as an array of 6 'digits' and address each individual
      * digit without doing any kind of conversion. 
      
      * The 'redefines' clause reinterprets an existing variable as a
      * different data type (pic).

       data division.
       working-storage section.
       01 the-number    pic 9(6).
       01 number-string redefines the-number.
          05 digits pic 9 occurs 6 times.

       77 lower-bound   pic 9(6).
       77 upper-bound   pic 9(6).

       77 idx           pic 9.
       77 digit         pic 9.

       77 good-count    pic 9(6) value 0.
       77 good-count-disp pic ZZZZZ9.

       77 number-status pic 9.
          88 good value 1.
          88 bad  value 0.

       procedure division.
       solve.
      * if you want to be a true mainframer and run this on z/OS, i, or
      * something of the sort, you might need to change these accepts.
      * I'm assuming the common case of running this on a toy computer
      * with MicroFocus or GnuCOBOL.
           accept lower-bound from argument-value.
           accept upper-bound from argument-value.

           if lower-bound = 0 and upper-bound = 0
             or lower-bound > upper-bound then
               display 'usage: aoc4 <lower-bound> <upper-bound>'
               display ' e.g.: aoc4 123456 456789'
               stop run
           end-if

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
      * we found a decreasing digit, so we're going to replace it and
      * all the following digits by the one before it, effectively
      * skipping an entire range of invalid numbers
      * e.g. 123210 -> 123333
           move digits(idx - 1) to digit.
           perform until idx > 6
               move digit to digits(idx)
               add 1 to idx
           end-perform.

       check-repeating.
           move 0 to number-status.

           perform varying idx from 2 by 1 until idx > 6
               if digits(idx) = digits(idx - 1) then
      *          * current digit same as previous

                   if idx = 6 or digits(idx) not = digits(idx + 1) then
      *             * next digit different from current or at end (good)
                      move 1 to number-status
                      exit paragraph
                   else
      *              * 3 or more consecutive digits (bad); skip them
                       move digits(idx) to digit
                       perform until idx > 6 or digits(idx) not = digit
                           add 1 to idx
                       end-perform
                   end-if
               end-if
           end-perform.
           