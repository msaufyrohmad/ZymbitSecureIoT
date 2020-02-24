import zymkey
from textwrap import fill


print('Testing get_ecdsa_public_key()...')
pk = zymkey.client.get_ecdsa_public_key()
s = fill(' '.join('{:02X}'.format(c) for c in pk), 49)
print(s)

print('Testing create_ecdsa_public_key_file()...')
zymkey.client.create_ecdsa_public_key_file('/tmp/ecdsa_publickey.pem')
