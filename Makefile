CXX = gcc
OUTDIR = build
CXXFLAGS = -x c++ -Wall -O3 -o
INCLUDE = -Isrc/include
LDFLAGS = 
ifeq ($(OS), Windows_NT)
	TARGET = main.exe
else
	TARGET = main
endif

.PHONY: build clean run test

all:
	$(MAKE) -si clean
	$(MAKE) -s build
	$(MAKE) -s run

test:
	$(MAKE) -si clean
	$(MAKE) -s build OUTDIR=test
	$(MAKE) -s run

build:
	mkdir $(OUTDIR)
	$(CXX) $(CXXFLAGS) ./$(OUTDIR)/$(TARGET) src/cipher.c src/stegano.c src/lodepng.c $(INCLUDE) $(LDFLAGS)

clean:
	rm -rf $(OUTDIR)

run:
	cd $(OUTDIR) && ./$(TARGET)