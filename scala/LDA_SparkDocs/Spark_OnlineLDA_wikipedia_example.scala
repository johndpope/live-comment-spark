import org.apache.spark.ml.feature.{CountVectorizer, RegexTokenizer, StopWordsRemover}
import org.apache.spark.mllib.clustering.{LDA, OnlineLDAOptimizer}
import org.apache.spark.mllib.linalg.Vector

import sqlContext.implicits._

val numTopics: Int = 100
val maxIterations: Int = 100
val vocabSize: Int = 10000

/**
 * Data Preprocessing
 */
// Load the raw articles, assign docIDs, and convert to DataFrame
val rawTextRDD = ... // loaded from S3
val docDF = rawTextRDD.zipWithIndex.toDF("text", "docId")

// Split each document into words
val tokens = new RegexTokenizer()
  .setGaps(false)
  .setPattern("\\p{L}+")
  .setInputCol("text")
  .setOutputCol("words")
  .transform(docDF)

// Filter out stopwords
val stopwords: Array[String] = ... // loaded from S3
val filteredTokens = new StopWordsRemover()
  .setStopWords(stopwords)
  .setCaseSensitive(false)
  .setInputCol("words")
  .setOutputCol("filtered")
  .transform(tokens)

// Limit to top `vocabSize` most common words and convert to word count vector features
val cvModel = new CountVectorizer()
  .setInputCol("filtered")
  .setOutputCol("features")
  .setVocabSize(vocabSize)
  .fit(filteredTokens)
val countVectors = cvModel.transform(filteredTokens)
  .select("docId", "features")
  .map { case Row(docId: Long, countVector: Vector) => (docId, countVector) }
  .cache()

/**
 * Configure and run LDA
 */
val mbf = {
  // add (1.0 / actualCorpusSize) to MiniBatchFraction be more robust on tiny datasets.
  val corpusSize = countVectors.count()
  2.0 / maxIterations + 1.0 / corpusSize
}
val lda = new LDA()
  .setOptimizer(new OnlineLDAOptimizer().setMiniBatchFraction(math.min(1.0, mbf)))
  .setK(numTopics)
  .setMaxIterations(2)
  .setDocConcentration(-1) // use default symmetric document-topic prior
  .setTopicConcentration(-1) // use default symmetric topic-word prior

val startTime = System.nanoTime()
val ldaModel = lda.run(countVectors)
val elapsed = (System.nanoTime() - startTime) / 1e9

/**
 * Print results.
 */
// Print training time
println(s"Finished training LDA model.  Summary:")
println(s"Training time (sec)\t$elapsed")
println(s"==========")

// Print the topics, showing the top-weighted terms for each topic.
val topicIndices = ldaModel.describeTopics(maxTermsPerTopic = 10)
val vocabArray = cvModel.vocabulary
val topics = topicIndices.map { case (terms, termWeights) =>
  terms.map(vocabArray(_)).zip(termWeights)
}
println(s"$numTopics topics:")
topics.zipWithIndex.foreach { case (topic, i) =>
  println(s"TOPIC $i")
  topic.foreach { case (term, weight) => println(s"$term\t$weight") }
  println(s"==========")
}
