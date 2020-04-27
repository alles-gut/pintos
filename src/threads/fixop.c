#define FRA (1<<14)

int int_mis_flt (int, int);
int int_mul_flt (int, int);
int flt_pls_int (int, int);
int flt_div_int (int, int);
int flt_pls_flt (int, int);
int flt_mis_flt (int, int);
int flt_mul_flt (int, int);
int flt_div_flt (int, int);


int int_mis_flt (int i, int f)
{
  return i*FRA - f;
}

int int_mul_flt (int i, int f)
{
  return i*f;
}

int flt_pls_int (int f, int i)
{
  return f + i*FRA;
}

int flt_div_int (int f, int i)
{
  return f / i;
}

int flt_pls_flt (int f, int f_)
{
  return f + f_;
}

int flt_mis_flt (int f, int f_)
{
  return f - f_;
}

int flt_mul_flt (int f, int f_)
{
  int64_t temp = f;
  temp = temp * f_ / FRA;
  return (int)temp;
int flt_div_flt (int f, int f_);
