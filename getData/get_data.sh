
#! /bin/sh

scidPath="../data/scid/"$1
trainDataPath="../data/train_data/"$1
tmpPath="tmp"
if [ ! -d "$trainDataPath" ];then
  mkdir $trainDataPath
  echo "-------------------mkdir path: ${trainDataPath}-----------------------"
fi

echo "-------------------START------------------------"
for scidfile in `ls $scidPath/* -B`
do
  python multi_getLiveComment.py $scidfile $1
done

cat ${tmpPath}/$1* > ${trainDataPath}/train.dat
