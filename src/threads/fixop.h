#ifndef FIXOP_H
#define FIXOP_H

#define FRA (1<<14)


int int_mis_flt (int, int);
int int_mul_flt (int, int);
int flt_pls_int (int, int);
int flt_div_int (int, int);
int flt_pls_flt (int, int);
int flt_mis_flt (int, int);
int flt_mul_flt (int, int);
int flt_div_flt (int, int);

#endif
