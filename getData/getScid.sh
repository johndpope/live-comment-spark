#grep 'http://weibo.com/tv/l/' .html | awk -F '"' '{print $2}' | awk -F '/' '{print $6}' >  scids.20170228_star.dat 

#! /bin/sh

htmlPath="../data/html/"$1
scidPath="../data/scid/"$1
if [ ! -d "$scidPath" ];then
  mkdir $scidPath
  echo "-------------------mkdir path: ${scidpath}-----------------------"
fi

echo "-------------------START------------------------"
for eachfile in `ls $htmlPath/*.html -B`
do
  filename_new=`echo ${eachfile%.html} | awk -F "/" '{print $5}'`
  scidName=`echo scids.${1}_${filename_new}.dat`
  grep 'http://weibo.com/tv/l/' ${eachfile} | awk -F '"' '{print $2}' | awk -F '/' '{print $6}' >  $scidPath/$scidName
  #filename=${eachfile%.dat}
   #filehead=`echo $filename | awk -F _ '{print $1 }'`
    #filelast=`awk -F '.' 'gsub(/\n/,"",$1) {print $0}'`
  echo $scidPath/$scidName" finish"  
  #content=`cat ./20161114/${eachfile%.dat}.dat | sort | uniq | xargs`
  #echo -e ${filename_new}'\t'${content}'\n' >> ./20161114/train.dat
done
echo "-------------------FINISH-----------------------"
