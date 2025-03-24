#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SETBIT(b, k) ((0200 >> (k)) | (b))
#define CLRBIT(b, k) (~(0200 >> (k)) & (b))
#define TESTBIT(b, k) (((0200 >> (k)) & (b)) ? 1 : 0)

int main(int argc, char *argv[]) {  
    long N;
    printf("Введите N:\n");
    scanf("%ld", &N);

    if (N < 2) {
        printf("Нет простых чисел, меньших или равных %ld.\n", N);
        return 0;
    }

    size_t nat_size = (N + 7) / 8;
    unsigned char *nat = malloc(nat_size);
    if (!nat) {
        printf("Ошибка выделения памяти.\n");
        return 1;
    }

    // Инициализация всех битов в 1
    for (size_t i = 0; i < nat_size; i++) {
        nat[i] = 0377; // 0377 в восьмеричной системе = 0xFF
    }

    // Установка битов для 0 и 1 в 0 (они не являются простыми числами)
    nat[0] = CLRBIT(nat[0], 0);
    nat[0] = CLRBIT(nat[0], 1);

    long sqrtN = (long)sqrt(N);
    for (long i = 2; i <= sqrtN; i++) {
        if (TESTBIT(nat[i / 8], i % 8)) {
            for (long j = i * i; j <= N; j += i) {
                nat[j / 8] = CLRBIT(nat[j / 8], j % 8);
            }
        }
    }

    printf("Простые числа до %ld:\n", N);
    for (long i = 2; i <= N; i++) {
        if (TESTBIT(nat[i / 8], i % 8)) {
            printf("%ld ", i);
        }
    }
    printf("\n");

    free(nat);
    return 0;
}