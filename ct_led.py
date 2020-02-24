import zymkey

print('Turning LED on...')
zymkey.client.led_on()


print('Turning LED off...')
zymkey.client.led_off()

print('Flashing LED off, 500ms on, 100ms off...')
zymkey.client.led_flash(500, 100)

