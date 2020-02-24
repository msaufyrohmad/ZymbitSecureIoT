import zymkey
from textwrap import fill


zymkey.client.led_flash(500, 100)

print('Testing zkCreateRandDataFile with 1MB...')
num = 1024 * 1024
file_path = '/tmp/random.bin'
zymkey.client.create_random_file(file_path, num)
print(file_path," created with random values")
print('Turning LED off...')
zymkey.client.led_off()

