#include "ifs_transform.h"

#define MODULE_NAME "ifsTransform"

ERR_RET down_sample(pixel_value *src, int src_width, int start_x, int start_y, int target_size, pixel_value* sample) {
    int dest_x = 0;
    int dest_y = 0;
    INCREMENT_FLOP_COUNT(2, 2, 0, 0)
    int index_x = start_x + target_size * 2;
    int index_y = start_y + target_size * 2;
    for (int y = start_y; y < index_y; y += 2) {
        INCREMENT_FLOP_COUNT(0, 2, 0, 0)
        for (int x = start_x; x < index_x; x += 2) {
            INCREMENT_FLOP_COUNT(3, 11, 0, 0)
            // Perform simple 2x2 average
            uint32_t pixel = 0;
            int index_x = y * src_width + x;
            int index_y = (y + 1) * src_width + x;
//            printf("Down_Sample: y: %d x: %d src_width: %d\n", y, x, src_width);
            pixel += src[index_x];
            pixel += src[index_x + 1];
            pixel += src[index_y];
            pixel += src[index_y + 1];
            pixel = pixel >> 2;
            sample[dest_y * target_size + dest_x] = pixel;
            dest_x++;
        }
        dest_y++;
        dest_x = 0;
    }
    return ERR_SUCCESS;
}

ERR_RET ifs_trans_push_back(struct ifs_transformation_list* list, struct ifs_transformation* transformation){
    struct ifs_transformation* new_transformation=(struct ifs_transformation*)malloc(sizeof(struct ifs_transformation));
    memcpy(new_transformation, transformation, sizeof(struct ifs_transformation));
    new_transformation->next=NULL;
    if(list->head==NULL && list->tail==NULL){
        list->head=new_transformation;
        list->tail=new_transformation;
    }else{
        assert(list->tail!=NULL);
        list->tail->next=new_transformation;
        list->tail=new_transformation;
    }
    list->elements++;
    return ERR_SUCCESS;
}

ERR_RET ifs_trans_clear_list(struct Transforms *transforms){
    for(int i=0;i<transforms->channels;++i){
        struct ifs_transformation_list* tr_list=transforms->ch+i;
        while(tr_list->head!=NULL){
            struct ifs_transformation* tmp=tr_list->head;
            tr_list->head=tr_list->head->next;
            free(tmp);
        }
        tr_list->tail=tr_list->head;
        assert(tr_list->tail==NULL);
    }

    return ERR_SUCCESS;
}

ERR_RET ifs_transformation_execute(struct ifs_transformation* transformation, pixel_value* src, u_int32_t src_width,
                                   pixel_value* dest, u_int32_t dest_width, bool downsampled, pixel_value* buffer){

    INCREMENT_FLOP_COUNT(6, 0, 0, 0)

    int from_x = transformation->from_x / 2;
    int from_y = transformation->from_y / 2;
    int d_x = 1;
    int d_y = 1;
    enum ifs_type symmetry=transformation->transformation_type;
    bool in_order = isScanlineOrder(symmetry);
    double scale=transformation->scale;
    u_int32_t offset=transformation->offset;

    if (!downsampled)
    {
        pixel_value* downsampled_img=buffer;
        down_sample(src, src_width, transformation->from_x, transformation->from_y, transformation->size, downsampled_img);
        src = downsampled_img;
        src_width = transformation->size;
        from_y = from_x = 0;
    }

    if (!isPositiveX(symmetry))
    {
        from_x += transformation->size - 1;
        d_x = -1;
    }

    if (!isPositiveY(symmetry))
    {
        from_y += transformation->size - 1;
        d_y = -1;
    }

    int start_x = from_x;
    int start_y = from_y;

    INCREMENT_FLOP_COUNT(2*transformation->size*transformation->size,
                         4*transformation->size*transformation->size, transformation->size*transformation->size, 0)
    INCREMENT_FLOP_COUNT(transformation->size,0,0,0)
    for (int to_y = transformation->to_y; to_y < (transformation->to_y +  transformation->size); to_y++)
    {
        for (int to_x = transformation->to_x; to_x < (transformation->to_x + transformation->size); to_x++)
        {
            int pixel = src[from_y * src_width + from_x];
            pixel = (int)(scale*pixel) + offset;

            if (pixel < 0)
                pixel = 0;
            if (pixel > 255)
                pixel = 255;

            dest[to_y * dest_width + to_x] = pixel;

            if (in_order)
                from_x += d_x;
            else
                from_y += d_y;
        }

        if (in_order)
        {
            from_x= start_x;
            from_y += d_y;
        }
        else
        {
            from_y = start_y;
            from_x += d_x;
        }
    }

    return ERR_SUCCESS;
}

bool isScanlineOrder(enum ifs_type symmetry)
{
    return (
        symmetry == SYM_NONE ||
        symmetry == SYM_R180 ||
        symmetry == SYM_HFLIP ||
        symmetry == SYM_VFLIP
    );
}

bool isPositiveX(enum ifs_type symmetry)
{
    return (
        symmetry == SYM_NONE ||
        symmetry == SYM_R90 ||
        symmetry == SYM_VFLIP ||
        symmetry == SYM_RDFLIP
    );
}

bool isPositiveY(enum ifs_type symmetry)
{
    return (
        symmetry == SYM_NONE ||
        symmetry == SYM_R270 ||
        symmetry == SYM_HFLIP ||
        symmetry == SYM_RDFLIP
    );
}
