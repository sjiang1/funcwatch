#Funcwatch Makefile
#This variable specifies the architecture
#Supported values are x86.
arch = x86
arch_dir = ./arch/$(arch)
include $(arch_dir)/arch.mk
arch_srcfiles := $(foreach file, $(arch_srcfiles), $(arch_dir)/$(file))

CC = gcc
CFLAGS = -std=gnu99 -g -I./includes $(arch_CFLAGS) -w -O0
LDFLAGS = -L $(arch_dir)/lib -g $(arch_LDFLAGS)
LDLIBS = -ldwarf -lelf $(arch_LDLIBS) -lcsv -lz
all: funcwatch tests_funcwatch/test \
     tests_funcwatch/test_struct_0 tests_funcwatch/test_struct_1_pointer_member tests_funcwatch/test_struct_2_struct_pointer_member \
     tests_funcwatch/test_primitive_0_int tests_funcwatch/test_primitive_1_char tests_funcwatch/test_primitive_2_uchar tests_funcwatch/test_primitive_3_short \
     tests_funcwatch/test_multiple_parameters_0 \
     tests_funcwatch/test_pointer_0_int tests_funcwatch/test_pointer_1_char tests_funcwatch/test_pointer_2_struct0 tests_funcwatch/test_pointer_3_struct1 \
     tests_funcwatch/test_union_0

clean:
	rm -f funcwatch tests_funcwatch/test \
		tests_funcwatch/test_struct_0 tests_funcwatch/test_struct_1_pointer_member tests_funcwatch/test_struct_2_struct_pointer_member \
		tests_funcwatch/test_primitive_0_int tests_funcwatch/test_primitive_1_char tests_funcwatch/test_primitive_2_uchar tests_funcwatch/test_primitive_3_short \
		tests_funcwatch/test_multiple_parameters_0 \
		tests_funcwatch/test_pointer_0_int tests_funcwatch/test_pointer_1_char tests_funcwatch/test_pointer_2_struct0 tests_funcwatch/test_pointer_3_struct1 \
		tests_funcwatch/test_union_0 
		`find -name \*.o` 

funcwatch: main.o funcwatch.o expression_stack.o $(arch_srcfiles) 

tests_funcwatch/test: 					tests_funcwatch/test.o
tests_funcwatch/test_struct_0: 				tests_funcwatch/test_struct_0.o
tests_funcwatch/test_struct_1_pointer_member: 		tests_funcwatch/test_struct_1_pointer_member.o
tests_funcwatch/test_struct_2_struct_pointer_member: 	tests_funcwatch/test_struct_2_struct_pointer_member.o
tests_funcwatch/test_primitive_0_int: 			tests_funcwatch/test_primitive_0_int.o
tests_funcwatch/test_primitive_1_char: 			tests_funcwatch/test_primitive_1_char.o
tests_funcwatch/test_primitive_2_uchar: 			tests_funcwatch/test_primitive_2_uchar.o
tests_funcwatch/test_primitive_3_short: 			tests_funcwatch/test_primitive_3_short.o
tests_funcwatch/test_multiple_parameters_0: 		tests_funcwatch/test_multiple_parameters_0.o
tests_funcwatch/test_pointer_0_int: 			tests_funcwatch/test_pointer_0_int.o
tests_funcwatch/test_pointer_1_char: 			tests_funcwatch/test_pointer_1_char.o
tests_funcwatch/test_pointer_2_struct0: 			tests_funcwatch/test_pointer_2_struct0.o
tests_funcwatch/test_pointer_3_struct1: 			tests_funcwatch/test_pointer_3_struct1.o
tests_funcwatch/test_union_0: 				tests_funcwatch/test_union_0.o

#summarize_results:summarize_results.o util.o parameter.o

