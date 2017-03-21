#include "QuadTreeDecoder.h"

void qtree_decode(struct Transforms* transforms, int height, int width) {
    struct image_data *img = (struct image_data *)malloc(sizeof(struct image_data));
    img->width = width;
    img->height = height;
    img->channels = 3;
    img->image_channel[0] = (pixel_value *)malloc(sizeof(pixel_value)* width * height);
    img->image_channel[1] = (pixel_value *)malloc(sizeof(pixel_value)* width * height); 
    img->image_channel[2] = (pixel_value *)malloc(sizeof(pixel_value)* width * height); 

    // Initialize to grey image
    for(int i = 0; i < img->channels; i++) {
        for(int j = 0; j < img.width * img.height; j++) {
            img->image_channel[i][j] = 127;
        }
    }

    // Decoding starts here
    img->channels = transforms->channels;
    for (int channel = 0; channel < img->channels; channel++) {
        pixel_value *original_image = img->image_channel[channel];

        // Iterate over Transforms
        struct ifs_transformations_list iter = transforms->ch[channel];
        struct ifs_transformation* temp = iter.head;
        while(temp != NULL) {
            execute(original_image, img->width, original_image, img->width, false)
            temp = temp->next;
        } 
    }

}