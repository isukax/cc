#include <iostream>
#include <stdlib.h>
#include <string>

enum 
{
    TK_NUM = 256,   // 整数
    TK_EOF,    
};

struct Token
{
    int type;       // トークン型
    long value;    
    char* input;    
};

Token tokens[100];

void Tokenize(char* p)
{
    int i = 0;
    while(*p)
    {
        if(isspace(*p))
        {
            ++p;
            continue;
        }

        if(*p == '+' || *p == '-')
        {
            tokens[i].type = *p;
            tokens[i].input = p;
            ++i;
            ++p;
            continue;
        }

        if(isdigit(*p))
        {
            tokens[i].type = TK_NUM;
            tokens[i].input = p;
            tokens[i].value = strtol(p, &p, 10);
            ++i;
            continue;
        }

        fprintf(stderr, "invalid token: %s\n", p);
        exit(-1);
    }

    tokens[i].type = TK_EOF;
    tokens[i].input = p;
}

void error(int i)
{
    fprintf(stderr, "error token: %s\n", tokens[i].input);
    exit(-1);
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "argc is invalid.");
        return -1;
    }

    Tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    if(tokens[0].type != TK_NUM) error(0);
    printf("    mov rax, %ld\n", tokens[0].value);

    uint32_t i = 1;
    while(tokens[i].type != TK_EOF)
    {
        if(tokens[i].type == '+')
        {
            ++i;
            if(tokens[i].type != TK_NUM) error(i);
            printf("    add rax, %ld\n", tokens[i].value);
            ++i;
            continue;
        }

        if(tokens[i].type == '-')
        {
            ++i;
            if(tokens[i].type != TK_NUM) error(i);
            printf("    sub rax, %ld\n", tokens[i].value);
            ++i;
            continue;
        }

        error(i);
    }

    printf("    ret\n");
    return 0;
}