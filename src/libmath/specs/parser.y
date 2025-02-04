%name RealMathParse
%token_type { real::math::parse::token* }
%token_prefix RMTOKEN_

%token ID PERC EQ DOT COMMA.

%include {

#include "parse.hpp"

#ifdef _MSC_VER
#pragma warning(disable: 4065)
#pragma warning(disable: 4100)
#pragma warning(disable: 4189)
#endif
#ifdef __GNUC__ // including clang
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#define YYMALLOCARGTYPE size_t

namespace parse = real::math::parse;
namespace ast = real::math::parse::ast;

}

%extra_argument { parse::parse_state* state }
%parse_failure {
  state->error = parse::parse_error{};
}
%parse_accept {
  state->done = true;
}

%type doc { parse::pool_ptr<ast::doc> }
%type compound_stmt { parse::pool_ptr<ast::compound_stmt> }
%type stmt { parse::pool_ptr<ast::stmt> }
%type eval_stmt { parse::pool_ptr<ast::eval_stmt> }
%type expr { parse::pool_ptr<ast::expr> }
%type atom { parse::pool_ptr<ast::atom> }
%type factor { parse::pool_ptr<ast::factor> }
%type expr_exp { parse::pool_ptr<ast::expr_exp> }

%start_symbol doc

doc(DOC) ::= compound_stmt(CSTMT). {
  DOC = ast::create_node(state, ast::doc{.root = CSTMT});
}
compound_stmt(GROUP) ::= compound_stmt(SUBGROUP) SEMICOL stmt(STMT). {
  ast::compound_stmt group;
  group.statements = SUBGROUP.get()->statements;
  group.statements.push_back(STMT);
  GROUP = ast::create_node(state, std::move(group));
}
compound_stmt(GROUP) ::= stmt(STMT). {
  ast::compound_stmt group;
  group.statements.push_back(STMT);
  GROUP = ast::create_node(state, std::move(group));
}

stmt(STMT) ::= eval_stmt(ESTMT). { STMT = ESTMT; }
eval_stmt(ESTMT) ::= expr(EXPR). { ESTMT = EXPR; }

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

expr_exp(EXPR) ::= factor(BASE) EXP atom(EXP). {
  ast::expr_exp exp{.base = BASE, .exp = EXP};
  EXPR = ast::create_node(state, std::move(exp));
}

atom(ATOM) ::= VALUE. { 
  //TODO: construct val properly.
  ast::value val{};
  ATOM = ast::create_node(state, ast::atom{val});
}
atom(ATOM) ::= LPAREN expr(EXPR) RPAREN. {
  ATOM = ast::create_node(state, ast::atom{EXPR});
}
