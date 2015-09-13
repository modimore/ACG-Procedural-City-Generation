ACG FINAL PROJECT: PROCEDURAL CITY GENERATION
Rachel King and Michael Mortimer

All the testing we have done is in Ubuntu 14.04 with CMake installed. To compile a version of our program, create a destination folder for the build, enter this folder in your terminal and use the command 'cmake ../src' assuming 'src' is our source folder and is located in the parent directory of your build directory. 

To run either version of the program, use the command './makecity' from the build directory. Optional arguments listed below can be supplied:

'-grid <path>/<file>': choose a specific input grid to run our program with. This also sets the global input file path.
'-building <file>' or '-bldg <file>': choose a building file, on a path relative to the program's global input file path
'-buildings <num_files> <file1> <file2> ...' or '-bldgs <num_files> <file1> <file2> ...': specify multiple building input files
'-bc <num>': amount of building models of each type to place in one iteration
'-bldg_max <num>' and '-bldg_min <num>': specify the maximum and minimum size of each modified building

As an example, try running './makecity -bldgs 3 ../inputs/pyramid.obj ../inputs/t_xy.obj ../inputs/box.obj -grid ../inputs/terrain_grid.txt'

Important Keyboard Events:
Pressing the spacebar creates a new set of buildings without deleting the old one.
Pressing 'r' or 'R' resets the simulation with iteration of buildings in place.
Pressing 'q' or 'Q' quits the simulation.
