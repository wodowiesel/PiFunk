#!/bin/bash
for filename in ~/pifunklite/sounds/*.wav; do
        echo "Now Playing: " $filename;
        sudo ~/pifunklite/pifunklite $filename 100.00 48000 stereo
done