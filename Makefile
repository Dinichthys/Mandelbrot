# CXXFLAGS = -I libs -D _DEBUG  -ggdb -g3 -D_FORTIFY_SOURCES=3 -std=c++17 -Og -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=81920 -Wstack-usage=81920 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

CXXFLAGS = -O2 -march=native  -I libs -std=c++17 -flto -D _DEBUG

SFML_FLAGS = -lsfml-window -lsfml-graphics -lsfml-system

CXX = clang++

WARNINGS = -Wall -Wextra
# -Wswitch-enum
ifeq ($(CXX), clang++)
  WARNINGS += -pedantic -Wconversion -Wdangling -Wdeprecated -Wdocumentation -Wformat -Wfortify-source -Wgcc-compat -Wgnu -Wignored-attributes -Wignored-pragmas -Wimplicit -Wmost -Wshadow-all -Wthread-safety -Wuninitialized -Wunused -Wformat
  WARNINGS += -Wargument-outside-range -Wassign-enum -Wbitwise-instead-of-logical -Wc23-extensions -Wc11-extensions -Wcast-align -Wcast-function-type -Wcast-qual -Wcomma -Wcomment -Wcompound-token-split -Wconditional-uninitialized -Wduplicate-decl-specifier -Wduplicate-enum -Wduplicate-method-arg -Wduplicate-method-match -Wempty-body -Wempty-init-stmt -Wenum-compare -Wenum-constexpr-conversion -Wextra-tokens -Wfixed-enum-extension -Wfloat-equal -Wloop-analysis -Wframe-address -Wheader-guard -Winfinite-recursion -Wno-gnu-binary-literal -Wint-conversion -Wint-in-bool-context -Wmain -Wmisleading-indentation -Wmissing-braces -Wmissing-prototypes -Wover-aligned -Wundef -Wvla
endif
ifeq ($(CXX), cc)
  WARNINGS += -pedantic -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-usage=8192 -Wstack-protector
endif
ifeq ($(CXX), g++)
  WARNINGS += -pedantic -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-usage=8192 -Wstack-protector
endif

all: mandelbrot

mandelbrot: build_mandelbrot main.o mandelbrot.o my_stdio.o logging.o print_error.o
	@$(CXX) $(CXXFLAGS) $(SFML_FLAGS) build/mandelbrot/*.o build/my_stdio.o build/logging.o build/print_error.o -o mandelbrot


build:
	@mkdir -p build

build_mandelbrot: build
	@mkdir -p build/mandelbrot


my_stdio.o: libs/My_lib/My_stdio/my_stdio.cpp
	@$(CXX) $(CXXFLAGS) -c libs/My_lib/My_stdio/my_stdio.cpp -o build/my_stdio.o

logging.o: libs/My_lib/Logger/logging.cpp
	@$(CXX) $(CXXFLAGS) -c libs/My_lib/Logger/logging.cpp -o build/logging.o

print_error.o: libs/My_lib/Assert/print_error.cpp
	@$(CXX) $(CXXFLAGS) -c libs/My_lib/Assert/print_error.cpp -o build/print_error.o


main.o: src/main.cpp
	@$(CXX) $(CXXFLAGS) -c src/main.cpp -o build/mandelbrot/main.o

mandelbrot.o: src/mandelbrot.cpp
	@$(CXX) $(CXXFLAGS) -c src/mandelbrot.cpp -o build/mandelbrot/mandelbrot.o


clean: rmdir_build
	rm -rf mandelbrot

rmdir_build: clean_build
	rmdir build

clean_build: rmdir_smandelbrot
	rm -rf build/*

rmdir_smandelbrot: clean_mandelbrot
	rmdir build/mandelbrot

clean_mandelbrot:
	rm -rf build/mandelbrot/*
