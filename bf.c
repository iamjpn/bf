#include "u.h"
#include "libc.h"
#include "bio.h"
#include "libString.h"

#define BUF_SIZE 30000L
#define PROG_SIZE 3000L

int debug;

int 
load(Biobuf *pf, int max, int *prog) {
	int i, c;

	i = 0;
	while((c = Bgetc(pf)) != Beof && i < max - 1) {
		prog[i++] = c;
	}
	prog[i] = Beof;
	return 0;
}

void 
dump(char *tape, int limit) {
	int i;

	for(i = 0; i < limit; i++) {
		print("%d", tape[i]);
		if (i && i % 80 == 0) 
			print("\n");
	}
	print("\n");
}

enum { STACK_OK, STACK_OVR, STACK_UND };
struct Stack {
	int ptr;
	int max;
	int *buffer;
};
typedef struct Stack Stack;


void
stack_create(Stack *stack, int max){
	stack->buffer = malloc(sizeof(int) * max); 
	stack->max = max;
	stack->ptr = 0;
}

void 
stack_destroy(Stack *stack){
	free(stack->buffer);
}

int 
stack_push(Stack *stack, int d) {
	if (!(stack->ptr < stack->max - 1))
		return STACK_OVR;
	stack->buffer[stack->ptr++] = d;
	return STACK_OK;
}

int 
stack_pop(Stack *stack, int *d) {
	if (stack->ptr == 0)
		return STACK_UND;
	*d = stack->buffer[--stack->ptr];
	return STACK_OK;
}

int
stack_empty(Stack *stack) {
	return(stack->ptr == 0);
}

void 
consume(int *prog, int *tape, Stack *stack) {
	int i;
	stack_push(&stack, pc);

}

void
main(int argc, char **argv)
{
	Biobuf  *pf;
	Stack stack;
	int prog[PROG_SIZE];
	Biobuf  bin;
	char tape[BUF_SIZE];
	int ptr, op, pc, d;
	debug = 1;

	memset(tape, 0, BUF_SIZE);
    Binit(&bin, 0, OREAD);

	if (argc != 2)
		exits("no program file\n");

	
	
	pf = Bopen(argv[1], OREAD);
	if (!pf)
		exits("could not open file\n");
		
	load(pf, PROG_SIZE, prog);
	print("as %c\n", prog[0]);
	ptr = pc = 0;
	stack_create(&stack, 256);

	while((op = prog[pc]) != Beof) {
		if (debug) print("op: %c\n", op);

		switch(op) {
			case '>':
				ptr++;
				break;
			case '<':
				ptr--;
				break;
			case '+':
				tape[ptr]++;
				break;
			case '-':
				tape[ptr]--;
				break;
			case '.':
				print("%c", tape[ptr]);
				break;
			case ',':
				tape[ptr] = Bgetc(&bin);
				break;
			case '[':
				if (tape[ptr]) {

				} else {

				}
				break;
			case ']':
				stack_pop(&stack, &d);
				break;
			default: 
				break;
		}
		pc++;
	}

	dump(tape, 100);

	/*
	int i;
	for(i = 0; i < 30000L; i++) {
		print("%d %d\n", i, buffer[i]);
	}
	//string = s_new();

	//s_read_line(in, string);
	//s_read(in, string, 10);

	line = Brdline(&bin, '\n');

	print("%s", line);

	Bterm(&bin);
	exits(0);
	*/
}

