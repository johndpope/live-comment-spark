# get data
python ../getData/multi_getComment_test.py $1 > ../getData/tmp/$1.dat

# wordseg
cd ../src/jieba/
sh wordseg.sh ../../getData/tmp/$1.dat

# build data
cat ./res/$1*gemsim* | awk -f "\n" '{print NR" "$1}' > ../../data/libsvm/$1_libsvm.data

# spark
cd ../../py/pyspark
sh run.sh PySparkMl_3.py ../../data/libsvm/$1_libsvm.data $1.out

cp $1.out ../../data/res/

cd ../../data/res

grep "^|" $1.out > $1.txt
sed -i "s/  / /g" $1.txt
python getWordsFromTopics_simple.py $1.txt
