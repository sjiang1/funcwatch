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
     tests/test_multiple_parameters_0 \
     tests/test_pointer_0_int tests/test_pointer_1_char tests/test_pointer_2_struct0 tests/test_pointer_3_struct1 \
     tests/test_union_0 tests/test_union_1 \
     tests/test_recursive_0

clean:
	rm -f funcwatch tests/test \
		tests/test_struct_0 tests/test_struct_1_pointer_member tests/test_struct_2_struct_pointer_member \
		tests/test_primitive_0_int tests/test_primitive_1_char tests/test_primitive_2_uchar tests/test_primitive_3_short \
		tests/test_multiple_parameters_0 \
		tests/test_pointer_0_int tests/test_pointer_1_char tests/test_pointer_2_struct0 tests/test_pointer_3_struct1 \
		tests/test_union_0 tests/test_union_1 \
                tests/test_recursive_0 \
		`find -name \*.o` 

funcwatch: main.o funcwatch.o expression_stack.o $(arch_srcfiles) 

tests/test: 					tests/test.o
tests/test_struct_0: 				tests/test_struct_0.o
tests/test_struct_1_pointer_member: 		tests/test_struct_1_pointer_member.o
tests/test_struct_2_struct_pointer_member: 	tests/test_struct_2_struct_pointer_member.o
tests/test_primitive_0_int: 			tests/test_primitive_0_int.o
tests/test_primitive_1_char: 			tests/test_primitive_1_char.o
tests/test_primitive_2_uchar: 			tests/test_primitive_2_uchar.o
tests/test_primitive_3_short: 			tests/test_primitive_3_short.o
tests/test_multiple_parameters_0: 		tests/test_multiple_parameters_0.o
tests/test_pointer_0_int: 			tests/test_pointer_0_int.o
tests/test_pointer_1_char: 			tests/test_pointer_1_char.o
tests/test_pointer_2_struct0: 			tests/test_pointer_2_struct0.o
tests/test_pointer_3_struct1: 			tests/test_pointer_3_struct1.o
tests/test_union_0: 				tests/test_union_0.o
tests/test_union_1: 				tests/test_union_1.o
tests/test_recursive_0:                         tests/test_recursive_0.o

#summarize_results:summarize_results.o util.o parameter.o

