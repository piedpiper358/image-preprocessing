CC = g++
CFLAGS = -ggdb 

ASM = nasm
ASMFLAGS = -f elf64

CPPC = g++
CPPFLAGS = -ggdb 

INCLUDE_DIRS = -iquote ../lab6/include -iquote include

OPENCLLIBS = -lm -l OpenCL 

OPENCVLIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui

ANSI = -ansi -pedantic

SOURCES =  ../lab6/src/greyscale.c ../lab6/src/bmp.c ../lab6/src/image.c ../lab6/src/rotate.c src/lab8.c 

OBJECTS = build/opencl.o build/lab8.o build/image.o build/bmp.o build/greyscale.o build/sepia.o build/rotate.o

all: together

together: build/sepia.o build/opencl.o build/lab8.o build/image.o build/greyscale.o build/bmp.o build/rotate.o 
	$(CC) $(CFLAGS)  $(INCLUDE_DIRS) -o build/main  $(OBJECTS) $(OPENCLLIBS)
	

build/sepia.o: src/sepia.asm 
	$(ASM) $(ASMFLAGS) -o build/sepia.o src/sepia.asm 

build/opencl.o: src/OpenCL.cpp
	$(CPPC) $(CPPFLAGS) $(INCLUDE_DIRS) -c -o build/opencl.o src/OpenCL.cpp

build/lab8.o: src/lab8.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o build/lab8.o src/lab8.c 

build/image.o: ../lab6/src/image.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o build/image.o ../lab6/src/image.c

build/bmp.o: ../lab6/src/bmp.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o build/bmp.o ../lab6/src/bmp.c

build/greyscale.o: ../lab6/src/greyscale.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o build/greyscale.o ../lab6/src/greyscale.c

build/rotate.o: ../lab6/src/rotate.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o build/rotate.o ../lab6/src/rotate.c



clean:
	rm -f build/*

	
new:
	rm ../lab6/res/bmp/1.bmp && cp ../lab6/res/bmp/1r.bmp ../lab6/res/bmp/1.bmp


exec:
	./build/main ../lab6/res/bmp/1.bmp

	




#.PHONY: all clean

#allcv: test -I/usr/local/include/opencv

#test: 
#	$(C++C) -I/usr/local/include  -lopencv_imgcodecs -L/usr/local/lib -o build/test src/OpenCV.cpp $(OPENCVLIBS)
        





