/* -*-c++-*- */

%{
/*
 * parse_netlist.y - parser for the Qucs netlist
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: parse_netlist.y,v 1.13 2004/12/07 22:33:31 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YYERROR_VERBOSE 42
#define YYDEBUG 1
#define YYMAXDEPTH 1000000

#include "check_netlist.h"
#include "logging.h"
#include "equation.h"

%}

%name-prefix="netlist_"

%token InvalidCharacter
%token Identifier
%token Assign
%token ScaleOrUnit
%token Eol
%token Eqn
%token DefSub
%token EndSub
%token REAL
%token IMAG
%token COMPLEX
%token Character
%token STRING

%right '='
%left '-' '+'
%left '*' '/' '%'
%left NEG     /* unary negation */
%left POS     /* unary non-negation */
%right '^'

%union {
  char * ident;
  char * str;
  double d;
  char chr;
  struct definition_t * definition;
  struct definition_t * subcircuit;
  struct node_t * node;
  struct pair_t * pair;
  struct value_t * value;
  struct {
    double r;
    double i;
  } c;
  eqn::node * eqn;
  eqn::constant * con;
  eqn::reference * ref;
  eqn::application * app;
  eqn::assignment * assign;
}

%type <ident> Identifier Assign
%type <str> ScaleOrUnit
%type <d> REAL IMAG
%type <c> COMPLEX
%type <chr> Character
%type <str> STRING
%type <definition> DefinitionLine ActionLine DefBody DefBodyLine
%type <subcircuit> DefBegin SubcircuitBody
%type <node> IdentifierList
%type <pair> PairList
%type <value> PropertyValue ValueList Value
%type <eqn> EquationList Expression ExpressionList
%type <assign> Equation
%type <con> Constant Range
%type <ref> Reference
%type <app> Application

%%

Input: /* nothing */
  | InputLine Input
;

InputLine:
  SubcircuitBody   { /* chain definition root */
    $1->next = definition_root;
    definition_root = $1;
  }
  | EquationLine   { /* nothing to do here */ }
  | ActionLine     { /* chain definition root */
    $1->next = definition_root;
    definition_root = $1;
  }
  | DefinitionLine { /* chain definition root */
    $1->next = definition_root;
    definition_root = $1;
  }
  | Eol            { }
;

ActionLine:
  '.' Identifier ':' Identifier PairList Eol { 
    $$ = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    $$->action = PROP_ACTION;
    $$->type = $2;
    $$->instance = $4;
    $$->pairs = $5;
    $$->line = netlist_lineno;
  }
;

DefinitionLine:
  Identifier ':' Identifier IdentifierList PairList Eol { 
    $$ = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    $$->action = PROP_COMPONENT;
    $$->type = $1;
    $$->instance = $3;
    $$->nodes = $4;
    $$->pairs = $5;
    $$->line = netlist_lineno;
  }
;

IdentifierList: /* nothing */ { $$ = NULL; }
  | Identifier IdentifierList {
    $$ = (struct node_t *) calloc (sizeof (struct node_t), 1);
    $$->node = $1;
    $$->next = $2;
  }
;

PairList: /* nothing */ { $$ = NULL; }
  | Assign Value PairList {
    $$ = (struct pair_t *) calloc (sizeof (struct pair_t), 1);
    $$->key = $1;
    $$->value = $2;
    $$->next = $3;
  }    
;

Value:
  PropertyValue {
    $$ = $1;
  }
  | '"' PropertyValue '"' {
    $$ = $2;
  }

PropertyValue:
  REAL {
    $$ = create_value ();
    $$->value = $1;
  }
  | REAL ScaleOrUnit {
    $$ = create_value ();
    $$->value = $1;
    $$->scale = $2;
  }
  | REAL ScaleOrUnit ScaleOrUnit {
    $$ = create_value ();
    $$->value = $1;
    $$->scale = $2;
    $$->unit = $3;
  }
  | Identifier {
    $$ = create_value ();
    $$->ident = $1;
  }
  | '[' ValueList ']' {
    $$ = $2;
  }
;

ValueList: /* nothing */ { $$ = NULL; }
  | REAL {
    struct value_t * here = create_value ();
    here->value = $1;
    here->next = $$;
    $$ = here;
  }
  | REAL ';' ValueList {
    struct value_t * here = create_value ();
    here->value = $1;
    here->next = $3;
    $$ = here;
  }
;

EquationLine:
  Eqn ':' Identifier Equation EquationList Eol {
    $4->setInstance ($3);
    $4->setNext (eqn::equations);
    $4->applyInstance ();
    eqn::equations = $4;
  }
;

EquationList: /* nothing */ { }
  | Equation EquationList { 
    $1->setNext (eqn::equations);
    eqn::equations = $1;
  }
;

Equation:
  Assign '"' Expression '"' {
    $$ = new eqn::assignment ();
    $$->result = strdup ($1);
    $$->body = $3;
  }
;

Expression:
    Constant {
    $$ = $1;
  }
  | Reference {
    $$ = $1;
  }
  | Application {
    $$ = $1;
  }
  | '(' Expression ')' {
    $$ = $2;
  }
;

Constant:
    REAL { 
    $$ = new eqn::constant (eqn::TAG_DOUBLE);
    $$->d = $1;
  }
  | IMAG {
    $$ = new eqn::constant (eqn::TAG_COMPLEX);
    $$->c = new complex (0.0, $1);
  }
  | Character {
    $$ = new eqn::constant (eqn::TAG_CHAR);
    $$->chr = $1;
  }
  | STRING {
    $$ = new eqn::constant (eqn::TAG_STRING);
    $$->s = $1;
  }
  | Range {
    $$ = new eqn::constant (eqn::TAG_RANGE);
  }
;

Range:
  REAL ':' {
  }
  | ':' REAL {
  }
  | REAL ':' REAL {
  }
  | ':' {
  }
;

Reference:
  Identifier {
    $$ = new eqn::reference ();
    $$->n = strdup ($1);
  }
;

Application:
    Identifier '(' ExpressionList ')' {
    $$ = new eqn::application ();
    $$->n = strdup ($1);
    $$->nargs = $3->count ();
    $$->args = $3;
    eqn::expressions = NULL;
  }
  | Reference '[' ExpressionList ']' {
    $$ = new eqn::application ();
    $$->n = strdup ("array");
    $$->nargs = 1 + $3->count ();
    $1->setNext ($3);
    $$->args = $1;
    eqn::expressions = NULL;
  }
  | Expression '+' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("+");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression '-' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("-");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression '*' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("*");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression '/' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("/");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression '%' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("%");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | '+' Expression %prec POS {
    $$ = new eqn::application ();
    $$->n = strdup ("+");
    $$->nargs = 1;
    $$->args = $2;
  }
  | '-' Expression %prec NEG {
    $$ = new eqn::application ();
    $$->n = strdup ("-");
    $$->nargs = 1;
    $$->args = $2;
  }
  | Expression '^' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("^");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
;

ExpressionList: /* nothing */ { $$ = eqn::expressions = NULL; }
  | Expression {
    $1->setNext (eqn::expressions);
    $$ = eqn::expressions = $1;
  }
  | Expression ',' ExpressionList {
    $1->setNext (eqn::expressions);
    $$ = eqn::expressions = $1;
  }
;

SubcircuitBody:
  DefBegin DefBody DefEnd { /* a full subcircuit definition found */
    $1->sub = $2;
    $$ = $1;
    $2 = NULL;
  }
;

DefBegin:
  DefSub Identifier IdentifierList Eol {
    /* create subcircuit definition right here */
    $$ = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    $$->type = strdup ("Def");
    $$->instance = $2;
    $$->nodes = $3;
    $$->action = PROP_ACTION;
    $$->line = netlist_lineno;
  }
;

DefBody: /* nothing */ { $$ = NULL; }
  | DefBodyLine DefBody { /* chain definitions here */
    if ($1) {
      $1->next = $2;
      $$ = $1;
    }
    else {
      $$ = $2;
    }
  }
;

DefEnd:
  EndSub Eol { /* nothing to do */ }
;

DefBodyLine:
  DefinitionLine { /* chain definitions here */
    $1->next = $$;
    $$ = $1;
  }
  | SubcircuitBody { /* do nothing here, see subcircuit rule */ }
  | Eol {
    $$ = NULL;
  }
;


%%

int netlist_error (char * error) {
  logprint (LOG_ERROR, "line %d: %s\n", netlist_lineno, error);
  return 0;
}
