#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

typedef struct TermNode {
    int coef;
    char variable;
    int expon;
    struct TermNode* next;
} TermNode;




typedef struct Equation {
    char name;
    int termCount;
    TermNode* head;
    struct Equation* next;
} Equation;

Equation* equations = NULL;

TermNode* createTerm(int coef, char variable, int expon) {
    TermNode* term = (TermNode*)malloc(sizeof(TermNode));
    term->coef = coef;
    term->variable = variable;
    term->expon = expon;
    term->next = NULL;
    return term;
}

void clearTerms(TermNode* head) {
    while (head) {
        TermNode* temp = head;
        head = head->next;
        free(temp);
    }
}

Equation* findEquation(char name) {
    Equation* curr = equations;
    while (curr) {
        if (curr->name == name) return curr;
        curr = curr->next;
    }
    return NULL;
}

Equation* createOrResetEquation(char name) {
    Equation* eq = findEquation(name);
    if (eq) {
        clearTerms(eq->head);
        eq->head = NULL;
        eq->termCount = 0;
        return eq;
    }

    eq = (Equation*)malloc(sizeof(Equation));
    eq->name = name;
    eq->termCount = 0;
    eq->head = NULL;
    eq->next = NULL;

    if (!equations) {
        equations = eq;
    } else {
        Equation* tail = equations;
        while (tail->next) tail = tail->next;
        tail->next = eq;
    }
    return eq;
}

// 修正排序條件：指數降序，相同指數則變數字母降序
void insertTerm(Equation* eq, int coef, char variable, int expon) {
    TermNode* term = createTerm(coef, variable, expon);
    TermNode** curr = &(eq->head);
    while (*curr && (
        (*curr)->expon > expon || 
        ((*curr)->expon == expon && (*curr)->variable > variable) // 改為大於
    )) {
        curr = &((*curr)->next);
    }
    term->next = *curr;
    *curr = term;
    eq->termCount++;
}

void printEquation(Equation* eq) {
    printf("%c %d\n", eq->name, eq->termCount);
    TermNode* curr = eq->head;
    while (curr) {
        printf("%d %d", curr->coef, curr->expon);
        if (curr->variable != '0') printf(" %c", curr->variable);
        printf("\n");
        curr = curr->next;
    }
}

// 加強名稱解析與錯誤處理
void parseEquationLine(const char* line) {
    char name;
    const char* eqSign = strchr(line, '=');
    if (!eqSign || eqSign == line) { // 檢查等號位置
        printf("ERROR\n");
        return;
    }

    if (sscanf(line, " %c=", &name) != 1) {
        printf("ERROR\n");
        return;
    }

    Equation* eq = createOrResetEquation(name);
    const char* ptr = eqSign + 1; // 跳過等號

    int first = 1;
    int prevExp = INT_MAX;
    int errorFlag = 0;

    while (*ptr && !errorFlag) {
        while (isspace(*ptr) || *ptr == '+') ptr++;
        if (*ptr == '\0' || *ptr == '\n') break;

        int sign = 1;
        if (*ptr == '-') {
            sign = -1;
            ptr++;
        } else if (*ptr == '+') {
            ptr++;
        }

        int coef = 0, hasCoef = 0;
        while (isdigit(*ptr)) {
            coef = coef * 10 + (*ptr - '0');
            hasCoef = 1;
            ptr++;
        }
        if (!hasCoef) coef = 1;
        coef *= sign;

        char var = '0';
        int expon = 0;

        if (isalpha(*ptr)) {
            var = *ptr++;
            if (*ptr == '^') {
                ptr++;
                if (!isdigit(*ptr)) {
                    errorFlag = 1;
                    break;
                }
                while (isdigit(*ptr)) {
                    expon = expon * 10 + (*ptr - '0');
                    ptr++;
                }
            } else {
                expon = 1;
            }
        } else if (*ptr != '\0' && !isspace(*ptr)) {
            errorFlag = 1;
            break;
        }

        if (coef == 0 || (var == '0' && expon != 0)) {
            errorFlag = 1;
            break;
        }

        if (!first && expon > prevExp) {
            errorFlag = 1;
            break;
        }

        prevExp = expon;
        first = 0;
        insertTerm(eq, coef, var, expon);

        while (*ptr && *ptr != '+' && *ptr != '-' && !isspace(*ptr)) ptr++;
    }

    if (errorFlag) {
        printf("ERROR\n");
        clearTerms(eq->head);
        eq->head = NULL;
        eq->termCount = 0;
        return;
    }

    printEquation(eq);
}

int main() {
    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        if (line[0] == '0') {
            printf("quit\n");
            break;
        }
        if (strchr(line, '=')) {
            parseEquationLine(line);
        }
    }
    // 釋放所有方程式記憶體
    Equation* curr = equations;
    while (curr) {
        Equation* temp = curr;
        curr = curr->next;
        clearTerms(temp->head);
        free(temp);
    }
    return 0;
}