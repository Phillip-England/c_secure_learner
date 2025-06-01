#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void lesson1_intro(void) {
	int age;
	printf("Enter your age: ");
	if (scanf("%d", &age) != 1) {
		printf("Invalid input.\n");
		return;
	}
	if (age >= 18) {
		printf("You're an adult.\n");
	} else {
		printf("You're not an adult yet.\n");
	}
}

void pointer_vs_array_test(void) {
	char array[6] = "World"; // 💡: Array of 6 chars (includes a null terminator)
	char *pointer = "World"; // 💡: Pointer to a string literal
	printf("array: %s\n", array);
	printf("pointer: %s\n", pointer);
	array[0] = 'H'; // ⚠️: when modifying a char foo[6] we must use single quotes
	printf("modified array: %s\n", array);
	// pointer[0] = 'H'; // ⚠️: string literals are stored in read-only memory and cannot be modified
	printf("pointer[0]: %c\n", pointer[0]); // 💡: you can still access string literals by index
	printf("Address of array: %p\n", (void *)array); // 💡: void pointers point to the address of the data without worrying about it's type
	printf("Address of pointer: %p\n", (void *)pointer);
}

void print_bytes(void *ptr, int num_bytes) {
	unsigned char *byte_ptr = ptr; // 💡: We "cast" our void *ptr into an unsigned char *byte_ptr
	for (int i = 0; i < num_bytes; i++) {
		printf("%02x ,", byte_ptr[i]);
	}
	printf("\n");
}

void print_void_pointer(void *ptr, int flag) {
	switch (flag) {
		case 1: {
			int *byte_ptr = (int *)ptr;
			printf("INT: %d\n", *byte_ptr);
			break;
		}
		case 2: {
			float *byte_ptr = (float *)ptr;
			printf("FLOAT: %.2f\n", *byte_ptr);
			break;
		}
		case 3: {
			char *byte_ptr = (char *)ptr;
			printf("CHAR: %c\n", *byte_ptr);
			break;
		}
		default:
			fprintf(stderr, "invalid flag");
			exit(EXIT_FAILURE);
	}
}

char *lesson2_strdup(const char *src) {
	size_t len = strlen(src); // 💡: gets the number of BYTES (not characters) in a string
	char *copy = malloc(len + 1); // 💡: always leave space for the null terminator
	if (!copy) {
		fprintf(stderr, "Memory allocation failure\n");
		exit(EXIT_FAILURE);
	}
	strcpy(copy, src); // 💡: could use memcpy to save memory (but is not string aware and doesn't care about null terminators)
	return copy;
}

char *lesson2_concat_unsafe(const char *a, const char *b) {
	size_t len = strlen(a) + strlen(b);
	char *space = malloc(len + 1); // 💡: accounting for the null terminator using + 1
	if (!space) {
		fprintf(stderr, "Memory allocation failure\n");
		exit(EXIT_FAILURE);
	}
	// ⚠️: these functions don't check the destination buffer size, leading to buffer overflow attacks
	strcpy(space, a);
	strcat(space, b);
	return space;
}

char *lesson2_concat_safe(const char *a, const char *b) {
	size_t max = strlen(a) + strlen(b) + 1;
	char *buffer = malloc(max);
	if (!buffer) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	strncpy(buffer, a, max);
	buffer[max - 1] = '\0'; // 💡: manually inserting a null terminator between each series of bytes
	strncat(buffer, b, max - strlen(buffer) - 1); // 💡: always leave room for the null terminator
	return buffer;
}

void lesson3_use_after_free(void) {
	int *buffer = malloc(sizeof(int));
	*buffer = 1;
	free(buffer);
	buffer = NULL; // 💡: best practice
	// printf("%d\n", *buffer); // 💥: use after free, undefined behavior
}

void lesson4_double_free_crash(void) {
	int *buffer = malloc(sizeof(int));
	*buffer = 1;
	free(buffer);
	// free(buffer); // 💥: double free system will crash
	buffer = NULL; 
}

void lesson4_double_free_safe(void) {
	int *buffer = malloc(sizeof(int));
	*buffer = 1;
	free(buffer);
	buffer = NULL; // 💡: prevents crash by setting to NULL
	free(buffer); // 💡: double free, but no crash
	printf("%d\n", *buffer);
}

void lesson4_null_dereference_crash(void) {
	int *buffer = malloc(sizeof(int));
	buffer = NULL;
	*buffer = 2; // 💥 deferencing NULL will crash system
	free(buffer);
}

void lesson5_dangling_pointer(void) {
	int *a = malloc(sizeof(int));
	*a = 42;
	printf("a points to: %p and holds: %d\n", (void *)a, *a);
	free(a);  // a is now dangling
	// a = NULL;
	// don't null a yet!
	int *b = malloc(sizeof(int));  // system may re-use same address
	*b = 99;
	printf("b points to: %p and holds: %d\n", (void *)b, *b);
	printf("a STILL points to: %p and might now say: %d\n", (void *)a, *a);  // ⚠️: might now show 99 or garbage
	free(b);
}

void lesson6_stack_vs_heap(void) {
	int stack_value = 42;
	int *heap_value = malloc(sizeof(int));
	if (!heap_value) {
			fprintf(stderr, "malloc failed\n");
			exit(EXIT_FAILURE);
	}
	*heap_value = 99;
	printf("stack_value address: %p, value: %d\n", (void *)&stack_value, stack_value);
	printf("heap_value address:  %p, value: %d\n", (void *)heap_value, *heap_value);
	free(heap_value);
}

void lesson6_stack_address_unsafe(void) {
	int x = 123;
	printf("%d\n", x);
	// return &x; // WARNING: cannot return address of local variable - becomes invalid
}

typedef struct {
	char name[50];
	int age;
} Person;

void lesson7_struct_on_heap(void) {
	Person *p = malloc(sizeof(Person));
	if (!p) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	strcpy(p->name, "Alice");
	p->age = 30;
	printf("Name: %s, Age: %d\n", p->name, p->age);
	free(p);
}

typedef struct {
	char *name;
	int age;
} PersonPointer;

void lesson7_struct_with_internal_malloc(void) {
	PersonPointer *p = malloc(sizeof(PersonPointer));
	if (!p) {
		fprintf(stderr, "Person allocation failure\n");
		exit(EXIT_FAILURE);
	}
	p->name = malloc(20);
	if (!p->name) {
		fprintf(stderr, "Name allocation failed\n");
		free(p);
		exit(EXIT_FAILURE);
	}
	strcpy(p->name, "Bob");
	p->age = 45;
	printf("Name: %s, Age: %d\n", p->name, p->age);
	free(p->name); // TIP: free internal fields first
	free(p);
}

void lesson8_memory_leak_demo(void) {
	PersonPointer *p = malloc(sizeof(PersonPointer));
	if (!p) {
			fprintf(stderr, "Person allocation failure\n");
			exit(EXIT_FAILURE);
	}
	p->name = malloc(20);
	if (!p->name) {
			fprintf(stderr, "Name allocation failure\n");
			free(p);
			exit(EXIT_FAILURE);
	}
	strcpy(p->name, "Bob");
	p->age = 50;
	printf("Name: %s, Age: %d\n", p->name, p->age);
	// free(p); // skip free(p->name); // WARNING: uncomment this to simulate a memory leak
	free(p->name); // TIP: always free a struct's members before you free the struct
	free(p);
}

typedef struct {
	PersonPointer *data;
	size_t size;
	size_t capacity;
} PersonList;

void init_person_list(PersonList *list) {
	list->capacity = 2;
	list->size = 0;
	list->data = malloc(sizeof(PersonPointer) * list->capacity);
	if (!list->data) {
		fprintf(stderr, "Failed to allocate person list\n");
		exit(EXIT_FAILURE);
	}
}

void add_person(PersonList *list, const char *name, int age) {
	if (list->size == list->capacity) {
		list->capacity *= 2; // TIP: duplicate the capacity instead of incrementing to avoid realloc on each add
		list->data = realloc(list->data, sizeof(PersonPointer) * list->capacity);
		if (!list->data) {
			fprintf(stderr, "Realloc failed\n");
			exit(EXIT_FAILURE);
		}
	}
	list->data[list->size].name = lesson2_strdup(name);
	list->data[list->size].age = age;
	list->size++;
}

void free_person_list(PersonList *list) {
	for (size_t i = 0; i < list->size; i++) {
		free(list->data[i].name);
	}
	free(list->data);
	list->data = NULL;
	list->size = 0;
	list->capacity = 0;
}

int compare_by_age(const void *a, const void *b) {
	const PersonPointer *pa = (const PersonPointer *)a;
	const PersonPointer *pb = (const PersonPointer *)b;
	return pa->age - pb->age;
}

void lesson9_dynamic_array(void) {
	PersonList list;
	init_person_list(&list);
	add_person(&list, "Alice", 30);
	add_person(&list, "Bob", 45);
	add_person(&list, "John", 33);
	qsort(list.data, list.size, sizeof(PersonPointer), compare_by_age);
	printf("\nSorted by age:\n");
	for (size_t i = 0; i < list.size; i++) {
		printf("Person %zu: %s, age %d\n", i+1, list.data[i].name, list.data[i].age);
	}
	free_person_list(&list);
}

void generic_print(const void *a, void callback(const void *a)) {
	callback(a);
}

void print_func(const void *a) {
	const int *p = (const int*)a;
	printf("%d\n", *p);
}

void greet(void) { 
	printf("Hello!\n"); 
}

void farewell(void) { 
	printf("Goodbye!\n"); 
}

void lesson10_function_pointer_array(void) {
	void (*actions[2])(void); // Array of 2 function pointers
	actions[0] = greet;
	actions[1] = farewell;
	for (int i = 0; i < 2; i++) {
		actions[i](); // call each function
	}
}

int calc_add(int a, int b) {
	return a + b;
}

int calc_subtract(int a, int b) {
	return a - b;
}

int calc_multiply(int a, int b) {
	return a * b;
}

int calc_divide(int a, int b) {
	if (b == 0) {
		fprintf(stderr, "Division by zero!\n");
		return 0;
	}
	return a / b;
}

void lesson11_calculator(void) {
	int (*ops[4])(int, int) = { calc_add, calc_subtract, calc_multiply, calc_divide };
	const char *names[4] = { "add", "subtract", "multiply", "divide" };
	int x = 20, y = 5;
	for (int i = 0; i < 4; i++) {
		printf("%s(%d, %d) = %d\n", names[i], x, y, ops[i](x, y));
	}
}

void lesson11_calculator_with_input(int x, int y, int operation) {
	switch (operation) {
		case 0: {
			printf("%d + %d == %d\n", x, y, calc_add(x, y));
			break;
		}
		case 1: {
			printf("%d - %d == %d\n", x, y, calc_subtract(x, y));
			break;
		}
		case 2: {
			printf("%d * %d == %d\n", x, y, calc_multiply(x, y));
			break;
		}
		case 3: {
			if (y == 0) {
				fprintf(stderr, "Division by zero!\n");
				exit(EXIT_FAILURE);
			}
			printf("%d / %d == %d\n", x, y, calc_divide(x, y));
			break;
		}
		default: {
			fprintf(stderr, "Invalid operation selected\n");
			exit(EXIT_FAILURE);
		}
	}
}

void lesson12_secure_input(void) {
	char buffer[10];
	printf("Enter your name (max 9 chars): ");
	if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
		fprintf(stderr, "Input error\n");
		return;
	}
	buffer[strcspn(buffer, "\n")] = '\0';
	printf("Hello, %s!\n", buffer);
}

void lesson_dev_tty(void) {
	FILE *tty = fopen("/dev/tty", "w");
	if (!tty) {
		perror("Failed to open /dev/tty");
		return;
	}
	fprintf(tty, "Hello from /dev/tty - this will always print on the terminal\n");
	fclose(tty);
}

void lesson13_read_file_line(void) {
	FILE *fp = fopen("data.txt", "r");
	if (!fp) {
		perror("Failed to open file");
		return;
	}
	char buffer[101];
	if (fgets(buffer, sizeof(buffer), fp) == NULL) {
		if (feof(fp)) {
			printf("Reached end of file\n");
		} else {
			perror("Error reading file");
		}
		fclose(fp);
		return;
	}
	buffer[strcspn(buffer, "\n")] = '\0';
	printf("File content: %s\n", buffer);
	fclose(fp);
}

void lesson13_read_file_all_lines(void) {
	FILE *fp = fopen("data.txt", "r");
	if (!fp) {
		perror("Failed to open file");
		return;
	}
	char buffer[101];
	while (fgets(buffer, sizeof(buffer), fp)) {
		buffer[strcspn(buffer, "\n")] = '\0'; // strip the newline
		printf("Line: %s\n", buffer);
	}
	if (ferror(fp)) {
		perror("Error reading file");
	}
	fclose(fp);
}

void lesson13_indexof(void) {
	char *str = "some string";
	int indexOfS = strcspn(str, "s");
	printf("%d\n", indexOfS);
	printf("%c\n", str[indexOfS]);
}

char *read_file_into_buffer(const char *filename) {
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		perror("Failed to open file");
		return NULL;
	}

	// Step 1: Seek to end to find file size
	if (fseek(fp, 0, SEEK_END) != 0) {
		perror("Seek failed");
		fclose(fp);
		return NULL;
	}
	long size = ftell(fp);
	if (size < 0) {
		perror("ftell failed");
		fclose(fp);
		return NULL;
	}
	rewind(fp); // Go back to the start

	// Step 2: Allocate buffer
	char *buffer = malloc(size + 1); // +1 for null terminator
	if (!buffer) {
		fprintf(stderr, "Memory allocation failed\n");
		fclose(fp);
		return NULL;
	}

	// Step 3: Read contents
	size_t bytes_read = fread(buffer, 1, size, fp);
	if ((long)bytes_read != size) {
		fprintf(stderr, "Failed to read entire file\n");
		free(buffer);
		fclose(fp);
		return NULL;
	}
	buffer[size] = '\0'; // Null-terminate
	fclose(fp);
	return buffer;
}

void lesson14_format_string_vulnerability(void) {
	char input[100];
	printf("Enter a message: ");
	if (fgets(input, sizeof(input), stdin) == NULL) {
		fprintf(stderr, "Input error\n");
		return;
	}
	input[strcspn(input, "\n")] = '\0';
	// printf(input); // WARNING: a call to printf without formatting the var can lead to vulnerabilities by injecting %s type placeholders
	printf("%s", input); // TIP: always format printing output with proper placeholders
}

void lesson15_buffer_overflow(void) {
	char buffer[10];
	printf("Enter something (max 9 chars): ");
	// gets(buffer); // WARNING: gets() has no bounds checking
	if (fgets(buffer, sizeof(buffer), stdin) == NULL) { // TIP fgets() places a "\n" at the end of the input
		fprintf(stderr, "Input error\n");
		return;
	}
	// buffer[strcspn(buffer, "\n")] = '\0';
	printf("You entered: %s\n", buffer);
}

void lesson16_stack_smashing(void) {
	char buffer[8];
	int secret = 12345;
	printf("Enter a string (try more than 8 chars): ");
	scanf("%s", buffer); // WARNING: no bounds checking
	printf("You typed: %s\n", buffer);
	printf("Secret is: %d\n", secret);
}

void lesson17_safe_string_copy(void) {
	char src[20];
	char dest[10];
	printf("Enter a short word (max 9 chars): ");
	if (fgets(src, sizeof(src), stdin) == NULL) {
		fprintf(stderr, "Input error\n");
		return;
	}
	src[strcspn(src, "\n")] = '\0';
	strncpy(dest, src, sizeof(dest) - 1);
	dest[sizeof(dest) - 1] = '\0';
	printf("copied safely: %s\n", dest);
}

void lesson18_strtok_demo(void) {
	char text[] = "one,two,three";
	char *token = strtok(text, ",");
	while (token != NULL) {
		printf("Token: %s\n", token);
		token = strtok(NULL, ",");
	}
}

void lesson18_invalid_bytes(void) {
	char some_text[] = "this is some textÆ"; // WARNING: Æ takes more than a single byte and we are not account for it
	for (size_t i = 0; i < strnlen(some_text, sizeof(some_text)); i++) {
		printf("%c\n", some_text[i]);
	}
}

void lesson18_pointer_arithmetic(void) {
	char text[] = "one,two,three"; // TIP: compiler can count the length for you in this format, so you can omit it
	char *start = text;
	char *token = strtok(start, ",");
	while (token != NULL) {
		printf("\nToken: %s\n", token);
		printf("Token adress: %p\n", (void *)token);
		printf("Offset from start: %ld bytes\n", token - start);
		for (char *p = token; *p != '\0'; p++) {
			printf("Char: %c at %p (offset %ld)\n", *p, (void *)p, p - start);
		}
		token = strtok(NULL, ",");
	}
}

typedef struct {
	const char *name;
	void (*action)(void);
} Command;

void cmd_hello(void) {
	printf("Hello!\n");
}

void cmd_exit(void) {
	printf("Exiting...\n");
}

void lesson19_struct_with_function_pointers(void) {
	Command commands[] = {
		{ "hello", cmd_hello },
		{ "exit", cmd_exit }
		// { "null", NULL } // WARNING: will cause segfault
	};
	for (int i = 0; i < 2; i++) { // WARNING: iterating past the len of the commands[] will also cause a segfault
		printf("Running command: %s\n", commands[i].name);
		commands[i].action();
	}
	// We can also run the loop with runtime checks to avoid segfaults (useful if we are going to be using a dynamic length)
	for (int i = 0; i < 2; i++) {
		Command cmd = commands[i];
		if (!cmd.action || !cmd.name) {
			printf("No action assigned\n");
			continue;
		}
		printf("Running command: %s\n", cmd.name);
		cmd.action();
	}
}

typedef struct Shape {
	const char *name;
	float (*area)(struct Shape *self);
	void (*describe)(struct Shape *self);
} Shape;

typedef struct {
	Shape base;
	float radius;
} Circle;

float circle_area(Shape *s) {
	Circle *c = (Circle*)s;
	return 3.14159f * c->radius * c->radius;
}

void circle_describe(Shape *s) {
	Circle *c = (Circle *)s;
	printf("I am a Circle with radius %.2f\n", c->radius);
}

void lesson20_vtable_simulation(void) {
	Circle c;
	c.base.name = "cicle";
	c.base.area = circle_area;
	c.base.describe = circle_describe;
	c.radius = 5.0f;
	c.base.describe((Shape *)&c);
	printf("Area: %.2f\n", c.base.area((Shape *)&c));
}

typedef struct {
	Shape base;
	float height;
	float width;
} Rectangle;

float rectangle_area(Shape *s) {
	Rectangle *r = (Rectangle *)s;
	return r->height * r->width;
}

void rectangle_describe(Shape *s) {
	printf("Rectangle with area of: %.2f\n", rectangle_area(s));
}

void lesson21_shape_array_polymorphism(void) {
	Circle *c = malloc(sizeof(Circle));
	c->base.name = "circle";
	c->base.area = circle_area;
	c->base.describe = circle_describe;
	c->radius = 3.0f;

	Rectangle *r = malloc(sizeof(Rectangle));
	r->base.name = "rectangle";
	r->base.area = rectangle_area;
	r->base.describe = rectangle_describe;
	r->height = 2.0;
	r->width = 5.0;

	Shape *shapes[] = {
		(Shape *)c,
		(Shape *)r
	};

	for (int i = 0; i < 2; i++) {
		shapes[i]->describe(shapes[i]);
		printf("Area: %.2f\n", shapes[i]->area(shapes[i]));
	}

	free(c);
	free(r);

}

void lesson22_bitwise_basics(void) {
	// unsigned char a = 0b11001100; // 204 in decimal but NOT portable
	unsigned char a = 0xCC; // 204 in decimal
	unsigned char b = 0x0F;

	// what happens if a cast the into an int?
	int some_int = (int)a;
	printf("%d\n", some_int); // 11001100 maps to the decimal number of 204

	// and what if we cast it into a signed char?
	char some_char = (char)a;
	printf("%c\n", some_char); // does 11001100 represent an ascii char? (ascii chars are the only chars represented by a single byte) if not, i'll get garbage printed to the console

	// so if 11001100 doesn't map to a valid ascii char, does 0x0F?
	char another_char = (char)b;
	printf("%c\n", another_char); // maps to a "space" character? Not entirely sure, it might be a zero-width space

	printf("a = 0x%02X\n", a); // how to print a value as hexidecimal
	printf("b = 0x%02X\n", b); // can any "byte" be converted into hex? I think so!

	printf("a & b = 0x%02X\n", a & b); // bitwise AND
	printf("a | b = 0x%02X\n", a | b); // bitwise OR
	printf("a ^ b = 0x%02X\n", a ^ b); // bitwise XOR
	printf("~a    = 0x%02X\n", (unsigned char)~a); // bitwise NOT (cast to suppress sign extension)
	printf("~a    = 0x%02X\n", ~a); // bitwise NOT (not casting for curiousity)
	printf("a << 1 = 0x%02X\n", a << 1); // left shift
	printf("a >> 1 = 0x%02X\n", a >> 1); // right shift
}

void print_binary(unsigned char value) {
	for (int i = 7; i >= 0; i--) {
		printf("%d", (value >> i) & 1);
	}
	printf("\n");
}


// TIP: bitwise AND is used to ISOLATE cetain bits!
void lesson22_bitwise_and(void) {
	unsigned int x = 1;
	unsigned int y = 2;
	unsigned int result = 1 & 2;
	print_binary(x);
	print_binary(y);
	print_binary(result);

	// okay, we can use a "mask" to check if a certain bit is turned on
	// lets check if the leading bit is "on"
	unsigned int z = 1; // 00000001
	unsigned int mask = 128; // 10000000
	unsigned int result_again = z & mask;
	print_binary(result_again); // will output 00000000

	// but if we do this we should get 128
	x = 128;
	result_again = x & mask;
	printf("%d\n", result_again); // 128 because the leading bit is set
}

void lesson22_bitwise_or(void) {
	unsigned int x = 1;
	unsigned int mask = 0;
	unsigned int result = x | mask;
	printf("%u\n", result); // should equal 1
	x = 128; // 10000000
	mask = 1; // 00000001
	result = x | mask; // 10000001
	printf("%u\n", result); // should equal 129
}

void lesson22_bitwise_exclusive_or(void) {
	unsigned int x = 0; // 00000000
	unsigned int mask = 22; // 00011010
	unsigned int result = x ^ mask;
	printf("%u\n", result); // should output 22

	x = 48; // 00110000
	mask = 1; // 00000001
	result = x ^ mask;
	printf("%u\n", result); // should output 49
}

void lesson22_bitwise_not(void) {

	// when using bitwise not (~) make sure we are working with a single byte
	unsigned char x = 0; // 00000000
	unsigned char result = ~x; // 11111111
	printf("%u\n", result); // should output 255

	// working with types that require more than 1 byte may output unexpected results
	int a = 0;
	printf("%d\n", ~a); // outputs -1

}

void explain_byte(uint8_t value) {
	printf("Binary: ");
	for (int i = 7; i >= 0; i--) {
		printf("%d", (value >> i) & 1);
	}
	printf("\n");
	printf("Unsigned: %u\n", value);
	printf("Signed (two's complement): %d\n", (int8_t)value);
}

void toggle_between_negative_and_positive(int8_t num) {
	uint8_t bits = (uint8_t)num;
	if (num > 0) {
		uint8_t flipped = ~bits + 1;
		printf("Flipped: %d\n", (int8_t)flipped);
	} else if (num < 0) {
		uint8_t flipped = ~bits + 1;
		printf("Flipped: %d\n", flipped); // prints positive version
	} else {
		printf("Zero stays zero: 0\n");
	}
}

void lesson22_bitwise_bitshift(void) {
	unsigned int x = 1; // 00000001
	unsigned int y = x << 2;
	printf("%u\n", y); // should print 4
	print_binary(y);
	y = y >> 2;
	printf("%u\n", y); // should be back at 1
}

// what if we have 01101101 and we want to extract he first two bits 01
void lesson23_extracting_bits(void) {
	uint8_t value = 109; // 01101101
	printf("Original: ");
	print_binary(value);
	uint8_t mask = 3; // 00000011
	unsigned char result = value & mask;
	print_binary(result); // 00000001
}

uint8_t bitwit_get(uint8_t flags, uint8_t position) {
	if (position >= 8) {
		return 0;
	}
	if (flags & (1 << position)) {
		return 1; // true
	}	
	return 0; // false
}

uint16_t bitwit_smash(uint8_t x, uint8_t y) {
	return ((uint16_t)x << 8) | y;
}

void lesson24_classify_byte(const char *c) {
	unsigned char byte = (unsigned char)*c;
	if ((byte & 128) == 0)  {
		printf("hit a one byte char\n");
	}
	if ((byte & 224) == 192) {
		printf("hit two byte char\n");
	}
	if ((byte & 240) == 224) {
		printf("hit three byte char\n");
	}
	if ((byte & 248) == 240) {
		printf("hit a four byte char\n");
	}
	if ((byte & 192) == 128) {
		printf("hit a continuation byte\n");
	}


}

void lesson25_ascii_code_points(void) {
	char input[100];
	printf("Enter a string (ASCII only): ");
	if (fgets(input, sizeof(input), stdin) == NULL) {
		fprintf(stderr, "Input error\n");
		return;
	}
	input[strcspn(input, "\n")] = '\0';
	for (size_t i = 0; i < strlen(input); i++) {
		unsigned char c = input[i];
		printf("Character: %c -> ASCII Code Point: %u (0x%02X)\n", c, c, c);
	}
}

void lesson_a1_char_types(void) {
	char c = -1;
	unsigned char uc = 255;
	int i = c;

	printf("Signed char c = %d\n", c);
	printf("Unsigned char us = %u\n", uc);
	printf("Assigned signed char to int: i = %d\n", i);

	 int c_size = sizeof(i);
	 printf("%d\n", c_size);
}

int main(void) {

	// lesson25_ascii_code_points();

	// lesson_a1_char_types();

	lesson24_classify_byte("∂");

	// uint16_t smashed = bitwit_smash(1, 2);
	// for (int i = 15; i >= 0; i--) {
	// 	uint8_t bit = (smashed >> i) & 1;
	// 	printf("%d", bit);
	// }
	// printf("\n");

	// u_int8_t is_set = bitwit_get(5, 0);
	// printf("%u\n", is_set);

	// lesson23_extracting_bits();

	// lesson22_bitwise_bitshift();

	// toggle_between_negative_and_positive(-22);

	// explain_byte(-1);

	// lesson22_bitwise_not();

	// lesson22_bitwise_exclusive_or();

	// lesson22_bitwise_or();

	// lesson22_bitwise_and();

	// lesson22_bitwise_basics();

	// lesson21_shape_array_polymorphism();

	// lesson20_vtable_simulation();

	// lesson19_struct_with_function_pointers();

	// lesson18_pointer_arithmetic();

// lesson18_invalid_bytes();

	// lesson18_strtok_demo();

	// lesson17_safe_string_copy();

	// lesson16_stack_smashing();

	// lesson15_buffer_overflow();

	// lesson14_format_string_vulnerability();

	// char *data = read_file_into_buffer("data.txt");
	// printf("%s\n", data); 
	// free(data);

	// lesson13_indexof();

	// lesson13_read_file_all_lines();

	// lesson13_read_file_line();

	// lesson_dev_tty();

	// lesson12_secure_input();

	// lesson11_calculator_with_input(2, 2, 2);

	// lesson11_calculator();

	// lesson10_function_pointer_array();

	// generic_print(&(int){12355}, print_func);

	// lesson9_dynamic_array();

	// lesson8_memory_leak_demo();

	// lesson7_struct_with_internal_malloc();

	// lesson7_struct_on_heap();

	// int *stack_addr = lesson6_stack_address_unsafe();

	// lesson6_stack_vs_heap();

	// lesson5_dangling_pointer();

	// lesson4_null_dereference_crash();

	// lesson4_double_free_safe();

	// lesson4_double_free_crash();

	// lesson3_use_after_free();
	
	// char *joined = lesson2_concat_safe("Hello", "World");
	// printf("Joined string: %s\n", joined);
	// free(joined);

	// char *joined = lesson2_unsafe_concat("Hello", "World");
	// printf("Joined string: %s\n", joined);
	// free(joined);
	
	// char *dup = lesson2_strdup("Secure copy!");
	// printf("Copied string: %s\n", dup);
	// free(dup);
	
	// print_void_pointer(&(int){12345}, 1);
	// print_void_pointer(&(float){3.14f}, 2);
	// print_void_pointer(&(char){'A'}, 3);
	
	// int x = 12345;
	// print_bytes(&(int){12345}, sizeof(int)); // 💡: you can inline a pointer using a "compound literal" which is how i passed an int to this func
	
	// pointer_vs_array_test();
	
	// lesson1_intro();

	return EXIT_SUCCESS;
}

