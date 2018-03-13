#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MUTATION_DECREASE_FACTOR    1.05

/*
time ./12-evol evol-quad-eq 2 -4 -2 100000 256 -100.0 100.0 1.0
real    0m20.554s
user    0m20.539s
sys 0m0.012s
*/

struct XF {
	float x, fit;
};

struct ABC {
	float a, b, c;
};

static void quad_eq_calc_fitness(struct XF* xf, const struct ABC* abc, float x)
{
    float v = abc->a * x * x + abc->b * x + abc->c;
    xf->fit = v * v;
    xf->x = x;
}

static int quad_eq_fitness_sort_proc(const struct XF** xf1, const struct XF** xf2)
{
    if((*xf1)->fit > (*xf2)->fit)
        return 1;

    if((*xf1)->fit < (*xf2)->fit)
        return -1;

    return 0;
}

static void quad_eq_calc_and_fill_fitness(struct XF* xf, struct XF** idx, const struct ABC* abc, unsigned count, float min, float max)
{
    float step = (max - min) / count;
    float val = min;
    unsigned i;

    for(i = 0; i < count; i ++, val += step) {
        idx[i] = &xf[i];
        quad_eq_calc_fitness(idx[i], abc, val);
    }

    for(i = count; i < 2 * count; i ++) {
        idx[i] = &xf[i];
    }
}

static void quad_eq_reproduce_and_mutate_x(struct XF** idx, const struct ABC* abc, unsigned count, float mut)
{
    unsigned i;
    for(i = 0; i < count; i ++)
        quad_eq_calc_fitness(idx[i + count], abc, idx[i]->x - mut + mut * 2.0 * drand48());
}

static void quad_eq_print_x_fitness(struct XF* const * idx, unsigned count)
{
    printf("x                    fitness\n");
    printf("----------------------------\n");
    while(count --) {
        printf("%-16g %11g\n", (*idx)->x, (*idx)->fit);
        idx ++;
    }
    printf("----------------------------\n");
}

static void evol_quad_eq(struct ABC* abc, unsigned gen_count, unsigned pop_count, float min, float max, float mut)
{
	struct XF pop[pop_count * 2];
	struct XF* index[pop_count * 2];

    quad_eq_calc_and_fill_fitness(pop, index, abc, pop_count, min, max);

	srand48(time(0));

	while(gen_count --) {
        quad_eq_reproduce_and_mutate_x(index, abc, pop_count, mut);
		qsort(index, pop_count * 2, sizeof(index[0]), (int (*)(const void*, const void*))quad_eq_fitness_sort_proc);
        mut /= MUTATION_DECREASE_FACTOR;
	}

	quad_eq_print_x_fitness(index, pop_count);
    printf("Final mutation: %g\n", mut);
}

static void evol_quad_eq_reverse(float x, unsigned gen_count, unsigned pop_count, float min, float max, float mut)
{
	struct ABC pop[pop_count * 2];
/*	
	int select_proc(const void* x1, const void* x2)
	{
		float v1 = a * *(float*)x1 * *(float*)x1 + b * *(float*)x1 + c;
		float v2 = a * *(float*)x2 * *(float*)x2 + b * *(float*)x2 + c;
		v1 *= v1;
		v2 *= v2;
		if(v1 > v2)
			return 1;

		if(v2 > v1)
			return -1;

		return 0;
	}

	void fill()
	{
		float step = (max - min) / pop_count;
		float val = min;
		unsigned i;
		for(i = 0; i < pop_count; i ++, val += step) {
			pop[i] = val;
		}
	}

	void reproduce_mutate()
	{
		unsigned i;
		for(i = 0; i < pop_count; i ++) {
			pop[i + pop_count] = pop[i] - mut + mut * 2.0 * drand48();
		}
	}

	void print() {
		unsigned i;
		for(i = 0; i < pop_count; i ++) {
			printf("%g ", pop[i]);
		}
		printf("\n----------------------------\n");
	}

	void sort_select()
	{
		qsort(pop, pop_count * 2, sizeof(pop[0]), select_proc);
	}


	fill();

	srand48(time(0));

	struct timespec t1, t2;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
	
	while(gen_count --) {
		reproduce_mutate();
		sort_select();
	}

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
	print();
	fprintf(stderr, "CPU TIME: %g us\n", ((double)t2.tv_nsec - (double)t1.tv_nsec) / 1000);
	*/
}

static int cmd_evol_quad_eq(int argc, char* argv[]) {
	if(argc != 9) {
		fprintf(stderr, "Usage: %s a, b, c, <generation count> <poplation count> <min value> <max value> <mutation>\n", argv[0]);
		fprintf(stderr, "Example: 2 -4 -2 256 256 -100.0 100.0 1.0\n");
		return 3;
	}
	argc --;
	argv ++;

	float min, max, mut;
	struct ABC abc = {0};
	unsigned gen_count, pop_count;
	if(1 != sscanf(argv[0], "%g", &abc.a)
	|| 1 != sscanf(argv[1], "%g", &abc.b)
	|| 1 != sscanf(argv[2], "%g", &abc.c)
	|| 1 != sscanf(argv[3], "%u", &gen_count)
	|| 1 != sscanf(argv[4], "%u", &pop_count)
	|| 1 != sscanf(argv[5], "%g", &min)
	|| 1 != sscanf(argv[6], "%g", &max)
	|| 1 != sscanf(argv[7], "%g", &mut)) {
		fprintf(stderr, "Invalid value\n");
		return 4;
	}
	evol_quad_eq(&abc, gen_count, pop_count, min, max, mut);
	return 0;
}

static int cmd_evol_quad_eq_reverse(int argc, char* argv[]) {
	if(argc != 7) {
		fprintf(stderr, "Usage: %s x, <generation count> <poplation count> <min value> <max value> <mutation>\n", argv[0]);
		fprintf(stderr, "Example: 2 -4 -2 256 256 -100.0 100.0 1.0\n");
		return 3;
	}
	argc --;
	argv ++;

	float x, min, max, mut;
	unsigned gen_count, pop_count;
	if(1 != sscanf(argv[0], "%g", &x)
	|| 1 != sscanf(argv[1], "%u", &gen_count)
	|| 1 != sscanf(argv[2], "%u", &pop_count)
	|| 1 != sscanf(argv[3], "%g", &min)
	|| 1 != sscanf(argv[4], "%g", &max)
	|| 1 != sscanf(argv[5], "%g", &mut)) {
		fprintf(stderr, "Invalid value\n");
		return 4;
	}
	evol_quad_eq_reverse(x, gen_count, pop_count, min, max, mut);
	return 0;
}

int main(int argc, char* argv[])
{
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <subcommand> <arguments>\n", argv[0]);
		fprintf(stderr, "Subcommands:\n");
		fprintf(stderr, "\tevol-quad-eq\n");
		fprintf(stderr, "\tevol-quad-reverse\n");
		return 1;
	}
	argc --;
	argv ++;
	if(!strcmp(*argv, "evol-quad-eq"))
		return cmd_evol_quad_eq(argc, argv);
	if(!strcmp(*argv, "evol-quad-eq-reverse"))
		return cmd_evol_quad_eq_reverse(argc, argv);
	fprintf(stderr, "Unknown subcommand: %s\n", *argv);
	return 2;
}

