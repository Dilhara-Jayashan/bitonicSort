#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

/**
 * Function: next_power_of_two
 * ----------------------------
 * Calculates the next power of 2 greater than or equal to n.
 * This is needed because bitonic sort requires array size to be a power of 2.
 * 
 
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
 * Function: int_compare
 * ---------------------
 * Comparison function for qsort to sort integers in ascending order.
 * 
 
 * 
 * Purpose: Used by qsort() to compare two integers
 */
static int int_compare(const void *a, const void *b)
{
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    if (lhs < rhs)
        return -1;
    if (lhs > rhs)
        return 1;
    return 0;
}

/**
 * Function: read_input_rank0
 * --------------------------
 * Reads integer values from an input file (only called by rank 0 process).
 * Uses dynamic memory allocation that grows as needed (starts at 1024, doubles when full).

 * 
 * Purpose: Loads the dataset to be sorted from file into memory (master process only)
 */
static int read_input_rank0(const char *path, int **data_out)
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
    *data_out = buffer;
    return size;
}

/**
 * Function: compare_and_swap
 * --------------------------
 * Bitonic comparator: compares two elements and swaps them if they're in wrong order.
 * This is the fundamental operation in bitonic sort.
 * 
 * @param a: Pointer to first element
 * @param b: Pointer to second element
 * @param direction: Sort direction (1 = ascending, 0 = descending)
 * 
 * Purpose: Ensures elements are ordered correctly based on sort direction
 * - If direction is ascending (1): ensures a <= b
 * - If direction is descending (0): ensures a >= b
 */
static void compare_and_swap(int *a, int *b, int direction)
{
    if ((direction == 1 && *a > *b) || (direction == 0 && *a < *b))
    {
        int tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

/**
 * Function: bitonic_merge
 * -----------------------
 * Recursively merges two bitonic sequences into one sorted sequence.
 * A bitonic sequence is one that first increases then decreases (or vice versa).
 *
 * 
 * Algorithm:
 * 1. Compare and swap elements that are 'size/2' apart
 * 2. Recursively merge the two halves
 * 
 * Purpose: Merges two adjacent bitonic sequences into one sorted sequence
 */
static void bitonic_merge(int *data, int start, int size, int direction)
{
    if (size > 1)
    {
        int mid = size / 2;
        // Compare elements in first half with corresponding elements in second half
        for (int i = start; i < start + mid; ++i)
        {
            compare_and_swap(&data[i], &data[i + mid], direction);
        }
        // Recursively merge both halves
        bitonic_merge(data, start, mid, direction);
        bitonic_merge(data, start + mid, mid, direction);
    }
}

/**
 * Function: bitonic_sort_recursive
 * --------------------------------
 * Recursively builds and sorts a bitonic sequence using divide-and-conquer.
 * This is the classic recursive bitonic sort algorithm.
 * 
 * Algorithm:
 * 1. Divide array into two halves
 * 2. Sort first half in ascending order (creates ascending bitonic sequence)
 * 3. Sort second half in descending order (creates descending bitonic sequence)
 * 4. Merge both halves to get final sorted sequence
 * 
 * Purpose: Each MPI process uses this to sort its local data before distributed merge
 */
static void bitonic_sort_recursive(int *data, int start, int size, int direction)
{
    if (size > 1)
    {
        int mid = size / 2;
        // Sort first half in ascending order
        bitonic_sort_recursive(data, start, mid, 1);
        // Sort second half in descending order
        bitonic_sort_recursive(data, start + mid, mid, 0);
        // Merge entire sequence in desired direction
        bitonic_merge(data, start, size, direction);
    }
}

/**
 * Function: merge_exchange
 * ------------------------
 * Performs a compare-exchange operation between two MPI processes.
 * This is the distributed version of compare_and_swap for inter-process communication.
 * 
 * @param local: Local data array of this process
 * @param local_n: Number of elements in local array
 * @param partner: Rank of the partner process to exchange with
 * @param ascending: Direction flag (1 = keep smaller half, 0 = keep larger half)
 * 
 * Algorithm:
 * 1. Exchange local data with partner process (MPI_Sendrecv)
 * 2. Merge both arrays into a sorted combined array
 * 3. If ascending: keep the smaller half of merged data
 *    If descending: keep the larger half of merged data (reversed)
 * 
 * Purpose: Enables distributed bitonic sort by allowing processes to exchange
 *          and redistribute data to maintain global sort order
 */
static void merge_exchange(int *local, int local_n, int partner, int ascending)
{
    int *recv_buf = malloc(local_n * sizeof(int));
    int *merged = malloc(2 * local_n * sizeof(int));
    if (!recv_buf || !merged)
    {
        fprintf(stderr, "Memory allocation failed during merge\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Exchange data with partner process (simultaneous send and receive)
    MPI_Sendrecv(local, local_n, MPI_INT, partner, 0,
                 recv_buf, local_n, MPI_INT, partner, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Merge the two sorted arrays
    int i = 0, j = 0, m = 0;
    while (i < local_n && j < local_n)
    {
        if (local[i] <= recv_buf[j])
        {
            merged[m++] = local[i++];
        }
        else
        {
            merged[m++] = recv_buf[j++];
        }
    }
    while (i < local_n)
        merged[m++] = local[i++];
    while (j < local_n)
        merged[m++] = recv_buf[j++];

    if (ascending)
    {
        // Ascending: keep smaller half (first half of merged array)
        memcpy(local, merged, local_n * sizeof(int));
    }
    else
    {
        // Descending: keep larger half in reverse order (last half of merged array)
        for (int idx = 0; idx < local_n; ++idx)
        {
            local[idx] = merged[2 * local_n - 1 - idx];
        }
    }

    free(merged);
    free(recv_buf);
}


/**
 * Function: write_output_rank0
 * ----------------------------
 * Writes the sorted array to an output file (only called by rank 0 process).
 * Numbers are written on a single line, separated by spaces.
 * 
 * Purpose: Saves the final sorted result to a file for verification
 */
static void write_output_rank0(const char *path, const int *data, int count)
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
 * Function: main
 * --------------
 * Main entry point for the MPI distributed bitonic sort program.
 * 
 * Overall Process:
 * 1. Initialize MPI and get process rank/size
 * 2. Rank 0 reads input and pads to appropriate size
 * 3. Distribute data chunks to all processes
 * 4. Each process sorts its local chunk
 * 5. Gather all sorted chunks back to rank 0
 * 6. Rank 0 merges all chunks into final sorted array
 * 7. Output results and timing information
 */
int main(int argc, char **argv)
{
    // Step 1: Initialize MPI
    MPI_Init(&argc, &argv);

    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);      // Get this process's rank (ID)
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Get total number of processes

    if (argc < 2)
    {
        if (rank == 0)
        {
            fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int *global_data = NULL;
    int original_count = 0;
    int padded_count = 0;

    // Step 2: Rank 0 reads input and prepares data
    if (rank == 0)
    {
        original_count = read_input_rank0(argv[1], &global_data);
        if (original_count <= 0)
        {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Calculate padded size: must be power of 2 AND divisible by world_size
        padded_count = next_power_of_two(original_count);
        while (padded_count % world_size != 0)
        {
            padded_count <<= 1;  // Keep doubling until divisible
        }

        // Pad array with INT_MAX (so padding sorts to the end)
        int required = padded_count - original_count;
        int *tmp = realloc(global_data, padded_count * sizeof(int));
        if (!tmp)
        {
            free(global_data);
            fprintf(stderr, "Memory allocation failed\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        global_data = tmp;
        for (int i = 0; i < required; ++i)
        {
            global_data[original_count + i] = INT_MAX;
        }
    }

    // Step 3: Broadcast counts to all processes
    MPI_Bcast(&original_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&padded_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 4: Allocate local buffer for this process's chunk
    int local_n = padded_count / world_size;  // Each process gets equal chunk
    int *local_data = malloc(local_n * sizeof(int));
    if (!local_data)
    {
        fprintf(stderr, "Rank %d failed to allocate local buffer\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Step 5: Distribute data chunks to all processes
    MPI_Scatter(global_data, local_n, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 6: Start timing after data distribution
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    // Step 7: Each process independently sorts its local data
    bitonic_sort_recursive(local_data, 0, local_n, 1);

    // Step 8: Gather all sorted chunks back to rank 0
    // All processes send their sorted data to rank 0, which will merge them
    int *all_data = NULL;
    if (rank == 0)
    {
        all_data = malloc(padded_count * sizeof(int));
        if (!all_data)
        {
            fprintf(stderr, "Memory allocation failed\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Gather(local_data, local_n, MPI_INT, all_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 9: Rank 0 merges all sorted chunks
    if (rank == 0)
    {
        // Allocate temporary buffer for merge operations
        int *temp_buf = malloc(padded_count * sizeof(int));
        if (!temp_buf)
        {
            fprintf(stderr, "Memory allocation failed\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Iteratively merge sorted chunks (merge sort approach)
        // Start with chunks of size local_n, double merge_width each iteration
        int *current = all_data;
        int *next = temp_buf;

        for (int merge_width = local_n; merge_width < padded_count; merge_width *= 2)
        {
            int res_idx = 0;
            // Merge pairs of sorted subarrays
            for (int base = 0; base < padded_count; base += 2 * merge_width)
            {
                int left_end = base + merge_width;
                int right_end = (base + 2 * merge_width < padded_count) ? base + 2 * merge_width : padded_count;
                if (left_end > padded_count)
                    left_end = padded_count;

                // Merge two sorted subarrays: [base, left_end) and [left_end, right_end)
                int l = base, r = left_end;
                while (l < left_end && r < right_end)
                {
                    if (current[l] <= current[r])
                    {
                        next[res_idx++] = current[l++];
                    }
                    else
                    {
                        next[res_idx++] = current[r++];
                    }
                }
                // Copy remaining elements
                while (l < left_end)
                    next[res_idx++] = current[l++];
                while (r < right_end)
                    next[res_idx++] = current[r++];
            }

            // Swap buffers (avoid copying entire array)
            int *swap = current;
            current = next;
            next = swap;
        }

        // Copy result back to all_data if needed
        if (current != all_data)
        {
            memcpy(all_data, current, padded_count * sizeof(int));
        }

        free(temp_buf);
    }

    // Step 10: Stop timing and synchronize all processes
    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    int *gathered = NULL;
    if (rank == 0)
    {
        gathered = all_data;
    }

    // Step 11: Rank 0 writes output and displays results
    if (rank == 0)
    {
        // Write sorted output (excluding padding elements)
        write_output_rank0("OutputFiles/mpi_output.txt", gathered, original_count);
        
        // Display performance metrics
        printf("Processes: %d\n", world_size);
        printf("Execution time (s): %.6f\n", end - start);
        
        free(gathered);
    }

    // Step 12: Clean up and finalize
    free(local_data);
    free(global_data);

    MPI_Finalize();
    return 0;
}
