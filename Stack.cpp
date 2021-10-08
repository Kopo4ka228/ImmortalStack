#include <stdlib.h>
#include <TXLib.h>
#include <assert.h>
#include <time.h>

//-----------------------------------------------------------------------------
int NUM_ERRORS = 0; //количество ошибок в программе
//-----------------------------------------------------------------------------

enum Error_Codes {
    stack_null = -1,
    size_bigger_cap = -2,
    stack_ok = 0
};
struct Stack
{
    unsigned int capacity;
    unsigned int size;
    double* data;
};

enum Error_Codes StackCtor (Stack* stack);
void StackDtor (Stack* stack);
enum Error_Codes StackPush (Stack*, double x);
enum Error_Codes Stack_Resize (Stack* stack);
enum Error_Codes StackPop (Stack* stack);
enum Error_Codes Stack_OK (Stack* stack);
void fill_log (enum Error_Codes code, int nLine, unsigned int* num_errors);
void Number_of_Errors (void);
void StackDump (Stack* stack);

//-----------------------------------------------------------------------------

int main()
{
    Stack st = {};
    StackCtor (&st);
    //проверить код ошибки
    for (int i = 0; i < 100; i++)
    {
        StackPush (&st, i);
    }
    for (int i = 0; i < 60; i++)
    {
        StackPop (&st);
    }
    Number_of_Errors ();
    StackDump (&st);
    return 0;
}

//-----------------------------------------------------------------------------
enum Error_Codes StackOK(Stack* stack)
{
    if (stack == nullptr)
    {
        return stack_null;
    }
    if ((stack -> size) > (stack -> capacity))
    {
        return size_bigger_cap;
    }
    else
    {
        return stack_ok;
    }
}

//-----------------------------------------------------------------------------
void fill_log (enum Error_Codes code, int nLine)  // нужно говорить о строке, в которой произошел вызов
{
    FILE* log = fopen ("log.txt", "a");

    if (code == stack_null)
    {
        fputs ("You had a zero pointer to a stack.", log);
        fprintf (log, "A line number is %d\n", nLine);
        NUM_ERRORS++;
    }
    if (code == size_bigger_cap)
    {
        fputs ("The number of values is bigger than the whole capacity.",log);
        fprintf (log, "A line number is %d\n", nLine);
        NUM_ERRORS++;
    }

    fclose (log);
}
//-----------------------------------------------------------------------------

enum Error_Codes StackCtor (Stack* stack)
{
    assert (stack != nullptr);
    stack -> capacity = 1;
    stack -> data = (double*) calloc (stack -> capacity, sizeof (double));
    stack -> size = 1;
    return stack_ok;
}
void StackDtor (Stack* stack)
{
    assert (stack != nullptr);

    memset (stack -> data, 0xF0, stack -> capacity * sizeof (double));
    stack -> size = -1;
    stack -> data = (double*) 13;
    free (stack -> data);
}

//-----------------------------------------------------------------------------

enum Error_Codes StackPush (Stack* stack, double x)
{
    assert (stack);
    stack -> data[stack -> size] = x;
    stack -> size++;
    if (stack -> size >= stack -> capacity)
    {
        Stack_Resize (stack);
    }
    Error_Codes error_code =  StackOK (stack);
    fill_log (error_code, __LINE__);
    return error_code;
}

//-----------------------------------------------------------------------------

enum Error_Codes Stack_Resize (Stack* stack)
{
    stack -> capacity = stack -> capacity * 2;
    realloc (stack -> data, stack -> capacity * sizeof (double));
    Error_Codes error_code =  StackOK (stack);
    fill_log (error_code, __LINE__);
    return error_code;
}

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

enum Error_Codes StackPop (Stack* stack)
{
    stack -> size--;
    if (stack -> size <= stack -> capacity * 0.35)     //0.35 - произвольная константа (мб от 0.35 до 0.4)
    {
        stack -> capacity = (stack -> capacity) / 2;
        realloc (stack -> data, stack -> capacity * sizeof (int));
    }
    Error_Codes error_code =  StackOK (stack);
    fill_log (error_code, __LINE__);
    return error_code;
}

//-----------------------------------------------------------------------------

void Number_of_Errors (void)
{
    FILE* log = fopen ("log.txt", "a");
    time_t rawtime = 0;
    time (&rawtime);
    struct tm* timeinfo = localtime (&rawtime);

    if (NUM_ERRORS == 0)
    {
        fprintf (log, "Compilation is OK\t%s", asctime (timeinfo));
    }
    else
    {
        fprintf (log, "%d errors\t%s", asctime (timeinfo));
    }
    fclose (log);
}

//-----------------------------------------------------------------------------

void StackDump (Stack* stack)
{
    FILE* dump = fopen ("dump.txt", "a");
    fprintf (dump, "number of elements of stack = %d\n", stack -> size);
    fprintf (dump, "number of bytes reserved for stack = %d\n", stack -> capacity);

    for (int i = 1; i < stack -> size + 1; i++)
    {
        fprintf (dump, "%d: address = %p value = %lf\n", i, stack -> data + i - 1, stack -> data[i-1]);
    }

    fprintf (dump, "ADDRESSES OF VARIABLES:\n");
    fprintf (dump, "size: %p\n", &(stack->size));
    fprintf (dump, "data: %p\n", stack->data);
    fprintf (dump, "capacity: %p\n", &(stack->capacity));
}
