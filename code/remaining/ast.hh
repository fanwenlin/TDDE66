#ifndef __AST_HH__
#define __AST_HH__

#include "symtab.hh"
#include "quads.hh"


/* This little ascii diagram describes the inheritance structure of the
   AST classes:

AST_NODE
  |
  +- AST_ELSIF
  |
  +- AST_STMT_LIST
  |
  +- AST_EXPR_LIST
  |
  +- AST_ELSIF_LIST
  |
  +- AST_FUNCTIONHEAD
  |
  +- AST_PROCEDUREHEAD
  |
  +- AST_STATEMENT
  |  |
  |  +- AST_PROCEDURECALL
  |  |
  |  +- AST_ASSIGN
  |  |
  |  +- AST_WHILE
  |  |
  |  +- AST_IF
  |  |
  |  +- AST_RETURN
  |
  +- AST_EXPRESSION
     |
     +- AST_LVALUE
     |  |
     |  +- AST_ID
     |  |
     |  +- AST_INDEXED
     |
     +- AST_BINARYRELATION
     |  |
     |  +- AST_EQUAL
     |  |
     |  +- AST_NOTEQUAL
     |  |
     |  +- AST_LESSTHAN
     |  |
     |  +- AST_GREATERTHAN
     |
     +- AST_BINARYOPERATION
     |  |
     |  +- AST_ADD
     |  |
     |  +- AST_SUB
     |  |
     |  +- AST_OR
     |  |
     |  +- AST_AND
     |  |
     |  +- AST_MULT
     |  |
     |  +- AST_DIVIDE
     |  |
     |  +- AST_IDIV
     |  |
     |  +- AST_MOD
     |
     +- AST_FUNCTIONCALL
     |
     +- AST_UMINUS
     |
     +- AST_NOT
     |
     +- AST_INTEGER
     |
     +- AST_REAL
     |
     +- AST_CAST
     |
     +- AST_PARAMETER
*/

/* The various types of AST node tags that can appear. If C++ had had an
   instanceof operator like Java, we wouldn't need this... These tags are
   set in the node constructor and should never be changed afterwards. */
enum ast_node_types {
  AST_NODE,
  AST_STATEMENT,
  AST_EXPRESSION,
  AST_BINARYOPERATION,
  AST_BINARYRELATION,
  AST_LVALUE,
  AST_EXPR_LIST,
  AST_STMT_LIST,
  AST_ELSIF_LIST,
  AST_ID,
  AST_INDEXED,
  AST_ADD,
  AST_SUB,
  AST_OR,
  AST_AND,
  AST_MULT,
  AST_DIVIDE,
  AST_IDIV,
  AST_MOD,
  AST_EQUAL,
  AST_NOTEQUAL,
  AST_LESSTHAN,
  AST_GREATERTHAN,
  AST_PROCEDURECALL,
  AST_ASSIGN,
  AST_WHILE,
  AST_IF,
  AST_RETURN,
  AST_FUNCTIONCALL,
  AST_UMINUS,
  AST_NOT,
  AST_ELSIF,
  AST_INTEGER,
  AST_REAL,
  AST_FUNCTIONHEAD,
  AST_PROCEDUREHEAD,
  AST_PARAMETER,
  AST_CAST
};
typedef enum ast_node_types ast_node_type;

/* Needed so we can refer to quad_list& as arguments. See below. */
class quad_list;

/* Class stubs to allow referencing the classes below before they're declared.
   See below. */
class ast_binaryoperation;
class ast_binaryrelation;
class ast_stmt_list;
class ast_id;
class ast_integer;
class ast_real;
class ast_cast;

class quad_list;

/*** Abstract classes ***/

/*! The superclass of all other AST nodes. It is essentially an empty
 * class, holding only position information, a tag (to help identify the
 * node type when downcasting is needed), and methods for printing AST nodes;
 * all of which are things common to all nodes.
 */
class ast_node {
protected:
  // Used for AST printing.
  static int indent_level;
  static bool branches[10000];

  // All these methods are concerned with printing the AST.
  void indent(ostream &);

  void indent_more();

  void indent_less();

  void begin_child(ostream &);

  void end_child(ostream &);

  void last_child(ostream &);

  virtual void print(ostream &);

  virtual void xprint(ostream &, string);

public:
  //! Holds line and column number for this node.
  position_information *pos;

  /*! Describes what kind of node this is. We need to be able to check this
      in a convenient way during AST optimization and C++ does not support
      reflective programming.
   */
  ast_node_type tag;

  // Constructor.
  ast_node(position_information *);

  /*! Perform type checking. See semantic.cc for the method bodies.
   * Note that it's an error to call type_check in this class. It should
   * only be called in the concrete AST nodes, see below.
   */
  virtual sym_index type_check();

  /*! Perform optimization. See optimize.cc for the method bodies.
   * Just as with type checking, it's an error to call optimize in this
   * class. It should only be called in the concrete AST nodes.
   */
  virtual void optimize();

  /*! Generate quads. See quads.cc for the method bodies. Like type checking,
   * generate_quads should only be called in concrete AST nodes.
   */
  virtual sym_index generate_quads(quad_list &) = 0;

  //! Allow an AST node to be sent to an outstream for printing.
  friend ostream &operator<<(ostream &, ast_node *);
};

/*! The superclass for all DIESEL constructs that do not return a value. */
class ast_statement : public ast_node {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_statement(position_information *);

  // It's an error if these methods are called. See the derived classes.
  virtual sym_index type_check();

  virtual void optimize();

  virtual sym_index generate_quads(quad_list &) = 0;
};

/*! The superclass for all DIESEL constructs that do return a value.
 * It has a #type attribute, which denotes the type of its value
 * (#real_type, #integer_type, or #void_type).
 */
class ast_expression : public ast_node {
protected:
  virtual void print(ostream &);

public:
  //! The return type of this expression.
  sym_index type;

  // Constructors.
  ast_expression(position_information *);

  ast_expression(position_information *, sym_index);

  // It's an error if these methods are called. See the derived classes.
  virtual sym_index type_check();

  virtual void optimize();

  virtual sym_index generate_quads(quad_list &) = 0;

  // Used for safe downcasting. We could provide a mechanism to safely
  // downcast ALL ast nodes... But these ones are the only ones we'll need
  // in this lab course. They will be used during AST optimization.
  // Note: It is not illegal to call these methods in other nodes than the
  // ast_integer, ast_real - but those nodes are the only ones which will
  // return non-NULL values.
  virtual ast_integer *get_ast_integer() { return NULL; }

  virtual ast_real *get_ast_real() { return NULL; }

  virtual ast_id *get_ast_id() { return NULL; }

  virtual ast_cast *get_ast_cast() { return NULL; }

  // This, however, is very illegal. It's also only used in optimize.cc, to
  // allow us to downcast an ast_expression to an ast_binaryoperation.
  // See the comments in that file for more information.
  virtual ast_binaryoperation *get_ast_binaryoperation() {
    fatal("Illegal downcast to ast_binaryoperation from ast_expression");
    return NULL;
  }

  virtual ast_binaryrelation *get_ast_binaryrelation() {
    fatal("Illegal downcast to ast_binaryrelation from ast_expression");
    return NULL;
  }
};

/*! Base class for all binary relation nodes. ``a < b``, etc.

   Always returns a integer value, where 0 (zero) means *false* and
   everything else means *true*.

   Note: the left and right operands are defined here instead of in the
   individual subclasses, making those rather trivial. */
class ast_binaryrelation : public ast_expression {
protected:
  virtual void print(ostream &);

  virtual void xprint(ostream &, string);

public:
  //! Left child of the operation.
  ast_expression *left;
  //! Right child of the operation.
  ast_expression *right;

  // Constructor.
  ast_binaryrelation(position_information *, ast_expression *,
                     ast_expression *);

  // It's an error if this method is called. See the derived classes.
  virtual sym_index type_check();

  virtual void optimize();

  virtual sym_index generate_quads(quad_list &) = 0;

  // Needed for safe downcasting.
  virtual ast_binaryrelation *get_ast_binaryrelation() { return this; }
};

/*! Base class for all binary operation nodes. ``a + b``, etc.
   Note: the left and right operands are defined here instead of in the
   individual subclasses, making those rather trivial. */
class ast_binaryoperation : public ast_expression {
protected:
  virtual void print(ostream &);

  virtual void xprint(ostream &, string);

public:
  //! Left child of the operation.
  ast_expression *left;
  //! Right child of the operation.
  ast_expression *right;

  // Constructor.
  ast_binaryoperation(position_information *, ast_expression *,
                      ast_expression *);

  // It's an error if these methods are called. See the derived classes.
  virtual sym_index type_check();

  virtual void optimize();

  virtual sym_index generate_quads(quad_list &) = 0;

  // Needed for safe downcasting.
  virtual ast_binaryoperation *get_ast_binaryoperation() {
    fatal("Illegal downcast to ast_binaryoperation");
    return NULL;
  }
};

/*! The superclass for all DIESEL constructs that can be assigned a value
 * (i.e., which evaluate to a reference to a storage location).
 * The name lvalue comes from the fact that it represents expressions
 * that can appear on the left-hand side of an assignment (e.g., ``x := 2;``).
 */
class ast_lvalue : public ast_expression {
protected:
  virtual void print(ostream &);

public:
  // Constructors.
  ast_lvalue(position_information *);

  ast_lvalue(position_information *, sym_index);

  // It's an error if this method is called. See the derived classes.
  virtual sym_index type_check();

  virtual void optimize();

  virtual sym_index generate_quads(quad_list &) = 0;

  virtual void generate_assignment(quad_list &, sym_index) = 0;
};

/*** Concrete classes - these nodes actually appear in the AST. ***/

/*** Classes derived from ast_node ***/

/*! represents an ``elsif`` clause inside an ``if``-statement.
 * The class is a bit special, so it inherits directly from ast_node.
 */
class ast_elsif : public ast_node {
protected:
  virtual void print(ostream &);

public:
  //! The test condition; decides if we should execute the body.
  ast_expression *condition;

  //! The body to execute if the condition evaluates to *true*.
  ast_stmt_list *body;

  // Constructor.
  ast_elsif(position_information *, ast_expression *, ast_stmt_list *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  virtual void generate_quads_and_jump(quad_list &, int);
};

/*! Contains a list of expressions. Currently only used for parameter lists.

   Note: The parameters will be stored in reverse order! This is due to how the
         grammar is written, it's easiest this way. */
class ast_expr_list : public ast_node {
protected:
  virtual void print(ostream &);

public:
  //! Points to the last (last added) expression in the list.
  ast_expression *last_expr;

  //! Points to a list of the preceding expressions. Is ``NULL`` when empty.
  ast_expr_list *preceding;

  //! Constructor for the first element of a list.
  ast_expr_list(position_information *, ast_expression *);

  //! Constructor to add a new expression to the list.
  ast_expr_list(position_information *, ast_expression *, ast_expr_list *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  virtual void generate_parameter_list(quad_list &, parameter_symbol *, int *);
};

/*! Contains a list of statements.
  The body of a program, for example, is represented by this class:
Example:
\verbatim embed:rst
::

  begin
    a := 1;
    b := a;
    c := b;
    d := c;
    e := d;
    f := e;
  end;

.. digraph :: lab3_ast_stmt_list

  s1 [label="ast_stmt_list"];
  s2 [label="ast_stmt_list"];
  s3 [label="ast_stmt_list"];
  s4 [label="ast_stmt_list"];
  s5 [label="ast_stmt_list"];
  s6 [label="ast_stmt_list"];
  a1 [label="ast_assign"];
  a2 [label="ast_assign"];
  a3 [label="ast_assign"];
  a4 [label="ast_assign"];
  a5 [label="ast_assign"];
  a6 [label="ast_assign"];
  s1 -> s2;
  s2 -> s3;
  s3 -> s4;
  s4 -> s5;
  s5 -> s6;
  s6 -> a1;
  s5 -> a2;
  s4 -> a3;
  s3 -> a4;
  s2 -> a5;
  s1 -> a6;
  a1 -> id1;
  a1 -> 1;
  a2 -> id3;
  a2 -> id2;
  a3 -> id5;
  a3 -> id4;
  a4 -> id7;
  a4 -> id6;
  a5 -> id9;
  a5 -> id8;
  a6 -> id11;
  a6 -> id10;
  id1 [label="a"];
  id2 [label="a"];
  id3 [label="b"];
  id4 [label="b"];
  id5 [label="c"];
  id6 [label="c"];
  id7 [label="d"];
  id8 [label="d"];
  id9 [label="e"];
  id10 [label="e"];
  id11 [label="f"];
\endverbatim
  */
class ast_stmt_list : public ast_node {
protected:
  virtual void print(ostream &);

public:
  //! Points to the last (last added) statement in the list.
  ast_statement *last_stmt;

  //! Points to a list of the preceding statements. Is ``NULL`` when empty.
  ast_stmt_list *preceding;

  //! Constructor for the first element of a list.
  ast_stmt_list(position_information *, ast_statement *);

  //! Constructor to add a new statement to the list.
  ast_stmt_list(position_information *, ast_statement *, ast_stmt_list *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! Contains a list of elsif clauses. */
class ast_elsif_list : public ast_node {
protected:
  virtual void print(ostream &);

public:
  //! Points to the last (last added) elsif clause in the list.
  ast_elsif *last_elsif;

  //! Points to a list of the preceding elsif clauses. Is ``NULL`` when empty.
  ast_elsif_list *preceding;

  //! Constructor for the first element of a list.
  ast_elsif_list(position_information *, ast_elsif *);

  //! Constructor to add a new elsif clause to the list.
  ast_elsif_list(position_information *, ast_elsif *, ast_elsif_list *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  virtual void generate_quads_and_jump(quad_list &, int);
};

/*! A node used to transfer information about an environment. Used in parser.y
   for setting the proper return type of a function, and type checking.
   It is never part of a function body. */
class ast_functionhead : public ast_node {
protected:
  virtual void print(ostream &);

public:
  //! Pointer to the function in the symbol table.
  sym_index sym_p;

  // Constructor.
  ast_functionhead(position_information *, sym_index);

  // Only here since we're using abstract virtual methods in ast_node.
  virtual void optimize();

  virtual sym_index generate_quads(quad_list &);

  /*!
  Starts the generation of a quad list for a program block pointed to by the
  argument. It then adds on the last label quad, and returns the list when it is
  done.

  \param s the function body to generate quads for.
  */
  quad_list *do_quads(ast_stmt_list *s);
};

/*! A node used to transfer information about an environment. Used in parser.y
   for setting the proper return type of a procedure. It is never part of a
   procedure body. */
class ast_procedurehead : public ast_node {
protected:
  virtual void print(ostream &);

public:
  //! Pointer to the procedure in the symbol table.
  sym_index sym_p;

  // Constructor.
  ast_procedurehead(position_information *, sym_index);

  // Only here since we're using abstract virtual methods in ast_node.
  virtual void optimize();

  virtual sym_index generate_quads(quad_list &);

  /*!
  Starts the generation of a quad list for a program block pointed to by the
  argument. It then adds on the last label quad, and returns the list when it is
  done.

  \param s the procedure body to generate quads for.
  */
  quad_list *do_quads(ast_stmt_list *s);
};

/*** Classes derived from ast_statement ***/

/*! Represents a procedure call.

Example:
\verbatim embed:rst
::

  foo(1, 2+3, c);

.. digraph :: diesel_7_ex_procedure_call
  :alt: AST for foo(1, 2+3, c) being called

  ordering=out;
  l1 [label="ast_expr_list"];
  l2 [label="ast_expr_list"];

  ast_procedure_call -> foo;
  ast_procedure_call -> l1;
  foo [label="ast_id\nfoo"];
  l1 -> l2;
  l1 -> "ast_id\nc";
  l2 -> "ast_integer\n1";
  l2 -> ast_plus;
  ast_plus -> "ast_integer\n2";
  ast_plus -> "ast_integer\n3";

\endverbatim

Example:
\verbatim embed:rst
::

  fie(1);

.. digraph :: diesel_7_ex_procedure_call_2
  :alt: AST for fie(1) being called

  ordering=out;
  ast_procedure_call -> "ast_id\nfie";
  ast_procedure_call -> ast_expr_list
  ast_expr_list -> "ast_integer\n1";
\endverbatim

Example:
\verbatim embed:rst
::

  if a > b then
    c := a;
  else
    c := b;
  end;

.. digraph :: diesel_7_ex_procedure_call_3
  :alt: AST for fum() being called

  ordering=out;
  ast_procedure_call -> "ast_id\nfum";
\endverbatim

*/
class ast_procedurecall : public ast_statement {
protected:
  virtual void print(ostream &);

public:
  //! The procedure's id node, contains a link to the symbol table.
  ast_id *id;

  //! A list of eventual parameters. If no parameters, this list is NULL.
  ast_expr_list *parameter_list;

  // Constructor.
  ast_procedurecall(position_information *, ast_id *, ast_expr_list *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! Assignment node. ``a := b`` or ``a[i] = b``.

  Note that this class inherits ast_statement, and therefore does
  not return a value. Constructs of the type ``a := (b := 2);`` are not
  allowed in DIESEL.
 */
class ast_assign : public ast_statement {
protected:
  virtual void print(ostream &);

public:
  //! The left hand side (lhs), ie, the variable being assigned to.
  ast_lvalue *lhs;

  //! The right hand side (rhs), ie, the value being assigned.
  ast_expression *rhs;

  // Constructor.
  ast_assign(position_information *, ast_lvalue *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! A while loop. Contains a test condition and a loop body.

 Compare this to other conditional nodes (e.g. ``ast_elsif``, ``ast_if``).
 */
class ast_while : public ast_statement {
protected:
  virtual void print(ostream &);

public:
  //! The test condition.
  ast_expression *condition;

  //! The loop body.
  ast_stmt_list *body;

  // Constructor.
  ast_while(position_information *, ast_expression *, ast_stmt_list *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! An if clause. Has lots of children. if - then - elsif - else.

Example:
\verbatim embed:rst
::

  if a > b then
    c := a;
  else
    c := b;
  end;

.. digraph :: diesel_7_ex_if_1
  :alt: AST for if-statement with an else-clause.

  ordering=out;
  ass1 [label="ast_assign"];
  ass2 [label="ast_assign"];
  id_a_1 [label="ast_id\na"];
  id_a_2 [label="ast_id\na"];
  id_b_1 [label="ast_id\nb"];
  id_b_2 [label="ast_id\nb"];
  id_c_1 [label="ast_id\nc"];
  id_c_2 [label="ast_id\nc"];

  ast_if -> ast_greaterthan [label="condition"];
  ast_if -> ass1 [label="body"];
  ast_if -> ass2 [label="else_body"];

  ast_greaterthan -> id_a_1;
  ast_greaterthan -> id_b_1;

  ass1 -> id_c_1;
  ass1 -> id_a_2;

  ass2 -> id_c_2;
  ass2 -> id_b_2;
\endverbatim

 Example:
\verbatim embed:rst
::

  if a > b then
    c := a
  elsif a = b
    then c := 0
  elsif a < b then
    c := b
  else
    c := -1
  end;

.. digraph :: diesel_7_ex_if_2
  :alt: AST for an if-statement with elsif clauses.

  ordering=out;
  ass1 [label="ast_assign"];
  ass2 [label="ast_assign"];
  ass3 [label="ast_assign"];
  ass4 [label="ast_assign"];
  elsif_l1 [label="ast_elsif_list"];
  elsif_l2 [label="ast_elsif_list"];
  id_a_1 [label="ast_id\na"];
  id_a_2 [label="ast_id\na"];
  id_a_3 [label="ast_id\na"];
  id_a_4 [label="ast_id\na"];
  id_b_1 [label="ast_id\nb"];
  id_b_2 [label="ast_id\nb"];
  id_b_3 [label="ast_id\nb"];
  id_b_4 [label="ast_id\nb"];
  id_c_1 [label="ast_id\nc"];
  id_c_2 [label="ast_id\nc"];
  id_c_3 [label="ast_id\nc"];
  id_c_4 [label="ast_id\nc"];

  ast_if -> ast_greaterthan [label="condition"];
  ast_if -> ass1 [label="body"];
  ast_if -> elsif_l1 [label="elsif_list"];
  ast_if -> ass4 [label="else_body"];

  ast_greaterthan -> id_a_1;
  ast_greaterthan -> id_b_1;

  ass1 -> id_c_1;
  ass1 -> id_a_2;

  elsif_l1 -> ast_eq;
  elsif_l1 -> ass2;
  elsif_l1 -> elsif_l2;
  ast_eq -> id_a_3;
  ast_eq -> id_b_2;
  ass2 -> id_c_2;
  ass2 -> "ast_integer\n0";

  elsif_l2 -> ast_lessthan;
  elsif_l2 -> ass3;
  ast_lessthan -> id_a_4;
  ast_lessthan -> id_b_3;
  ass3 -> id_c_3;
  ass3 -> id_b_4;

  ass4 -> id_c_4;
  ass4 -> ast_uminus;
  ast_uminus -> "ast_integer\n1";
\endverbatim
 */
class ast_if : public ast_statement {
protected:
  virtual void print(ostream &);

public:
  //! The primary if-condition.
  ast_expression *condition;

  //! The primary if-body, executed if #condition evaluates to non-zero.
  ast_stmt_list *body;

  //! A list of elsif clauses. May be NULL.
  ast_elsif_list *elsif_list;

  //! The else body, executed if 'condition' evaluates to zero. May be NULL.
  ast_stmt_list *else_body;

  // Constructor.
  ast_if(position_information *, ast_expression *, ast_stmt_list *,
         ast_elsif_list *, ast_stmt_list *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! A return statement, used both for procedures and functions.
   If no value is returned (i.e. in a procedure), 'value' should be set
   to NULL.

  Note that the return statement itself has no value, i.e., it cannot
  stand on the right-hand side of an assignment since it inherits
  ``ast_statement``.
  */
class ast_return : public ast_statement {
protected:
  virtual void print(ostream &);

public:
  //! The return value.
  ast_expression *value;

  //! Constructor for no return value.
  ast_return(position_information *);

  //! Constructor with a return value.
  ast_return(position_information *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*** Classes derived from ast_expression ***/

/*! Represents a function call. ``a = calc(foo);``

 If you wonder how the return value is handled, or where the function
 call is actually connected to the function body... Don’t worry.
 It will become clear later on.
 Remember that we parse one block at a time, so that the call of a
 function takes place on one lexical level higher (as in less deeply nested)
 than the execution of the function body, except in certain cases
 involving recursive function calls.
 */
class ast_functioncall : public ast_expression {
protected:
  virtual void print(ostream &);

public:
  //! The function's id node, contains a link to the symbol table.
  ast_id *id;

  //! A list of actual parameters. If no parameters, this list is NULL.
  ast_expr_list *parameter_list;

  // Constructor.
  ast_functioncall(position_information *, ast_id *, ast_expr_list *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! A unary minus node.

 Note that there is no unary plus.
 */
class ast_uminus : public ast_expression {
protected:
  virtual void print(ostream &);

public:
  //! The expression to negate.
  ast_expression *expr;

  // Constructor.
  ast_uminus(position_information *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! A logical negation node. */
class ast_not : public ast_expression {
protected:
  virtual void print(ostream &);

public:
  //! The expression being negated.
  ast_expression *expr;

  // Constructor.
  ast_not(position_information *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! An integer node. Represents an integer number, like ``5``. */
class ast_integer : public ast_expression {
protected:
  virtual void print(ostream &);

public:
  //! The integer value of the node.
  long value;

  // Constructor.
  ast_integer(position_information *, long);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasting.
  virtual ast_integer *get_ast_integer() { return this; }
};

/*! A real node. Represents a real number, like ``2.5``. */
class ast_real : public ast_expression {
protected:
  virtual void print(ostream &);

public:
  //! The floating point value of the node.
  double value;

  // Constructor.
  ast_real(position_information *, double);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasting.
  virtual ast_real *get_ast_real() { return this; }
};

/*! A cast node.

 This class represents type transformation, casting an integer to a real.
 It is inserted into the AST at appropriate places during type checking.
 See semantic.cc. */
class ast_cast : public ast_expression {
protected:
  virtual void print(ostream &);

public:
  //! The expression to cast to real.
  ast_expression *expr;

  // Constructor.
  ast_cast(position_information *, ast_expression *);

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasting.
  virtual ast_cast *get_ast_cast() { return this; }
};

/*** Classes derived from ast_binaryrelation ***/

/*! Equality operator. ``a = b``. */
class ast_equal : public ast_binaryrelation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_equal(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! Not-equal operator. ``a <> b``. */
class ast_notequal : public ast_binaryrelation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_notequal(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! Less than operator. ``a < b``. */
class ast_lessthan : public ast_binaryrelation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_lessthan(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*! Greater than operator. ``a > b``. */
class ast_greaterthan : public ast_binaryrelation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_greaterthan(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);
};

/*** Classes derived from ast_binaryoperation ***/

/*! Plus node. ``a + b``.

  Example:
\verbatim embed:rst
::

  -a + 5 * c

.. graphviz::
   :alt: AST for an addition.

   digraph diesel {
      ordering=out;
      id_a [label="ast_id\na"];
      id_c [label="ast_id\nc"];
      i5 [label="ast_integer\n5"];
      ast_add -> ast_uminus;
      ast_add -> ast_mult;
      ast_uminus -> id_a;
      ast_mult -> i5;
      ast_mult -> id_c;
   }
\endverbatim
 */
class ast_add : public ast_binaryoperation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_add(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasts.
  virtual ast_add *get_ast_binaryoperation() { return this; }
};

/*! Minus node. ``a - b``. */
class ast_sub : public ast_binaryoperation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_sub(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasts.
  virtual ast_sub *get_ast_binaryoperation() { return this; }
};

/*! Logical OR node. ``a OR b``. */
class ast_or : public ast_binaryoperation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_or(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasts.
  virtual ast_or *get_ast_binaryoperation() { return this; }
};

/*! Logical AND node. ``a AND b``. */
class ast_and : public ast_binaryoperation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_and(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasts.
  virtual ast_and *get_ast_binaryoperation() { return this; }
};

/*! Multiplication node. ``a * b``. */
class ast_mult : public ast_binaryoperation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_mult(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasts.
  virtual ast_mult *get_ast_binaryoperation() { return this; }
};

/*! Real division node. ``a / b``, where at least one of a and b have real type.
 */
class ast_divide : public ast_binaryoperation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_divide(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasts.
  virtual ast_divide *get_ast_binaryoperation() { return this; }
};

/*! Integer division node. ``a div b``, where both operands have integer type.
 */
class ast_idiv : public ast_binaryoperation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_idiv(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasts.
  virtual ast_idiv *get_ast_binaryoperation() { return this; }
};

/*! Integer mod node. ``a mod b``, where both operands have integer type. */
class ast_mod : public ast_binaryoperation {
protected:
  virtual void print(ostream &);

public:
  // Constructor.
  ast_mod(position_information *, ast_expression *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  // Safe downcasts.
  virtual ast_mod *get_ast_binaryoperation() { return this; }
};

/*! An identifier node. Can be the name of a variable, function, constant,
   etc... */
class ast_id : public ast_lvalue {
protected:
  virtual void print(ostream &);

public:
  //! A symbol table index for this symbol.
  sym_index sym_p;

  // Constructors.
  ast_id(position_information *);

  ast_id(position_information *, sym_index);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  virtual void generate_assignment(quad_list &, sym_index);

  // Safe downcasting.
  virtual ast_id *get_ast_id() { return this; }
};

/*! An array identifier node. Index must be of integer type.

Example:
\verbatim embed:rst
::

  a[a[1]] := a[2] + 3;

.. digraph :: diesel_assigned_index
  :alt: AST for an indexed assignment.

  ordering=out;
  ass [label="ast_assign"];
  add [label="ast_add"];
  ix1 [label="ast_indexed"];
  ix2 [label="ast_indexed"];
  ix3 [label="ast_indexed"];
  id1 [label="ast_id\na"];
  id2 [label="ast_id\nc"];
  id3 [label="ast_id\nb"];
  i1 [label="ast_integer\n1"];
  i2 [label="ast_integer\n2"];
  i3 [label="ast_integer\n3"];
  ass -> ix1;
  ix1 -> id1;
  ix1 -> ix2;
  ix2 -> id2;
  ix2 -> i1;
  ass -> add;
  add -> ix3;
  ix3 -> id3;
  ix3 -> i2;
  add -> i3;
\endverbatim
 */
class ast_indexed : public ast_lvalue {
protected:
  virtual void print(ostream &);

public:
  //! The array's id node, which contains a link into the symbol table.
  ast_id *id;

  //! The index expression. Must be of type integer.
  ast_expression *index;

  // Constructor.
  ast_indexed(position_information *, ast_id *, ast_expression *);

  // Perform type checking.
  virtual sym_index type_check();

  // AST optimization.
  virtual void optimize();

  // Quad generation.
  virtual sym_index generate_quads(quad_list &);

  virtual void generate_assignment(quad_list &, sym_index);
};

/* Allow a node to be sent to an outstream for printing. */
ostream &operator<<(ostream &, ast_node *);

#endif
