import os 
import sys

def newline():
  print "\n"

def run_assemble(inputfile, log=False, flag=False):
  prefix = inputfile.split(".")[0]
  exe = "./assembler input/"

  writeint = " out/" + prefix + ".int" 
  writeout = " out/" + prefix + ".out"

  if flag:
    exe = "./assembler -p1 input/"
    writeout = ""

  command = exe + inputfile + writeint + writeout 
  if log:
    command += " -log log/" + prefix + ".txt"
  os.system(command)

def run_diff(inputfile, dir, file_type):
  prefix = inputfile.split(".")[0]
  command = "diff " + dir + "/" + prefix + "." + file_type + " " + dir \
    + "/ref/" + prefix + "_ref." + file_type
  os.system(command)

def run_tests(inputs, mode):
  if mode == "error":
    print "* ========== p1_errors.s ========== *"
    run_assemble(inputs[0], True, True)
    run_diff(inputs[0], "log", "txt")
    newline()

    print "* ========== p2_errors.s ========== *"
    run_assemble(inputs[1], True)
    run_diff(inputs[1], "log", "txt")
    newline()
  else:
    for inputfile in inputs:
      print "* ========== " + inputfile + " ========== *"
      run_assemble(inputfile)
      print "Running diff on ." + mode + " files...."
      run_diff(inputfile, "out", mode)
      newline()

def throw():
   print "Error: Invalid or no argument specified"
   print "Use the following: int out error" 

def main():
  # get all files in directory /input
  inputs = os.listdir("input");

  # append more test files to inputs here
  # inputs.append("test_file.s");
  # make sure you add appropriate results in out/ref/ using MARS

  if len(sys.argv) < 2:
    throw()
    return

  mode = sys.argv[1]
  if mode == "error":
    inputs = ["p1_errors.s", "p2_errors.s"]
  elif mode != "int" and mode != "out":
    throw()
    return
  else:
    inputs.remove("p1_errors.s")
    inputs.remove("p2_errors.s")

  # make the project
  os.system("make")
  newline()

  run_tests(inputs, mode)

main()
