#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_FILES 100

typedef struct
{
    int num_bytes;
    int num_chars;
    int num_lines;
    int num_words;
    const char* filename;
} file_info_t;

file_info_t file_info[MAX_FILES];

int filecount = 0;

int count_bytes = 0;
int count_chars = 0;
int count_lines = 0;
int count_words = 0;

int digits = 0;

int digit_count(int num)
{
    int count = 0;
    while (num)
    {
        num /= 10;
        count++;
    }
    return count;
}

void print_num(int num, int space)
{
    if (space)
    {
        printf(" ");
    }
    printf("%*d", digits, num);
}

void print_output(int ind)
{
    if (count_lines)
    {
        print_num(file_info[ind].num_lines, 0);
    }

    if (count_words)
    {
        print_num(file_info[ind].num_words, count_lines);
    }

    if (count_bytes)
    {
        print_num(file_info[ind].num_bytes, count_lines || count_words);
    }

    if (count_chars)
    {
        print_num(file_info[ind].num_chars, count_lines || count_words || count_bytes);
    }

    printf(" %s\n", file_info[ind].filename);
}

void process_file(FILE* file)
{
    fseek(file, 0, SEEK_END);
    int num_bytes = ftell(file);
    fseek(file, 0, SEEK_SET);
    int num_lines = 0;
    int num_words = 0;
    int num_chars = 0;
    int c;
    int in_word = 0;
    while ((c = fgetc(file)) != EOF)
    {
        num_chars++;
        if (c == '\n')
        {
            num_lines++;
        }

        if (isspace(c))
        {
            if (in_word)
            {
                num_words++;
                in_word = 0;
            }
        }
        else
        {
            in_word = 1;
        }
    }

    file_info[filecount].num_words = num_words;
    file_info[filecount].num_lines = num_lines;
    file_info[filecount].num_bytes = num_bytes;
    file_info[filecount].num_chars = num_chars;

    

    file_info[0].num_words += num_words;
    file_info[0].num_words += num_words;
    file_info[0].num_lines += num_lines;
    file_info[0].num_bytes += num_bytes;
    file_info[0].num_chars += num_chars;
}

int main(int argc, char **argv)
{
    int c;
    int use_default_flags = 1;
    while ((c = getopt (argc, argv, "clmw:")) != -1)
    {
        switch (c)
          {
          case 'c':
              count_bytes = 1;
              use_default_flags = 0;
              break;
          case 'l':
              count_lines = 1;
              use_default_flags = 0;
              break;
          case 'm':
              count_chars = 1;
              use_default_flags = 0;
              break;
          case 'w':
              count_words = 1;
              use_default_flags = 0;
              break;
          default:
              break;
          }
    }

    if (use_default_flags)
    {
        count_lines = 1;
        count_words = 1;
        count_chars = 1;
    }

    filecount = 0;

    file_info[0] = (file_info_t){0, 0, 0, 0, "total"};

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            continue;
        }

        filecount++;
        if (filecount >= MAX_FILES)
        {
            fprintf(stderr, "%s: too many files\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        if (access(argv[i], F_OK) == -1)
        {
            fprintf(stderr, "%s %s: No such file or directory\n", argv[0], argv[i]);
            exit(EXIT_FAILURE);
        }

        FILE *file = fopen(argv[i], "r");
        file_info[filecount] = (file_info_t){0, 0, 0, 0, argv[i]};
        if (file == NULL)
        {
            fclose(file);
            fprintf(stderr, "%s %s: No such file or directory\n", argv[0], argv[i]);
            exit(EXIT_FAILURE);
        }
        process_file(file);
        fclose(file);
    }

    if (filecount == 0)
    {
        file_info[0].filename = "";
        process_file(stdin);
    }

    if (count_bytes)
    {
        digits = digit_count(file_info[0].num_bytes);
    } else if (count_chars)
    {
        digits = digit_count(file_info[0].num_chars);
    } else if (count_words)
    {
        digits = digit_count(file_info[0].num_words);
    } else if (count_lines)
    {
        digits = digit_count(file_info[0].num_lines);
    }

    for (int i = 1; i <= filecount; i++)
    {
        print_output(i);
    }

    if (filecount == 0 || filecount > 1)
    {
        print_output(0);
    }

    exit(EXIT_SUCCESS);
}
