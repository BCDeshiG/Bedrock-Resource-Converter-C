#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fixTextures.h"
#include "helper.h"

#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#define STBI_MAX_DIMENSIONS (1<<14) // Max texture size of 16k (way overkill)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
