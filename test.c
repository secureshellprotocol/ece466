int f()
{
    int i;
    (void *) i++;
    return 1;
}

int main()
{
    int f = 0;
    f = f + 1;
}
