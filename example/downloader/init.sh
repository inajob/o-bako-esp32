echo "upload /downloader/sprite.bmp"
curl --data-urlencode "body@sprite.bmp" http://192.168.0.1/file/downloader/sprite.bmp

echo "upload /downloader/main.lua"
curl --data-urlencode "body@main.lua" http://192.168.0.1/file/downloader/main.lua

echo "upload /downloader/json.lua"
curl --data-urlencode "body@json.lua" http://192.168.0.1/file/downloader/json.lua

