%include {

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif
#ifdef __GNUC__ // including clang
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

}

%start_symbol prog
%left PLUS MINUS.
%left MUL DIV.
%right EXP.
%nonassoc COMMA LPAREN RPAREN.

prog ::= expr.
expr ::= expr PLUS expr.
expr ::= expr MINUS expr.
expr ::= expr MUL expr.
expr ::= expr DIV expr.
expr ::= expr EXP expr.
expr ::= LPAREN expr RPAREN.
expr ::= VALUE.
expr ::= func.
func ::= ID LPAREN param_seq RPAREN.
param_seq ::= param_seq COMMA param_seq.
param_seq ::= expr.
