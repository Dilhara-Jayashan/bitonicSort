#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// Function to find next power of 2
int next_pow2(int n) {
    int p = 1;
    while (p < n) p <<= 1;
    return p;
}

// Serial Bitonic Sort
void bitonicSort(int *arr, int n) {
    for (int k = 2; k <= n; k <<= 1) {
        for (int j = k >> 1; j > 0; j >>= 1) {
            for (int i = 0; i < n; i++) {
                int ij = i ^ j;
                if (ij > i) {
                    int ascending = ((i & k) == 0);
                    if ((ascending && arr[i] > arr[ij]) ||
                        (!ascending && arr[i] < arr[ij])) 
                    {
                        int temp = arr[i];
                        arr[i] = arr[ij];
                        arr[ij] = temp;
                    }
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    const char* input_path = argv[1];
    FILE* fp = fopen(input_path, "r");
    if (!fp) {
        printf("Error opening input file!\n");
        return 1;
    }

    // Read input values
    int cap = 1024, size = 0, val;
    int *arr = malloc(cap * sizeof(int));
    while (fscanf(fp, "%d", &val) == 1) {
        if (size == cap) {
            cap *= 2;
            arr = realloc(arr, cap * sizeof(int));
        }
        arr[size++] = val;
    }
    fclose(fp);

    // Pad dataset
    int padded = next_pow2(size);
    arr = realloc(arr, padded * sizeof(int));
    for (int i = size; i < padded; i++)
        arr[i] = INT_MAX;

    // Timing starts
    clock_t start = clock();
    bitonicSort(arr, padded);
    clock_t end = clock();

    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    system("mkdir -p OutputFiles");
    FILE *out = fopen("OutputFiles/serial_output.txt", "w");
    for (int i = 0; i < size; i++)
        fprintf(out, "%d ", arr[i]);
    fclose(out);

    printf("Dataset size: %d (padded to %d)\n", size, padded);
    printf("Serial execution time: %.6f seconds\n", time_taken);
    printf("Sorted output saved to OutputFiles/serial_output.txt\n");

    free(arr);
    return 0;
}
