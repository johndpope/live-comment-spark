# CMake generated Testfile for 
# Source directory: /home/kovean/WORK/live-comment-spark/src/cppjieba-master
# Build directory: /home/kovean/WORK/live-comment-spark/src/jieba
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(./test/test.run "./test/test.run")
add_test(./load_test "./load_test")
add_test(./demo "./demo")
subdirs(deps)
subdirs(test)
