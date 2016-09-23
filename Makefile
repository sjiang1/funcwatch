#Funcwatch Makefile
#This variable specifies the architecture
#Supported values are x86.
bits := $(shell getconf LONG_BIT)
arch := x86_$(bits)
arch_dir = ./arch/$(arch)
include $(arch_dir)/arch.mk
arch_srcfiles := $(foreach file, $(arch_srcfiles), $(arch_dir)/$(file))

CC = gcc
CFLAGS = -std=gnu99 -g -I./includes $(arch_CFLAGS) -w -O0
LDFLAGS = -L $(arch_dir)/lib -g $(arch_LDFLAGS)
LDLIBS = -ldwarf -lelf $(arch_LDLIBS) -lcsv -lz
all: funcwatch tests/test \
     tests/test_struct_0 tests/test_struct_1_pointer_member tests/test_struct_2_struct_pointer_member \
     tests/test_primitive_0_int tests/test_primitive_1_char tests/test_primitive_2_uchar tests/test_primitive_3_short \
     tests/test_multiple_parameters_0 tests/test_multiple_parameters_1 \
     tests/test_pointer_0_int tests/test_pointer_1_char tests/test_pointer_2_struct0 tests/test_pointer_3_struct1 \
     tests/test_pointer_4_pointer0 \
     tests/test_union_0 tests/test_union_1 \
     tests/test_recursive_0

clean:
	rm -f funcwatch tests/test \
		tests/test_struct_0 tests/test_struct_1_pointer_member tests/test_struct_2_struct_pointer_member \
		tests/test_primitive_0_int tests/test_primitive_1_char tests/test_primitive_2_uchar tests/test_primitive_3_short \
		tests/test_multiple_parameters_0 tests/test_multiple_parameters_1 \
		tests/test_pointer_0_int tests/test_pointer_1_char tests/test_pointer_2_struct0 tests/test_pointer_3_struct1 \
                tests/test_pointer_4_pointer0 \
		tests/test_union_0 tests/test_union_1 \
                tests/test_recursive_0 \
		`find -name \*.o` 

funcwatch: main.o funcwatch.o funcwatch_param_util.o dwarf_util.o funcwatch_output.o expression_stack.o vector.o stringutil.o commandhandle.o dynstring.o $(arch_srcfiles) 

tests/test: 					tests/test.o
tests/test_struct_0: 				tests/test_struct_0.o
tests/test_struct_1_pointer_member: 		tests/test_struct_1_pointer_member.o
tests/test_struct_2_struct_pointer_member: 	tests/test_struct_2_struct_pointer_member.o
tests/test_primitive_0_int: 			tests/test_primitive_0_int.o
tests/test_primitive_1_char: 			tests/test_primitive_1_char.o
tests/test_primitive_2_uchar: 			tests/test_primitive_2_uchar.o
tests/test_primitive_3_short: 			tests/test_primitive_3_short.o
tests/test_multiple_parameters_0: 		tests/test_multiple_parameters_0.o
tests/test_multiple_parameters_1:               tests/test_multiple_parameters_1.o
tests/test_pointer_0_int: 			tests/test_pointer_0_int.o
tests/test_pointer_1_char: 			tests/test_pointer_1_char.o
tests/test_pointer_2_struct0: 			tests/test_pointer_2_struct0.o
tests/test_pointer_3_struct1: 			tests/test_pointer_3_struct1.o
tests/test_pointer_4_pointer0:                  tests/test_pointer_4_pointer0.o
tests/test_union_0: 				tests/test_union_0.o
tests/test_union_1: 				tests/test_union_1.o
tests/test_recursive_0:                         tests/test_recursive_0.o

#summarize_results:summarize_results.o vector.o  stringutil.o parameter.o

CXX = g++ 
CPPFLAGS = -isystem ./includes
CXXFLAGS = -std=gnu99 -g -I./includes $(arch_CFLAGS) -w -O0 -Wall

unittests: dynstring_unittests funcwatch_output_unittests

dynstring_unittests.o: unittests/dynstring_unittests.cc dynstring.h stringutil.h includes/gtest/*.h includes/gtest/internal/*.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c unittests/dynstring_unittests.cc
dynstring_unittests: dynstring.o stringutil.o dynstring_unittests.o $(arch_dir)/lib/gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ -lpthread 

funcwatch_output_unittests.o: unittests/funcwatch_output_unittests.cc funcwatch_output.h funcwatch_param_util.h dynstring.h stringutil.h vector.h includes/gtest/*.h includes/gtest/internal/*.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c unittests/funcwatch_output_unittests.cc
funcwatch_output_unittests: funcwatch_output.o funcwatch_param_util.o dynstring.o stringutil.o vector.o funcwatch_output_unittests.o $(arch_dir)/lib/gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ -lpthread 
