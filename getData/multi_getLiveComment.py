# -*- coding: utf-8 -*-
import  sys
reload(sys)
sys.setdefaultencoding('utf-8')

import  json
import  urllib2
import requests
from  multiprocessing import Pool
import  pycurl
import  StringIO

def getUrl(url):
#  url="http://mp.weixin.qq.com/s?__biz=MjM5ODEzMTIwMA==&mid=2655956935&idx=2&sn=7e22372ca2c8ba5704bbe75e74b8024a#rd"
  urlIO=StringIO.StringIO()
  url_curl=pycurl.Curl()
  url_curl.setopt(pycurl.URL, url)
  url_curl.setopt(pycurl.HTTPHEADER, ["Accept:"])
  url_curl.setopt(pycurl.WRITEFUNCTION, urlIO.write)
  url_curl.setopt(pycurl.FOLLOWLOCATION, 1)
  url_curl.setopt(pycurl.MAXREDIRS, 5)
  url_curl.perform()
  res=urlIO.getvalue()
#  print url_curl.getinfo(url_curl.HTTP_CODE) 
  urlIO.close()
  return res

def getComments(scid):
  ts = 0
#  f2 = open('./data/20161114/' + scid + '.dat','w')             #评论内容文件
  while 1:
    url ="http://wb.yizhibo.com/live/h5api/get_playback_event?scid=" + scid +"&ts=" + str(ts)
    json_obj = getUrl(url)
    res = json.loads(json_obj)
    if res['result'] == 401 or res['result'] == 402:
      break
    elif res['data']['list']:
      for x in res['data']['list']:
#        print x['content']
        f2.write(x['content']+"\n")
    else:
      break
    ts = int(ts)+4000
#  f2.close()
#    print url+"---->"+res_uid
#    res_uid = json.loads(json_obj)

    #解决json格式编码utf-8乱码
#    res_uid = json.dumps(res_uid, ensure_ascii=False).encode('utf-8')
#    if 'error_code' in res_uid:

if len(sys.argv) < 3:
  print "need argv: [in_url_file] [date]"
#scid = sys.argv[1]             # 返回一个文件对象
f = open(sys.argv[1])             # 返回一个文件对象
lines = f.readlines()
f.close()
for scid in lines:
    scid = scid.strip('\n');
    global f2
    f2 = open('./tmp/'+ sys.argv[2] + scid + '.dat','w')             #评论内容文件
    getComments(scid)
    f2.close()

#pool = Pool()
#pool.map(getComments,lines)
#pool.close()
#
#f2.close()
