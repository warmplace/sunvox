all:
	cp ../lib_x86/sunvox.dll ./
	cp ../../resources/test.sunvox ./
	i586-mingw32msvc-gcc -g -fno-exceptions -c test.cpp
	i586-mingw32msvc-gcc -o test.exe test.o

clean:
	rm -f *.o *.dll *.exe *.sunvox