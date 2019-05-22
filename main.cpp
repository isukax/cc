#include <iostream>
#include <stdlib.h>
#include <string>
#include <string.h>

enum NodeType
{
    ND_NUM,
};

struct Node
{
    int type;
    Node *lhs;
    Node *rhs;
    int value;
};

Node *CreateNode(int type, Node *lhs, Node *rhs)
{
    Node *node = new Node();
    node->type = type;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *CreateNodeNumber(int value)
{
    Node *node = new Node();
    node->type = NodeType::ND_NUM;
    node->value = value;
    return node;
}

enum TokenType
{
    TK_NUM = 256, // 整数
    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_EOF,
};

struct Token
{
    int type; // トークン型
    long value;
    char *input;
};

Token tokens[100];
int pos = 0;

void Tokenize(char *p)
{
    int i = 0;
    while (*p)
    {
        if (isspace(*p))
        {
            ++p;
            continue;
        }

        // switch(*p)
        // {
        //     case '=':

        //         break;
        //     case '!':

        //         break;
        //     case '<':

        //         break;
        //     case '>':

        //         break;
        //     case '0'...'9':
        //             break;
        // }
        if (!strncmp(p, "==", 2))
        {
            tokens[i].type = TK_EQ;
            tokens[i].input = p;
            ++i;
            p += 2;
            continue;
        }

        if (!strncmp(p, "!=", 2))
        {
            tokens[i].type = TK_NE;
            tokens[i].input = p;
            ++i;
            p += 2;
            continue;
        }

        if (!strncmp(p, "<=", 2))
        {
            tokens[i].type = TK_LE;
            tokens[i].input = p;
            ++i;
            p += 2;
            continue;
        }

        if (!strncmp(p, ">=", 2))
        {
            tokens[i].type = TK_GE;
            tokens[i].input = p;
            ++i;
            p += 2;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '>' || *p == '<')
        {
            tokens[i].type = *p;
            tokens[i].input = p;
            ++i;
            ++p;
            continue;
        }

        if (isdigit(*p))
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

bool consume(int ty)
{
    if (tokens[pos].type != ty)
    {
        return false;
    }
    pos++;
    return true;
}

Node *term();
Node *unary();
Node *mul();
Node *add();
Node *relational();
Node *equality();
Node *expr();

Node* expr()
{
    return equality();
}

Node *equality()
{
    Node *node = relational();

    for (;;)
    {
        if (consume(TK_EQ))
        {
            node = CreateNode(TK_EQ, node, relational());
        }
        else if (consume(TK_NE))
        {
            node = CreateNode(TK_NE, node, relational());
        }
        else
        {
            return node;
        }
    }
}

Node *relational()
{
    Node *node = add();

    for (;;)
    {
        if (consume(TK_LE))
        {
            node = CreateNode(TK_LE, node, add());
        }
        else if (consume(TK_GE))
        {
            node = CreateNode(TK_GE, node, add());
        }
        else if (consume('<'))
        {
            node = CreateNode('<', node, add());
        }
        else if (consume('>'))
        {
            node = CreateNode('>', node, add());
        }
        else
        {
            return node;
        }
    }
}

Node *add()
{
    Node *node = mul();

    for (;;)
    {
        if (consume('+'))
        {
            node = CreateNode('+', node, mul());
        }
        else if (consume('-'))
        {
            node = CreateNode('-', node, mul());
        }
        else
        {
            return node;
        }
    }
}

Node *mul()
{
    Node *node = unary();
    for (;;)
    {
        if (consume('*'))
        {
            node = CreateNode('*', node, unary());
        }
        else if (consume('/'))
        {
            node = CreateNode('/', node, unary());
        }
        else
        {
            return node;
        }
    }
}

Node *unary()
{
    if (consume('+'))
    {
        return term();
    }
    else if (consume('-'))
    {
        return CreateNode('-', CreateNodeNumber(0), term());
    }
    return term();
}

Node *term()
{
    if (consume('('))
    {
        Node *node = expr();
        if (!consume(')'))
        {
            error(pos);
        }
        return node;
    }

    if (tokens[pos].type == TokenType::TK_NUM)
    {
        return CreateNodeNumber(tokens[pos++].value);
    }
    error(pos);
    return nullptr;
}

void gen(Node *node)
{
    if (node->type == ND_NUM)
    {
        printf("    push %d\n", node->value);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->type)
    {
    case TK_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case TK_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case TK_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    case TK_GE:
        printf("    cmp rax, rdi\n");
        printf("    setge al\n");
        printf("    movzb rax, al\n");
        break;
    case '+':
        printf("    add rax, rdi\n");
        break;
    case '-':
        printf("    sub rax, rdi\n");
        break;
    case '*':
        printf("    mul rdi\n");
        break;
    case '/':
        printf("    mov rdx, 0\n");
        printf("    div rdi\n");
        break;
    case '<':
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case '>':
        printf("    cmp rax, rdi\n");
        printf("    setg al\n");
        printf("    movzb rax, al\n");
        break;
    }

    printf("    push rax\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "argc is invalid.");
        return -1;
    }

    Tokenize(argv[1]);

    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

#if 0
    if (tokens[0].type != TK_NUM)
        error(0);
    printf("    mov rax, %ld\n", tokens[0].value);

    uint32_t i = 1;
    while (tokens[i].type != TK_EOF)
    {
        if (tokens[i].type == '+')
        {
            ++i;
            if (tokens[i].type != TK_NUM)
                error(i);
            printf("    add rax, %ld\n", tokens[i].value);
            ++i;
            continue;
        }

        if (tokens[i].type == '-')
        {
            ++i;
            if (tokens[i].type != TK_NUM)
                error(i);
            printf("    sub rax, %ld\n", tokens[i].value);
            ++i;
            continue;
        }

        error(i);
    }
    printf("    ret\n");
#else
    gen(node);
    printf("    pop rax\n");
    printf("    ret\n");
#endif
    return 0;
}