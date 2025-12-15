
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "codegen.hh"
#include "quads.hh"
#include "symtab.hh"

using namespace std;

// Defined in main.cc.
extern bool assembler_trace;

// Used in parser.y. Ideally the filename should be parametrized, but it's not
// _that_ important...
code_generator *code_gen = new code_generator("d.out");

// Constructor.
code_generator::code_generator(const string object_file_name) {
  out.open(object_file_name);

  reg[RAX] = "rax";
  reg[RCX] = "rcx";
  reg[RDX] = "rdx";
}

/* Destructor. */
code_generator::~code_generator() {
  // Make sure we close the outfile before exiting the compiler.
  out << flush;
  out.close();
}

/* This method is called from parser.y when code generation is to start.
   The argument is a quad_list representing the body of the procedure, and
   the symbol for the environment for which code is being generated. */
void code_generator::generate_assembler(quad_list *q, symbol *env) {
  prologue(env);
  expand(q);
  epilogue(env);
}

/* This method aligns a frame size on an 8-byte boundary. Used by prologue().
 */
int code_generator::align(int frame_size) { return ((frame_size + 7) / 8) * 8; }

/* This method generates assembler code for initialisating a procedure or
   function. */
void code_generator::prologue(symbol *new_env) {
  int ar_size;
  int label_nr;
  // Used to count parameters.
  parameter_symbol *last_arg;

  // Again, we need a safe downcast for a procedure/function.
  // Note that since we have already generated quads for the entire block
  // before we expand it to assembler, the size of the activation record
  // is known here (ar_size).
  if (new_env->tag == SYM_PROC) {
    procedure_symbol *proc = new_env->get_procedure_symbol();
    ar_size = align(proc->ar_size);
    label_nr = proc->label_nr;
    last_arg = proc->last_parameter;
  } else if (new_env->tag == SYM_FUNC) {
    function_symbol *func = new_env->get_function_symbol();
    /* Make sure ar_size is a multiple of eight */
    ar_size = align(func->ar_size);
    label_nr = func->label_nr;
    last_arg = func->last_parameter;
  } else {
    fatal("code_generator::prologue() called for non-proc/func");
    return;
  }

  /* Print out the label number (a SYM_PROC/ SYM_FUNC attribute) */
  out << "L" << label_nr << ":" << "\t\t\t" << "# " <<
      /* Print out the function/procedure name */
      sym_tab->pool_lookup(new_env->id) << endl;

  if (assembler_trace) {
    out << "\t" << "# PROLOGUE (" << short_symbols << new_env << long_symbols
        << ")" << endl;
  }

  /* Your code here */

  // store the previous RBP
  out << "\t\t" << "push" << "\t" << "rbp" << endl;

  // save the previous RSP in a temporary location
  // this is also this frame's RBP
  // we will set it into RBP register later, after we set DISPLAY area for this
  // frame
  out << "\t\t" << "mov" << "\t" << "rcx, rsp" << endl;

  // copy the display area statically because we know the level of the block
  {
    for (int i = 0; i < new_env->level; i++) {
      out << "\t\t" << "push" << "\t" << "[rbp-" << (i + 1) * 8 << "]" << endl;
    }
  }

  // at the end of DISPLAY area set our own RBP, as stored in rcx before
  out << "\t\t" << "push" << "\t" << "rcx" << endl;
  // also set our RBP register, as stored before
  out << "\t\t" << "mov" << "\t" << "rbp, rcx" << endl;

  // allocate space for the local variables
  out << "\t\t" << "sub" << "\t" << "rsp, " << ar_size << endl;

  out << flush;
}

/* This method generates assembler code for leaving a procedure or function. */
void code_generator::epilogue(symbol *old_env) {
  if (assembler_trace) {
    out << "\t" << "# EPILOGUE (" << short_symbols << old_env << long_symbols
        << ")" << endl;
  }

  /* Your code here */
  out << "\t\t" << "leave" << endl;
  out << "\t\t" << "ret" << endl;

  out << flush;
}

/* This function finds the display register level and offset for a variable,
   array or a parameter. Note the pass-by-pointer arguments. */
void code_generator::find(sym_index sym_p, int *level, int *offset) {
  /* Your code here */
  symbol *sym = sym_tab->get_symbol(sym_p);
  if (sym->tag == SYM_VAR || sym->tag == SYM_ARRAY) {
    *level = sym->level;
    *offset = - ((sym->offset) + ((sym->level + 1) * STACK_WIDTH));
  } else if (sym->tag == SYM_PARAM) {
    *level = sym->level;
    // parameters are stored in the caller's activation record, so we need to
    // subtract the offset from the callee's activation record, plus old_rbp(8) + return_addr(8)
    *offset = + (sym->offset + 2*STACK_WIDTH);
  } else {
    fatal("find() called for non-var/param/array");
  }
}

/*
 * Generates code for getting the address of a frame for the specified scope
 * level.
 */
void code_generator::frame_address(int level, const register_type dest) {
  /* Your code here */
  out << "\t\t" << "mov" << "\t" << reg[dest] << ", [rbp-" << level * 8 << "]"
      << endl;
}

/* This function fetches the value of a variable or a constant into a
   register. */
void code_generator::fetch(sym_index sym_p, register_type dest) {
  /* Your code here */
  if (sym_p == NULL_SYM) {
    return;
  }
  // if variable
  symbol *sym = sym_tab->get_symbol(sym_p);
  if (sym->tag == SYM_VAR || sym->tag == SYM_PARAM) {
    block_level level;
    int offset;
    find(sym_p, &level, &offset);
    // store the frame's address into the register first, then find with offset
    frame_address(level, dest);
    out << "\t\t" << "mov" << "\t" << reg[dest] << ", [" << reg[dest] << (offset <= 0 ? "-" : "+")
        << abs(offset) << "]" << endl;
  } else if (sym->tag == SYM_CONST) {
    // const
    out << "\t\t" << "mov" << "\t" << reg[dest] << ", "
        << sym->get_constant_symbol()->const_value.ival << endl;
  }
}

void code_generator::fetch_float(sym_index sym_p) {
  /* Your code here */
  if (sym_p == NULL_SYM) {
    return;
  }
  symbol *sym = sym_tab->get_symbol(sym_p);
  if (sym->tag == SYM_VAR || sym->tag == SYM_PARAM) {
    block_level level;
    int offset;
    find(sym_p, &level, &offset);
    // store the frame's address into the register first, then find with offset
    frame_address(level, RCX);
    out << "\t\t" << "fld" << "\t" << "qword ptr [RCX" << (offset <= 0 ? "-" : "+") << abs(offset) << "] "
        << endl;
  } else {
    // const
    long value = sym_tab->ieee(sym->get_constant_symbol()->const_value.rval);
    out << "\t\t" << "mov" << "\t" << "rax, " << value << endl;
    out << "\t\t" << "push" << "\t" << "rax" << endl;
    out << "\t\t" << "fld" << "\t" << "qword ptr [rsp]" << endl;
    out << "\t\t" << "add" << "\t" << "rsp, 8" << endl;
  }
}

register_type another_register(const register_type reg) {
  if (reg == RAX) {
    return RCX;
  } else if (reg == RCX) {
    return RAX;
  }
  return RCX;
}

/* This function stores the value of a register into a variable. */
void code_generator::store(register_type src, sym_index sym_p) {
  /* Your code here */
  if (sym_p == NULL_SYM) {
    return;
  }
  block_level level;
  int offset;
  find(sym_p, &level, &offset);
  // store the frame's address into the register first, then find with offset
  // we assume `store` will only be called once in a single quad, so we can use
  // another temp register to avoid overwriting the source register
  frame_address(level, another_register(src));
  out << "\t\t" << "mov" << "\t" << "[" << reg[another_register(src)] << (offset <= 0 ? "-" : "+")
      << abs(offset) << "], " << reg[src] << endl;
}

void code_generator::store_float(sym_index sym_p) {
  /* Your code here */
  if (sym_p == NULL_SYM) {
    return;
  }
  block_level level;
  int offset;
  find(sym_p, &level, &offset);
  // Preserve RAX, use it to compute the destination address, and write ST(0)
  // to memory while popping the FPU stack.

  frame_address(level, RAX);
  out << "\t\t" << "fstp" << "\t" << "qword ptr [RAX" << (offset <= 0 ? "-" : "+") << abs(offset) << "]"
      << endl;
}

/* This function fetches the base address of an array. */
void code_generator::array_address(sym_index sym_p, register_type dest) {
  /* Your code here */
  block_level level;
  int offset;
  find(sym_p, &level, &offset);
  // store the frame's address into the register first, then compute element base
  frame_address(level, dest);
  if (offset >= 0) {
    out << "\t\t" << "add" << "\t" << reg[dest] << ", " << offset << endl;
  } else {
    out << "\t\t" << "sub" << "\t" << reg[dest] << ", " << abs(offset) << endl;
  }
}

/* This method expands a quad_list into assembler code, quad for quad. */
void code_generator::expand(quad_list *q_list) {
  long quad_nr = 0; // Just to make debug output easier to read.

  // We use this iterator to loop through the quad list.
  quad_list_iterator *ql_iterator = new quad_list_iterator(q_list);

  quadruple *q = ql_iterator->get_current(); // This is the head of the list.

  while (q != NULL) {
    quad_nr++;

    // We always do labels here so that a branch doesn't miss the
    // trace code.
    if (q->op_code == q_labl) {
      out << "L" << q->int1 << ":" << endl;
    }

    // Debug output.
    if (assembler_trace) {
      out << "\t" << "# QUAD " << quad_nr << ": " << short_symbols << q
          << long_symbols << endl;
    }

    // The main switch on quad type. This is where code is actually
    // generated.
    switch (q->op_code) {
    case q_rload:
    case q_iload:
      out << "\t\t" << "mov" << "\t" << "rax, " << q->int1 << endl;
      store(RAX, q->sym3);
      break;

    case q_inot: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      fetch(q->sym1, RAX);
      out << "\t\t" << "cmp" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "je" << "\t" << "L" << label << endl;
      // Not equal branch
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // Equal branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_ruminus:
      fetch_float(q->sym1);
      out << "\t\t" << "fchs" << endl;
      store_float(q->sym3);
      break;

    case q_iuminus:
      fetch(q->sym1, RAX);
      out << "\t\t" << "neg" << "\t" << "rax" << endl;
      store(RAX, q->sym3);
      break;

    case q_rplus:
      fetch_float(q->sym1);
      fetch_float(q->sym2);
      out << "\t\t" << "faddp" << endl;
      store_float(q->sym3);
      break;

    case q_iplus:
      fetch(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "add" << "\t" << "rax, rcx" << endl;
      store(RAX, q->sym3);
      break;

    case q_rminus:
      fetch_float(q->sym1);
      fetch_float(q->sym2);
      out << "\t\t" << "fsubp" << endl;
      store_float(q->sym3);
      break;

    case q_iminus:
      fetch(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "sub" << "\t" << "rax, rcx" << endl;
      store(RAX, q->sym3);
      break;

    case q_ior: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      fetch(q->sym1, RAX);
      out << "\t\t" << "cmp" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jne" << "\t" << "L" << label << endl;
      fetch(q->sym2, RAX);
      out << "\t\t" << "cmp" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jne" << "\t" << "L" << label << endl;
      // False branch
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // True branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_iand: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      fetch(q->sym1, RAX);
      out << "\t\t" << "cmp" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "je" << "\t" << "L" << label << endl;
      fetch(q->sym2, RAX);
      out << "\t\t" << "cmp" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "je" << "\t" << "L" << label << endl;
      // True branch
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // False branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_rmult:
      fetch_float(q->sym1);
      fetch_float(q->sym2);
      out << "\t\t" << "fmulp" << endl;
      store_float(q->sym3);
      break;

    case q_imult:
      fetch(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "imul" << "\t" << "rax, rcx" << endl;
      store(RAX, q->sym3);
      break;

    case q_rdivide:
      fetch_float(q->sym1);
      fetch_float(q->sym2);
      out << "\t\t" << "fdivp" << endl;
      store_float(q->sym3);
      break;

    case q_idivide:
      fetch(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "cqo" << endl;
      out << "\t\t" << "idiv" << "\t" << "rax, rcx" << endl;
      store(RAX, q->sym3);
      break;

    case q_imod:
      fetch(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "cqo" << endl;
      out << "\t\t" << "idiv" << "\t" << "rax, rcx" << endl;
      store(RDX, q->sym3);
      break;

    case q_req: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      fetch_float(q->sym1);
      fetch_float(q->sym2);
      out << "\t\t" << "fcomip" << "\t" << "ST(0), ST(1)" << endl;
      // Clear the stack
      out << "\t\t" << "fstp" << "\t" << "ST(0)" << endl;
      out << "\t\t" << "je" << "\t" << "L" << label << endl;
      // False branch
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // True branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_ieq: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      fetch(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "cmp" << "\t" << "rax, rcx" << endl;
      out << "\t\t" << "je" << "\t" << "L" << label << endl;
      // False branch
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // True branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_rne: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      fetch_float(q->sym1);
      fetch_float(q->sym2);
      out << "\t\t" << "fcomip" << "\t" << "ST(0), ST(1)" << endl;
      // Clear the stack
      out << "\t\t" << "fstp" << "\t" << "ST(0)" << endl;
      out << "\t\t" << "jne" << "\t" << "L" << label << endl;
      // False branch
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // True branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_ine: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      fetch(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "cmp" << "\t" << "rax, rcx" << endl;
      out << "\t\t" << "jne" << "\t" << "L" << label << endl;
      // False branch
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // True branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_rlt: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      // We need to push in reverse order for this to work
      fetch_float(q->sym2);
      fetch_float(q->sym1);
      out << "\t\t" << "fcomip" << "\t" << "ST(0), ST(1)" << endl;
      // Clear the stack
      out << "\t\t" << "fstp" << "\t" << "ST(0)" << endl;
      out << "\t\t" << "jb" << "\t" << "L" << label << endl;
      // False branch
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // True branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_ilt: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      fetch(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "cmp" << "\t" << "rax, rcx" << endl;
      out << "\t\t" << "jl" << "\t" << "L" << label << endl;
      // False branch
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // True branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_rgt: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      // We need to push in reverse order for this to work
      fetch_float(q->sym2);
      fetch_float(q->sym1);
      out << "\t\t" << "fcomip" << "\t" << "ST(0), ST(1)" << endl;
      // Clear the stack
      out << "\t\t" << "fstp" << "\t" << "ST(0)" << endl;
      out << "\t\t" << "ja" << "\t" << "L" << label << endl;
      // False branch
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // True branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_igt: {
      int label = sym_tab->get_next_label();
      int label2 = sym_tab->get_next_label();

      fetch(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "cmp" << "\t" << "rax, rcx" << endl;
      out << "\t\t" << "jg" << "\t" << "L" << label << endl;
      // False branch
      out << "\t\t" << "mov" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "jmp" << "\t" << "L" << label2 << endl;
      // True branch
      out << "L" << label << ":" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, 1" << endl;

      out << "L" << label2 << ":" << endl;
      store(RAX, q->sym3);
      break;
    }
    case q_rstore:
    case q_istore:
      fetch(q->sym1, RAX);
      fetch(q->sym3, RCX);
      out << "\t\t" << "mov" << "\t" << "[rcx], rax" << endl;
      break;

    case q_rassign:
    case q_iassign:
      fetch(q->sym1, RAX);
      store(RAX, q->sym3);
      break;

    case q_param:
      /* Your code here */
      fetch(q->sym1, RAX);
      out << "\t\t" << "push" << "\t" << "rax" << endl;
      break;

    case q_call: {
      /* Your code here */
      symbol *sym = sym_tab->get_symbol(q->sym1);
      if (sym->tag == SYM_PROC) {
        out << "\t\t" << "call" << "\t" << "L"
            << sym->get_procedure_symbol()->label_nr << "\t" << "# "
            << sym_tab->pool_lookup(sym->id) << endl;

      } else if (sym->tag == SYM_FUNC) {
        out << "\t\t" << "call" << "\t" << "L"
            << sym->get_function_symbol()->label_nr << "\t" << "# "
            << sym_tab->pool_lookup(sym->id) << endl;

      } else {
        fatal("code_generator::expand(): q_call quadruple produced.");
        return;
      }
      if (q->int2 > 0) {
        out << "\t\t" << "add" << "\t" << "rsp, " << q->int2 * 8 << endl;
      }
      if (q->sym3 != NULL_SYM) {
        store(RAX, q->sym3);
      }
      break;
    }
    case q_rreturn:
    case q_ireturn:
      fetch(q->sym2, RAX);
      out << "\t\t" << "jmp" << "\t" << "L" << q->int1 << endl;
      break;

    case q_lindex:
      array_address(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "imul" << "\t" << "rcx, " << STACK_WIDTH << endl;
      out << "\t\t" << "sub" << "\t" << "rax, rcx" << endl;
      store(RAX, q->sym3);
      break;

    case q_rrindex:
    case q_irindex:
      array_address(q->sym1, RAX);
      fetch(q->sym2, RCX);
      out << "\t\t" << "imul" << "\t" << "rcx, " << STACK_WIDTH << endl;
      out << "\t\t" << "sub" << "\t" << "rax, rcx" << endl;
      out << "\t\t" << "mov" << "\t" << "rax, [rax]" << endl;
      store(RAX, q->sym3);
      break;

    case q_itor: {
      block_level level; // Current scope level.
      int offset;        // Offset within current activation record.

      find(q->sym1, &level, &offset);
      frame_address(level, RCX);
      out << "\t\t" << "fild" << "\t" << "qword ptr [rcx";
      if (offset >= 0) {
        out << "\t\t" << "+" << offset;
      } else {
        out << offset; // Implicit "-"
      }
      out << "]" << endl;
      store_float(q->sym3);
    } break;

    case q_jmp:
      out << "\t\t" << "jmp" << "\t" << "L" << q->int1 << endl;
      break;

    case q_jmpf:
      fetch(q->sym2, RAX);
      out << "\t\t" << "cmp" << "\t" << "rax, 0" << endl;
      out << "\t\t" << "je" << "\t" << "L" << q->int1 << endl;
      break;

    case q_labl:
      // We handled this one above already.
      break;

    case q_nop:
      // q_nop quads should never be generated.
      fatal("code_generator::expand(): q_nop quadruple produced.");
      return;
    }

    // Get the next quad from the list.
    q = ql_iterator->get_next();
  }

  // Flush the generated code to file.
  out << flush;
}
