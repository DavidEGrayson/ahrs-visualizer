#!/usr/bin/python -u

import time

def out(*args):
  print "%f %f %f %f %f %f %f %f %f  %f %f %f  %f %f %f" % args

# Origin
while True:
  out(1, 0, 0,  0, 1, 0,  0, 1, 0,  1, 1, 1,  -1, 1, 1)
  time.sleep(3)
  out(1, 0, 0,  0, 1, -0.2,  0, 0.2, 1,  1, 1, 1,  -1, 1, 1)
  time.sleep(0.3)

