from picamera import PiCamera
from time import sleep
import cv2 as cv
from fer import FER

#print(cv.__version__)
detector = FER()

cam = PiCamera()
#cam.start_preview()
while (1):
    sleep(1)
    cam.capture('/home/pi/Desktop/EE522/Final/selfie.jpg')
    selfie = cv.imread('/home/pi/Desktop/EE522/Final/selfie.jpg')
    sleep(1)
    #cam.stop_preview()
    #detector.detect_emotions(selfie)
    emotion, score = detector.top_emotion(selfie)
    print(emotion)
