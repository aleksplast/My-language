CC := g++ -Wno-pointer-arith -Wwrite-strings
SRC := main.cpp mylang.cpp Onegin-sort\text-sort.cpp Differentiator\diftree.cpp Differentiator\recursivedescent.cpp stack.cpp
DIR := C:\Users\USER\Documents\My-language
DIRB := C:\Users\USER\Documents\My-language
DIRCPU := C:\Users\USER\Documents\GitHub\Processor
DIRASM := C:\Users\USER\Documents\GitHub\Assembler
SRCCPU := $(DIRCPU)\main.cpp $(DIRCPU)\processor.cpp $(DIRCPU)\stack.cpp $(DIRCPU)\text-sort.cpp
SRCASM := $(DIRASM)\main.cpp $(DIRASM)\assembler.cpp $(DIRASM)\stack.cpp $(DIRASM)\text-sort.cpp

.PHONY: all clean

asm: $(SRCASM)
	$(CC) $^ -o $(DIRB)\$@

cpu: $(SRCCPU)
	$(CC) $^ -o $(DIRB)\$@

main: $(SRC)
	$(CC) $^ -o $(DIR)\$@

rec: $(REC)
	$(CC) $^ -o $(DIR)\$@

clean:
	rm -rf *.png *.dot
