#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int count_bytes = 0;
int count_chars = 0;
int count_lines = 0;
int count_words = 0;

int total_bytes = 0;
int total_chars = 0;
int total_lines = 0;
int total_words = 0;

void print_num(int num, int space)
{
    if (space)
    {
        printf(" ");
    }
    printf("%6d", num);
}

void print_output(int num_lines, int num_words, int num_bytes, int num_chars)
{
    if (count_lines)
    {
        print_num(num_lines, 0);
        total_lines += num_lines;
    }

    if (count_words)
    {
        print_num(num_words, count_lines);
        total_words += num_words;
    }

    if (count_bytes)
    {
        print_num(num_bytes, count_lines || count_words);
        total_bytes += num_bytes;
    }

    if (count_chars)
    {
        print_num(num_chars, count_lines || count_words || count_bytes);
        total_chars += num_chars;
    }
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

    print_output(num_lines, num_words, num_bytes, num_chars);
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

    int filecount = 0;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            filecount++;
            FILE *file = fopen(argv[i], "r");
            if (file == NULL)
            {
                fclose(file);
                fprintf(stderr, "%s %s: No such file or directory\n", argv[0], argv[i]);
                exit(EXIT_FAILURE);
            }
            process_file(file);
            printf(" %s\n", argv[i]);
            fclose(file);
        }
    }

    if (filecount == 0)
    {
        process_file(stdin);
        printf("\n");
    }

    if (filecount > 1)
    {
        print_output(total_lines, total_words, total_bytes, total_chars);
        printf(" total\n");
    }

    exit(EXIT_SUCCESS);
}
