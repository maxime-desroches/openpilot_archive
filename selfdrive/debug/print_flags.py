#!/usr/bin/env python3
from openpilot.selfdrive.car.values import BRANDS

for brand in BRANDS:
  all_flags = set()
  for platform in brand:
    if platform.config.flags != 0:
      all_flags |= set(platform.config.flags)

  if len(all_flags):
    print(brand.__module__.split('.')[-2].upper() + ':')
    for flag in sorted(all_flags):
      print(f'  {flag.name:<24}: ', end='')
      for platform in brand:
        if platform.config.flags & flag:
          print(platform.name, end=', ')
      print()
    print()
