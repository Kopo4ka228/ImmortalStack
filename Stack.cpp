#include <stdlib.h>
#include <TXLib.h>
#include <assert.h>

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
void fill_log (enum Error_Codes code, int nLine);

//-----------------------------------------------------------------------------

int main()
{
    FILE* name = fopen ("log.txt", "w");
    fclose (name);
    Stack st = {};
    StackCtor (&st);
    //��������� ��� ������
    for (int i = 0; i < 100; i++)
    {
        StackPush (&st, i);
    }
    for (int i = 0; i < 60; i++)
    {
        StackPop (&st);
    }
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
void fill_log (enum Error_Codes code, int nLine)  // ����� �������� � ������, � ������� ��������� �����
{
    FILE* log = fopen ("log.txt", "a");

    if (code == stack_null)
    {
        fputs ("You had a zero pointer to a stack.", log);
        fprintf (log, "A line number is %d\n", nLine);
    }
    if (code == size_bigger_cap)
    {
        fputs ("The number of values is bigger than the whole capacity.",log);
        fprintf (log, "A line number is %d\n", nLine);
    }
    if (code == stack_ok)
    {
        fputs ("Process terminated with status 0.", log);
        fprintf (log, "A line number is %d\n", nLine);
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
    if (stack -> size <= stack -> capacity * 0.35)     //0.35 - ������������ ��������� (�� �� 0.35 �� 0.4)
    {
        stack -> capacity = (stack -> capacity) / 2;
        realloc (stack -> data, stack -> capacity * sizeof (int));
    }
    Error_Codes error_code =  StackOK (stack);
    fill_log (error_code, __LINE__);
    return error_code;
}

//-----------------------------------------------------------------------------


