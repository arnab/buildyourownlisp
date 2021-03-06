#include "mpc.h"
#include <editline/readline.h>
#include <string.h>
#include <math.h>

long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  if (strcmp(op, "%") == 0) { return x % y; }
  if (strcmp(op, "^") == 0) { return pow(x, y); }
  if (strcmp(op, "min") == 0) { return x <= y ? x : y; }
  if (strcmp(op, "max") == 0) { return x >= y ? x : y; }
  fprintf(stderr, "WARNING: unknown operator '%s'\n", op);
  return 0;
}

long eval(mpc_ast_t* t) {
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  char* op = t->children[1]->contents;
  long x   = eval(t->children[2]);

  for (int i = 3; strstr(t->children[i]->tag, "expr"); i++) {
    x = eval_op(x, op, eval(t->children[i]));
  }

  return x;
}

long count_leaves(mpc_ast_t* t) {
  long max = t->children_num;

  if (max == 0) {
    return 1;
  } else {
    int total = 0;
    for (int i = 0; i < max; i++) {
      total += count_leaves(t->children[i]);
    }
    return total;
  }
}

int main() {
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");
  mpc_result_t r;

  mpca_lang(MPCA_LANG_DEFAULT, "                                \
            number   : /-?[0-9]+(\\.[0-9]+)?/;                  \
            operator : '+' | '-' | '*' | '/' | '%' | '^' | /[a-zA-Z][a-zA-Z0-9-]*/; \
            expr     : <number> | '(' <operator> <expr>+ ')';   \
            lispy    : /^/ <operator> <expr>+ /$/;              ",
            Number, Operator, Expr, Lispy);

  while (1) {
    char * input = readline("lispy> ");
    if (!input) break;
    add_history(input);

    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      mpc_ast_print(r.output);

      long result = eval(r.output);
      printf("= %li\n", result);
      printf("# %li\n", count_leaves(r.output));

      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }


    free(input);
  }

  mpc_cleanup(4, Number, Operator, Expr, Lispy);
}
