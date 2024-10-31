The Lab Environment
===================

Important: Checking out the Lab Skeleton
----------------------------------------

To work with the labs and hand in your results, you will need to use
git.
Start by creating a private fork of the course `repository <https://gitlab.liu.se/tdde66-student/TDDE66>`_,
using the `gitlab <https://gitlab.liu.se>`_ web interface; give your lab assistant access to the repository.
You might need to read some of the `help <https://gitlab.liu.se/help/#getting-started-with-gitlab>`_
to get started with git.

.. warning ::

  Creating a public copy of the code is considered cheating; as is using another lab group's code.
  See the `department's rules <https://www.ida.liu.se/edu/ugrad/lab_exam/index.en.shtml>`_ for computer lab assignments.
  The `university's rules <https://www.student.liu.se/studenttjanster/lagar-regler-rattigheter/disciplinarenden/fusk?l=en>`_ require teachers to report any *suspected* attempts of cheating.

.. note ::

  It is important that your work is based on the current year's branch
  (the default branch at the course start) as it greatly simplifies
  correction of the labs.

.. note ::

  You need to document the changes you perform on the code via frequent
  git commits.
  For example, make a commit at the end of each coding session documenting
  what you did.
  Also make a commit for each milestone you complete in a lab (tasks in
  a lab).
  This makes it easier for you to demonstrate your work as well as making
  it easy to revert changes that did not work (by going back to a working
  commit or seeing what changes you made since the last commit).

g++ vs gcc
----------

In the course, the compiler ``g++`` will be used. ``g++`` is really just
a script which calls the standard GNU compiler ``gcc`` with options to
recognize C++.

To compile the DIESEL compiler the ``make`` command is used and the
requisite parts of the compiler will be compiled. ``make`` ensures that
the relevant files are compiled with the ``-g`` flag to generate
debugging information. Another very useful option is ``-Wall``, which
makes the compiler generate warnings that might otherwise escape notice.
When recompiling the files produced during the last compile can be
removed with ``make clean``. Take a peek at the Makefiles you use in the
various labs for more information.

After lab 3, executable file gets the name ``compiler`` but is run using
the command ``diesel``, which is a shell script serving as a compiler
wrapper, handling flags and a bunch of other things. Look at it for more
information. To learn more about the extensive options and capabilities
of the GNU gcc compiler, refer to the man pages for ``g++`` and ``gcc``,
respectively.

For each lab, there is also a target like ``make lab1``, which will
compile the compiler, run the compiler on the test and compare it to a
trace-file. This is faster than comparing the compiler output by hand,
but does not necessarily mean that the compiler is working correctly.
Depending on optimizations, the output may be different but the compiler
is still working correctly. So use the trace-files, but think a bit what
the results mean and if the compiler will work for all valid programs.

.. _third_party_doc:

Third-party documentation
-------------------------

The online documentation of software used in the labs is included here:

* `The flex manual <_static/flex/index.html>`_
* `The bison manual <_static/bison/index.html>`_
* `The GCC manual <_static/gcc.html>`_
* `The x86 instruction set <_static/x86doc>`_

Lab directory structure
-----------------------

The directories are organized as follows:

scan
   contains all files necessary to complete the first lab.

symtab
   contains all files necessary to complete the second lab.

remaining
   contains all the files necessary to complete the entire lab course.
   You will be using these files from lab 3 (parsing) and onwards,
   adding things to new files as required. The individual lab chapters
   state which files(s) should be edited at what point during the lab
   course.

testpgm
   contains a multitude of DIESEL programs which you can (and will) use
   to test your compiler. The individual labs suggest files to use, but
   it never hurts to test as rigorously as possible, as early as
   possible.

trace
   contains the output from the test programs for the labs.

To help you many of the files in ``scan`` and ``symtab`` are symbolic
links to ``remaining``. In that way they all share the same contents and
you won’t have to copy or move around any files.

Also note that even though you won’t be using the same environment
(Makefiles, main programs, etc) for all the labs, you *should* use your
completed versions of files in subsequent labs. Again, this is explained
in more detail in the appropriate lab chapters. A brief description of
all the skeleton files follows:

scanner.l
   This is the input file to the scanner generator ``flex``, and you
   will complete it in the first lab. Use it in all subsequent labs.

scanner.hh
   This is a header file which is only used in the first and second
   labs. The file called

parser.hh
   is used from lab 3 on.

scanner.cc
   This file is automatically generated by ``flex`` during compilation.
   It won’t be very easy to comprehend, but feel free to take a peek at
   it to see what a generated scanner can look like.

scantest.cc
   This file is only used in the first lab, as a test program.

symtab.hh, symbol.cc, symtab.cc
   These files are concerned with the symbol table. You will study them
   in lab 2, and return to them occasionally. They are included in all
   the labs. Use them in all subsequent labs.

symtabtest.cc
   This file is only used in lab 2 during symbol table testing.

parser.y
   This is the input file to the parser generator ``bison``. You will
   complete it in lab 3. It should be used from then on.

parser.hh
   This header file will replace ``scanner.hh`` from lab 3 on. It is
   automatically generated by ``bison`` during compilation.

parser.cc
   This file is automatically generated by ``bison`` during compilation.
   It is the parser implementation, and is if possible even harder to
   read than ``scanner.cc``.

semantic.hh, semantic.cc
   These files are concerned with type checking and the like. They are
   edited in lab 4, but are included in compilation from lab 3 onwards.

ast.hh, ast.cc
   These files concern abstract syntax trees, and while you should not
   need to edit them, you will have to study them in close detail from
   lab 3 on.

optimize.hh, optimize.cc
   These files are concerned with optimizing of the program’s internal
   form. They are edited in lab 5, but are included in compilation from
   lab 3 on.

quads.hh, quads.cc
   These files implement quadruple generation. They are edited in lab 6,
   but are included in compilation from lab 3 on.

codegen.hh, codegen.cc
   These files translate quadruples to assembler code. They are edited
   in the last lab, but are included in compilation from lab 3 on.

error.hh, error.cc
   These files contain error handling code. You will never need to edit
   them. They are used in all the labs.

main.cc
   This file ties all the parts of the compiler together, handles
   command-line parsing and starts the compiler. It is used from lab 3
   on.

compiler
   This is the binary file which will be generated when you type
   ``make`` from lab 3 on.

diesel
   A shell script used as a wrapper around the ``compiler`` binary file.
   Use it when you want to try to compile a DIESEL program. It accepts
   lots of flags to control debug information; see the comments in the
   file, and also below.

Makefile
   This file handles the compiling of your compiler. Different Makefiles
   are used in lab 1, 2 and 3. From lab 3 on, you can use the same one,
   though.

Files ending with d
   These are DIESEL source code files, which you can try to compile when
   testing your compiler.

Files ending with .trace
   These files contain listings of output generated when compiling a
   certain file (the file ``x.trace`` contains the result of compiling
   the file ``x.d``, etc). Compare your output to them to see if your
   compiler works correctly.

Using the diesel script
-----------------------

The script accepts a multitude of flags which control how far to
continue the compiling, as well as how much debug information to
generate. The script is used from lab 3 (parsing) and onwards. The
following flags are available (this list can also be found at the top of
the script file):

-  ``-a`` : print abstract syntax tree to stdout for each code block at
   compile time.

-  ``-b`` : do not generate a binary file.

-  ``-c`` : do not perform type checking.

-  ``-d`` : turn on bison debugging (spam warning).

-  ``-e`` : run the compiler through gdb to obtain the backtrace of a
   crash.

-  ``-f`` : do not optimize the abstract syntax tree.

-  ``-o <outfile>`` : place the binary executable file in ``<outfile>``
   instead of “``a.out``”.

-  ``-p`` : do not generate quads; stop after optimization.

-  ``-q`` : print quad lists to stdout at compile time.

-  ``-s`` : do not generate assembler code; stop after quads.

-  ``-t`` : include quad trace printouts in the assembler output file
   (“``d.out``”).

-  ``-y`` : print symbol table to stdout at compile time.

Below follows some examples of “standard” flags for the various labs.
They are only examples, but will probably work well unless you desire
more specific information on something. Experiment with the flags to
make sure you understand what they are useful for; at certain points you
will want to use other ones than these.

Also note that if you don’t include the ``-b`` flag in any lab but the
last, you might get an error message since the ``diesel`` script will
try to generate a binary file from assembler code that doesn’t exist or
is faulty.

+--------------------+-----------------------------------------+
| lab 3 (parsing)    | ``diesel -a -b -c -f -p -y testfile.d`` |
+--------------------+-----------------------------------------+
| lab 4 (semantic)   | ``diesel -a -b -f -p -y testfile.d``    |
+--------------------+-----------------------------------------+
| lab 5 (optimizing) | ``diesel -a -b -p -y testfile.d``       |
+--------------------+-----------------------------------------+
| lab 6 (quads)      | ``diesel -b -q -y testfile.d``          |
+--------------------+-----------------------------------------+
| lab 7 (codegen)    | ``diesel -t -y testfile.d``             |
+--------------------+-----------------------------------------+


.. _handing-in-results:

Handing in Results
------------------

It is preferable to first perform a demonstration of the lab to your
lab assistant in person. After the demonstration is approved,
hand in results by sharing your private gitlab
repository with your lab assistant, creating a branch called for example
lab1 containing your changes for lab1 and emailing your lab assistant
that you have prepared the code for review.
