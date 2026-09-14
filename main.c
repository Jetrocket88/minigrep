#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef struct {
    const char *data;
    size_t count;
} String_View;

String_View sv(const char *cstr) {
    return (String_View) {
        .count = strlen(cstr),
        .data = cstr,
    };
}

void sv_print(String_View* sv) {
    printf("%.*s", sv->count, sv->data);
}

void sv_print_color(String_View* sv, const char* code) {
    printf("%s%.*s%s", code, sv->count, sv->data, ANSI_COLOR_RESET);
}

void sv_chop_right(String_View* sv, size_t n) {
    if (n > sv->count) n = sv->count;
    sv->count -= n;
}

void sv_chop_right_until(String_View* sv, char c) {
    for (size_t i = 0; i < sv->count; i++) {
        if (sv->data[i] == c) {
            sv_chop_right(sv, sv->count - i - 1);
            break;
        }
    }
}

void sv_chop_left(String_View* sv, size_t n) {
    if (n > sv->count) n = sv->count;
    sv->data += n;
    sv->count -= n;
}

int main(int argc, char** argv) {

    //we know the 1st element of argv is the string we want to search
    char *string = argv[1];
    size_t string_len = strlen(string);
    char line[256];
    String_View middle_view;
    String_View left_view;
    String_View right_view;


    //and we know everything after that are the files we want to search
    for (int i = 2; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");
        if (file == NULL) {
            printf("Error opening file: %s!", argv[i]);
            return 1;
        }
        while (fgets(line, sizeof(line), file)) {
            size_t lines_read = strlen(line);
            //now we need to search the text here for the string
            char *result = strstr(line, string);
            if (result != NULL) {
                size_t index = result - line;

                middle_view = sv(line);
                left_view = right_view = middle_view;

                //left
                sv_chop_right(&left_view, lines_read - index);
                sv_print(&left_view);
                //printf("\n");

                //middle
                sv_chop_left(&middle_view, index);
                sv_chop_right(&middle_view, lines_read - (index + string_len));
                sv_print_color(&middle_view, ANSI_COLOR_RED);

                //right
                sv_chop_left(&right_view, index + string_len);
                sv_chop_right_until(&right_view, '.');
                sv_print(&right_view);
                printf("\n");
            }
        }
    }
    return 0;
}
