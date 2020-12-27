#include <stdio.h>
#include <omp.h>
#include <time.h> 

int main() {

    int width = 1920, height = 1080, max = 30;
    char colors[max][3], black[3] = {0,0,0};

    FILE* image = fopen("mandelbrotcpp.pgm", "wb"); 
    fprintf(image, "P6\n");                     // P5 filetype
    fprintf(image, "%d %d\n", width, height);   // dimensions
    fprintf(image, "255\n");                    // Max pixel

    clock_t start = clock();

    #pragma omp parellel 
    {
        #pragma omp parallel for
            for(int i = 0 ; i < max ; i++) { 
                for(int j = 0 ; j < 3 ; j++) {
                    colors[i][j] = ( i * 10 + j * 10 ) % 255;
                }
            }
        
        #pragma omp parallel for ordered 
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                double c_re = (col - width/2)*4.0/width;
                c_re -= 0.5;
                double c_im = (row - height/2)*3.0/height;
                double x = 0, y = 0;
                int iteration = 0;
                while (x*x+y*y < 4 && iteration < max) {
                    double x_new = x*x-y*y+c_re;
                    y = 2*x*y+c_im;
                    x = x_new;
                    iteration++;
                }
                
                #pragma omp ordered 
                    if (iteration < max) fwrite(colors[iteration], sizeof(char), 3, image);
                    else fwrite(black, sizeof(char), 3, image);
            }
        }
    } 

    printf("Execution time: %fms\n", (double) (clock() - start) * 1000 / CLOCKS_PER_SEC);

    fclose(image);
    return 0;
}