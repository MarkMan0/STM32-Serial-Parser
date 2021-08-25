Import("env")

#
# Dump build environment (for debug)
# print(env.Dump())
#

env.Append(
  LINKFLAGS=[
    "-mfpu=fpv4-sp-d16",
    "-mfloat-abi=hard",
    "-mthumb"
  ]
)
env.Append(
  CCFLAGS=[
    "-mfpu=fpv4-sp-d16",
    "-mfloat-abi=hard",
    "-mthumb"
  ]
)
