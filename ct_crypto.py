from __future__ import print_function

import zymkey

from zymkey.exceptions import VerificationError

# Flash the LED to indicate the operation is underway
zymkey.client.led_flash(500, 100)

# Generate random blocks of 512 to fill a 1MB array
bs = 512
num_blocks = 256
print('Generating random block ({!r} bytes)...'.format(bs * num_blocks))
random_bytes = []
for x in range(num_blocks):
    random_bytes += zymkey.client.get_random(bs)

# Encrypt the random data
print('Encrypting random block...')
encrypted = zymkey.client.lock(random_bytes)

# Decrypt the random data
print('Decrypting encrypted block...')
decrypted = zymkey.client.unlock(encrypted)

decrypted_list = list(decrypted)
random_list = list(random_bytes)

if decrypted_list == random_list:
    print('PASS: Decrypted data matches original random data')
else:
    print('Decrypted data does not match original random data')

# Turn off the LED
zymkey.client.led_off()

print('Done!')
