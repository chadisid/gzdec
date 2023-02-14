# GStreamer plugins for decoding gz files

This GStreamer plugin gzdec will decode data with zlib library.

GStreamer 1.0 and 0.10 are both supported.

```
# Basic build tools:
sudo apt-get install git build-essential automake libtool itstool gtk-doc-tools yelp-tools gnome-common gnome-doc-utils yasm flex bison

# Zlib:
sudo apt install zlib1g-dev

# GStreamer 1.0:
sudo apt install gstreamer1.0-plugins-base libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev libgstreamer1.0-0

# GStreamer 0.10:
sudo apt install gstreamer0.10-plugins-base libgstreamer-plugins-base0.10-dev libgstreamer0.10-dev libgstreamer0.10-0
```

### Installing

Clone the git repo:
```
git clone https://github.com/chadisid/gzdec.git
```

Build and install the plugins:
```
cd gzdec
./configure --gstreamer-version=1.0
make
make install
```

You can choose which GStreamer version to use with the `--gstreamer-version` option. (--gstreamer-version=0.10 to use GStreamer 0.10)

## Running tests on plugins

```
GST_PLUGIN_PATH=/usr/local/lib/gstreamer-1.0 gst-launch-1.0 -v filesrc location=file.txt.gz ! gzdec ! filesink location=file_dec.txt
```
