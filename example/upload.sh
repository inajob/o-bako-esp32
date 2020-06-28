#!/bin/sh
set -eu

# upload script

O_BAKO_IP=${O_BAKO_IP:-"192.168.4.1"}
TARGET=$1

echo O_BAKO_IP: ${O_BAKO_IP}
echo TARGET: ${TARGET}

cd $(dirname $0)

for FILE in `ls ${TARGET}/*.{js,lua,bmp}`; do
  echo $FILE
  echo "uploading ${FILE}"
  FILENAME=`basename ${FILE}`
  curl --data-urlencode "body@${FILE}" http://${O_BAKO_IP}/file/${TARGET}/${FILENAME}
done

