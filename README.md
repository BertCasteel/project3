project3
========
:authors: Kevin Beick and Robert Casteel

Develop an implementation of a device driver for a virtual device, a large-scale storage array.

:source files:
* Makefile ------- use to easily compile the project
* smsa_driver.c -- The only file we altered. This holds the 4 functions we implemented (mount, unmount, read, write) plus helper functions.
* Several other files that were originally included in the started code tarball - we did not alter these.

###Compilation Instructions:
cd into the project directory. Enter 'make' into the terminal to compile.  The executable is called 'smsasim' so enter `./smasasim` to run it. Enter `./smasasim -h` for usage.


###General Comments:
We successfully implemented the 4 functions such that outputted logfiles match the supplied solution logfiles for the 3 sample workloads.  We used the verification tool provided in verify.c to confirm this. eg: `./smsasim random.dat 2>&1 | ./verify random-output.log`  

#####time spent:
~15 hrs total between the two of us
This was substantially easier than the 2nd project. However, it forced us to think critically about the workflow for read and write processes.
