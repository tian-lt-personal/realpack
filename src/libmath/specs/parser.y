%name RealMathParse
%token_type { real::math::parse::token* }
%token_prefix RMTOKEN_

%token ID PERC EQ DOT COMMA.

%include {

#include <expected>

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
  state->result = std::unexpected(parse::parse_error{});
}
%parse_accept {
  state->done = true;
}

%type compound_stmt { parse::pool_ptr<ast::compound_stmt> }
%type stmt { parse::pool_ptr<ast::stmt> }
%type eval_stmt { parse::pool_ptr<ast::eval_stmt> }
%type expr { parse::pool_ptr<ast::expr> }
%type atom { parse::pool_ptr<ast::atom> }
%type factor { parse::pool_ptr<ast::factor> }
%type expr_exp { parse::pool_ptr<ast::expr_exp> }
%type expr_sum { parse::pool_ptr<ast::expr_sum> }
%type expr_sub { parse::pool_ptr<ast::expr_sub> }
%type expr_mul { parse::pool_ptr<ast::expr_mul> }
%type expr_div { parse::pool_ptr<ast::expr_div> }
%type term { parse::pool_ptr<ast::term> }

%start_symbol doc

doc ::= compound_stmt(CSTMT). {
  state->result = ast::create_node(state, ast::doc{.root = CSTMT});
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

expr(EXPR) ::= expr_sum(SUM). {
  EXPR = ast::create_node(state, ast::expr{SUM});
}
expr(EXPR) ::= expr_sub(SUB). {
  EXPR = ast::create_node(state, ast::expr{SUB});
}
expr(EXPR) ::= term(TERM). {
  EXPR = ast::create_node(state, ast::expr{TERM});
}

expr_sum(SUM) ::= expr(SUMMAND) PLUS term(ADDEND). {
  ast::expr_sum sum{.summand = SUMMAND, .addend = ADDEND};
  SUM = ast::create_node(state, std::move(sum));
}
expr_sub(SUB) ::= expr(MINUEND) MINUS term(SUBTRAHEND). {
  ast::expr_sub sub{.minuend = MINUEND, .subtrahend = SUBTRAHEND};
  SUB = ast::create_node(state, std::move(sub));
}

term(TERM) ::= expr_mul(EXPR). {
  TERM = ast::create_node(state, ast::term{EXPR});
}
term(TERM) ::= expr_div(EXPR). {
  TERM = ast::create_node(state, ast::term{EXPR});
}
term(TERM) ::= factor(FAC). {
  TERM = ast::create_node(state, ast::term{FAC});
}

expr_mul(MUL) ::= term(MULTIPLICAND) MUL factor(MULTIPLIER). {
  ast::expr_mul mul{.multiplicand = MULTIPLICAND, .multiplier = MULTIPLIER};
  MUL = ast::create_node(state, std::move(mul));
}
expr_div(DIV) ::= term(DIVIDEND) DIV factor(DIVISOR). {
  ast::expr_div div{.dividend = DIVIDEND, .divisor = DIVISOR};
  DIV = ast::create_node(state, std::move(div));
}

factor(FAC) ::= expr_exp(EXPR). {
  FAC = ast::create_node(state, ast::factor{EXPR});
}
factor(FAC) ::= atom(ATOM). {
  FAC = ast::create_node(state, ast::factor{ATOM});
}

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
