#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

typedef struct equNode *equPointer;
typedef struct equNode {
    int coef;
    char variable;
    int expon;
    equPointer link;
} equNode;

typedef struct equHead *headPointer;
typedef struct equHead {
    int nodeNumber;
    char equNotation;
    equPointer link;
    headPointer hlink;
} equHead;

headPointer equations = NULL;

headPointer createEquation(char notation) {
    headPointer newEq = (headPointer)malloc(sizeof(equHead));
    if (!newEq) exit(1);
    newEq->equNotation = notation;
    newEq->nodeNumber = 0;
    newEq->link = NULL;
    newEq->hlink = NULL;
    return newEq;
}

void insertNode(headPointer eq, int coef, char variable, int expon) {
    if (!eq) return;
    equPointer newNode = (equPointer)malloc(sizeof(equNode));
    if (!newNode) exit(1);
    newNode->coef = coef;
    newNode->variable = variable;
    newNode->expon = expon;
    newNode->link = NULL;

    equPointer *curr = &(eq->link);
    while (*curr && ((*curr)->expon > expon || ((*curr)->expon == expon && (*curr)->variable > variable))) {
        curr = &((*curr)->link);
    }

    if (*curr && (*curr)->expon == expon && (*curr)->variable == variable) {
        (*curr)->coef += coef;
        if ((*curr)->coef == 0) {
            equPointer temp = *curr;
            *curr = (*curr)->link;
            free(temp);
            eq->nodeNumber--;
        }
    } else {
        newNode->link = *curr;
        *curr = newNode;
        eq->nodeNumber++;
    }
}

void eread() {
    char input[100];
    fgets(input, sizeof(input), stdin);

    char notation;
    char *ptr = input;

    if (sscanf(ptr, "%c=", &notation) != 1) {
        printf("ERROR\n");
        return;
    }
    ptr = strchr(ptr, '=') + 1;

    headPointer eq = equations, prevEq = NULL;
    while (eq && eq->equNotation != notation) {
        prevEq = eq;
        eq = eq->hlink;
    }
    if (eq == NULL) {
        eq = createEquation(notation);
        if (prevEq) prevEq->hlink = eq;
        else equations = eq;
    } else {
        equPointer temp;
        while (eq->link) {
            temp = eq->link;
            eq->link = temp->link;
            free(temp);
        }
        eq->nodeNumber = 0;
    }

    int errorFlag = 0;
    int prevExpon = INT_MAX;
    int firstTerm = 1;

    while (*ptr) {
        while (*ptr == ' ' || *ptr == '+' || *ptr == '\n') ptr++;
        if (*ptr == '\0') break;

        int coef = 1;
        char variable = '0';
        int expon = 0;
        int sign = 1;

        if (*ptr == '-') {
            sign = -1;
            ptr++;
        } else if (*ptr == '+') {
            ptr++;
        }

        int coef_num = 0;
        int has_coef = 0;
        while (isdigit(*ptr)) {
            has_coef = 1;
            coef_num = coef_num * 10 + (*ptr - '0');
            ptr++;
        }
        if (has_coef) {
            coef = sign * coef_num;
        } else {
            coef = sign * 1;
        }

        if (isalpha(*ptr)) {
            variable = *ptr;
            ptr++;

            if (*ptr == '^') {
                ptr++;
                expon = 0;
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
        } else {
            variable = '0';
            expon = 0;
        }

        if (variable == '0' && expon != 0) {
            errorFlag = 1;
            break;
        }

        if (!firstTerm && expon > prevExpon) {
            errorFlag = 1;
            break;
        }

        prevExpon = expon;
        firstTerm = 0;

        insertNode(eq, coef, variable, expon);

        while (*ptr && *ptr != '+' && *ptr != '-' && *ptr != '\n') ptr++;
        if (*ptr == '\0' || *ptr == '\n') break;
    }

    if (errorFlag) {
        printf("ERROR\n");
        equPointer temp;
        while (eq->link) {
            temp = eq->link;
            eq->link = temp->link;
            free(temp);
        }
        eq->nodeNumber = 0;
        return;
    }

    // ✅ 即時印出該條 equation 的內容（格式統一）
    // ✅ 即時印出該條 equation 的內容（簡潔格式）
    printf("%c %d\n", eq->equNotation, eq->nodeNumber);
    equPointer p = eq->link;
    while (p) {
        printf("%d %d", p->coef, p->expon);
        if (p->variable != '0') {
            printf(" %c", p->variable);
        }
        printf("\n");
        p = p->link;
    }
}





void printAll() {
    if (equations == NULL) {
        printf("NO EQUATION\n");
        return;
    }

    headPointer eq = equations;

    eq = equations;
    while (eq) {
        printf("%c=", eq->equNotation);
        equPointer temp = eq->link;
        int first = 1;
        while (temp) {
            if (temp->coef > 0 && !first) {
                printf("+");
            }
            if (temp->coef == 1 && temp->variable != '0') {
                // skip
            } else if (temp->coef == -1 && temp->variable != '0') {
                printf("-");
            } else {
                printf("%d", temp->coef);
            }

            if (temp->variable != '0') {
                printf("%c", temp->variable);
                if (temp->expon > 1) {
                    printf("^%d", temp->expon);
                }
            }
            temp = temp->link;
            first = 0;
        }
        printf("\n");
        eq = eq->hlink;
    }
}

int main() {
    int command;
    while (1) {
        scanf("%d", &command);
        getchar();

        if (command == 1) {
            eread();
        } else if (command == 3) {
            printAll();
        } else if (command == 0) {
            printf("quit\n");
            break;
        }
    }
    return 0;
}