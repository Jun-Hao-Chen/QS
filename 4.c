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
    newEq->equNotation = notation;
    newEq->nodeNumber = 0;
    newEq->link = NULL;
    newEq->hlink = NULL;
    return newEq;
}

void insertNode(headPointer eq, int coef, char variable, int expon) {
    if (!eq) return;
    equPointer newNode = (equPointer)malloc(sizeof(equNode));
    newNode->coef = coef;
    newNode->variable = variable;
    newNode->expon = expon;
    newNode->link = NULL;

    equPointer *curr = &(eq->link);
    while (*curr && ((*curr)->expon > expon || ((*curr)->expon == expon && (*curr)->variable < variable))) {
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
        }

        int coef_num = 0;
        int has_coef = 0;
        while (isdigit(*ptr)) {
            has_coef = 1;
            coef_num = coef_num * 10 + (*ptr - '0');
            ptr++;
        }
        coef = has_coef ? sign * coef_num : sign * 1;

        if (isalpha(*ptr)) {
            variable = *ptr++;
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

    printf("%c %d\n", eq->equNotation, eq->nodeNumber);
    equPointer p = eq->link;
    while (p) {
        printf("%d %d", p->coef, p->expon);
        if (p->variable != '0') printf(" %c", p->variable);
        printf("\n");
        p = p->link;
    }
}

void printAll() {
    if (!equations) {
        printf("NO EQUATION\n");
        return;
    }

    headPointer eq = equations;
    while (eq) {
        printf("%c=", eq->equNotation);
        equPointer temp = eq->link;
        int first = 1;
        while (temp) {
            if (temp->coef > 0 && !first) printf("+");
            if (temp->coef == 1 && temp->variable != '0') ;
            else if (temp->coef == -1 && temp->variable != '0') printf("-");
            else printf("%d", temp->coef);
            if (temp->variable != '0') {
                printf("%c", temp->variable);
                if (temp->expon > 1) printf("^%d", temp->expon);
            }
            temp = temp->link;
            first = 0;
        }
        printf("\n");
        eq = eq->hlink;
    }
}

headPointer findEquation(char notation) {
    headPointer eq = equations;
    while (eq) {
        if (eq->equNotation == notation) return eq;
        eq = eq->hlink;
    }
    return NULL;
}

headPointer addEquations(headPointer a, headPointer b, char resultNotation) {
    headPointer result = createEquation(resultNotation);
    equPointer pa = a->link, pb = b->link;
    while (pa && pb) {
        if (pa->expon > pb->expon || (pa->expon == pb->expon && pa->variable < pb->variable)) {
            insertNode(result, pa->coef, pa->variable, pa->expon);
            pa = pa->link;
        } else if (pa->expon < pb->expon || (pa->expon == pb->expon && pa->variable > pb->variable)) {
            insertNode(result, pb->coef, pb->variable, pb->expon);
            pb = pb->link;
        } else {
            insertNode(result, pa->coef + pb->coef, pa->variable, pa->expon);
            pa = pa->link;
            pb = pb->link;
        }
    }
    while (pa) {
        insertNode(result, pa->coef, pa->variable, pa->expon);
        pa = pa->link;
    }
    while (pb) {
        insertNode(result, pb->coef, pb->variable, pb->expon);
        pb = pb->link;
    }
    return result;
}

void handleAddition() {
    char input[100];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    char resultNotation = '\0', left = '\0', right = '\0';
    int matched = sscanf(input, "%c=%c+%c", &resultNotation, &left, &right);
    if (matched == 3) {
        headPointer leftEq = findEquation(left);
        headPointer rightEq = findEquation(right);
        if (!leftEq) {
            printf("NO EQUATION %c\n", left);
            return;
        }
        if (!rightEq) {
            printf("NO EQUATION %c\n", right);
            return;
        }
        headPointer sum = addEquations(leftEq, rightEq, resultNotation);

        headPointer prev = NULL, curr = equations;
        while (curr && curr->equNotation != resultNotation) {
            prev = curr;
            curr = curr->hlink;
        }

        if (curr) {
            equPointer temp;
            while (curr->link) {
                temp = curr->link;
                curr->link = temp->link;
                free(temp);
            }
            curr->nodeNumber = 0;
            equPointer cp = sum->link;
            while (cp) {
                insertNode(curr, cp->coef, cp->variable, cp->expon);
                cp = cp->link;
            }
            free(sum);
        } else {
            if (prev) prev->hlink = sum;
            else equations = sum;
        }

        printf("%c=", resultNotation);
        equPointer p = findEquation(resultNotation)->link;
        int first = 1;
        while (p) {
            if (p->coef > 0 && !first) printf("+");
            if (p->coef == 1 && p->variable != '0') ;
            else if (p->coef == -1 && p->variable != '0') printf("-");
            else printf("%d", p->coef);
            if (p->variable != '0') {
                printf("%c", p->variable);
                if (p->expon > 1) printf("^%d", p->expon);
            }
            first = 0;
            p = p->link;
        }
        printf("\n");
        return;
    }

    matched = sscanf(input, "%c+%c", &left, &right);
    if (matched == 2) {
        headPointer leftEq = findEquation(left);
        headPointer rightEq = findEquation(right);
        if (!leftEq) {
            printf("NO EQUATION %c\n", left);
            return;
        }
        if (!rightEq) {
            printf("NO EQUATION %c\n", right);
            return;
        }

        headPointer sum = addEquations(leftEq, rightEq, '\0');
        printf("%c+%c=", left, right);
        equPointer p = sum->link;
        int first = 1;
        while (p) {
            if (p->coef > 0 && !first) printf("+");
            if (p->coef == 1 && p->variable != '0') ;
            else if (p->coef == -1 && p->variable != '0') printf("-");
            else printf("%d", p->coef);
            if (p->variable != '0') {
                printf("%c", p->variable);
                if (p->expon > 1) printf("^%d", p->expon);
            }
            first = 0;
            p = p->link;
        }
        printf("\n");

        equPointer del = sum->link;
        while (del) {
            equPointer temp = del;
            del = del->link;
            free(temp);
        }
        free(sum);
        return;
    }

    printf("ERROR\n");
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
        } else if (command == 4) {
            handleAddition();
        } else if (command == 0) {
            printf("quit\n");
            break;
        }
    }
    return 0;
}
