#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glob.h>
#include <iconv.h>
#include <errno.h>

#define MAX_STRING 1024
#define COMPARE_LEN 5

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

typedef struct TreeNode {
    BibRecord record;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

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

int compare_records(const BibRecord *a, const BibRecord *b) {
    int author_cmp = strncmp_cp1251(a->author, b->author, COMPARE_LEN);
    if (author_cmp != 0) return author_cmp;

    int title_cmp = strncmp_cp1251(a->title, b->title, COMPARE_LEN);
    if (title_cmp != 0) return title_cmp;

    return strncmp_cp1251(a->edition, b->edition, COMPARE_LEN);
}

TreeNode* create_node(const BibRecord *record) {
    TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(EXIT_FAILURE);
    }
    node->record = *record;
    node->left = NULL;
    node->right = NULL;
    return node;
}

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

void free_tree(TreeNode *root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

void search_records(TreeNode *root, const char *search_term, FILE *output) {
    if (root == NULL) return;

    search_records(root->left, search_term, output);

    if (strncmp_cp1251(root->record.author, search_term, COMPARE_LEN) == 0 ||
        strncmp_cp1251(root->record.title, search_term, COMPARE_LEN) == 0 ||
        strncmp_cp1251(root->record.journal, search_term, COMPARE_LEN) == 0 ||
        strncmp_cp1251(root->record.publisher, search_term, COMPARE_LEN) == 0 ||
        strncmp_cp1251(root->record.isbn, search_term, COMPARE_LEN) == 0) {
        
        fprintf(output, "@article{,\n");
        fprintf(output, "   author =    {%s},\n", root->record.author);
        fprintf(output, "   title =     {%s},\n", root->record.title);
        if (root->record.journal[0] != '\0')
            fprintf(output, "   journal =   {%s},\n", root->record.journal);
        if (root->record.year[0] != '\0')
            fprintf(output, "   year =      {%s},\n", root->record.year);
        if (root->record.publisher[0] != '\0')
            fprintf(output, "   publisher = {%s},\n", root->record.publisher);
        if (root->record.isbn[0] != '\0')
            fprintf(output, "   isbn =      {%s},\n", root->record.isbn);
        if (root->record.series[0] != '\0')
            fprintf(output, "   series =    {%s},\n", root->record.series);
        if (root->record.edition[0] != '\0')
            fprintf(output, "   edition =   {%s},\n", root->record.edition);
        if (root->record.volume[0] != '\0')
            fprintf(output, "   volume =    {%s},\n", root->record.volume);
        if (root->record.url[0] != '\0')
            fprintf(output, "   url =       {%s},\n", root->record.url);
        fprintf(output, "}\n\n");
    }

    search_records(root->right, search_term, output);
}

void write_tree_to_file(TreeNode *root, FILE *output) {
    if (root == NULL) return;

    write_tree_to_file(root->left, output);

    fprintf(output, "@article{,\n");
    fprintf(output, "   author =    {%s},\n", root->record.author);
    fprintf(output, "   title =     {%s},\n", root->record.title);
    if (root->record.journal[0] != '\0')
        fprintf(output, "   journal =   {%s},\n", root->record.journal);
    if (root->record.year[0] != '\0')
        fprintf(output, "   year =      {%s},\n", root->record.year);
    if (root->record.publisher[0] != '\0')
        fprintf(output, "   publisher = {%s},\n", root->record.publisher);
    if (root->record.isbn[0] != '\0')
        fprintf(output, "   isbn =      {%s},\n", root->record.isbn);
    if (root->record.series[0] != '\0')
        fprintf(output, "   series =    {%s},\n", root->record.series);
    if (root->record.edition[0] != '\0')
        fprintf(output, "   edition =   {%s},\n", root->record.edition);
    if (root->record.volume[0] != '\0')
        fprintf(output, "   volume =    {%s},\n", root->record.volume);
    if (root->record.url[0] != '\0')
        fprintf(output, "   url =       {%s},\n", root->record.url);
    fprintf(output, "}\n\n");

    write_tree_to_file(root->right, output);
}

void extract_value(const char *line, char *dest) {
    const char *start = strchr(line, '{');
    const char *end = strrchr(line, '}');

    if (start && end && start < end) {
        size_t len = end - start - 1;
        if (len >= MAX_STRING) len = MAX_STRING - 1;
        strncpy(dest, start + 1, len);
        dest[len] = '\0';

        while (isspace(*dest) || *dest == '"' || *dest == '\'') {
            memmove(dest, dest+1, strlen(dest));
        }
        
        char *end_ptr = dest + strlen(dest) - 1;
        while (end_ptr >= dest && (isspace(*end_ptr) || *end_ptr == '"' || *end_ptr == '\'')) {
            *end_ptr = '\0';
            end_ptr--;
        }
    } else {
        dest[0] = '\0';
    }
}

void parse_bib_file(const char *filename, TreeNode **root) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Не удалось открыть файл: %s\n", filename);
        return;
    }

    char line[MAX_STRING];
    BibRecord current_record;
    int in_entry = 0;

    memset(&current_record, 0, sizeof(BibRecord));

    while (fgets(line, sizeof(line), file) != NULL) {
        char *trimmed_line = line;
        while (isspace(*trimmed_line)) trimmed_line++;
        char *end = trimmed_line + strlen(trimmed_line) - 1;
        while (end > trimmed_line && isspace(*end)) end--;
        *(end + 1) = '\0';

        if (strstr(trimmed_line, "@")) {
            if (in_entry) {
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

    if (in_entry) {
        insert_node(root, &current_record);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s файл1.bib файл2.bib ... [термин_поиска]\n", argv[0]);
        printf("Или: %s *.bib [термин_поиска]\n", argv[0]);
        return EXIT_FAILURE;
    }

    TreeNode *root = NULL;
    glob_t glob_result;
    int glob_flags = 0;
    int last_arg_is_search = 0;
    char *search_term = NULL;

    if (argc > 2 && strchr(argv[argc-1], '@') == NULL && strchr(argv[argc-1], '.') == NULL) {
        last_arg_is_search = 1;
        search_term = argv[argc-1];
    }

    int file_args = last_arg_is_search ? argc-1 : argc;
    for (int i = 1; i < file_args; i++) {
        glob(argv[i], glob_flags, NULL, &glob_result);
        glob_flags |= GLOB_APPEND;
    }

    for (size_t i = 0; i < glob_result.gl_pathc; i++) {
        parse_bib_file(glob_result.gl_pathv[i], &root);
    }
    globfree(&glob_result);

    FILE *output = fopen("output.txt", "w");
    if (!output) {
        fprintf(stderr, "Не удалось открыть выходной файл.\n");
        free_tree(root);
        return EXIT_FAILURE;
    }

    if (last_arg_is_search) {
        fprintf(output, "%% Результаты поиска для '%s':\n\n", search_term);
        search_records(root, search_term, output);
    } else {
        write_tree_to_file(root, output);
    }

    fclose(output);
    free_tree(root);

    printf("Обработка завершена. Результаты записаны в output.txt\n");
    
    return EXIT_SUCCESS;
}
//для запуска ./main 0000_0000.bib/*.bib 0000_0001.bib/*.bib 0000_0002.bib/*.bib
//для поиска ./main 0000_0000.bib/*.bib 0000_0001.bib/*.bib 0000_0002.bib/*.bib ""