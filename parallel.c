// FA22-BCS-028
// Zuhra Salman

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <omp.h>
#include <sys/time.h>
#include <sys/sysinfo.h>

#define SIZE 10000

double get_clk()
{
    struct timeval tp;
    if(gettimeofday(&tp,0))
    {
        fprintf(stderr,"get_clk: gettimeofday failed \n");
        return -1;
    }
    return ((double) tp.tv_sec*1000000.0 +(double) tp.tv_usec)/1000000;
}

int main() {
    int i;
    double *data;
    char *RegNo;
    unsigned int seed = 0;

    RegNo = (char*) malloc(13 * sizeof(char));
    strcpy(RegNo, "FA22-BCS-028");

    for (i = 0; i < strlen(RegNo); i++)
        seed += (unsigned int)RegNo[i];
    srand(seed);

    data = (double*) malloc(SIZE * sizeof(double));
    printf("Generated Temperature Readings:\n");
    for (i = 0; i < SIZE; i++) {
        data[i] = ((double)rand() / RAND_MAX) * 70.0 - 20.0;
        //printf("%.2lf °C\n", data[i]);
    }
    omp_set_num_threads(8);
    double s, e;
    int tc;
    
    #pragma omp parallel
    {
        #pragma omp single
        tc = omp_get_num_threads();
    }
    printf("Number of threads: %d\n", tc);

    s = get_clk();
    double sum = 0, avg = 0,max = 0, min = 0;
    int count = 0;
    double threshold = 42.75;
    
    #pragma omp parallel for reduction(+:sum) schedule(static, SIZE/tc) 
    for (int i = 0; i < SIZE; i++) {
        sum += data[i];
        if (data[i] > max) max = data[i];
        if (data[i] < min) min = data[i];
        if (data[i] > threshold) count++;
    }
    avg = sum / SIZE;

    e = get_clk();
    printf("Total Sum: %.2lf\n", sum);
    printf("Average: %.2lf\n", avg);
    printf("Maximum Temperature: %.2lf\n", max);
    printf("Minimum Temperature: %.2lf\n", min);
    printf("Readings above %.2lf°C: %d\n", threshold, count);
    printf("Total Computation Time: %lf\n", e - s);

    return 0;
}
