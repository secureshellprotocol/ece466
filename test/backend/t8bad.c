int f()
{
    int x, y;
    x = x + 1;
    {
        y = y + 1;
        {
            int z;
            z = z + 1;
        }
        z = z - 1;
    }
    y = y - 1;
}
