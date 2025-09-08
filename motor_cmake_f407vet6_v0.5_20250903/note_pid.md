# 1
## spend time when motor rotates 180 degrees
### for dual pid
- PID_UPDATE_TIME = 500ms
    motor is out of control
- PID_UPDATE_TIME = 200ms
    spend time = 435ms
- PID_UPDATE_TIME = 100ms
    spend time = 145ms (180ms when it's not in normal)
- PID_UPDATE_TIME = 50ms
    spend time = 110ms
- PID_UPDATE_TIME = 30ms
    spend time = 94ms
    spend time = 105ms after a long run, and spend more time if run time is too long
- PID_UPDATE_TIME = 10ms
    spend time = 78ms
