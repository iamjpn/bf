#include "u.h"
#include "libc.h"
#include "bio.h"
#include "libString.h"

enum {
	TAPESIZE = 30000L,
	PROGSIZE = 30000L
};

int debug;
int test;
char *e;

int 
load(Biobuf *pf, int max, int *prog) 
{
	int i, c;

	i = 0;
	while((c = Bgetc(pf)) != Beof && i < max - 1){
		prog[i++] = c;
	}
	prog[i] = Beof;
	return 0;
}

void 
dump(char *tape, int lower, int upper) 
{
	int i;

	for(i = lower; i < upper; i++){
		fprint(2, "%d\t%d\n", i, tape[i]);
	}
}

enum { 
	STACKDEPTH = 32
};
struct Stack {
	int ptr;
	int buffer[STACKDEPTH];
};
typedef struct Stack Stack;



int 
push(Stack *stack, int d) 
{
	if(!(stack->ptr < STACKDEPTH - 1))
		return 0;
	stack->buffer[stack->ptr++] = d;
	return 1;
}

int 
pop(Stack *stack, int *d) 
{
	if(stack->ptr == 0)
		return 0;
	*d = stack->buffer[--stack->ptr];
	return 1;
}

int
empty(Stack *stack) 
{
	return(stack->ptr == 0);
}

void *
emalloc(int n) 
{
	void *ptr;

	ptr = mallocz(n, 1);
	if (ptr == nil) {
		exits(0);
	}
	return ptr;
}


void 
bf(int *prog, char *tape) 
{
	Biobuf  bin;
	Stack *stack;
	int execute;
	int ptr, op, pc, n, ok;

    Binit(&bin, 0, OREAD);
	stack = emalloc(sizeof(Stack));
	execute = 1;
	ptr = pc = 0;

	while((op = prog[pc]) != Beof) {
		if (debug) print("op: %c\n", op);
		switch(op){
		case '>': if(execute) ptr++; break;
		case '<': if(execute) ptr--; break;
		case '+': if(execute) tape[ptr]++; break;
		case '-': if(execute) tape[ptr]--; break;
		case '.': if(execute) print("%c", tape[ptr]); break;
		case ',': if(execute) (tape[ptr] = Bgetc(&bin)); break;
		case '[':
			if (debug) print("pushed %d\n", pc);
			ok = push(stack, pc);
			if (!ok) {
				e = "stack overflow";
				return;
			}
			if (execute) execute = tape[ptr];
			break;
		case ']':
			ok = pop(stack, &n);
			if (debug) print("popped %d\n", n);
			if (!ok) {
				e = "stack underflow";
				return;
			}
			if(execute){
				pc = n;
				continue;
			} 
			if (empty(stack))
				execute = 1;
			break;
		default: 
			break;
		}
		pc++;
	}
}

void 
usage(void) 
{
	fprint(2, "usage: bf [-dt [upper lower]] file\n");
	exits("usage");
}


enum { 
	USAGECODE = 1,
	OPENCODE = 2,
	UNDERCODE = 3,
	OVERCODE = 4,
};
/* Need this for Unix */
int
exitcode(char *s) 
{
	if(strstr(s, "usage"))
		return USAGECODE;
	if(strstr(s, "open"))
		return OPENCODE;
	if(strstr(s, "stack underflow"))
		return UNDERCODE;
	if(strstr(s, "stack overflow"))
		return OVERCODE;
	return 1;
}

void
main(int argc, char **argv)
{
	Biobuf  *pf;
	int prog[PROGSIZE];
	char tape[TAPESIZE];
	char *x;
	int lower, upper;

	memset(tape, 0, TAPESIZE);

	ARGBEGIN {
	case 'd': 
		debug++ ; 
		break;
	case 't': 
		x = ARGF();
		if(x == 0)
			usage();
		lower = atoi(x);
		x = ARGF();
		if(x == 0)
			usage();
		upper = atoi(x);
		test++ ; 
		break;
	default: 
		usage();
	} ARGEND

	if(argc != 1)
		usage();
	

	pf = Bopen(argv[0], OREAD);
	if(pf == nil){
		fprint(2, "bf: could not open %s %r\n", argv[0]);
		exits("open");
	}
		
	load(pf, PROGSIZE, prog);
	bf(prog, tape);
	if (test) dump(tape, lower, upper);
	exits(e);
}

