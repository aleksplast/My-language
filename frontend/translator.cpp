#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <strings.h>

#include "translator.h"

int TranslateStandart(Node* node, FILE* out)
{
    fprintf(out, "����� ������ �������� ���� ���\n");
    fprintf(out, "����� ������ �������� ����� ������ ���� �������\n");
    fprintf(out, "����� ������� �������� ����� ������ ���� �������\n");
    fprintf(out, "����� ����� �������� ����� ���� ���� �������\n\n");

    int indent = 0;

    TranslateNodeFromStandart(node, &indent, out);

    return NOERR;
}

int TranslateNodeFromStandart(Node* node, int* indent, FILE* out)
{
    PrintStart(node, indent, out);

    if (node->leftchild)
        TranslateNodeFromStandart(node->leftchild, indent, out);

    PrintMiddle(node, indent, out);

    if (node->rightchild)
        TranslateNodeFromStandart(node->rightchild, indent, out);

    PrintEnd(node, indent, out);

    return NOERR;
}

int PrintStart(Node* node, int* indent, FILE* out)
{
    switch (node->type)
    {
        case NUM_TYPE:
            fprintf(out, "%.0lg ", node->val);
            if (node->ancestor->optype == OP_FUNC || node->ancestor->optype == OP_CALL)
                fprintf(out, "�������� ");
            break;
        case VAR_TYPE:
        {
            fprintf(out, "%s ", node->varvalue);
            if (node->ancestor->optype == OP_FUNC || node->ancestor->optype == OP_CALL)
                fprintf(out, "�������� ");
            break;
        }
        case OP_TYPE:
        {
            switch (node->optype)
            {
                case OP_FUNC:
                    IndentPrint(*indent, out);
                    fprintf(out, "����� ");
                    break;
                case OP_ADD:
                {
                    if (Priority(node->ancestor) > Priority(node))
                        fprintf(out, "�������� ");
                    break;
                }
                case OP_SUB:
                {
                    if (Priority(node->ancestor) > Priority(node))
                        fprintf(out, "�������� ");
                    break;
                }
                case OP_MUL:
                {
                    if (Priority(node->ancestor) > Priority(node))
                        fprintf(out, "�������� ");
                    break;
                }
                case OP_DIV:
                {
                    if (Priority(node->ancestor) > Priority(node))
                        fprintf(out, "�������� ");
                    break;
                }
                case OP_VAR:
                {
                    if (node->ancestor->optype != OP_PARAM)
                        IndentPrint(*indent, out);
                    fprintf(out, "����� ");
                    break;
                }
                case OP_IF:
                    IndentPrint(*indent, out);
                    fprintf(out, "��� �������� ");
                    break;
                case OP_WHILE:
                    IndentPrint(*indent, out);
                    fprintf(out, "������ �������� ");
                    break;
                case OP_RET:
                    IndentPrint(*indent, out);
                    fprintf(out, "����� ");
                    break;
                case OP_PARAM:
                {
                    if (node->ancestor->optype == OP_PARAM)
                        fprintf(out, "����� ");
                    break;
                }
                case OP_EQ:
                    IndentPrint(*indent, out);
                    break;
                case OP_CALL:
                {
                    if (node->ancestor->optype == OP_STAT)
                        IndentPrint(*indent, out);
                }
                default:
                    break;
            }
        }
        default:
            break;
    }
    return NOERR;
}

int PrintMiddle(Node* node, int* indent, FILE* out)
{
    switch (node->type)
    {
        case NUM_TYPE:
            break;
        case VAR_TYPE:
            break;
        case OP_TYPE:
        {
            switch (node->optype)
            {
                case OP_IS_EE:
                    fprintf(out, "���1 ");
                    break;
                case OP_IS_GE:
                    fprintf(out, "���2 ");
                    break;
                case OP_IS_BE:
                    fprintf(out, "���3 ");
                    break;
                case OP_IS_GT:
                    fprintf(out, "���4 ");
                    break;
                case OP_IS_BT:
                    fprintf(out, "���5 ");
                    break;
                case OP_IS_NE:
                    fprintf(out, "���6 ");
                    break;
                case OP_EQ:
                    fprintf(out, "����� ");
                    break;
                case OP_ADD:
                    fprintf(out, "���� ");
                    break;
                case OP_SUB:
                    fprintf(out, "����� ");
                    break;
                case OP_MUL:
                    fprintf(out, "���������� ");
                    break;
                case OP_DIV:
                    fprintf(out, "���� ");
                    break;
                case OP_IF:
                    fprintf(out, "����\n");
                    IndentPrint(*indent, out);
                    *indent += 1;
                    fprintf(out, "�������\n");
                    break;
                case OP_FUNC:
                    fprintf(out, "����\n");
                    IndentPrint(*indent, out);
                    *indent += 1;
                    fprintf(out, "�������\n");
                    break;
                case OP_WHILE:
                    fprintf(out, "����\n");
                    IndentPrint(*indent, out);
                    *indent += 1;
                    fprintf(out, "�������\n");
                    break;
                case OP_ELSE:
                    *indent -= 1;
                    IndentPrint(*indent, out);
                    fprintf(out, "����\n");
                    IndentPrint(*indent, out);
                    fprintf(out, "���\n");
                    IndentPrint(*indent, out);
                    *indent += 1;
                    fprintf(out, "�������\n");
                    break;
                case OP_VAR:
                {
                    if (node->ancestor->optype != OP_PARAM)
                        fprintf(out, "����� ");
                    break;
                }
                default:
                    break;
            }
        }
        default:
            break;
    }
    return NOERR;
}

int PrintEnd(Node* node, int* indent, FILE* out)
{
    int random = rand() % 3;
    char sep[40] = "";

    switch (random)
    {
        case 0:
            sprintf(sep, "���");
            break;
        case 1:
            sprintf(sep, "�������");
            break;
        case 2:
            sprintf(sep, "�������");
            break;
        default:
            sprintf(sep, "���");
            break;
    }
    switch (node->type)
    {
        case NUM_TYPE:
            break;
        case VAR_TYPE:
            break;
        case OP_TYPE:
        {
            switch (node->optype)
            {
                case OP_FUNC:
                    fprintf(out, "����\n\n");
                    *indent -= 1;
                    break;
                case OP_ADD:
                {
                    if (Priority(node->ancestor) > Priority(node))
                        fprintf(out, "���� ");
                    break;
                }
                case OP_SUB:
                {
                    if (Priority(node->ancestor) > Priority(node))
                        fprintf(out, "���� ");
                    break;
                }
                case OP_MUL:
                {
                    if (Priority(node->ancestor) > Priority(node))
                        fprintf(out, "���� ");
                    break;
                }
                case OP_DIV:
                {
                    if (Priority(node->ancestor) > Priority(node))
                        fprintf(out, "���� ");
                    break;
                }
                case OP_ELSE:
                    *indent -= 1;
                    IndentPrint(*indent, out);
                    fprintf(out, "����\n");
                    break;
                case OP_WHILE:
                    *indent -= 1;
                    IndentPrint(*indent, out);
                    fprintf(out, "����\n");
                    break;
                case OP_RET:
                    fprintf(out, "%s\n", sep);
                    break;
                case OP_IF:
                {
                    if (node->rightchild && node->rightchild->optype != OP_ELSE)
                    {
                        *indent -= 1;
                        IndentPrint(*indent, out);
                        fprintf(out, "����\n");
                    }
                    break;
                }
                case OP_VAR:
                {
                    if (node->ancestor->optype != OP_PARAM)
                        fprintf(out, "%s\n", sep);
                    break;
                }
                case OP_EQ:
                    fprintf(out, "%s\n", sep);
                    break;
                case OP_CALL:
                {
                    fprintf(out, "���� ");
                    if (node->ancestor->optype == OP_STAT)
                        fprintf(out, "%s\n", sep);
                    break;
                }
                default:
                    break;
            }
        }
        default:
            break;
    }
    return NOERR;
}

int Priority(Node* node)
{
    if (node->optype == OP_ADD || node->optype == OP_SUB)
        return 1;
    else if (node->optype == OP_MUL || node->optype == OP_DIV)
        return 2;
    else
        return -1;

    return NOERR;
}

int IndentPrint(int indent, FILE* out)
{
    for (int counter = 0; counter < indent; counter++)
    {
        fprintf(out, "\t");
    }

    return NOERR;
}
