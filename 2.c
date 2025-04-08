#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Term {
    int coeff;
    int exponent;
    char var;
    struct Term *next;
} Term;

typedef struct Equation {
    char name;
    Term *terms;
    struct Equation *next;
} Equation;

Equation *global_equations = NULL;

// 解析單個項的完整實現（修正係數和變數處理）
void parse_term(const char *term_str, int *coeff, char *var, int *exponent) {
    *coeff = 1;
    *var = '\0';
    *exponent = 0;

    int sign = 1;
    const char *ptr = term_str;

    // 處理符號
    if (*ptr == '-') {
        sign = -1;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }

    // 提取係數
    int num = 0;
    int has_coeff = 0;
    while (*ptr && (isdigit(*ptr) || *ptr == '.')) { // 僅處理整數
        num = num * 10 + (*ptr - '0');
        has_coeff = 1;
        ptr++;
    }

    // 處理係數特殊情況
    if (has_coeff) {
        *coeff = sign * num;
    } else if (*ptr && !isalpha(*ptr)) {
        *coeff = sign * 1; // 無係數但有符號的情況
    } else {
        *coeff = sign * 1;
    }

    // 提取變數
    if (isalpha(*ptr)) {
        *var = *ptr;
        ptr++;
    } else {
        *var = '\0';
    }

    // 處理指數
    if (*ptr == '^') {
        ptr++;
        *exponent = atoi(ptr);
    } else if (*var != '\0') {
        *exponent = 1; // 默認指數為1
    } else {
        *exponent = 0; // 無變數則指數為0
    }

    // 常數項特殊處理
    if (*var == '\0') {
        *exponent = 0;
        if (!has_coeff) { // 純符號情況如"+1"
            *coeff = sign * 1;
        }
    }
}

// 加強版多項式解析（修正首項符號處理）
Term* parse_polynomial(const char* poly) {
    Term* head = NULL;
    Term** tail = &head;
    const char* start = poly;
    const char* current = poly;

    while (*current) {
        // 尋找項的邊界（修正首項無符號處理）
        if ((current != poly) && (*current == '+' || *current == '-')) {
            int length = current - start;
            char term_str[256];
            strncpy(term_str, start, length);
            term_str[length] = '\0';

            int coeff;
            char var;
            int exp;
            parse_term(term_str, &coeff, &var, &exp);

            Term* new_term = malloc(sizeof(Term));
            new_term->coeff = coeff;
            new_term->var = var;
            new_term->exponent = exp;
            new_term->next = NULL;
            *tail = new_term;
            tail = &(*tail)->next;

            start = current;
        }
        current++;
    }

    // 處理最後一項
    char term_str[256];
    strcpy(term_str, start);
    int coeff;
    char var;
    int exp;
    parse_term(term_str, &coeff, &var, &exp);

    Term* new_term = malloc(sizeof(Term));
    new_term->coeff = coeff;
    new_term->var = var;
    new_term->exponent = exp;
    new_term->next = NULL;
    *tail = new_term;

    return head;
}

// 完整方程式覆蓋邏輯（修正記憶體釋放）
void add_equation(Equation* new_eq) {
    Equation** ptr = &global_equations;
    while (*ptr) {
        if ((*ptr)->name == new_eq->name) {
            // 釋放舊方程式記憶體
            Equation* to_free = *ptr;
            *ptr = (*ptr)->next;
            
            Term* term = to_free->terms;
            while (term) {
                Term* next = term->next;
                free(term);
                term = next;
            }
            free(to_free);
            break;
        }
        ptr = &(*ptr)->next;
    }
    
    // 插入新方程式到開頭
    new_eq->next = global_equations;
    global_equations = new_eq;
}

// 修正存儲格式輸出
void print_equation_storage(const Equation* eq) {
    int count = 0;
    for (Term* t = eq->terms; t; t = t->next) count++;
    printf("%c %d\n", eq->name, count);

    for (Term* t = eq->terms; t; t = t->next) {
        if (t->var == '\0' || t->exponent == 0) {
            printf("%d %d\n", t->coeff, t->exponent);
        } else {
            printf("%d %d %c\n", t->coeff, t->exponent, t->var);
        }
    }
}

// 加強版方程式格式輸出
void print_equation_form(const Equation* eq) {
    printf("%c=", eq->name);
    int first = 1;
    
    for (Term* t = eq->terms; t; t = t->next) {
        if (t->coeff == 0) continue;

        // 處理符號
        if (!first) {
            putchar(t->coeff > 0 ? '+' : '-');
        } else if (t->coeff < 0) {
            putchar('-');
        }

        // 處理絕對值係數
        int abs_coeff = abs(t->coeff);
        
        // 輸出係數（特殊情況處理）
        if (t->exponent == 0) { // 常數項
            printf("%d", abs_coeff);
        } else {
            if (abs_coeff != 1 || t->exponent == 0) {
                printf("%d", abs_coeff);
            }
        }

        // 輸出變數和指數
        if (t->var != '\0' && t->exponent != 0) {
            putchar(t->var);
            if (t->exponent != 1) {
                printf("^%d", t->exponent);
            }
        }

        first = 0;
    }
    putchar('\n');
}

int main() {
    int cmd;
    while (scanf("%d", &cmd) == 1 && cmd != 0) {
        if (cmd == 1) {
            char input[1024];
            scanf("%s", input);
            
            // 解析方程式名稱和多項式
            char name = input[0];
            const char* poly = strchr(input, '=') + 1;
            
            Term* terms = parse_polynomial(poly);
            Equation* eq = malloc(sizeof(Equation));
            eq->name = name;
            eq->terms = terms;
            eq->next = NULL;
            
            add_equation(eq);
            print_equation_storage(eq);
        } else if (cmd == 2) {
            char name;
            scanf(" %c", &name);
            
            Equation* found = NULL;
            for (Equation* e = global_equations; e; e = e->next) {
                if (e->name == name) {
                    found = e;
                    break;
                }
            }
            
            if (found) {
                print_equation_form(found);
            } else {
                printf("NO EQUATION\n");
            }
        }
    }
    printf("quit\n");
    
    // 清理所有記憶體
    while (global_equations) {
        Equation* next_eq = global_equations->next;
        Term* term = global_equations->terms;
        while (term) {
            Term* next_term = term->next;
            free(term);
            term = next_term;
        }
        free(global_equations);
        global_equations = next_eq;
    }
    
    return 0;
}