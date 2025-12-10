# CMake generated Testfile for 
# Source directory: D:/OOP_LABS/lab7
# Build directory: D:/OOP_LABS/lab7/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(MyProjectTests "D:/OOP_LABS/lab7/build/tests.exe")
set_tests_properties(MyProjectTests PROPERTIES  _BACKTRACE_TRIPLES "D:/OOP_LABS/lab7/CMakeLists.txt;37;add_test;D:/OOP_LABS/lab7/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
