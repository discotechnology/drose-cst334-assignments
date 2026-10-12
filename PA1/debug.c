#include "student_code.h"
#include <stdlib.h>
#include <string.h>

int main() {
	String test1 = String__init("Hello");
	String test2 = String__init("Goodbye");
	String__insert(&test1, &test2, 2);
	printf("%s\n", test1.data);
}
