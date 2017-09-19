import cv2
import numpy as np

QF = 33
if cv2.__version__ == '2.4.8':
    bgr2ycrbr = cv2.cv.CV_BGR2YCrCb
    ycrbr2bgr = cv2.cv.CV_YCrCb2BGR
else:
    bgr2ycrbr = cv2.COLOR_BGR2YCrCb
    ycrbr2bgr = cv2.COLOR_YCrCb2BGR


QY=np.array([[16,11,10,16,24,40,51,61],
                 [12,12,14,19,26,48,60,55],
                 [14,13,16,24,40,57,69,56],
                 [14,17,22,29,51,87,80,62],
                 [18,22,37,56,68,109,103,77],
                 [24,35,55,64,81,104,113,92],
                 [49,64,78,87,103,121,120,101],
                 [72,92,95,98,112,100,103,99]])

QC=np.array([[17,18,24,47,99,99,99,99],
                 [18,21,26,66,99,99,99,99],
                 [24,26,56,99,99,99,99,99],
                 [47,66,99,99,99,99,99,99],
                 [99,99,99,99,99,99,99,99],
                 [99,99,99,99,99,99,99,99],
                 [99,99,99,99,99,99,99,99],
                 [99,99,99,99,99,99,99,99]])

b_size = 8
