# Script to override package files.
# Apply patches from "patches" folder into ".pio/lib_deps/<env>/"
# The "patches" folder must match ".pio/lib_deps/<env>/" structure

from os import walk
from os.path import join, isfile, relpath

Import("env")

LIB_DEPS_DIR = join(env.subst("$PROJECT_LIBDEPS_DIR"), env.subst("$PIOENV")) 
PATCHES_DIR = "patches"

for dir_, _, files in walk(PATCHES_DIR):
    for file_name in files:
        rel_dir = relpath(dir_, PATCHES_DIR)
        rel_file = join(rel_dir, file_name)
        
        original_file = join(LIB_DEPS_DIR, rel_file)
        patch_file = join(PATCHES_DIR, rel_file)

        patched = isfile(f"{original_file}.patched")

        # patch file only if we didn't do it before
        if not patched:
            
            assert isfile(original_file) and isfile(patch_file)
            
            print("patch %s %s" % (original_file, patch_file))
            env.Execute("patch %s %s" % (original_file, patch_file))
            env.Execute("touch " + f"{original_file}.patched")
