rm -rf include
rm -rf lib

mkdir include
mkdir lib

cp c99/msinttypes/*.h include/

cp -R /usr/local/include/libavdevice include/
cp -R /usr/local/include/libavfilter include/
cp -R /usr/local/include/libavformat include/
cp -R /usr/local/include/libavcodec include/
cp -R /usr/local/include/libswresample include/
cp -R /usr/local/include/libswscale include/
cp -R /usr/local/include/libavutil include/

cp /usr/local/lib/libavdevice.a lib/libavdevice.lib
cp /usr/local/lib/libavfilter.a lib/libavfilter.lib
cp /usr/local/lib/libavformat.a lib/libavformat.lib
cp /usr/local/lib/libavcodec.a lib/libavcodec.lib
cp /usr/local/lib/libswresample.a lib/libswresample.lib
cp /usr/local/lib/libswscale.a lib/libswscale.lib
cp /usr/local/lib/libavutil.a lib/libavutil.lib

