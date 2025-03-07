#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <omp.h>
#include <sys/time.h>
#include<sys/sysinfo.h>

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

#define SIZE 10000


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

    double s1,s2,s3,e1,e2,e3;
    int tc;
    // parallel
#pragma omp parallel
{
    #pragma omp single
    tc = omp_get_num_threads();
}
printf("Number of threads: %d\n", tc);
    // Parallel sections
    #pragma omp sections
    {

        // section 1: sum and avg
        s1 = get_clk();
        #pragma omp section
        { 
            double sum = 0,avg=0;
            #pragma omp parallel for reduction(+:sum)
            for (int i = 0; i < SIZE; i++) {
                #pragma omp critical
                sum += data[i];
            }

            avg = sum / SIZE; 
           
            printf("Total Sum: %.2lf\n", sum);
            printf("Average: %.2lf\n", avg);
            

        }
        e1 = get_clk();
        printf("Time for 1st section %lf\n", e1-s1);

        // section 2: max and min
        s2=get_clk();
        #pragma omp section
        {
            double max = 0, min = 0;
            #pragma omp parallel for schedule(dynamic,SIZE/tc)
            for (int i = 0; i < SIZE; i++) {
                if (data[i] > max) max = data[i];
                if (data[i] < min) min = data[i];
            }
        
                printf("Maximum Temperature: %.2lf\n", max);
                printf("Minimum Temperature: %.2lf\n", min);
            
        }
        e2=get_clk();
        printf("Time for 2nd section %lf\n", e2-s2);

        // Section 3: Count  threshold
        s3 = get_clk();
        #pragma omp section
        {
            int count = 0;
            double threshold = 42.75;
            #pragma omp parallel for schedule(dynamic,SIZE/tc)
            for (int i = 0; i < SIZE; i++) {
                if (data[i] > threshold) count++;
            }
           
            printf("Readings above %.2lf°C: %d\n", threshold, count);
        }
        e3=get_clk();
        printf("Time for 3rd section %lf\n", e3-s3);
    }

   

    return 0;
}
