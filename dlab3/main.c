#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iconv.h"

// Структура библиографической карточки
typedef struct btb {
    char *type;      // Тип объекта (книга, статья, отчет)
    char *title;     // Название
    char *author;    // Автор
    char *publisher; // Издательство
    char *tsbn;      // Библиотечный индекс (индексы)
    int year;        // Год издания
    char *series;    // Серия (если есть)
    char *edition;   // Издание (если есть)
    char *volume;    // Том (если есть)
    char *url;       // Пока всегда пустое поле
} BIB;

// Структура узла дерева
typedef struct node {
    BIB *one;           // Указатель на библиографическую карточку
    struct node *left;  // Левый потомок
    struct node *right; // Правый потомок
} NODE;

NODE *tree = NULL; // Дерево для карточек

// Массив тегов для полей карточки
char *tags[] = { "title", "author", "publisher", "tsbn", "year", "series", "edition", "volume", "url" };

int main(int argc, char *argv[]) {
    FILE *F;
    char s[1024];

    // Обработка аргументов командной строки (имена файлов)
    while (--argc) {
        F = fopen(argv[argc], "r");
        printf("File %s\n", argv[argc]);

        if (F == NULL) {
            printf("Ошибка открытия %s!\n", argv[argc]);
            exit(1);
        }

        // Чтение файла построчно
        while (fgets(s, 1024, F) != NULL) {
            // Здесь можно добавить обработку строки
        }

        fclose(F); // Закрытие файла
    }

    return 0;
}