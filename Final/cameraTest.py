from picamera import PiCamera
from time import sleep
import cv2 as cv
from fer import FER
import paho.mqtt.client as mqtt

broker_address="192.168.0.117" 

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe("$SYS/#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client = mqtt.Client("Emotion") #create new instance
client.connect(broker_address) #connect to broker

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
#client.loop_forever()

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
	#image = cv2.imread(imagePath)
	gray = cv.cvtColor(selfie, cv.COLOR_BGR2GRAY)
	faceCascade = cv.CascadeClassifier(cv.data.haarcascades + "haarcascade_frontalface_default.xml")
	faces = faceCascade.detectMultiScale(
	  gray,
	  scaleFactor=1.3,
	  minNeighbors=3,
	  minSize=(30, 30)
	)

	#print("Found {0} Faces!".format(len(faces)))
	if len(faces) > 0:
		emotion, score = detector.top_emotion(selfie)
		print(emotion)
		client.publish("data/emotion",emotion)#publish
	else:
		print("Face not found.")
