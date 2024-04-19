
### Ubuntu

Steps should be similar on other Linux distributions.

```
apt-get install git cmake libgstreamer-plugins-base1.0-dev
git clone https://github.com/lozanomt/reamer-plugins.git
cd gstreamer-plugins
mkdir build
cd build
cmake ..
make
```

### Installation and packaging

To install plugins, first make sure you've set `CMAKE_INSTALL_PREFIX` properly,
the default might not be desired (e.g., system path). For finer grained control
you can set `PLUGIN_INSTALL_DIR` and related variables to specify exactly where
you want to install plugins
```
make install
```
- To create a package of all compiled plugins run:
```
make package

### Run in Docker container
```
sudo docker run -it -v $(pwd):/gstreamer-plugins lozanomt/gstreamer-dev:latest
```

## KLV

KLV support is based on a GStreamer [merge request](https://gitlab.freedesktop.org/gstreamer/gst-plugins-base/-/merge_requests/124) that has yet to be merged, so it is included here in the klv library. By default KLV support is disabled. To enable it set the CMake flag `ENABLE_KLV`. This will create the klv plugin, and make the pleora plugin dependent on the klv library. You'll need to ensure `libgstklv-1.0-1.dll` is in the system `PATH` on Windows, or on Linux make sure `libgstklv-1.0-1.so` is in the `LD_LIBRARY_PATH`.