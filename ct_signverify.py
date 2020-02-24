from __future__ import print_function

import zymkey
import hashlib

from zymkey.exceptions import VerificationError

secret_message = 'Hi Alice,--Bob'
# Flash the LED to indicate the operation is underway
zymkey.client.led_flash(500, 100)
print('Signing data...', end='')
signature = zymkey.client.sign(secret_message)
print('OK')

print('Verifying data...', end='')
zymkey.client.verify(secret_message, signature)
print('OK')

print('Verifying tainted data...', end='')
try:
    zymkey.client.verify(secret_message.replace('Alice', 'Eve'), signature)
except VerificationError:
    print('FAIL, yay!')
else:
    raise Exception('verification should have failed, but passed')


