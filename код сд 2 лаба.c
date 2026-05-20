#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <time.h>
#include <math.h>
#include <cblas.h>

#define N 2048  // Размер матрицы
#define TOTAL_OPS (2.0 * N * N * N)

void generate_random_matrix(double complex *matrix, int size) {
    for (int i = 0; i < size * size; i++) {
        matrix[i] = (double complex)(
            (double)rand() / RAND_MAX * 10.0,
            (double)rand() / RAND_MAX * 10.0
        );
    }
}

// 1-й вариант: классический алгоритм
void naive_matrix_multiply(double complex *A, double complex *B, double complex *C, int n) {
    for (int i = 0; i < n * n; i++) C[i] = 0;
    
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            double complex aik = A[i * n + k];
            for (int j = 0; j < n; j++) {
                C[i * n + j] += aik * B[k * n + j];
            }
        }
    }
}

// 3-й вариант: блочно-оптимизированный алгоритм
void optimized_block_multiply(double complex *A, double complex *B, double complex *C, int n) {
    int block_size = 64;
    
    for (int i = 0; i < n * n; i++) C[i] = 0;
    
    for (int i_block = 0; i_block < n; i_block += block_size) {
        int i_max = (i_block + block_size < n) ? i_block + block_size : n;
        
        for (int j_block = 0; j_block < n; j_block += block_size) {
            int j_max = (j_block + block_size < n) ? j_block + block_size : n;
            
            for (int k_block = 0; k_block < n; k_block += block_size) {
                int k_max = (k_block + block_size < n) ? k_block + block_size : n;
                
                for (int i = i_block; i < i_max; i++) {
                    for (int k = k_block; k < k_max; k++) {
                        double complex aik = A[i * n + k];
                        for (int j = j_block; j < j_max; j++) {
                            C[i * n + j] += aik * B[k * n + j];
                        }
                    }
                }
            }
        }
    }
}

double measure_time(void (*func)(double complex*, double complex*, double complex*, int),
                     double complex *A, double complex *B, double complex *C, int n) {
    clock_t start = clock();
    func(A, B, C, n);
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

int verify_results(double complex *C1, double complex *C2, double complex *C3, int n) {
    double epsilon = 1e-6;
    int errors = 0;
    
    for (int i = 0; i < n * n && errors < 5; i++) {
        if (cabs(C1[i] - C2[i]) > epsilon || cabs(C1[i] - C3[i]) > epsilon) {
            errors++;
        }
    }
    return errors;
}

int main() {
    printf("=== Умножение комплексных матриц %dx%d ===\n", N, N);
    printf("Теоретическая сложность: 2 * %d^3 = %.0e операций\n\n", N, TOTAL_OPS);
    
    // Выделение памяти
    double complex *A = malloc(N * N * sizeof(double complex));
    double complex *B = malloc(N * N * sizeof(double complex));
    double complex *C1 = malloc(N * N * sizeof(double complex));
    double complex *C2 = malloc(N * N * sizeof(double complex));
    double complex *C3 = malloc(N * N * sizeof(double complex));
    
    if (!A || !B || !C1 || !C2 || !C3) {
        printf("Ошибка выделения памяти!\n");
        return 1;
    }
    
    srand(42);
    printf("Генерация случайных матриц...\n");
    generate_random_matrix(A, N);
    generate_random_matrix(B, N);
    
    printf("\n=== Результаты ===\n\n");
    
    // 1-й вариант
    printf("1-й вариант (наивный алгоритм):\n");
    double time1 = measure_time(naive_matrix_multiply, A, B, C1, N);
    double mflops1 = TOTAL_OPS / time1 / 1e6;
    printf("  Время: %.4f секунд\n", time1);
    printf("  Производительность: %.2f MFLOPS\n\n", mflops1);
    
    // 2-й вариант (BLAS)
    printf("2-й вариант (BLAS cblas_zgemm):\n");
    clock_t start = clock();
    double complex alpha = 1.0, beta = 0.0;
    cblas_zgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                N, N, N,
                &alpha, A, N, B, N,
                &beta, C2, N);
    double time2 = (double)(clock() - start) / CLOCKS_PER_SEC;
    double mflops2 = TOTAL_OPS / time2 / 1e6;
    printf("  Время: %.4f секунд\n", time2);
    printf("  Производительность: %.2f MFLOPS\n\n", mflops2);
    
    // 3-й вариант
    printf("3-й вариант (блочно-оптимизированный):\n");
    double time3 = measure_time(optimized_block_multiply, A, B, C3, N);
    double mflops3 = TOTAL_OPS / time3 / 1e6;
    printf("  Время: %.4f секунд\n", time3);
    printf("  Производительность: %.2f MFLOPS\n", mflops3);
    printf("  %% от BLAS: %.1f%%\n\n", (mflops3 / mflops2) * 100);
    
    // Проверка
    printf("Проверка корректности:\n");
    int errors = verify_results(C1, C2, C3, N);
    if (errors == 0) {
        printf("✓ Все результаты совпадают!\n");
    } else {
        printf("✗ Обнаружено %d несовпадений\n", errors);
    }
    
    // Сводная таблица
    printf("\n=== Сводная таблица ===\n");
    printf("------------------------------------------\n");
    printf("Вариант          | Время (с) | MFLOPS   |%% от BLAS\n");
    printf("------------------------------------------\n");
    printf("1. Наивный       | %8.4f | %8.2f | %6.1f%%\n", time1, mflops1, (mflops1/mflops2)*100);
    printf("2. BLAS (OpenBLAS)| %8.4f | %8.2f | 100.0%%\n", time2, mflops2);
    printf("3. Блочный (опт) | %8.4f | %8.2f | %6.1f%%\n", time3, mflops3, (mflops3/mflops2)*100);
    printf("------------------------------------------\n");
    
    free(A); free(B); free(C1); free(C2); free(C3);
    return 0;
}
