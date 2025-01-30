%include {

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif
#ifdef __GNUC__ // including clang
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

}

%start_symbol prog
%left PLUS MINUS.

prog ::= expr.
expr ::= expr PLUS expr.
expr ::= VALUE.
