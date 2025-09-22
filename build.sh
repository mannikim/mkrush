#!/bin/sh
gcc mkrush.c -o mkrush.so -fvisibility=hidden -fPIC -DPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2` 
