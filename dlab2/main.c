#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct word {
    int count;
    char *w;
} WORD;

int cmpword_alpha(const void *a, const void *b);
int cmpword_quant(const void *a, const void *b);
char *getword(FILE *F);

WORD *words[100000]; // Массив указателей на структуры
int totalw = 0;

int main(int argc, char *argv[]) {
    FILE *OUT1, *OUT2, *IN;
    char *pword;

    // Открываем файлы для вывода
    OUT1 = fopen("sorted_alpha.txt", "w");
    OUT2 = fopen("sorted_quant.txt", "w");
    if (!OUT1 || !OUT2) {
        printf("Ошибка открытия файлов вывода!\n");
        exit(1);
    }

    if (argc < 2) {
        printf("Укажите файлы для обработки!\n");
        exit(0);
    }

    // Обработка файлов (аргументы с 1 до argc-1)
    for (int i = 1; i < argc; i++) {
        IN = fopen(argv[i], "r");
        if (!IN) {
            printf("Ошибка открытия %s\n", argv[i]);
            continue;
        }
        printf("Обрабатывается файл %s\n", argv[i]);

        while ((pword = getword(IN)) != NULL) {
            int found = 0;
            // Поиск существующего слова
            for (int j = 0; j < totalw; j++) {
                if (strcmp(words[j]->w, pword) == 0) {
                    words[j]->count++;
                    found = 1;
                    free(pword); // Освобождаем временную копию
                    break;
                }
            }
            // Добавление нового слова
            if (!found) {
                WORD *new_word = malloc(sizeof(WORD));
                new_word->w = pword;
                new_word->count = 1;
                words[totalw++] = new_word;
            }
        }
        fclose(IN);
    }

    // Сортировка по алфавиту
    qsort(words, totalw, sizeof(WORD*), cmpword_alpha);
    for (int i = 0; i < totalw; i++) {
        fprintf(OUT1, "%s: %d\n", words[i]->w, words[i]->count);
    }

    // Сортировка по частоте
    qsort(words, totalw, sizeof(WORD*), cmpword_quant);
    for (int i = 0; i < totalw; i++) {
        fprintf(OUT2, "%s: %d\n", words[i]->w, words[i]->count);
    }

    // Освобождение памяти
    for (int i = 0; i < totalw; i++) {
        free(words[i]->w);
        free(words[i]);
    }

    fclose(OUT1);
    fclose(OUT2);
    return 0;
}

char *getword(FILE *F) {
    int c;
    char buffer[256];
    int idx = 0;

    // Пропуск символов до первой буквы
    while ((c = fgetc(F)) != EOF && !isalpha(c));

    if (c == EOF) return NULL;

    buffer[idx++] = tolower(c);

    // Чтение букв и цифр после первой буквы
    while ((c = fgetc(F)) != EOF && (isalnum(c))) {
        if (idx < 255) buffer[idx++] = tolower(c);
    }
    buffer[idx] = '\0';

    char *word = malloc(idx + 1);
    strcpy(word, buffer);
    return word;
}

int cmpword_alpha(const void *a, const void *b) {
    const WORD *word1 = *(const WORD**)a;
    const WORD *word2 = *(const WORD**)b;
    return strcmp(word1->w, word2->w);
}

int cmpword_quant(const void *a, const void *b) {
    const WORD *word1 = *(const WORD**)a;
    const WORD *word2 = *(const WORD**)b;
    return word2->count - word1->count;
}
//для запуска ./main texts/*.txt