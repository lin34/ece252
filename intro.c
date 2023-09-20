#include <stdio.h>
#include "custom.h"
#include <stdbool.h>
#include <string.h>
#include <errno.h>

int bar( int v1, int v2);

typedef struct {
    double x;
    double y;
    double z;
} point_t;

typedef int pid_t;
int init_header(struct header * h, int v1, double v2, struct thing * v3);
int stack_push(stack* s, int* value);
void add_to_point( point_t t1, point_t t2)
{

}
/*argc  is num variables
argv is array of character arrays*/
int main ( int argc, char** argv){
    int count = atoi(argv[2]); //converts anything into an int
    return 0;
}

void string_stuff(){
    printf("Hello world!\n"); //string literal

    int z = -25;
    printf( "The vlaue of z is %d, \n",z);

    char null_term = '\0';
    char zero = '0';
}

void allocate_memory(){
    int* x = malloc(sizeof(int));
    int* y = x;
    *x = 0;
    free( x );
    point_t * p = malloc(sizeof(point_t));
    p->x = 99.9;
    free(p);

    int stack_array[10];
    int * heap_array = malloc( 10 * sizeof(int));
    stack_array[5]=7;
    heap_array[5] = 6;
    free( heap_array);
}

void initialize_array( int* a, int capacity)
{
    memset(a, 0 capacity * sizeof(int)jj);
}

int function_name( double arg1, int arg2, char arg3) {
    int i;
    for ( i = 0; i < 10; i++) {
        /* Comments */
    }
    struct point_t p1;
    p1.x = 4.3;
    p1.y = -4.24;
    p1.z = -5.35;
    return 0;
}

point_t add_points( point_t p1, point_t p2){
    point_t result;
    result.x = p1.x + p2.x;
    return result;
}

int foo() {
    return bar(7,11);
}