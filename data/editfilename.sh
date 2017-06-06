#! /bin/sh
for eachfile in `ls ./20161114 -B`
do
 filename_new=`echo ${eachfile%.dat}`
  #filename=${eachfile%.dat}
   #filehead=`echo $filename | awk -F _ '{print $1 }'`
    #filelast=`awk -F '.' 'gsub(/\n/,"",$1) {print $0}'`
 echo $filename_new
 content=`cat ./20161114/${eachfile%.dat}.dat | sort | uniq | xargs`
 echo -e ${filename_new}'\t'${content}'\n' >> ./20161114/train.dat
done
