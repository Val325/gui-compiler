compiler = g++
libs = -lglfw -lGL -lglut -lGLEW
namecode = main.cpp
outputfile = window
includedir = -Inuklear/ -Icode/

make: main.cpp
	${compiler} -g ${namecode} ${libs} -o ${outputfile} ${includedir}
