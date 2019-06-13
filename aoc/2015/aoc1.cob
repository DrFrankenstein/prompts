       identification division.
       program-id. aoc1.

       environment division.
           input-output section.
             file-control.
             select input-file assign to 'input.txt'
               organization is sequential
               file status is file-status.

       data division.
           file section.
           fd input-file.
           01 elevator-command pic x.
             88 elevator-up value '('.
             88 elevator-down value ')'.

           working-storage section.
           77 floor-num pic S99999 value 0.
           77 file-status pic 99.
           77 command-position pic 9999 value 0.
           77 seen-basement pic 9 value 0.

       procedure division.
       0000-main.
           open input input-file.
           perform 0010-run-elevator-command 
             until file-status equal to 10.                             (end of file)
           close input-file.

           display 'Floor: ' floor-num.
           stop run.

       0010-run-elevator-command.
           read input-file
             at end exit paragraph.

           add 1 to command-position.

           evaluate true
             when elevator-up
               add 1 to floor-num
             when elevator-down
               subtract 1 from floor-num
             when other
               display 
                 'Invalid character in input file: ' elevator-command.

           if seen-basement equal to 0 and floor-num less than 0 then
             display 'Entered basement at ' command-position
             move 1 to seen-basement.
