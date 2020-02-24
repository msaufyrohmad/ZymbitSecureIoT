import zymkey
from textwrap import fill

print('Zymbit API : Testing data Lock and Unlock...')
src = bytearray(b'\x01\x02\x03\x04')
dst = zymkey.client.lock(src)

print('Original Data')
s = fill(' '.join('{:02X}'.format(c) for c in src), 49)
print(s)

print('Encrypted Data')
s = fill(' '.join('{:02X}'.format(c) for c in dst), 49)
print(s)

print('Testing data unlock...')
new_src = dst
new_dst = zymkey.client.unlock(new_src)

print('Decryped Data')
s = fill(' '.join('{:02X}'.format(c) for c in new_dst), 49)
print(s)

