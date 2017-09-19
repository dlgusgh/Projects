import pyspark
from pyspark import SparkContext
import cv2
import numpy as np
import scipy as sp
import struct
from helper_functions import *
from constants import *

def do_Y_cb_cr_matrix(image):
    return [((image[0], x) , y) for x,y in enumerate(convert_to_YCrCb(image[1]))]

#image = ((image id, y/cr/cb id), imageY/cr/cb)
def do_sub_blocks(image):
    dimen = image[1].shape
    index_x = dimen[1]/b_size
    index_y = dimen[0]/b_size
    rt= []
    for y in range(0,index_y):
        for x in range(0,index_x):
            rt += [((image[0][0],image[0][1]),(dimen[0],dimen[1],y*b_size,x*b_size,image[1][y*b_size: y*b_size+b_size, x*b_size:x*b_size+b_size]))]
    return rt

def dct_block_1(image):
    rt_image = image[1][4].astype(np.float32) - 128 
    return (image[0],(image[1][0],image[1][1],image[1][2],image[1][3],dct_block(rt_image)))

def quantize_block_1(image):
    return (image[0],(image[1][0],image[1][1],image[1][2],image[1][3],quantize_block(image[1][4], not image[0][1],QF)))

def quantize_block_2(image):
    return (image[0],(image[1][0],image[1][1],image[1][2],image[1][3],quantize_block(image[1][4],not image[0][1],QF,True)))

def dct_block_2(image):
    block = dct_block(image[1][4],True)+128
    block[block<0],block[block>255] = 0,255
    return (image[0],(image[1][0],image[1][1],image[1][2],image[1][3],block,1))
#parts = [y dimension of whole image, x dimension of whole image, y location of image, x location of image, subpart,status]
#                   0                               1                          2                3               4      5
def combs_1(part1,part2):
    if (part1[5]):
        rt_matrix = np.zeros((part1[0],part1[1]), np.float32)
        rt_matrix[part1[2]:part1[2]+b_size,part1[3]:part1[3]+b_size]=part1[4]
    else:
        rt_matrix = part1[4]
    rt_matrix[part2[2]:part2[2]+b_size,part2[3]:part2[3]+b_size]=part2[4]
    return (part1[0],part1[1],part1[2],part1[3],rt_matrix,0)

def reduce_key(image):
    return (image[0][0],(image[0][1],image[1][0],image[1][1],image[1][4],1))


#parts = [y/cr/cb image?, y dimension of whole image, x dimension of whole image, whole image, status]
#              0                       1                            2                  3         4
def combs_2(part1,part2):
    if (part1[4]):
        if (part1[0]==0):
            rt_matrix = np.zeros((part1[1],part1[2],3),np.uint8)
            rt_matrix[:,:,0]=part1[3]
        else:
            rt_matrix = np.zeros((part1[1]*2,part1[2]*2,3),np.uint8)
            rt_matrix[:,:,part1[0]] = resize_image(part1[3],part1[2]*2,part1[1]*2)
    else:
        rt_matrix = part1[3]

    if (part2[0]==0):
        rt_matrix[:,:,0]=part2[3]
    else:
        rt_matrix[:,:,part2[0]] = resize_image(part2[3],part2[2]*2,part2[1]*2)
    return (3,part1[1],part1[2],rt_matrix,0)

def reduce_to_image(image):
    return (image[0],image[1][3])

### WRITE ALL HELPER FUNCTIONS ABOVE THIS LINE ###


def generate_Y_cb_cr_matrices(rdd):
    """
    THIS FUNCTION MUST RETURN AN RDD
    """

    return rdd.flatMap(do_Y_cb_cr_matrix)

def generate_sub_blocks(rdd):
    """
    THIS FUNCTION MUST RETURN AN RDD
    """
    return rdd.flatMap(do_sub_blocks)

def apply_transformations(rdd):
    """
    THIS FUNCTION MUST RETURN AN RDD
    """
    rdd = rdd.map(dct_block_1)
    rdd = rdd.map(quantize_block_1)
    rdd = rdd.map(quantize_block_2)
    return rdd.map(dct_block_2)

def combine_sub_blocks(rdd):
    """
    Given an rdd of subblocks from many different images, combine them together to reform the images.
    Should your rdd should contain values that are np arrays of size (height, width).

    THIS FUNCTION MUST RETURN AN RDD
    """
    ### BEGIN SOLUTION ###

    rdd = rdd.reduceByKey(combs_1)
    rdd = rdd.map(reduce_key)
    rdd = rdd.reduceByKey(combs_2)
    rdd = rdd.map(reduce_to_image)
    return rdd

def run(images):
    """
    THIS FUNCTION MUST RETURN AN RDD

    Returns an RDD where all the images will be proccessed once the RDD is aggregated.
    The format returned in the RDD should be (image_id, image_matrix) where image_matrix
    is an np array of size (height, width, 3).
    """
    sc = SparkContext()
    rdd = sc.parallelize(images, 16).map(truncate).repartition(16)
    rdd = generate_Y_cb_cr_matrices(rdd)
    rdd = generate_sub_blocks(rdd)
    rdd = apply_transformations(rdd)
    rdd = combine_sub_blocks(rdd)

    ### BEGIN SOLUTION HERE ###
    rdd = rdd.mapValues(to_rgb)
    # Add any other necessary functions you would like to perform on the rdd here
    # Feel free to write as many helper functions as necessary
    return  rdd
