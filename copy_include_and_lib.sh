rm -rf output

mkdir output
mkdir output/include
mkdir output/lib

OUTPUT_INCLUDE=output/include
OUTPUT_LIB=output/lib

cp c99/msinttypes/inttypes.h $OUTPUT_INCLUDE/

cp -R /usr/local/include/libavdevice $OUTPUT_INCLUDE/
cp -R /usr/local/include/libavfilter $OUTPUT_INCLUDE/
cp -R /usr/local/include/libavformat $OUTPUT_INCLUDE/
cp -R /usr/local/include/libavcodec $OUTPUT_INCLUDE/
cp -R /usr/local/include/libswresample $OUTPUT_INCLUDE/
cp -R /usr/local/include/libswscale $OUTPUT_INCLUDE/
cp -R /usr/local/include/libavutil $OUTPUT_INCLUDE/

cp /usr/local/lib/libavdevice.a $OUTPUT_LIB/libavdevice.lib
cp /usr/local/lib/libavfilter.a $OUTPUT_LIB/libavfilter.lib
cp /usr/local/lib/libavformat.a $OUTPUT_LIB/libavformat.lib
cp /usr/local/lib/libavcodec.a $OUTPUT_LIB/libavcodec.lib
cp /usr/local/lib/libswresample.a $OUTPUT_LIB/libswresample.lib
cp /usr/local/lib/libswscale.a $OUTPUT_LIB/libswscale.lib
cp /usr/local/lib/libavutil.a $OUTPUT_LIB/libavutil.lib

