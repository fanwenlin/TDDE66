#include "optimize.hh"
#include "ast.hh"

/*** This file contains all code pertaining to AST optimisation. It currently
     implements a simple optimisation called "constant folding". Most of the
     methods in this file are empty, or just relay optimize calls downward
     in the AST. If a more powerful AST optimization scheme were to be
     implemented, only methods in this file should need to be changed. ***/

ast_optimizer *optimizer = new ast_optimizer();

/* The optimizer's interface method. Starts a recursive optimize call down
   the AST nodes, searching for binary operators with constant children. */
void ast_optimizer::do_optimize(ast_stmt_list *body) {
  if (body != NULL) {
    body->optimize();
  }
}

/* Returns 1 if an AST expression is a subclass of ast_binaryoperation,
   ie, eligible for constant folding. */
bool ast_optimizer::is_binop(ast_expression *node) {
  switch (node->tag) {
  case AST_ADD:
  case AST_SUB:
  case AST_OR:
  case AST_AND:
  case AST_MULT:
  case AST_DIVIDE:
  case AST_IDIV:
  case AST_MOD:
    return true;
  default:
    return false;
  }
}

bool ast_optimizer::is_binary_relation(ast_expression *node) {
  switch (node->tag) {
  case AST_EQUAL:
  case AST_NOTEQUAL:
  case AST_LESSTHAN:
  case AST_GREATERTHAN:
    return true;
  default:
    return false;
  }
}

/* We overload this method for the various ast_node subclasses that can
   appear in the AST. By use of virtual (dynamic) methods, we ensure that
   the correct method is invoked even if the pointers in the AST refer to
   one of the abstract classes such as ast_expression or ast_statement. */
void ast_node::optimize() {
  fatal("Trying to optimize abstract class ast_node.");
}

void ast_statement::optimize() {
  fatal("Trying to optimize abstract class ast_statement.");
}

void ast_expression::optimize() {
  fatal("Trying to optimize abstract class ast_expression.");
}

void ast_lvalue::optimize() {
  fatal("Trying to optimize abstract class ast_lvalue.");
}

void ast_binaryoperation::optimize() {
  fatal("Trying to optimize abstract class ast_binaryoperation.");
}

void ast_binaryrelation::optimize() {
  fatal("Trying to optimize abstract class ast_binaryrelation.");
}

/*** The optimize methods for the concrete AST classes. ***/

/* Optimize a statement list. */
void ast_stmt_list::optimize() {
  if (preceding != NULL) {
    preceding->optimize();
  }
  if (last_stmt != NULL) {
    last_stmt->optimize();
  }
}

/* Optimize a list of expressions. */
void ast_expr_list::optimize() {
  /* Your code here */
  if (preceding != NULL) {
    preceding->optimize();
  }
  if (last_expr != NULL) {
    last_expr->optimize();
    last_expr = optimizer->fold_constants(last_expr);
  }
}

/* Optimize an elsif list. */
void ast_elsif_list::optimize() {
  /* Your code here */
  // cout << "optimizing elsif list" << endl;
  if (preceding != NULL) {
    preceding->optimize();
  }
  if (last_elsif != NULL) {
    last_elsif->optimize();
  }
}

/* An identifier's value can change at run-time, so we can't perform
   constant folding optimization on it unless it is a constant.
   Thus we just do nothing here. It can be treated in the fold_constants()
   method, however. */
void ast_id::optimize() {}

void ast_indexed::optimize() {
  /* Your code here */
  if (index != NULL) {
    index->optimize();
  }
}

// check if it is a constant node
bool is_constant(ast_expression *node) {
  if (node->tag == AST_INTEGER || node->tag == AST_REAL) {
    return true;
  } else if (node->tag == AST_ID) {
    sym_index sym_p = node->get_ast_id()->sym_p;
    if (sym_p != NULL_SYM && sym_tab->get_symbol(sym_p)->tag == SYM_CONST) {
      constant_symbol *const_sym =
          sym_tab->get_symbol(sym_p)->get_constant_symbol();
      return const_sym->type == integer_type || const_sym->type == real_type;
    }
    return false;
  } else if (node->tag == AST_CAST) {
    sym_index cast_type = node->get_ast_cast()->type;
    if (cast_type == integer_type || cast_type == real_type) {
      return is_constant(node->get_ast_cast()->expr);
    }
    return false;
  }
  return false;
}

// known node is a constant node, check if it should be an integer type
bool should_be_integer(ast_expression *node) {
  if (node->tag == AST_INTEGER) {
    return true;
  } else if (node->tag == AST_ID) {
    sym_index sym_p = node->get_ast_id()->sym_p;
    if (sym_p != NULL_SYM && sym_tab->get_symbol(sym_p)->tag == SYM_CONST) {
      constant_symbol *const_sym =
          sym_tab->get_symbol(sym_p)->get_constant_symbol();
      return const_sym->type == integer_type;
    }
    return false;
  } else if (node->tag == AST_CAST) {
    sym_index cast_type = node->get_ast_cast()->type;
    return cast_type == integer_type;
  }
  return false;
}

// known node is a constant node, get the real value or cast by default
double get_real_constant(ast_expression *node) {
  if (node->tag == AST_REAL) {
    return node->get_ast_real()->value;
  }

  if (node->tag == AST_ID) {
    sym_index sym_p = node->get_ast_id()->sym_p;
    if (sym_p != NULL_SYM && sym_tab->get_symbol(sym_p)->tag == SYM_CONST) {
      constant_symbol *const_sym =
          sym_tab->get_symbol(sym_p)->get_constant_symbol();
      return const_sym->const_value.rval;
    }
  } else if (node->tag == AST_CAST && node->get_ast_cast()->type == real_type) {
    return double(get_int_constant(node->get_ast_cast()->expr));
  }
  if (node->tag == AST_CAST && node->get_ast_cast()->type == real_type) {
    // get underlying integer constant and convert to real
    return double(get_int_constant(node->get_ast_cast()->expr));
  }

  return double(get_int_constant(node));
}

// known node is a constant integer node, get the integer value, but will not
// cast to real by default
long get_int_constant(ast_expression *node) {
  if (node->tag == AST_INTEGER) {
    return node->get_ast_integer()->value;
  }
  if (node->tag == AST_ID) {
    sym_index sym_p = node->get_ast_id()->sym_p;
    if (sym_p != NULL_SYM && sym_tab->get_symbol(sym_p)->tag == SYM_CONST) {
      return sym_tab->get_symbol(sym_p)
          ->get_constant_symbol()
          ->const_value.ival;
    }
  }
  if (node->tag == AST_CAST) {
    return long(get_real_constant(node->get_ast_cast()->expr));
  }
  return 0;
}

/* This convenience method is used to apply constant folding to all
   binary operations. It returns either the resulting optimized node or the
   original node if no optimization could be performed. */
ast_expression *ast_optimizer::fold_constants(ast_expression *node) {
  /* Your code here */
  if (optimizer->is_binop(node)) {
    // cout << "folding constants for " << node << endl;
    ast_binaryoperation *binop = node->get_ast_binaryoperation();
    binop->left->optimize();
    binop->right->optimize();
    binop->left = fold_constants(binop->left);
    binop->right = fold_constants(binop->right);
    if (!is_constant(binop->left) || !is_constant(binop->right)) {
      return node;
    }
    if (should_be_integer(binop->left) && should_be_integer(binop->right)) {
      // cout << "folding integer constants " << binop->left << " and "
      //      << binop->right << endl;
      // cout << "operation is " << binop->tag << endl;
      long left_int = get_int_constant(binop->left);
      long right_int = get_int_constant(binop->right);

      switch (binop->tag) {
      case AST_ADD:
        return new ast_integer(binop->pos, left_int + right_int);
        break;
      case AST_SUB:
        return new ast_integer(binop->pos, left_int - right_int);
        break;
      case AST_MULT:
        return new ast_integer(binop->pos, left_int * right_int);
        break;
      case AST_DIVIDE:
        return new ast_real(binop->pos, double(left_int) / right_int);
        break;
      case AST_MOD:
        return new ast_integer(binop->pos, left_int % right_int);
        break;
      case AST_IDIV:
        return new ast_integer(binop->pos, left_int / right_int);
        break;
      case AST_OR:
        // cout << "folding or " << binop->left << " and " << binop->right
        //  << " result is " << (left_int | right_int) << endl;
        return new ast_integer(binop->pos, left_int | right_int);
        break;
      case AST_AND:
        return new ast_integer(binop->pos, left_int & right_int);
        break;
      default:
        fatal("Trying to fold unknown binary operation");
      }

    } else {
      double left_real = get_real_constant(binop->left);
      double right_real = get_real_constant(binop->right);
      switch (binop->tag) {
      case AST_ADD:
        return new ast_real(binop->pos, left_real + right_real);
        break;
      case AST_SUB:
        return new ast_real(binop->pos, left_real - right_real);
        break;
      case AST_MULT:
        return new ast_real(binop->pos, left_real * right_real);
        break;
      case AST_DIVIDE:
        return new ast_real(binop->pos, left_real / right_real);
        break;
      case AST_MOD:
        return new ast_real(binop->pos, fmod(left_real, right_real));
        break;
      default:
        fatal("Trying to fold unknown binary operation");
      }
    }
  } else if (optimizer->is_binary_relation(node)) {
    ast_binaryrelation *binrel = node->get_ast_binaryrelation();
    binrel->left->optimize();
    binrel->right->optimize();
    binrel->left = fold_constants(binrel->left);
    binrel->right = fold_constants(binrel->right);
    if (!is_constant(binrel->left) || !is_constant(binrel->right)) {
      return node;
    }
    if (should_be_integer(binrel->left) && should_be_integer(binrel->right)) {
      long left_int = get_int_constant(binrel->left);
      long right_int = get_int_constant(binrel->right);
      switch (binrel->tag) {
      case AST_EQUAL:
        return new ast_integer(binrel->pos, left_int == right_int);
        break;
      case AST_NOTEQUAL:
        return new ast_integer(binrel->pos, left_int != right_int);
        break;
      case AST_LESSTHAN:
        return new ast_integer(binrel->pos, left_int < right_int);
        break;
      case AST_GREATERTHAN:
        return new ast_integer(binrel->pos, left_int > right_int);
        break;
      default:
        fatal("Trying to fold unknown binary relation");
      }
    } else {
      double left_real = get_real_constant(binrel->left);
      double right_real = get_real_constant(binrel->right);
      switch (binrel->tag) {
      case AST_EQUAL:
        return new ast_integer(binrel->pos, left_real == right_real);
        break;
      case AST_NOTEQUAL:
        return new ast_integer(binrel->pos, left_real != right_real);
        break;
      case AST_LESSTHAN:
        return new ast_integer(binrel->pos, left_real < right_real);
        break;
      case AST_GREATERTHAN:
        return new ast_integer(binrel->pos, left_real > right_real);
        break;
      default:
        fatal("Trying to fold unknown binary relation");
      }
    }
  }
  return node;
}

/* All the binary operations should already have been detected in their
   parent nodes, so we don't need to do anything at all here. */
void ast_add::optimize() { /* Your code here */ }

void ast_sub::optimize() { /* Your code here */ }

void ast_mult::optimize() { /* Your code here */ }

void ast_divide::optimize() { /* Your code here */ }

void ast_or::optimize() { /* Your code here */ }

void ast_and::optimize() { /* Your code here */ }

void ast_idiv::optimize() { /* Your code here */ }

void ast_mod::optimize() { /* Your code here */ }

/* We can apply constant folding to binary relations as well. */
void ast_equal::optimize() { /* Your code here */ }

void ast_notequal::optimize() { /* Your code here */ }

void ast_lessthan::optimize() { /* Your code here */ }

void ast_greaterthan::optimize() { /* Your code here */ }

/*** The various classes derived from ast_statement. ***/

void ast_procedurecall::optimize() {
  /* Your code here */
  if (id != NULL) {
    id->optimize();
  }
  if (parameter_list != NULL) {
    parameter_list->optimize();
  }
}

void ast_assign::optimize() {
  /* Your code here */
  if (lhs != NULL) {
    lhs->optimize();
  }
  if (rhs != NULL) {
    rhs->optimize();
    rhs = optimizer->fold_constants(rhs);
  }
}

void ast_while::optimize() { /* Your code here */
  if (condition != NULL) {
    condition->optimize();
    // cout << "folding while condition " << condition << endl;
    condition = optimizer->fold_constants(condition);
    // cout << "after folding while condition " << condition << endl;
  }
  if (body != NULL) {
    body->optimize();
  }
}

void ast_if::optimize() {
  /* Your code here */
  if (condition != NULL) {
    condition->optimize();
    // cout << "folding if condition " << condition << endl;
    condition = optimizer->fold_constants(condition);
    // cout << "after folding if condition " << condition << endl;
  }
  if (body != NULL) {
    body->optimize();
  }
  if (elsif_list != NULL) {
    elsif_list->optimize();
  }
  if (else_body != NULL) {
    else_body->optimize();
  }
}

void ast_return::optimize() {
  /* Your code here */
  if (value != NULL) {
    value->optimize();
    value = optimizer->fold_constants(value);
  }
}

void ast_functioncall::optimize() {
  /* Your code here */
  if (id != NULL) {
    id->optimize();
  }
  if (parameter_list != NULL) {
    parameter_list->optimize();
  }
}

void ast_uminus::optimize() { /* Your code here */
  if (expr != NULL) {
    expr->optimize();
    expr = optimizer->fold_constants(expr);
  }
}

void ast_not::optimize() { /* Your code here */
  if (expr != NULL) {
    expr->optimize();
    expr = optimizer->fold_constants(expr);
  }
}

void ast_elsif::optimize() {
  /* Your code here */
  if (condition != NULL) {
    condition->optimize();
    // cout << "folding elsif condition " << condition << endl;
    condition = optimizer->fold_constants(condition);
    // cout << "after folding elsif condition " << condition << endl;
  }
  if (body != NULL) {
    body->optimize();
  }
}

void ast_integer::optimize() { /* Your code here */ }

void ast_real::optimize() { /* Your code here */ }

/* Note: See the comment in fold_constants() about casts and folding. */
void ast_cast::optimize() {
  /* Your code here */
  if (expr != NULL) {
    expr->optimize();
    expr = optimizer->fold_constants(expr);
  }
}

void ast_procedurehead::optimize() {
  fatal("Trying to call ast_procedurehead::optimize()");
}

void ast_functionhead::optimize() {
  fatal("Trying to call ast_functionhead::optimize()");
}
