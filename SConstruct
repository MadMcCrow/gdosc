#!/usr/bin/env python
import os
import sys

# Godot-cpp gives us the base SConscript to include
env = SConscript("godot-cpp/SConstruct")
env.Append(CPPPATH=['.', 'src/', 'oscpack/', 'oscpack/osc/', 'oscpack/ip/'])

# outpath defines the path we'll use for producing our library
out_path = ("bin/")

# Common sources
sources = []
sources = Glob("src/*.cpp")
sources.append(Glob("oscpack/osc/*.cpp"))
sources.append(Glob("oscpack/ip/*.cpp"))

# storing in local variables for later :
platform = env["platform"]
target = env["target"]
suffix = env["suffix"]
shlib  = env["SHLIBSUFFIX"]

# Linux Build
if platform in ["linuxbsd", "linux"]:
    env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++17'])
    sources.append(Glob("oscpack/ip/posix/*.cpp"))
    library = env.SharedLibrary( f"{out_path}/libgdosc{suffix}{shlib}", source=sources)
# MacOS build (not called darwin for some reason !)
elif platform  == "macos":
    # these flags won't work on aarch64
    # TODO : aarch64 build and maybe move to a newer std (c++20 ?) 
    env.Append(CCFLAGS = ['-g','-O3', '-arch', 'x86_64', '-std=c++17'])
    env.Append(LINKFLAGS = ['-arch', 'x86_64'])
    sources.append(Glob("oscpack/ip/posix/*.cpp"))
    library = env.SharedLibrary(f"{out_path}/libgdosc.{platform}.{target}.framework/libgdosc.{platform}.{target}", source=sources)

else: # platform  == "windows":

    if target == "debug":
        env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '/MDd', '-DOSC_HOST_LITTLE_ENDIAN'])
    else:
        env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '/MD', '-DOSC_HOST_LITTLE_ENDIAN'])\
    
    # add libraries :
    env.Append(LIBS=["ws2_32", "wsock32", "winmm"])
    sources.append(Glob("oscpack/ip/win32/*.cpp"))
    library = env.SharedLibrary(
        f"{out_path}/libgdosc{suffix}{shlib}",
        source=sources
    )

# Just build !
Default(library)