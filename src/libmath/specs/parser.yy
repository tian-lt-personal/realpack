%include {

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif
#ifdef __GNUC__ // including clang
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

}

%start_symbol tu 

tu ::= compound_stmt.
compound_stmt ::= compound_stmt SEMICOL stmt.
compound_stmt ::= stmt.

stmt ::= eval_stmt.
eval_stmt ::= expr.

%left PLUS MINUS.
%left MUL DIV.
%right EXP.

expr ::= expr_sum.
expr ::= expr_sub.
expr ::= term.

expr_sum ::= expr PLUS term.
expr_sub ::= expr MINUS term.

term ::= expr_mul.
term ::= expr_div.
term ::= factor.

expr_mul ::= term MUL factor.
expr_div ::= term DIV factor.

factor ::= expr_exp.
factor ::= atom.

expr_exp ::= factor EXP atom.

atom ::= VALUE.
atom ::= LPAREN expr RPAREN.
