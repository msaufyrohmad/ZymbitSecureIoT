import zymkey
import time

while True:
    time.sleep(1)
    acc=zymkey.client.get_accelerometer_data()
    print acc
