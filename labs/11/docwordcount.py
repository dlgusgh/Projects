import sys
import re

from pyspark import SparkContext

def flat_map(document):
    """
    Takes in document, which is a key, value pair, where document[0] is the
    document ID and document[1] is the contents of the document. We want
    a distinct list of all the words in each document to pass in to our map
    function, but if we call the transformation distinct() on all the words from
    all the documents with the current code, we won't get quite what we want. How
    can we use the document ID to help get around that problem?
    """
    """ Your code here. """
    a = []
    for w in re.findall(r"\w+", document[1]):
        if w not in a:
            a.append(w) 

    return a

def map(arg1):
    """ Your code here. """
    return (arg1, 1)

def reduce(arg1, arg2):
    """ Your code here. """
    return arg1 + arg2

def docwordcount(file_name, output="wc-out-docwordcount"):
    sc = SparkContext("local[8]", "DocWordCount")
    file = sc.sequenceFile(file_name)

    counts = file.flatMap(flat_map) \
                 .map(map) \
                 .reduceByKey(reduce).sortBy(lambda x: x[0].lower())

    counts.coalesce(1).saveAsTextFile(output)

""" Do not worry about this """
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 2:
        docwordcount(argv[1])
    else:
        docwordcount(argv[1], argv[2])



