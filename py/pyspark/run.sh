export JAVA_HOME=/usr/lib/jvm/java-1.7.0-openjdk-amd64
/bin/spark-2.1.0/bin/spark-submit \
	--packages "com.github.fommil.netlib:all:1.1.2,mysql:mysql-connector-java:5.1.41" \
	--driver-memory 4g \
	--executor-memory 15g \
	$1 $2\
	&> $3
