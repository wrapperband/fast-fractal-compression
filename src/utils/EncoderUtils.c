#include "EncoderUtils.h"

ERR_RET filter_grayscale(struct image_data* src, struct image_data* dst){
    //TODO: add checks of sizes

    int size=src->width*src->height;
    for (int i=0; i<size; ++i){
        u_int32_t rgb=(src->image_channels[R][i]*77 +
                       src->image_channels[G][i]*151+
                       src->image_channels[B][i]*28)>>8;

        dst->image_channels[R][i]=(rgb<<16)|(rgb<<8)|rgb;
        dst->image_channels[G][i]=(rgb<<16)|(rgb<<8)|rgb;
        dst->image_channels[B][i]=(rgb<<16)|(rgb<<8)|rgb;
    }

    return ERR_SUCCESS;
}

ERR_RET adjust_image_size(struct image_data* src, struct image_data* dst, u_int32_t rows, u_int32_t col){
    u_int32_t width=src->width;
    while(src->width%rows!=0)
        width--;

    u_int32_t height=src->height;
    while(src->height%col!=0){
        height--;
    }

    return ERR_SUCCESS;
}

int get_average_pixel(pixel_value* domain_data, int domain_width,
	int domain_x, int domain_y, int size, int* average_pixel)
{
	int top = 0;
	int bottom = (size * size);

	// Simple average of all pixels.
	for (int y = domain_y; y < domain_y + size; y++)
	{
		for (int x = domain_x; x < domain_x + size; x++)
		{
			top += domain_data[y * domain_width + x];

			if (top < 0)
			{
				printf("Error: Accumulator rolled over averaging pixels.\n");
				return ERR_ACCUM_ROLL;
			}
		}
	}
	*average_pixel = top/bottom;
	return ERR_SUCCESS;
}


double get_error(
	pixel_value* domain_data, int domain_width, int domain_x, int domain_y, int domain_avg,
	pixel_value* range_data, int range_width, int range_x, int range_y, int range_avg,
	int size, double scale, double* error)
{
	double top = 0;
	double bottom = (double)(size * size);

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			int domain = (domain_data[(domain_y + y) * domain_width + (domain_x + x)] - domain_avg);
			int range = (range_data[(range_y + y) * range_width + (range_x + x)] - range_avg);
			int diff = (int)(scale * (double)domain) - range;

			// According to the formula we want (DIFF*DIFF)/(SIZE*SIZE)
			top += (diff * diff);

			if (top < 0)
			{
				printf("Error: Overflow occured during error %lf\n", top);
				return ERR_OVERFLOW;
			}
		}
	}

	*error = top / bottom;
}


double get_scale_factor(
	pixel_value* domain_data, int domain_width, int domain_x, int domain_y, int domain_avg,
	pixel_value* range_Data, int range_width, int range_x, int range_y, int range_avg,
	int size, double* scale_factor)
{
	int top = 0;
	int bottom = 0;

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			int domain = (domain_data[(domain_y + y) * domain_width + (domain_x + x)] - domain_avg);
			int range = (range_Data[(range_y + y) * range_width + (range_x + x)] - range_avg);

			// According to the formula we want (R*D) / (D*D)
			top += range * domain;
			bottom += domain * domain;

			if (bottom < 0)
			{
				printf("Error: Overflow occured during scaling %d %d %d %d\n", y, domain_width, bottom, top);
				return ERR_OVERFLOW;
			}
		}
	}

	if (bottom == 0)
	{
		top = 0;
		bottom = 1;
	}

	*scale_factor = ((double)top) / ((double)bottom);
}
