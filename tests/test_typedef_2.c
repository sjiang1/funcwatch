/*
 * To test funcwatch, run funcwatch against foo.
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

typedef struct Books {
  char title[50];
  char author[50];
  char subject[100];
  int book_id;
} Book;


int foo(Book parameter_1){
  return parameter_1.book_id;
}

int main(int argc, char *argv[]) {
  Book mybook;
  mybook.book_id = 10;
  strncpy(mybook.author, "Adam", 50);

  int ret = foo(mybook);
  return 0;
}

