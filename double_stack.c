#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "double_stack.h"

void init_double_stack_s(struct double_stack_s *stack)
{
    stack->nalloc = stack->n = 0;
    stack->values = NULL;
}

void free_double_stack_s(struct double_stack_s *stack)
{
    if (stack->values)
        free(stack->values);
    init_double_stack_s(stack);
}

void push_double(struct double_stack_s *stack, double value)
{
    if (stack->n == stack->nalloc) {
        stack->nalloc += REALLOC_INCREMENT;
        stack->values =
            realloc(stack->values, sizeof *stack->values * stack->nalloc);
    }
    stack->values[stack->n++] = value;
}

double pop_double(struct double_stack_s *stack)
{
    double value = NAN;

    if (stack->n > 0) {
        value = stack->values[--stack->n];
        if (stack->n == 0)
            free_double_stack_s(stack);
        else if (stack->n == stack->nalloc - REALLOC_INCREMENT) {
            stack->nalloc -= REALLOC_INCREMENT;
            stack->values =
                realloc(stack->values, sizeof *stack->values * stack->nalloc);
        }
    }

    return value;
}

int find_double(struct double_stack_s *stack, double value)
{
    int i;

    for (i = 0; i < stack->n; i++)
        if (stack->values[i] == value)
            return i;

    return -1;
}

int compare_doubles(const void *p1, const void *p2)
{
    double value1 = *((double *)p1);
    double value2 = *((double *)p2);

    return (value1 > value2) - (value1 < value2);
}
