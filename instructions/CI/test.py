#!/usr/bin/env python3

from junit_xml import TestSuite, TestCase
import subprocess
import time

def test(name, cl, cwd, cmd, expected, errorExpected=False):
  status = True
  t1 = time.time()
  try:
    output = subprocess.check_output(cmd, cwd=cwd, stderr=subprocess.STDOUT)
  except subprocess.CalledProcessError as e:
    output = e.output
    status = False
  t2 = time.time()
  t = t2 - t1
  output=output.decode('ascii')
  if (not status) and (not errorExpected):
    tc=TestCase(name, cl, t, output.strip())
    tc.error_message = "Error: Processes returned with non-zero status code"
    return tc
  elif status and errorExpected:
    tc=TestCase(name, cl, t, output.strip())
    tc.error_message = "Error: Processes did not return with non-zero status code"
    return tc
  with open("tmp","w") as f:
    f.write(output)
  tc=TestCase(name, cl, t, output.strip())
  try:
    subprocess.check_output(["diff","-ub",expected,"tmp"], stderr=subprocess.STDOUT)
  except subprocess.CalledProcessError as e:
    tc.failure_message = "Unexpected output"
    tc.failure_output = e.output.decode("ascii").strip()
  return tc

subprocess.check_output(["make","-C","scan"])
subprocess.check_output(["make","-C","symtab"])
subprocess.check_output(["make","-C","remaining"])

test_cases = [
  test("lab1", "lab1", "scan", ["./scanner","../testpgm/scannertest1.d"], "trace/scannertest1.trace"),
  test("lab2a", "lab2", "symtab", ["./symtab","a"], "trace/symtab2a.trace"),
  test("lab2b", "lab2", "symtab", ["./symtab","b"], "trace/symtab2b.trace"),
  test("lab2c", "lab2", "symtab", ["./symtab","c"], "trace/symtab2c.trace"),
  test("lab3.1", "lab3", "remaining", ["./diesel","-a","-b","-c","-f","-p","../testpgm/parstest1.d"], "trace/parstest1.trace"),
  test("lab3.2", "lab3", "remaining", ["./diesel","-a","-b","-c","-f","-p","../testpgm/parstest2.d"], "trace/parstest2.trace", errorExpected=True),
  test("lab3.3", "lab3", "remaining", ["./diesel","-a","-b","-c","-f","-p","../testpgm/parstest3.d"], "trace/parstest3.trace"),
  test("lab4.1", "lab4", "remaining", ["./diesel","-a","-b","-f","-p","-y","../testpgm/semtest1.d"], "trace/semtest1.trace"),
  test("lab4.2", "lab4", "remaining", ["./diesel","-a","-b","-f","-p","-y","../testpgm/semtest2.d"], "trace/semtest2.trace", errorExpected=True),
  test("lab5", "lab5", "remaining", ["./diesel","-a","-b","-p","../testpgm/opttest1.d"], "trace/opttest1.trace"),
  test("lab6", "lab6", "remaining", ["./diesel","-b","-q","-y","../testpgm/quadtest1.d"], "trace/quadtest1.trace"),
  test("lab7", "lab7", "remaining", ["./diesel","-y","../testpgm/codetest1.d"], "trace/codetest1.trace")
]
ts = TestSuite("TDDE66 Labs", test_cases)
with open('output.xml', 'w') as f:
  TestSuite.to_file(f, [ts], prettyprint=False)
