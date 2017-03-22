#ifndef QUADTREEDECODER_H
#define QUADTREEDECODER_H

#include <ifs_transformations/ifs_transform.h>
#include <utils/ImageData.h>

void qtree_decode(struct Transforms* transforms, int height, int width, struct image_data *destination);

#endif // QUADTREEDECODER_H
