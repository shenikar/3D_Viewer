TARGET= 3DViewer
CC= gcc
CFLAGS= -Wall -Werror -Wextra -std=c11 -pedantic -g
SOURCES= *.c
OBJECTS=$(SOURCES:.c=.o)
OS= $(shell uname)
TEST_FILES= tests/*.c


ifeq ($(OS), Darwin)
	TEST_FLAGS=-lcheck 
	OPEN_CMD = open
else
	TEST_FLAGS=-pthread -lcheck -pthread -lrt -lm -lsubunit
	OPEN_CMD = xdg-open
endif

all: uninstall install gcov_report dvi dist

install: del_new_dir
ifeq ($(OS),Darwin)
	cd cpp && qmake6 && make && make clean && rm Makefile && cd ../ && mv cpp/3DViewer.app/Contents/MacOS/$(TARGET) build/
else
	cd cpp && qmake6 && make && make clean && rm Makefile && cd ../ && mv cpp/$(TARGET) build/
endif
uninstall:
	rm -rf build

test: clean $(TARGET).a
	$(CC) $(CFLAGS)  $(SOURCES) $(TEST_FILES) -o test -g $(TEST_FLAGS)
	@./test

add_coverage_flag:
	$(eval CFLAGS += --coverage)

gcov_report: add_coverage_flag tests
	$(CC) $(CFLAGS)  $(SOURCES) $(TEST_FILES) -o test -g $(TEST_FLAGS)
	./test
	lcov -t "test" -o test.info -c -d . 
	genhtml -o report test.info
	$(OPEN_CMD) ./report/index.html
	
clean:
	rm -rf *.a *.so *.out *.o report *.gc* *.info *.txt test *.gch *.pdf *.tar view/*.o
	rm -rf  *.html *.css test.info test.dSYM report temp_obj.obj

dvi :
	${OPEN_CMD} Readme.md

dist:
	rm -rf dist.tar
	tar  -cvzf dist.tar build/$(TARGET)

$(TARGET).a: clean
	$(CC) $(CFLAGS) $(SOURCES) -c 
	ar rc $(TARGET).a *.o
	ranlib $(TARGET).a

del_new_dir:
	rm -rf build
	mkdir build

rebuild:
	make clean
	make all

check: test
	cppcheck --enable=all --suppress=missingIncludeSystem --inconclusive --check-config $(SOURCES) *.c *.h
ifeq ($(OS), Darwin)
	leaks --atExit -- ./test
else
	CK_FORK=no valgrind --vgdb=no --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./test
endif

clang:
	mv ../materials/linters/.clang-format .clang-format
	clang-format -i -style=Google *.c *.h tests/*.c cpp/*.cpp cpp/*.h
	clang-format -n -style=Google *.c *.h tests/*.c cpp/*.cpp cpp/*.h
	mv .clang-format ../materials/linters/.clang-format
