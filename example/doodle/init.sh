echo "upload /init/sprite.bmp"
curl --data-urlencode "body@sprite.bmp" http://192.168.3.254/file/doodle/sprite.bmp

echo "upload /init/main.lua"
curl --data-urlencode "body@main.lua" http://192.168.3.254/file/doodle/main.lua

