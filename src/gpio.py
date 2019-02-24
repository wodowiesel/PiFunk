
import RPi.GPIO as GPIO
# setmode ermöglicht die Einstellung welche GPIO-Bezeichnung verwendet werden soll, hier werden die Pinnummern der Stiftleiste benutzt
GPIO.setmode(GPIO.BOARD)
# setup setzt die Funktion des Bezeichneten GPIO, hier 11 als Eingang und 12 als Ausgang
GPIO.setup(11, GPIO.IN)
GPIO.setup(12, GPIO.OUT)
# input gibt den Pegel aus, der am Eingang anliegt, hier abfrage des Pegesl GPIO 11 und Ablage in der Variable "input_value"
input_value = GPIO.input(11)
# output setzt den Pegel des GPIO, hier GPIO 12 auf high
GPIO.output(12, GPIO.HIGH)
# Das gleiche wie oben, allerdings werden die vorgegebenen GPIO-Bezeichnungen des Chipherstellers benutzt
GPIO.setmode(GPIO.BCM)
GPIO.setup(17, GPIO.IN)
GPIO.setup(18, GPIO.OUT)
input_value = GPIO.input(17)
GPIO.output(18, GPIO.HIGH)

