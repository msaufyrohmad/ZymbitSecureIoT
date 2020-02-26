import time
import board
import adafruit_dht
import zymkey
import RPi.GPIO as GPIO
from textwrap import fill

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
buzzer=23
GPIO.setup(buzzer,GPIO.OUT)

    # Initial the dht device, with data pin connected to:
dhtDevice =adafruit_dht.DHT11(board.D24)
     
while True:
    try:
            # Print the values to the serial port
        temperature_c = dhtDevice.temperature
        temperature_f = temperature_c * (9 / 5) + 32
        humidity = dhtDevice.humidity
        print("Temp: {:.1f} F / {:.1f} C    Humidity: {}% ".format(temperature_f, temperature_c, humidity))
        plain=bytearray(temperature_c)
        print("Plain Text :",plain)
        
        cipher=zymkey.client.lock(plain)
        print("Cipher Text :",cipher)
    
        plain_check=zymkey.client.unlock(cipher)
        print("Original Text :",plain_check)

        if plain==plain_check:
            print("Zymbit lock unlock works! ")
            GPIO.output(buzzer,GPIO.HIGH)
        
    
    except RuntimeError as error:
        # Errors happen fairly often, DHT's are hard to read, just keep going
#	print(error.args[0])
        
        time.sleep(2.0)
