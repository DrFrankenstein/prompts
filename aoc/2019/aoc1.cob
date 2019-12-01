       identification division.
       program-id. aoc1.

       environment division.
           input-output section.
             file-control.
               select input-file assign to 'input.txt'
                 organization is line sequential
                 file status is file-status.

       data division.
           file section.
           fd input-file.
           01 mass        pic X(6).

           working-storage section.
           77 file-status pic 99.
           77 total-fuel  pic 9(8) value 0.
           77 curr-mass   pic S9(6).
           77 curr-fuel   pic S9(6).

       procedure division.
       0000-main.
           open input input-file.
           perform 0010-calculate-fuel
             until file-status equal to 10.                             (end of file)
           close input-file.
           
           display 'Total fuel needed: ' total-fuel.
           stop run.
       
       0010-calculate-fuel.
           read input-file
             at end exit paragraph.

           move function numval(mass) to curr-mass.
           
           perform until curr-mass is <= 0
               compute curr-fuel = curr-mass / 3 - 2

               if curr-fuel is > 0 then
                  add curr-fuel to total-fuel
               end-if
               
               move curr-fuel to curr-mass
           end-perform.
