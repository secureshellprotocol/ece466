int times;
int _start()
{
    times = 4;

    int terms[2];
    terms[0] = 1;
    terms[1] = 1;
    int i;
    i = 0;
    while(i < times)
    {
        int h;
        h = terms[0] + terms[1];
        terms[0] = terms[1];
        terms[1] = h;
        i = i + 1;
    }

    return terms[1]; 
}
