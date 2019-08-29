#!/usr/bin/env bash

# ./gst-launch-1.0 --gst-debug-level=3 v4l2src device=/dev/video0 ! "image/jpeg,width=640,height=480,framerate=25/1" ! jpegdec ! videoconvert ! mpph264enc ! queue ! h264parse ! flvmux streamable=true ! queue ! rtmpsink location='rtmp://127.0.0.1/live/test live=true'

#echo will push stream to rtmp://-live/${1}
#./gst-launch-1.0 --gst-debug-level=1 v4l2src device=/dev/video0 ! "image/jpeg,width=640,height=480,framerate=30/1" ! jpegdec ! videoconvert ! mpph264enc ! queue ! h264parse ! flvmux streamable=true ! queue ! rtmpsink sync=false location="rtmp://192.168.0.132/live/$1 live=true"


#./gst-launch-1.0 --gst-debug-level=3 v4l2src device=/dev/video0 num-buffers=10 ! jpegenc ! filesink location=/dev/shm/1.jpeg
#./gst-launch-1.0 audiotestsrc ! vorbisenc ! oggmux ! filesink location=/dev/shm/test.ogg


mount -o remount,size=555M tmpfs /dev/shm

cd /usr/local/rkmpp/bin/

export GST_PLUGIN_PATH=/usr/local/rkmpp/lib/gstreamer-1.0


exec ./gst-launch-1.0 -e --gst-debug-level=1 v4l2src device=/dev/video0 ! "image/jpeg,width=640,height=480,framerate=25/1" ! jpegdec ! queue ! videoconvert ! tee name=t \
    t. ! queue flush-on-eos=true ! jpegenc ! multifilesink post-messages=true location=/dev/shm/raw.jpg max-files=0
