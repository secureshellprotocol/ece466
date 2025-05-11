int a;

int func1()
{
    int b;

    b = 1;

    a = b++;
    {
        int c;
        c = b;
    }
    return b;
}
