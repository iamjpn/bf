#include "u.h"
#include "libc.h"
#include "bio.h"
#include "libString.h"

#define BUF_SIZE 30000L
#define PROG_SIZE 30000L

int debug;
int execute;

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
main(int argc, char **argv)
{
	Biobuf  *pf;
	Stack stack;
	int prog[PROG_SIZE];
	Biobuf  bin;
	char tape[BUF_SIZE];
	int ptr, op, pc, d;
	debug = 0;
	execute = 1;

	memset(tape, 0, BUF_SIZE);
    Binit(&bin, 0, OREAD);

	if (argc != 2)
		exits("no program file\n");
	
	pf = Bopen(argv[1], OREAD);
	if (!pf)
		exits("could not open file\n");
		
	load(pf, PROG_SIZE, prog);
	ptr = pc = 0;
	stack_create(&stack, 256);

	while((op = prog[pc]) != Beof) {
		if (debug) print("op: %c\n", op);

		switch(op) {
			case '>':
				if (execute) ptr++;
				break;
			case '<':
				if (execute) ptr--;
				break;
			case '+':
				if (execute) tape[ptr]++;
				break;
			case '-':
				if (execute) tape[ptr]--;
				break;
			case '.':
				if (execute) print("%c", tape[ptr]);
				break;
			case ',':
				if (execute) (tape[ptr] = Bgetc(&bin));
				break;
			case '[':
				stack_push(&stack, pc);
				if (execute) execute = tape[ptr];
				break;
			case ']':
				stack_pop(&stack, &d);
				if (execute) {
					pc = d;
					continue;
				} 
				if (stack_empty(&stack)) {
					execute = 1;
				}
				break;
			default: 
				break;
		}
		pc++;
	}
}

