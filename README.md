PokerHandFrequencies
====================

Concurrent Processing - C++

Description:
A parallel application using MPI to simulate drawing 5-card poker hands from a 
simulated deck of cards and use this to compile statistics regarding the relative 
frequencies of each hand type. The simulation will terminate when at least one of every 
hand type has occurred. Message passing will be used during the simulation so that the 
master process will be able to terminate the simulation in all processes the moment 
every poker hand type has been drawn at least once.

How to run:
Serial:
- Run project normally via visual studio

Serial via Sharcnet:
- Create a new directory and add PokerHandsSerial.h, and PokerHandsSerial.cpp
- run the command cc PokerHandsSerial.cpp -o hands
- sqjobs (to see if the job has finished)
- sqsub -q serial -o hands.log -r 1m ./hands
- cat hands.log

Parallel via commandline: 
- Build the project
- Open cmd and path to PokerHandsFrequencies.exe in the PokerHandFrequenciesParallel > Debug folder.
- Type mpiexec -n (number of processes) PokerHandFrequenciesParallel.exe

Parallel via Sharcnet:
- Create a new directory and add PokerHandsMPI.h, and PokerHandsMPI.cpp
- Run the command mpicc PokerHandsMPI.cpp -o hands
- sqjobs (to see if the job has finished)
- sqsub -q mpi -o hands.log -n (number of processes) -r 1. ./hands
- cat hands.log

Set Up MPI Dependency:
1. Install MPICH2 (v 1.4.1p1) as follows:
  a. Go to http://www.mpich.org/downloads/ and click the x86 link beside Windows under 
  Unofficial Binary Packages at the bottom of the page. 
  b. Save the msi file to a location on your hard drive since you may have to run the install 
  more than once.
  c. Run the downloaded “msi” installer and follow the directions accepting all default 
  settings. You must have administrator privileges via the current windows user account.
  [If you have trouble you might try turning off User Account Control then reinstalling.
  Another alternate solution might be to open a Command Prompt window “As 
  Administrator”, navigate to the folder containing the MPICH2 msi file, and execute the 
  installer by typing the file name and pressing the Enter key.] By default the MPICH2 
  folder will be installed under Program Files (x86).
  d. Add the full path to the MPICH2\bin folder to your PATH environment variable.
  e. if you encounter problems, try these more detailed install instructions: 
  http://www.mpich.org/static/downloads/3.0.4/mpich-3.0.4-installguide.pdf. Review the 
  section entitled Windows Version. Even though the document is for a different version 
  the instructions are applicable.
2. Now open Visual Studio to add the paths to the MPICH2 lib and include folders:
  a. Open the Example project (examples.sln) that is in the MPICH2/examples folder. You 
  may want to copy this solution to your normal documents folder before you open it.
  Visual Studio will likely prompt you to upgrade the solution to work with your current 
  version of Visual Studio. Don’t worry if the upgrade fails for some projects as long as the 
  cxxpi project converts okay.
  b. Make sure the Property Manager is open. Sometimes this window is docked with the 
  Solution Explorer. If this isn’t open, select the menu item View->Property Manager or 
  View->Other Windows->Property Manager.
  c. In the Property Manager:
    i. Double-click on the build configuration for the current project (e.g. Debug | 
    Win32 under cpi or cxxpi), This should bring-up the Property Pages dialog.
    ii. Select VC++ Directories , then Include Directories then Edit and browse to add 
    the path to your MPICH2\include folder.
    iii. Similarly, select Library Directories then Edit and browse to add the path to your 
    MPICH2\lib folder.
    iv. Click Apply and OK to close the Property Pages.
3. Test the installation as follows:
  a. Build the project named cxxpi. You will likely get a linker error due to a reference to a 
  non-existent library file (cxxd.lib). If so, simply switch from the Debug to the Release
  build via the dropdown field in the toolbar.
  b. Run the program by opening a Command Prompt window in the example project’s 
  Debug or Release folder (depending on which build configuration you’re using) then try 
  the following two command lines:
    i. cxxpi.exe
    ii. mpiexec –n 1 cxxpi.exe
  Note the the –n switch is used to select the number of parallel processes. In each case 
  the program should run to calculate an estimated value for PI.
