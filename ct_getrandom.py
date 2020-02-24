import zymkey
from textwrap import fill

print('Turning LED on...')
zymkey.client.led_on()

print('Testing get_random() with 512 bytes...')
num = 512
random_bytes = zymkey.client.get_random(num)
s = fill(' '.join('{:02X}'.format(c) for c in random_bytes), 49)
print(s)

print('Turning LED off...')
zymkey.client.led_off()

