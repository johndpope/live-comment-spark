# CMake generated Testfile for 
# Source directory: /home/yqk/live_comment_new/src/cppjieba-master
# Build directory: /home/yqk/live_comment_new/src/jieba
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(./test/test.run "./test/test.run")
add_test(./load_test "./load_test")
add_test(./demo "./demo")
subdirs(deps)
subdirs(test)
