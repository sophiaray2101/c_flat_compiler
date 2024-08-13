# C-flat Compiler

This project is a simplified C-flat compiler designed to demonstrate the process of compiling and executing a programming language!

## Overview

The `c_flat_compiler` is a compiler project that converts code from a subset of the C-flat language into X86 assembly. This project provides a detailed example of a compuler design, covering numerous stages such as lexical analysis, parsing, type checking, lowering, and code generation.

## Key Features

- **Lexical Analysis**: This stage tokenizes the source code into meaningful symbols.
- **Parsing**: Analyzes the tokenized input and builds an abstract syntax tree.
- **Type Checking**: Checks for semantic errors and enforces the C-flat language rules (shown at bottom of the page).
- **Lower Level Intermediate Represtation**: Simplifies code to make it easier to read and optimize.
- **Code Generation**: Translates the lower level representation into machine code.
- **Error Handling**: Provides detailed error messages and debugging info.

## Usage

Clone the repo using 'git clone' and compile and execute using the following command:

```bash
make
./codegen <source_file.txt>
```

## About the Language

<details>
<summary>Click to expand Token Info</summary>

```
TOKEN        REGEX
---------------------------------------------
Num          [0-9]+
Id           [a-zA-Z][a-zA-Z0-9]*
Int          int
Struct       struct
Nil          nil
Break        break
Continue     continue
Return       return
If           if
Else         else
While        while
New          new
Let          let
Extern       extern
Fn           fn
Address      &
Colon        :
Semicolon    ;
Comma        ,
Underscore   _
Arrow        ->
Plus         +
Dash         -
Star         *
Slash        /
Equal        ==
NotEq        !=
Lt           <
Lte          <=
Gt           >
Gte          >=
Dot          .
Gets         =
OpenParen    (
CloseParen   )
OpenBracket  [
CloseBracket ]
OpenBrace    {
CloseBrace   }
Error        <no description>
```

Here are the regular expression descriptions of whitespace and comments (in the C-style comment regexes `*` is used for a literal asterisk and `⋆` is used for kleene star):

```
        whitespace = (' ' | '\t' | '\n' | '\r')+
       c++-comment = //[^'\n']*
         c-comment = /*([^*]|*⋆[^*/])⋆*+/
unclosed-c-comment = /*([^*]|(*+[^*/]))⋆*⋆
```
</details>

<details>
<summary>Click to expand Language Grammar</summary>

```c
# LL(1) cflat grammar

# cflat types; `&` is right-associative.
   type ::= `&`* type_ad
type_ad ::= `int`
          | id
          | `(` type_op
type_op ::= `)` type_ar
          | type type_fp
type_fp ::= `)` type_ar?
          | (`,` type)+ `)` type_ar
type_ar ::= `->` rettyp

# function type (used in extern declarations).
funtype ::= `(` (type (`,` type)*)? `)` `->` rettyp
 rettyp ::= type 
          | `_`

# cflat program.
 program ::= toplevel+
toplevel ::= glob      # global variable declaration
           | typedef   # struct type definition
           | extern    # external function declaration
           | fundef    # function definition

# global variable declaration.
glob ::= `let` decls `;`

# struct type definition.
typdef ::= `struct` id `{` decls `}`

# variable / field declaration(s).
 decl ::= id `:` type
decls ::= decl (`,` decl)*

# external function declaration.
extern ::= `extern` id `:` funtype `;`

# function definition.
fundef ::= `fn` id `(` decls? `)` `->` rettyp `{` let* stmt+ `}`

# local variable declaration / initialization.
let ::= `let` decl (`=` exp)? (`,` decl (`=` exp)?)* `;`

# statement.
stmt ::= cond               # conditional
       | loop               # loop
       | assign_or_call `;` # assignment or function call
       | `break` `;`        # break out of a loop
       | `continue` `;`     # continue to next iteration of loop
       | `return` exp? `;`  # return from function

# a conditional with optional else clause.
cond ::= `if` exp block (`else` block)?

# loops: 'while' loop.
loop ::= `while` exp block 

# a block of statements.
block ::= `{` stmt* `}`

# assignment or function call.
assign_or_call ::= lval gets_or_args
  gets_or_args ::= `=` rhs
                 | `(` args? `)`
           rhs ::= exp 
                 | `new` type exp?

# lvalues for assignments and call statements. for lval `*` is right-
# associative, access is left-associative; access binds tighter than `*`, 
# e.g., `*id[2]` means `*(id[2])` and to get `(*id)[2]` we must write 
# `id[0][2]`.
  lval ::= `*`* id access*
access ::= `[` exp `]` 
         | `.` id

# call arguments.
args ::= exp (`,` exp)*

# expression; all binary operators and exp_ac are left-associative, all unary
# operators are right-associative. exp_ac binds tighter than `*`, e.g.,
# `*id[2]` means `*(id[2])`; to get `(*id)[2]` we need to write `id[0][2]`.
   exp ::= exp_p4 (binop_p3 exp_p4)*
exp_p4 ::= exp_p3 (binop_p2 exp_p3)*
exp_p3 ::= exp_p2 (binop_p1 exp_p2)*
exp_p2 ::= unop* exp_p1
exp_p1 ::= num
         | `nil`
         | `(` exp `)`
         | id exp_ac*
exp_ac ::= `[` exp `]`
         | `.` id
         | `(` args? `)`

# unary operators.
unop ::= `*`
       | `-`

# binary operators (from highest to lowest precedence).
binop_p1 ::= `*` | `/`
binop_p2 ::= `+` | `-`
binop_p3 ::= `==` | `!=` | `<` | `<=` | `>` | `>=`
```
</details>

## Contact Me
For any inquiries, please reach out at <sophiaray.2101@gmail.com>