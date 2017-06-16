# CMake generated Testfile for 
# Source directory: /home/yqk/live-comment-spark-master/src/cppjieba-master
# Build directory: /home/yqk/live-comment-spark-master/src/jieba
# 
# This file includes the relevent testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(./test/test.run "./test/test.run")
ADD_TEST(./load_test "./load_test")
ADD_TEST(./demo "./demo")
SUBDIRS(deps)
SUBDIRS(test)
