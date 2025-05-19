int f()
{
    int x, y, z;
    x = x + 1;
    {
        y = y + 1;
        {
            z = z + 1;
        }
        z = z - 1;
    }
    y = y - 1;
}
