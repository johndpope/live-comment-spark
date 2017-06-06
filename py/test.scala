#import module
import re
from operator import add

#read input file
file_in = sc.textFile('/home/spark/WordCount.txt')

#count lines
print 'number of lines in file: %s' % file_in.count()

#add up lenths of each line
chars = file_in.map(lambda s: len(s)).reduce(add)
print 'number of cjaracters in file: %s' % chars

#get words from the input file
words = file_in.flatMap(lambda line: re.split('\W+',line.lower().strip()))

#words of more than 3 characters
words = words.filter(lambda x: len(x) > 3)

#set count 1 per word 
words = words.map(lambda w: (w,1))

#reduce phase - sum count all the words 
words = words.reduceByKey(add)

#create tuple (count,words) and sort in descending
words = words.map(lambda x: (x[1],x[0])).sortByKey(False)

words.take(10)
