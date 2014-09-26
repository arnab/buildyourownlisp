struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

typedef lval*(*lbuiltin)(lenv*, lval*);
struct lval {
  int type;

  // basic
  long num;
  char* err;
  char* sym;

  // function
  lbuiltin builtin;
  lenv* env;
  lval* formals;
  lval* body;

  // expression
  int count; // count and pointer to list of lval*
  struct lval** cell;
};

struct lenv {
  lenv* par;
  int count;
  char** syms;
  lval** vals;
};

enum { LVAL_NUM, LVAL_BOOL, LVAL_ERR, LVAL_SYM, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR }; // lval_types

char* ltype_name(int t);
lenv* lenv_new(void);
void lenv_del(lenv* e);
lval* lenv_get(lenv* e, lval* k);
void lenv_put(lenv* e, lval* k, lval* v);
void lenv_def(lenv* e, lval* k, lval* v);
lenv* lenv_copy(lenv* e);
lval* lval_abstract();
lval* lval_num(long x);
lval* lval_bool(int x);
lval* lval_err(char* fmt, ...);
lval* lval_sym(char* x);
lval* lval_sexpr(void);
lval* lval_qepxr(void);
lval* lval_builtin_function(lbuiltin builtin);
lval* lval_lambda(lval* formals, lval* body);
void lval_del(lval* v);
lval* lval_add(lval* v, lval* x);
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
lval* lval_copy(lval* v);
void lval_expr_print(lval* v, char open, char close);
void lval_print(lval* v);
void lval_println(lval* v);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
lval* builtin_op(lenv* e, lval* a, char* op);
lval* lval_eval(lenv* e, lval* v);
lval* lval_eval_sexpr(lenv* e, lval* y);
lval* lval_call(lenv* e, lval* f, lval* a);
void lenv_add_builtin(lenv* e, char* name, lbuiltin func);
void lenv_add_builtin_constant(lenv* e, char* name, lval* val);
void lenv_add_builtins(lenv* e);
lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);
lval* builtin_head(lenv* e, lval* a);
lval* builtin_tail(lenv* e, lval* a);
lval* builtin_list(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);
lval* builtin_join(lenv* e, lval* a);
lval* builtin_cons(lenv* e, lval* a);
lval* lval_join(lval* x, lval* y);
lval* builtin_len(lenv* e, lval* a);
lval* builtin_init(lenv* e, lval* a);
lval* builtin_var(lenv* e, lval* a, char* func);
lval* builtin_def(lenv* e, lval* a);
lval* builtin_put(lenv* e, lval* a);
lval* builtin_lambda(lenv* e, lval* a);
lval* builtin_ord(lenv* e, lval* a, char* op);
lval* builtin_gt(lenv* e, lval* a);
lval* builtin_gte(lenv* e, lval* a);
lval* builtin_lt(lenv* e, lval* a);
lval* builtin_lte(lenv* e, lval* a);
int lval_eq(lval* a, lval* b);
lval* builtin_cmp(lenv* e, lval* a, char* op);
lval* builtin_eq(lenv* e, lval* a);
lval* builtin_ne(lenv* e, lval* a);
lval* builtin_if(lenv* e, lval* a);

#define LASSERT(args, cond, err) if (!(cond)) { lval_del(args); return lval_err(err); }

#define LASSERT_NUM_ARGS(args, function, num, expected_num) { \
  if (num != expected_num) { \
    char* format = "Function '%s' passed wrong type. Got %d, expected %d."; \
    lval* err = lval_err(format, function, num, expected_num); \
    lval_del(args); \
    return err; \
  } \
};

#define LASSERT_TYPE(args, function, actual, expected_type) { \
  if (actual != expected_type) { \
    char* format = "Function '%s' passed wrong type. Got %s, expected %s."; \
    lval* err = lval_err(format, function, ltype_name(actual), ltype_name(expected_type)); \
    lval_del(args); \
    return err; \
  } \
};
