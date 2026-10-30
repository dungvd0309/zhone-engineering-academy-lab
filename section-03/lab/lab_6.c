long add(long a, long b)
{
    return a + b;
}

long myfunc(long a, long b, long c, long d,
            long e, long f, long g, long h)
{
  long xx = a + b + c + d + e + f + g + h;
  long yy = add(xx, 10);
  return yy + 20;
}

int main() 
{
    long result = myfunc(1, 2, 3, 4, 5, 6, 7, 8);
    return result;
}