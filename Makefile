CC := g++ -Wno-pointer-arith -Wwrite-strings
DIRFRONT := C:\Users\USER\Documents\GitHub\My-language\frontend
DIRBACK := C:\Users\USER\Documents\GitHub\My-language\backend
DIRMIDDLE := C:\Users\USER\Documents\GitHub\My-language\middleend
DIRFUNC := C:\Users\USER\Documents\GitHub\My-language\functions
SRC := main.cpp $(DIRFRONT)\frontend.cpp $(DIRFUNC)\text-sort.cpp $(DIRFUNC)\diftree.cpp $(DIRBACK)\backend.cpp $(DIRFUNC)\stack.cpp $(DIRMIDDLE)\middleend.cpp
SRCTRANS := $(DIRFRONT)\maintranslator.cpp $(DIRFRONT)\translator.cpp $(DIRFUNC)\text-sort.cpp $(DIRFUNC)\diftree.cpp
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
	$(CC) $^ -o $(DIRB)\$@

translator: $(SRCTRANS)
	$(CC) $^ -o $(DIRB)\$@

rec: $(REC)
	$(CC) $^ -o $(DIR)\$@

clean:
	rm -rf *.png *.dot
