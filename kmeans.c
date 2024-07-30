#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

void free_memory_array(double ** arr, int len);
void print_centroids(double **cents, int K, int d);
int is_same(double **cents1, double **cents2, int K, int d);
void divide_by_scalar(double *arr, int d, int size_s);
void copy_array_from_to(double **arr_from, double **arr_to, int K, int d);
void sum_by_coordinates(double *arr_sum, double *arr_to_add_to_sum, int d);
void fill_zeros(double **arr, int K, int d);
double distance(double *arr1, double *arr2, int d);
int find_min(double *x_i, double **centroids, int K, int d);

int main(int argc, char* argv[]) {
    
    int K, N, d, curr_len_data_points;
    int i,j,rows,columns, j_min;
    int max_iter, curr_iter;
    int *counts;

    double *first_row;
    double **data_points, **centroids_old, **centroids_new;
    double curr_num;
    
    char curr_char;
    char *p;

    long num;
    
    /*we assume each row has 1000 chars at most*/
    first_row = (double*) malloc(1000*sizeof(double));
    rows = 0;
    columns = 0;
    N = 0;

    while (scanf("%lf%c", &curr_num, &curr_char) == 2) {
        
        first_row[columns] = curr_num;

        if (curr_char == '\n') {
            break;
        }
        if (curr_char == ',') {
            columns++;
        }

    }
    first_row[columns] = curr_num;

    d = columns + 1;
    data_points = (double**) malloc(100 * sizeof(double));
    assert(data_points != NULL);

    for (i = 0 ; i < 100 ; i++) {
        data_points[i] = (double*) malloc(d * sizeof(double));
        assert(data_points[i] != NULL);
    }

    curr_len_data_points = 100;

    errno = 0;
    num = strtol(argv[1], &p, 10);

    if (errno != 0 || *p != '\0') {
        printf("please enter a number");
        return 0;
    }    
    else {    
        K = num;
    }
    if (argc == 3) {

        errno = 0;
        num = strtol(argv[2], &p, 10);
        if (errno != 0 || *p != '\0') {
            printf("please enter a number");
            return 0;
        }    
        else {    
            max_iter = num;
        }
    }
    else {
        max_iter = 200;
    }
    
    if (K <= 0 || d <= 0 || max_iter <=0 ) {
        printf("something is wrong, enter new arguments");
        return 0;
    }
    
     for (i = 0 ; i < d ; i++) {
         data_points[0][i] = first_row[i];
     }
    N = 1;
    rows = 1;
    columns = 0;
    while (scanf("%lf%c", &curr_num, &curr_char) == 2) {

        data_points[rows][columns] = curr_num;

        if (curr_char == '\n') {
            columns = 0;
            rows++;

            N++;

            /* make data points array bigger when full */
            if (N == curr_len_data_points) {

                curr_len_data_points *= 2;
                data_points = (double**) realloc(data_points, curr_len_data_points * sizeof(double));
                assert(data_points);
                for (i = curr_len_data_points / 2 ; i < curr_len_data_points ; i++) {
                    data_points[i] = (double*) malloc(d * sizeof(double));
                    assert(data_points[i] != NULL);
                }
            }
        }
        if (curr_char == ',') {
            columns++;
        }
    }
    data_points[rows][columns] = curr_num;

    if (N <= K) {
        printf("something is wrong, enter new arguments");
        return 0;
    }

    /* create arrays for centroids */
    centroids_old = (double**) malloc(K * sizeof(double));
    centroids_new = (double**) malloc(K * sizeof(double));
    assert(centroids_old != NULL);
    assert(centroids_new != NULL);
    
    /* create arrays for data points in each centroid */
    for (i = 0 ; i < K ; i++) {
        centroids_old[i] = (double*) malloc(d * sizeof(double));
        centroids_new[i] = (double*) malloc(d * sizeof(double));
        assert(centroids_old[i] != NULL);
        assert(centroids_new[i] != NULL);
    }

    /* initialize counts array - counts[i] is size of set S_j */
    counts = (int*) calloc(K, sizeof(int));
    assert(counts);

    /* step 1: initialize centroids */
    for (i = 0 ; i < K ; i++) {
        for (j = 0; j < d ; j++) {
            centroids_new[i][j] = data_points[i][j];
        }
    }
    curr_iter = 0;

    while (curr_iter <= max_iter && !is_same(centroids_new, centroids_old, K, d)) {

        copy_array_from_to(centroids_new, centroids_old, K, d);
        fill_zeros(centroids_new, K, d);

        /* step 3 */
        for (i = 0 ; i < N ; i++) {

            /* find index j, data_points[i] is closest to the cluster S_j */
            j_min = find_min(data_points[i], centroids_old, K, d);
            sum_by_coordinates(centroids_new[j_min], data_points[i], d);
            counts[j_min] = counts[j_min] + 1;
        }
        /* step 4 */
        for (i = 0 ; i < K ; i++) {
            divide_by_scalar(centroids_new[i], d, counts[i]);
        }

        for (i = 0 ; i < K ; i++) {
            counts[i] = 0;
        }
        curr_iter++;
    }

    /* print output */
    print_centroids(centroids_new, K, d);

    /* free memory */
    free_memory_array(centroids_new, K);
    free_memory_array(centroids_old, K);
    free(first_row);
    free(counts);

    return 0;
}


void free_memory_array(double **arr, int len) {
    
    int i;
    for (i = 0; i < len; i++) {
        free(arr[i]);
    }

    free(arr);
}


void print_centroids(double **cents, int K, int d) {

    int m,q;

    for (m = 0 ; m < K ; m++) {
        for (q = 0 ; q < d -1 ; q++) {
            printf("%.4f,", cents[m][q]);
        }
        printf("%.4f", cents[m][q]);
        printf("\n");
    }

}


int is_same(double **cents1, double **cents2, int K, int d) {

    int m,q;

    for (m = 0 ; m < K ; m++) {
        for (q = 0 ; q < d  ; q++) {
            if (cents1[m][q] != cents2[m][q]) {
                return 0;
            }
        }
    }
    return 1;
}


void divide_by_scalar(double *arr, int d, int size_s) {

    int m;

    for (m = 0 ; m < d ; m++) {
 
        arr[m] = (double)(arr[m] / size_s);
        
    }
}


void copy_array_from_to(double **arr_from, double **arr_to, int K, int d) {

    int m,q;

    for (m = 0 ; m < K ; m++) {
        for (q = 0 ; q < d  ; q++) {
            arr_to[m][q] = arr_from[m][q];
        }
    }
}

void sum_by_coordinates(double *arr_sum, double *arr_to_add_to_sum, int d) {

    int m;

    for (m = 0 ; m < d ; m++) {
        arr_sum[m] += arr_to_add_to_sum[m];
    }

}

void fill_zeros(double **arr, int K, int d) {

    int m,q;

    for (m = 0 ; m < K ; m++) {
        for (q = 0 ; q < d  ; q++) {
            arr[m][q] = 0;
        }
    }
}

double distance(double *arr1, double *arr2, int d) {

    int m;
    double distance;

    distance = 0;
    for (m = 0 ; m < d ; m++) {
        distance += (double) ((arr1[m] - arr2[m]) * (arr1[m] - arr2[m]));
    }
    return distance;
}

int find_min(double *x_i, double **centroids, int K, int d) {

    int min_index, m;
    double curr_distance, min_distance;

    min_index = 0;
    min_distance = distance(x_i, centroids[0], d);

    for (m = 1 ; m < K ; m++) {

        curr_distance = distance(x_i, centroids[m], d);

        if (curr_distance < min_distance) {
            min_distance = curr_distance;
            min_index = m;
        }
        
    }
    return min_index;
}