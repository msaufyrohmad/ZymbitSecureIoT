import zymkey
import time

zymkey.client.clear_perimeter_detect_info()
while True:
    print(zymkey.client.get_perimeter_detect_info())
    time.sleep(1)

But when I tried to set event action and retreive this event, the function wait_for_perimeter_event() always return None. Here’s the script I made :

import zymkey
import time

print(zymkey.client.clear_perimeter_detect_info())
print(zymkey.client.set_perimeter_event_actions(1, action_notify=True, action_self_destruct=False))
print(zymkey.client.set_perimeter_event_actions(0, action_notify=True, action_self_destruct=False))
print(zymkey.client.clear_perimeter_detect_info())
while True:
    res = zymkey.client.wait_for_perimeter_event()
    print(res)
    if not res:
        print(zymkey.client.get_perimeter_detect_info())
        zymkey.client.clear_perimeter_detect_info()
    time.sleep(1)

