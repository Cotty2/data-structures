#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <limits.h>

#define MAX_STRING 1024
#define COMPARE_LEN 5

// Структура для хранения библиографической записи
typedef struct {
    char author[MAX_STRING];
    char title[MAX_STRING];
    char journal[MAX_STRING];
    char year[MAX_STRING];
    char publisher[MAX_STRING];
    char isbn[MAX_STRING];
    char series[MAX_STRING];
    char edition[MAX_STRING];
    char volume[MAX_STRING];
    char url[MAX_STRING];
} BibRecord;

// Узел двоичного дерева
typedef struct TreeNode {
    BibRecord record;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

// Функция для сравнения русских строк по первым 5 символам (CP1251)
int strncmp_cp1251(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        if (s1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

// Функция для сравнения библиографических записей
int compare_records(const BibRecord *a, const BibRecord *b) {
    int author_cmp = strncmp_cp1251(a->author, b->author, COMPARE_LEN);
    if (author_cmp != 0) return author_cmp;

    int title_cmp = strncmp_cp1251(a->title, b->title, COMPARE_LEN);
    if (title_cmp != 0) return title_cmp;

    return strncmp_cp1251(a->journal, b->journal, COMPARE_LEN);
}

// Создание нового узла дерева
TreeNode* create_node(const BibRecord *record) {
    TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->record = *record;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Вставка записи в дерево
void insert_node(TreeNode **root, const BibRecord *record) {
    if (*root == NULL) {
        *root = create_node(record);
        return;
    }

    if (compare_records(record, &(*root)->record) < 0) {
        insert_node(&(*root)->left, record);
    } else {
        insert_node(&(*root)->right, record);
    }
}

// Освобождение памяти дерева
void free_tree(TreeNode *root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

// Поиск записей по первым 5 символам в любом поле
void search_records(TreeNode *root, const char *search_term, FILE *output) {
    if (root == NULL) return;

    search_records(root->left, search_term, output);

    // Проверяем все поля на совпадение первых 5 символов
    if (strncmp(root->record.author, search_term, COMPARE_LEN) == 0 ||
        strncmp(root->record.title, search_term, COMPARE_LEN) == 0 ||
        strncmp(root->record.journal, search_term, COMPARE_LEN) == 0 ||
        strncmp(root->record.publisher, search_term, COMPARE_LEN) == 0 ||
        strncmp(root->record.isbn, search_term, COMPARE_LEN) == 0) {
        
        fprintf(output, "   title =     {%s},\n", root->record.title);
        fprintf(output, "   author =    {%s},\n", root->record.author);
        fprintf(output, "   publisher = {%s},\n", root->record.publisher);
        fprintf(output, "   isbn =      {%s},\n", root->record.isbn);
        fprintf(output, "   year =      {%s},\n", root->record.year);
        fprintf(output, "   series =    {%s},\n", root->record.series);
        fprintf(output, "   edition =   {%s},\n", root->record.edition);
        fprintf(output, "   volume =    {%s},\n", root->record.volume);
        fprintf(output, "   url =       {%s},\n\n", root->record.url);
    }

    search_records(root->right, search_term, output);
}

// Обход дерева в порядке in-order и запись в файл
void write_tree_to_file(TreeNode *root, FILE *output) {
    if (root == NULL) return;

    write_tree_to_file(root->left, output);

    fprintf(output, "   title =     {%s},\n", root->record.title);
    fprintf(output, "   author =    {%s},\n", root->record.author);
    fprintf(output, "   publisher = {%s},\n", root->record.publisher);
    fprintf(output, "   isbn =      {%s},\n", root->record.isbn);
    fprintf(output, "   year =      {%s},\n", root->record.year);
    fprintf(output, "   series =    {%s},\n", root->record.series);
    fprintf(output, "   edition =   {%s},\n", root->record.edition);
    fprintf(output, "   volume =    {%s},\n", root->record.volume);
    fprintf(output, "   url =       {%s},\n\n", root->record.url);

    write_tree_to_file(root->right, output);
}

// Функция для извлечения значения из строки .bib файла
void extract_value(const char *line, char *dest) {
    const char *start = strchr(line, '{');
    const char *end = strchr(line, '}');

    if (start && end && start < end) {
        size_t len = end - start - 1;
        if (len >= MAX_STRING) len = MAX_STRING - 1;
        strncpy(dest, start + 1, len);
        dest[len] = '\0';
    } else {
        dest[0] = '\0';
    }
}

// Парсинг .bib файла и добавление записей в дерево
void parse_bib_file(const char *filename, TreeNode **root) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return;
    }

    char line[MAX_STRING];
    BibRecord current_record;
    int in_entry = 0;

    memset(&current_record, 0, sizeof(BibRecord));

    while (fgets(line, sizeof(line), file)) {
        // Удаляем лишние пробелы и переносы строк
        char *trimmed_line = line;
        while (isspace(*trimmed_line)) trimmed_line++;
        char *end = trimmed_line + strlen(trimmed_line) - 1;
        while (end > trimmed_line && isspace(*end)) end--;
        *(end + 1) = '\0';

        if (strstr(trimmed_line, "@")) {
            // Начало новой записи
            if (in_entry) {
                // Добавляем предыдущую запись в дерево
                insert_node(root, &current_record);
                memset(&current_record, 0, sizeof(BibRecord));
            }
            in_entry = 1;
        } else if (in_entry) {
            if (strstr(trimmed_line, "author")) {
                extract_value(trimmed_line, current_record.author);
            } else if (strstr(trimmed_line, "title")) {
                extract_value(trimmed_line, current_record.title);
            } else if (strstr(trimmed_line, "journal") || strstr(trimmed_line, "booktitle")) {
                extract_value(trimmed_line, current_record.journal);
            } else if (strstr(trimmed_line, "year")) {
                extract_value(trimmed_line, current_record.year);
            } else if (strstr(trimmed_line, "publisher")) {
                extract_value(trimmed_line, current_record.publisher);
            } else if (strstr(trimmed_line, "isbn")) {
                extract_value(trimmed_line, current_record.isbn);
            } else if (strstr(trimmed_line, "series")) {
                extract_value(trimmed_line, current_record.series);
            } else if (strstr(trimmed_line, "edition")) {
                extract_value(trimmed_line, current_record.edition);
            } else if (strstr(trimmed_line, "volume")) {
                extract_value(trimmed_line, current_record.volume);
            } else if (strstr(trimmed_line, "url")) {
                extract_value(trimmed_line, current_record.url);
            }
        }
    }

    // Добавляем последнюю запись
    if (in_entry) {
        insert_node(root, &current_record);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s file1.bib file2.bib ... [search_term]\n", argv[0]);
        return EXIT_FAILURE;
    }

    TreeNode *root = NULL;

    // Парсим все файлы .bib
    for (int i = 1; i < argc; i++) {
        // Проверяем, не является ли аргумент поисковым термином
        if (i == argc - 1 && strchr(argv[i], '@') == NULL && strchr(argv[i], '.') == NULL) {
            break; // Последний аргумент - поисковый термин
        }
        parse_bib_file(argv[i], &root);
    }

    FILE *output = fopen("output.txt", "w");
    if (!output) {
        fprintf(stderr, "Could not open output file.\n");
        free_tree(root);
        return EXIT_FAILURE;
    }

    // Если указан поисковый термин, ищем, иначе выводим все записи
    if (argc > 2 && strchr(argv[argc-1], '@') == NULL && strchr(argv[argc-1], '.') == NULL) {
        fprintf(output, "Search results for '%s':\n\n", argv[argc-1]);
        search_records(root, argv[argc-1], output);
    } else {
        write_tree_to_file(root, output);
    }

    fclose(output);
    free_tree(root);

    printf("Processing completed. Results written to output.txt\n");
    
    return EXIT_SUCCESS;
}
//для запуска ./main 0000_0000.bib/*.bib 0000_0001.bib/*.bib 0000_0002.bib/*.bib
//для поиска ./main 0000_0000.bib/*.bib 0000_0001.bib/*.bib 0000_0002.bib/*.bib ""