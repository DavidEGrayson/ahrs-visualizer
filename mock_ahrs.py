#!/usr/bin/env python2

import time
import sys

def out(*args):
  print "%f %f %f %f %f %f %f %f %f  %f %f %f  %f %f %f" % args
  sys.stdout.flush()

# Origin
while True:
  out(1, 0, 0,  0, 1, 0,  0, 1, 0,  1, 1, 1,  -1, 1, 1)
  time.sleep(3)
  out(1, 0, 0,  0, 1, -0.2,  0, 0.2, 1,  1, 1, 1,  -1, 1, 1)
  time.sleep(0.3)

