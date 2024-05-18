Hi!

I am writing a custom database to learn more about the internals of a database!

From this project, I have learned c++, bash and how databases worked internally

TIMELINE/CHALLENGES FACED:

14 May 2024:
Worked on writing test scripts for the program. Piping input from an input file 
to the program and storing the output in a file was a real struggle due to
unfamiliarity with bash.

15 May 2024:
Continued work on test scripts. Compartmentalized testing into different
scripts. main feeds input to sqlite and stores the output. test.sh reads the
output and evaluates correctness based on output and whether the input was valid
or malformed. Learned about exec and file descriptors in bash.

16-17 May 2024:
Fixed bug in test script after 2 days. Turned out to be the shebang line in
test.sh where it was #/bin/bash instead of #!/bin/bash. Also worked on error
checking in sqlite.cpp. Limited size of username and email. IDs can now only be
positive numbers.
