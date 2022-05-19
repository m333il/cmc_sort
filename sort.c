#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>

typedef struct test_sort
{
    int len;
    int mode;
    long long *arr_qsort;
    long long *arr_shell_sort;
    long long *arr_selection_sort;
    int swaps_shell_sort;
    int swaps_selection_sort;
    int comparisons_shell_sort;
    int comparisons_selection_sort;
    int ok_shell_sort;
    int ok_selection_sort;

} test_sort;

//generates ramdom long long number
long long rand_ll(void) {
	union bigRand {
		long long l;
		int i[2];   
	} b;
    b.i[0] =  rand();
    b.i[1] =  rand();
    return b.l;
}

//make swap and increase amount of swaps
void swap(long long *a, long long *b, int *swaps) {
    if (a == b)
        return;
    ++(*swaps);
    long long t = *a;
    *a = *b;
    *b = t;
}

//compare values and increase amount of comparisons
int cmp(const long long *a, const long long *b, int *comparisons) {
    ++(*comparisons);
    if (*a == LLONG_MIN)
        return 1;
    else if (*b == LLONG_MIN)
        return 0;
    if (llabs(*a) == llabs(*b))
        return *b > *a;
    return llabs(*a) > llabs(*b);
}

//same as cmp(), but without increasing comparisons, because it is used in qsort and generating sorted array
int cmp_for_qsort1(const long long *a, const long long *b) {
    if (*a == LLONG_MIN)
        return 0;
    else if (*b == LLONG_MIN)
        return 1;
    if (llabs(*a) == llabs(*b))
        return *a > *b;
    return llabs(*b) > llabs(*a);
}

//used in generating reverse-sorted array
int cmp_for_qsort2(const long long *a, const long long *b) {
    if (*a == LLONG_MIN)
        return 0;
    else if (*b == LLONG_MIN)
        return 1;
    return llabs(*a) > llabs(*b);
}

//generates array and makes 3 copies of it
void gen_arr(int n, int mode, long long *arr1, long long *arr2, long long *arr3) {
    //random
    if (mode == 4 || mode == 3)
        for (int i = 0; i < n; ++i) {
            long long el = rand_ll();
            arr1[i] = arr2[i] = arr3[i] = el;
        }
    //revers-sorted
    if (mode == 2) {
        for (int i = 0; i < n; ++i) {
            long long el = rand_ll();
            arr1[i] = el;
        }
        qsort(arr1, n, sizeof(long long), (int(*) (const void*, const void*)) cmp_for_qsort2);
        for (int i = 0; i < n; ++i)
            arr2[i] = arr3[i] = arr1[i];
    }
    //sorted
    if (mode == 1) {
        for (int i = 0; i < n; ++i) {
            long long el = rand_ll();
            arr1[i] = el;
        }
        qsort(arr1, n, sizeof(long long), (int(*) (const void*, const void*)) cmp_for_qsort1);
        for (int i = 0; i < n; ++i)
            arr2[i] = arr3[i] = arr1[i];
    }
}

//prints array
void print_arr(long long *a, int n) {
    for (int i = 0; i < n; ++i)
        printf("%lld ", a[i]);
    printf("\n");
}

void selection_sort(long long *a, int n, int *comp, int *swaps) {
    *comp = *swaps = 0;
    for (int i = 0; i < n; ++i) {
        if (a[i] == LLONG_MIN) {
            swap(&a[i], &a[0], swaps);
            continue;
        }
        long long max_n = llabs(a[i]), max_i = i;
        for (int j = i + 1; j < n; ++j) {
            if (cmp(&a[j], &max_n, comp)) {
                max_n = llabs(a[j]);
                max_i = j;
            }
        }
        if (i != max_i)
            swap(&a[i], &a[max_i], swaps);
    }
}

void shell_sort(long long *a, int n, int *comp, int *swaps) {
    *comp = *swaps = 0;
    int hib_len = 1, t = 1;
    while (t < n) {
        t *= 2;
        ++hib_len;
    }
    int *hib_arr = malloc(hib_len * sizeof(int));
    for (int i = 0; i < hib_len; ++i) {
        hib_arr[i] = 1;
        for (int j = 0; j < i + 1; ++j)
            hib_arr[i] *= 2;
        --hib_arr[i];
    }

    for (int h = hib_len - 1; h >= 0; --h) {
        int gap = hib_arr[h];
        for (int i = gap; i < n; ++i) {
            long long t = a[i];
            int j;
            for (j = i; j >= gap && !cmp(&a[j - gap], &t, comp); j-= gap)
                swap(&a[j], &a[j - gap], swaps);
            a[j] = t;
            ++(*swaps);
        }
    }
}

//checks if sorting is right or wrong by comparing each element to element from qsorted array
int check(long long *a, long long *b, int n) {
    for (int i = 0; i < n; ++i)
        if (a[i] != b[i])
            return 0;
    return 1;
}

void test(test_sort *t) {
    t->comparisons_selection_sort = t->comparisons_shell_sort = 0;
    t->swaps_selection_sort = t->swaps_shell_sort = 0;

    t->arr_qsort = malloc(t->len * sizeof(long long));
    t->arr_selection_sort = malloc(t->len * sizeof(long long));
    t->arr_shell_sort = malloc(t->len * sizeof(long long));

    gen_arr(t->len, t->mode, t->arr_qsort, t->arr_selection_sort, t->arr_shell_sort);

    shell_sort(t->arr_shell_sort, t->len, &t->comparisons_shell_sort, &t->swaps_shell_sort);
    selection_sort(t->arr_selection_sort, t->len, &t->comparisons_selection_sort, &t->swaps_selection_sort);
    qsort(t->arr_qsort, t->len, sizeof(long long), (int(*) (const void*, const void*)) cmp_for_qsort1);

    t->ok_shell_sort = check(t->arr_shell_sort, t->arr_qsort, t->len);
    t->ok_selection_sort = check(t->arr_selection_sort, t->arr_qsort, t->len);
}

void check_test(test_sort *t, int n) {
    printf("test №%d with %d elements\n", n + 1, t->len);
    if (t->mode == 1)
        printf("Mode: sorted\n");
    else if (t->mode == 2)
        printf("Mode: reverse sorted\n");
    else
        printf("Mode: random\n");
    printf("shell sort:\ncomparisons: %d\nswaps: %d\nok: %d", t->comparisons_shell_sort, t->swaps_shell_sort, t->ok_shell_sort);
    printf("\nselection sort:\ncomparisons: %d\nswaps: %d\nok: %d\n\n\n", t->comparisons_selection_sort, t->swaps_selection_sort, t->ok_selection_sort);
}

void free_test(test_sort *t) {
    free(t->arr_qsort);
    free(t->arr_selection_sort);
    free(t->arr_shell_sort);
}

int main(void) {
    printf("111");
    srand(time(NULL));
    int amount = 16;
    test_sort t[amount];
    
    t[0].len = 10; t[0].mode = 1;
    t[1].len = 10; t[1].mode = 2;
    t[2].len = 10; t[2].mode = 3;
    t[3].len = 10; t[3].mode = 4;
    t[4].len = 100; t[4].mode = 1;
    t[5].len = 100; t[5].mode = 2;
    t[6].len = 100; t[6].mode = 3;
    t[7].len = 100; t[7].mode = 4;
    t[8].len = 1000; t[8].mode = 1;
    t[9].len = 1000; t[9].mode = 2;
    t[10].len = 1000; t[10].mode = 3;
    t[11].len = 1000; t[11].mode = 4;
    t[12].len = 10000; t[12].mode = 1;
    t[13].len = 10000; t[13].mode = 2;
    t[14].len = 10000; t[14].mode = 3;
    t[15].len = 10000; t[15].mode = 4;

    for (int i = 0; i < amount; ++i)
        test(&t[i]);
    for (int i = 0; i < amount; ++i)
        check_test(&t[i], i);
    for (int i = 0; i < amount; ++i)
        free_test(&t[i]);
    return 0;
}