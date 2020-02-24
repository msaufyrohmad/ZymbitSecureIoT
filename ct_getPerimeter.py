import zymkey
import time

while True:
    time.sleep(1)
    acc=zymkey.client.get_perimeter_detect_info()
    print acc

