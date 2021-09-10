// extern "C" int printf (const char *, ...);

// class a {
//  public:
//   void sub (int i)
//     {
//       printf ("__FUNCTION__ = %s\n", __FUNCTION__);
//       printf ("__PRETTY_FUNCTION__ = %s\n", __PRETTY_FUNCTION__);
//     }
// };

// int
// main (void)
// {
//   a ax;
//   ax.sub (0);
//   return 0;
// }

extern "C" int printf(const char *, ...);

struct a {
  void sub (int i);
};

void a::sub(int i)
{
    printf("__FUNCTION__ = %s\n", __FUNCTION__);
    printf("__PRETTY_FUNCTION__ = %s\n", __PRETTY_FUNCTION__);
}

int main(void)
{
  a ax;
  ax.sub (0);
  return 0;
}