Import("env")
import shutil

def store_hex(source, target, env):
  print(env)
  shutil.copy(target[0].rstr(), "./output/o-bako.bin")

env.AddPostAction("$BUILD_DIR/firmware.bin", store_hex);
