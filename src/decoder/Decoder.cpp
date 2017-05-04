#include "Decoder.h"

Decoder::Decoder(){

}

void Decoder::Decode(Transforms* transform, Image& result, int maxphases){
    struct image_data* destination = &result.img;
    destination->width = result.GetWidth();
    destination->height = result.GetHeight();
    destination->channels = 3;

    for(int i = 0; i < destination->channels; i++) {
        for(int j = 0; j < destination->width * destination->height; j++) {
            destination->image_channels[i][j] = 127;
        }
    }
    for (int phase = 1; phase <= maxphases; phase++) {
        qtree_decode(transform, result.GetHeight(), result.GetWidth(), destination);
    }

    destination->color_mode=transform->color_mode;
    if(destination->color_mode==COLOR_MODE_YCbCr){
        convert_from_YCbCr_to_RGB(destination);
    }
}
