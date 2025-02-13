struct double_stack_s
{
    double *values;
    int n;
    int nalloc;
};

/* double_stack.c */
void init_double_stack_s(struct double_stack_s *);
void free_double_stack_s(struct double_stack_s *);
void push_double(struct double_stack_s *, double);
double pop_double(struct double_stack_s *);
int find_double(struct double_stack_s *, double);
int compare_doubles(const void *, const void *);
