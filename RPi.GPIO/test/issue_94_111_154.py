import time
import RPi.GPIO as GPIO

LED_PIN = 12

def issue_154():
    # fails with led off at around 400
    count = 0
    pinRef = GPIO.PWM(LED_PIN,50) # create new PWM instance
    while True:
        pinRef.start(10) # update PWM value
        time.sleep(0.05)
        pinRef.stop()
        GPIO.output(LED_PIN,0)
        time.sleep(0.05)
        count = count + 1
        print count

def issue_94(cycles):
    # led flickers.  Bug = LED stays off at around cycle 400
    pwm = GPIO.PWM(LED_PIN, 1)
    for i in xrange(cycles):
        print(i)
        pwm.ChangeFrequency(25)
        pwm.start(50)
        time.sleep(1)
        pwm.stop()

if __name__ == '__main__':
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(LED_PIN, GPIO.OUT)
    try:
#        issue_94(1000)
        issue_154()
    finally:
        GPIO.cleanup()
