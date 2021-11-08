#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "hash.cpp"

//-----------------------------------------------------------------------------
int NUM_ERRORS = 0; //���������� ������ � ���������
typedef long int canary_t;
canary_t  CANARY_VALUE = 666;  //���������
uint32_t SEED_VALUE = 100; // �������� ����� ��� ���-�������
//-----------------------------------------------------------------------------

enum Error_Codes {
    diff_hashes = -4,
    stack_null = -1,
    size_bigger_cap = -2,
    wrong_resize = -3,
    stack_ok = 0,
    size_less = 1,
    size_more = 2
};
struct Stack
{
    canary_t canary_1 = CANARY_VALUE;
    uint32_t data_hash = 0;
    unsigned long int capacity;
    unsigned long int size;
    double* data;
    canary_t canary_2 = CANARY_VALUE;
};

//-----------------------------------------------------------------------------

enum Error_Codes StackOK(Stack* stack);
enum Error_Codes StackCtor (Stack* stack);
void StackDtor (Stack* stack);
double StackPush (Stack*, double x);
enum Error_Codes Stack_Resize (Stack* stack, enum Error_Codes resize_st);
double StackPop (Stack* stack);
enum Error_Codes Stack_OK (Stack* stack, uint32_t old_hash);
void fill_log (enum Error_Codes code, int nLine);
void Number_of_Errors (void);
void StackDump (Stack* stack);
void HashUpdate (Stack* stack);

//-----------------------------------------------------------------------------

int main()
{
    Stack st = {};
    StackCtor (&st);
    //��������� ��� ������

    StackPush (&st, 5);
    StackPush (&st, 6);
    StackPush (&st, 7);
    StackPush (&st, 12);
    StackPush (&st, 13);
    StackPop  (&st);
    StackPop  (&st);
    StackPop  (&st);
    StackPop  (&st);

    Number_of_Errors ();
    StackDump (&st);
    StackDtor (&st);
    return 0;
}

//-----------------------------------------------------------------------------

enum Error_Codes StackOK(Stack* stack, uint32_t old_hash)
{
    if (stack == nullptr)
    {
        return stack_null;
    }

    if ( (stack -> size) > (stack -> capacity) )
    {
        return size_bigger_cap;
    }

    HashUpdate (stack);
    if ( fabs( (stack -> data_hash) - old_hash ) > 1 )
    {
        return diff_hashes;
    }

    else return stack_ok;
}

//-----------------------------------------------------------------------------

void fill_log (enum Error_Codes code, int nLine)  // ����� �������� � ������, � ������� ��������� �����
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
    stack -> data = (double*) calloc (stack->capacity + 2, sizeof (double));
    stack -> size = 0;
    stack -> data[0] = CANARY_VALUE;

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

double StackPush (Stack* stack, double x)
{
    assert (stack);
    double old_hash = (stack -> data_hash);
    StackOK (stack, old_hash);

    stack -> size++;
    if ((stack -> size) > (stack -> capacity))
    {
        Stack_Resize (stack, size_more);
    }
    stack -> data[stack -> size] = x;
    HashUpdate (stack);

    old_hash = (stack -> data_hash);
    Error_Codes error_code =  StackOK (stack, old_hash);
    fill_log (error_code, __LINE__);
    return x;
}

//-----------------------------------------------------------------------------

enum Error_Codes Stack_Resize (Stack* stack, enum Error_Codes resize_st)
{
    assert (stack);
    double old_hash = (stack -> data_hash);
    StackOK (stack, old_hash);

    if (resize_st == size_more)
    {
        (stack -> capacity) = (stack -> capacity) * 2;
        (stack -> data) = (double*) (realloc (stack -> data, (stack -> capacity + 1) * sizeof (double)));
    }

    else if (resize_st == size_less)
    {
        (stack -> capacity) = (stack -> capacity) / 2;
        (stack -> data) = (double*) (realloc (stack -> data, (stack -> capacity + 1) * sizeof (double)));
    }

    else
    {
        fill_log (wrong_resize, __LINE__);
        return wrong_resize;
    }
    HashUpdate (stack);

    old_hash = (stack -> data_hash);
    Error_Codes error_code =  StackOK (stack, old_hash);
    fill_log (error_code, __LINE__);
    return error_code;
}

//-----------------------------------------------------------------------------

double StackPop (Stack* stack)
{
    assert (stack);
    double old_hash = (stack -> data_hash);
    StackOK (stack, old_hash);

    double pop = (stack->data)[stack->size];
    stack -> size--;
    HashUpdate (stack);

    old_hash = (stack -> data_hash);
    Error_Codes error_code =  StackOK (stack, old_hash);
    fill_log (error_code, __LINE__);

    if (( stack -> size ) <= ( ( stack -> capacity ) * 0.35 ))     //0.35 - ������������ ��������� (�� �� 0.35 �� 0.4)
    {
        Stack_Resize (stack, size_less);
        HashUpdate (stack);
    }

    old_hash = (stack -> data_hash);
    error_code =  StackOK (stack, old_hash);
    fill_log (error_code, __LINE__);

    return pop;
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
        fprintf (log, "%d errors\t%s", NUM_ERRORS, asctime (timeinfo));
    }
    fclose (log);
}

//-----------------------------------------------------------------------------

void StackDump (Stack* stack)
{
    assert (stack);
    FILE* dump = fopen ("dump.txt", "a");

    time_t rawtime = 0;
    time (&rawtime);
    struct tm* timeinfo = localtime (&rawtime);

    fprintf (dump, "size = %li\n", stack -> size);
    fprintf (dump, "capacity = %li\n", stack -> capacity);

    for (unsigned int i = 0; i < (stack -> capacity) + 1; i++)
    {
        fprintf (dump, "%d: address = %p value = %lf\n", i, stack -> data + i, stack -> data[i]);
    }

    fprintf (dump, "ADDRESSES OF VARIABLES:\n");
    fprintf (dump, "capacity: %p\n", &(stack->capacity));
    fprintf (dump, "size: %p\n", &(stack->size));
    fprintf (dump, "data: %p\n", stack->data);
    fprintf (dump, "canary value in data: %lf\n", (stack->data[0]));
    fprintf (dump, "first canary check in structure: %li\n", (stack->canary_1));
    fprintf (dump, "second canary check in structure: %li\n", (stack->canary_2));
    fprintf (dump, "%s", asctime (timeinfo));
    fprintf (dump,"//-----------------------------------------------------------------------------\n");
}

//-----------------------------------------------------------------------------

void HashUpdate (Stack* stack)
{
    (stack -> data_hash) = MurmurHash2 (stack -> data, stack -> capacity,  SEED_VALUE);
}

