To Use for Qt on PC, add the .cpp .hpp (and/or .c .h) files inside this file and open the .pro
You may have to add manually the .cpp into the project. You may also have to change the main.cpp depending on the fact that pthread can or cannot run on Windows PC. If it does, you may have to also add the linker options -lpthread to be able to compile.
