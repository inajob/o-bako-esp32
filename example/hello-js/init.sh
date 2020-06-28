OBAKO_IP=192.168.3.254
echo "upload /hello-js/sprite.bmp"
curl --data-urlencode "body@sprite.bmp" http://${OBAKO_IP}/file/hello-js/sprite.bmp

echo "upload /hello-js/main.js"
curl --data-urlencode "body@main.js" http://${OBAKO_IP}/file/hello-js/main.js

