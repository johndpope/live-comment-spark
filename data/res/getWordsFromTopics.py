# -*- coding:utf-8 -*-
import sys

idf_file = open(sys.argv[1], 'r')
words_map = map(lambda x: x.strip('\n').split(' ')[0], idf_file.readlines())

topics_file = open(sys.argv[2], 'rw')

topics =  map(lambda x: x.strip('\n').split('|')[0].split(' ')[1].split(','), topics_file.readlines())
for topic in topics:
  words = ','.join(map(lambda x: words_map[int(x)], topic))
  print(words)
