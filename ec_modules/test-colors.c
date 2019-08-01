#include <stdio.h>

#define LXC 20

struct C {
	const char *t;
	int c;
};
struct B {
	struct C *s_c;
	int b;
};

struct G {
	int x;
};

struct A *get_A(struct G *gg) {
	printf("getting A\n");
	gg->x;
	;

	return
}

int main(void) {
	struct G *s_g;
	struct C cc;
	cc.c = 23;



}
