#!/usr/bin/awk -f
BEGIN {

	#print ARGC;
	Index[ARGC];#记录各个文件的行下标
	for(t=1;t<=ARGC;t++)  {
		Index[t]=0;	
	}
}
{
	#文件数ARGC-1,第一个参数是应用程序名awk.
	for(t=1;t<=ARGC;t++)  {
		
		if(FILENAME==ARGV[t])	{ 			
			line[t,Index[t]]=$0;#$0=整行,前提是各个文件行列之间已经被\t制表符分隔。
			#line[t,Index[t]]=sprintf("%s\t%s",$1,$2);#如果固定为几列，也可以用这个。
			Index[t]++;
		}

	}
}
END {
	maxcount=0;
	for(i=1;i<=ARGC;i++)  {
		if(Index[i]>maxcount) maxcount=Index[i];
	}

	#printf("maxcount:%d",maxcount);
	for(j=0;j<maxcount;j++)  {
		for(i=1;i<=ARGC;i++)  {
			#多个文件的当前行拼接成一行
			if(i==1) {#第一个文件
				if(length(line[i,j])==0)
					str="\t\t";#一般操作的文件都是两列
				else
					str=line[i,j];#第一次去掉制表符\t			
			}
			else  {#中间文件
				if(length(line[i,j])==0)   #中间行为空，将行内容替换为制表符\t
					str=sprintf("%s\t\t",str);
				else
					str=sprintf("%s\t%s",str,line[i,j]);	
			}
		}
		printf("%s\n",str);	
	}

}

