# PlatformIO convert only the first INO files found in src folder.
# This implementation convert only the first INO files found in custom_example_dir.
#
# Refer to: https://github.com/platformio/platformio-core/issues/4730#issuecomment-1836004680
# Xavier BRASSOUD - v1.0


import glob
import os

Import("env")

def FindInoNodes(env):
    project_dir = glob.escape(env.subst("$PROJECT_DIR"))
    example_dir = env.GetProjectOption("custom_example_dir")
    return env.Glob(os.path.join(project_dir, f"{example_dir}/*.ino"))

env.AddMethod(FindInoNodes)
