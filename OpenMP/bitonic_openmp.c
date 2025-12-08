#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>

/**
 * Function: next_power_of_two
 * ----------------------------
 * Calculates the next power of 2 greater than or equal to n.
 * This is needed because bitonic sort requires array size to be a power of 2.

 */
static int next_power_of_two(int n)
{
    int p = 1;
    while (p < n)
    {
        p <<= 1;  // Left shift is equivalent to multiplying by 2
    }
    return p;
}

/**
 * Function: read_input
 * --------------------
 * Reads integer values from an input file and stores them in a dynamically allocated array.
 * Uses dynamic memory allocation that grows as needed (starts at 1024, doubles when full).
 * 
 * @param path: Path to the input file containing integers
 * @param out_data: Pointer to store the allocated array of integers
 * @return: Number of integers read, or -1 on error
 * 
 * Purpose: Loads the dataset to be sorted from file into memory
 */
static int read_input(const char *path, int **out_data)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        perror("Failed to open input file");
        return -1;
    }

    int capacity = 1024;  // Initial capacity
    int size = 0;         // Current number of elements
    int *buffer = malloc(capacity * sizeof(int));
    if (!buffer)
    {
        fclose(fp);
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    while (1)
    {
        int value;
        int scanned = fscanf(fp, "%d", &value);
        if (scanned == 1)
        {
            // If buffer is full, double its capacity
            if (size == capacity)
            {
                capacity *= 2;
                int *tmp = realloc(buffer, capacity * sizeof(int));
                if (!tmp)
                {
                    free(buffer);
                    fclose(fp);
                    fprintf(stderr, "Memory allocation failed\n");
                    return -1;
                }
                buffer = tmp;
            }
            buffer[size++] = value;
        }
        else if (scanned == EOF)
        {
            break;  // End of file reached
        }
        else
        {
            free(buffer);
            fclose(fp);
            fprintf(stderr, "Invalid data in input file\n");
            return -1;
        }
    }

    fclose(fp);
    *out_data = buffer;
    return size;
}

/**
 * Function: write_output
 * ----------------------
 * Writes the sorted array to an output file.
 * Numbers are written on a single line, separated by spaces.
 * 
 * @param path: Path to the output file
 * @param data: Pointer to the sorted array of integers
 * @param count: Number of elements to write (excludes padding)
 * 
 * Purpose: Saves the sorted result to a file for verification
 */
static void write_output(const char *path, const int *data, int count)
{
    FILE *fp = fopen(path, "w");
    if (!fp)
    {
        perror("Failed to open output file");
        return;
    }

    for (int i = 0; i < count; ++i)
    {
        // Write number, add space unless it's the last element
        fprintf(fp, "%d%s", data[i], (i + 1 == count) ? "" : " ");
    }
    fprintf(fp, "\n");
    fclose(fp);
}

/**
 * Function: bitonic_sort
 * ----------------------
 * Implements the parallel bitonic sort algorithm using OpenMP.
 * Bitonic sort works by repeatedly building and merging bitonic sequences.
 * 
 * Algorithm:
 * - Outer loop (k): Controls the size of bitonic sequences (2, 4, 8, ..., n)
 * - Middle loop (j): Controls the comparison distance within each sequence
 * - Inner loop (i): Performs parallel comparisons and swaps
 * 
 * - XOR operation (^) determines which elements to compare
 * - Bitwise AND (&) determines if sorting ascending or descending
 * - Elements are swapped if they're in wrong order relative to direction
 */
static void bitonic_sort(int *data, int n)
{
    // k represents the size of bitonic sequences being built
    for (int k = 2; k <= n; k <<= 1)
    {
        // j represents the comparison distance
        for (int j = k >> 1; j > 0; j >>= 1)
        {
            // Parallelize the comparison loop - each thread handles different indices
#pragma omp parallel for schedule(static)
            for (int i = 0; i < n; ++i)
            {
                int ixj = i ^ j;  // XOR gives the partner index to compare with
                if (ixj > i)      // Only process each pair once
                {
                    // Determine sort direction: ascending (1) or descending (0)
                    int ascending = ((i & k) == 0);
                    // Swap if elements are in wrong order for this direction
                    if ((data[i] > data[ixj]) == ascending)
                    {
                        int tmp = data[i];
                        data[i] = data[ixj];
                        data[ixj] = tmp;
                    }
                }
            }
        }
    }
}

/**
 * Function: main
 * --------------
 * Main entry point for the OpenMP parallel bitonic sort program.
 * 
 * Steps:
 * 1. Read input data from file
 * 2. Pad array to next power of 2 (required for bitonic sort)
 * 3. Perform parallel bitonic sort using OpenMP threads
 * 4. Measure and display execution time
 * 5. Write sorted output to file
 * Note: Number of threads used is controlled by OMP_NUM_THREADS environment variable
 */
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Step 1: Read input data
    int *values = NULL;
    int count = read_input(argv[1], &values);
    if (count <= 0)
    {
        return 1;
    }

    // Step 2: Pad to next power of 2 if needed
    int padded = next_power_of_two(count);
    if (padded != count)
    {
        int *tmp = realloc(values, padded * sizeof(int));
        if (!tmp)
        {
            free(values);
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
        values = tmp;
        // Fill padding with INT_MAX so they sort to the end
        for (int i = count; i < padded; ++i)
        {
            values[i] = INT_MAX; // sentinel to keep padding at the end
        }
    }

    // Step 3: Sort with timing
    double start = omp_get_wtime();  // Start timing
    bitonic_sort(values, padded);    // Perform parallel sort
    double end = omp_get_wtime();    // End timing

    // Step 4: Display results
    int threads_used = omp_get_max_threads();
    printf("Dataset size: %d\n", count);
    printf("Threads: %d\n", threads_used);
    printf("Execution time (s): %.6f\n", end - start);

    // Step 5: Write sorted output (excluding padding)
    write_output("OutputFiles/openmp_output.txt", values, count);

    free(values);
    return 0;
}
