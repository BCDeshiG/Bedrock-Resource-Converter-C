# Bedrock Resource Converter

* Converts Minecraft Bedrock Resource Packs to Java
* Ported to C as a learning project
* * Probably won't be updated much as a result
* [Original project link here](https://github.com/BCDeshiG/Bedrock-Resource-Converter-PY)

* Usage: `./br-convert packFolder destinationFolder`
* Note: WIP, not all entities are converted (see `fixme.txt`)

## Libraries Used

* [libarchive](https://github.com/libarchive/libarchive)
* * Get this from your package manager
* [cJSON](https://github.com/DaveGamble/cJSON)
* * Included in source as a header file
* [stb_image](https://github.com/nothings/stb)
* * Included in source as header files

## Build Instructions (Linux)

* Enter the `src` directory and run `make`
