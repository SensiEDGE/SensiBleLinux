gcc examples\ibm-watsons\humming-publish.c \
$(pkg-config --cflags --libs glib-2.0) -Igattlib/include -Iiot-embeddedc/src -Iiot-embeddedc/lib \
-Lgattlib/build/dbus -l:libgattlib.so -Liot-embeddedc/build/src -l:libiotfdeviceclient.so \
-Liot-embeddedc/build/lib -l:libmbedtls.a -l:libmbedcrypto.a -l:libmbedx509.a -l:libmqttlib.a -l:libcJSON.a \
-lgcov -o humming-publish
