#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

int mycompare(const void* a, const void* b)
{
    double m = *(double*)a;
    double n = *(double*)b;
    if (m < n ) {
        return -1;
    } else if (m == n) {
        return 0;
    } else {
        return 1;
    }
}

void fill_rng(double *buffer, size_t n)
{
    const gsl_rng_type* T;
    gsl_rng* r;
    double sigma = 2.0;

    gsl_rng_env_setup();

    //T = gsl_rng_default;
    T = gsl_rng_mt19937;
    r = gsl_rng_alloc(T);

    for (size_t i = 0; i < n; i++)
    {
         double x = gsl_ran_gaussian(r, sigma);
         double v = gsl_ran_gaussian_pdf(x, sigma);
         buffer[i] = v * 100.0;
        //printf ("%.5f\n", u);
    }

    gsl_rng_free (r);
}

void dump(double* arr, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        printf("%.3f\t", arr[i]);
    }
    printf("\n");
}

void test()
{
    const size_t size = 1000;
    double vals[size];
    double sorted[size];
    const size_t stride = 1;

    fill_rng(vals, size);

    double mean = gsl_stats_mean(vals, stride, size);
    double varn = gsl_stats_variance(vals, stride, size);
    double varm = gsl_stats_variance_m(vals, stride, size, mean);
    double _min, _max;
    gsl_stats_minmax(&_min, &_max, vals, stride, size);

    printf("mean: %.3f, varn: %.3f, varm: %.3f\n",
        mean, varn, varm);
    printf("min: %.3f, max: %.3f\n", _min, _max);

    // copy vals to sorted
    memcpy(sorted, vals, size*sizeof(double));
    //printf("vals:\n");
    //dump(vals, size);
    //printf("sorted:\n");
    //dump(sorted, size);
    qsort(sorted, size, sizeof(double), mycompare);

    //printf("after qsort()\n");
    //dump(sorted, size);
    printf("0: %.3f, last: %.3f\n", sorted[0], sorted[size-1]);
}
