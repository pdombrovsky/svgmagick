/*
MIT License

Copyright (c) 2021 Pavel Dombrovsky

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "php_svgmagick.h"
#include "php_svgmagick_defs.h"
#include "php_svgmagick_helpers.h"

data_t *get_data_defaults(void) {

	data_t *p;

	p = (data_t *)malloc(sizeof(data_t));
	if (!p) {
		return NULL;
	}
	p->fci = empty_fcall_info;
	p->fci_cache = empty_fcall_info_cache;
	return p;
}
vectorizer_object_t *get_vectorizer_defaults()
{
	vectorizer_object_t *obj;

	obj = (vectorizer_object_t *)malloc(sizeof(vectorizer_object_t));
	if (!obj) {
		return NULL;
	}
	obj->bitmap = NULL;
	obj->options = NULL;
	obj->state = NULL;
	obj->stack_bm = 0;
	obj->transform.angle = 0.0;
	obj->transform.rotation_point.x = 0.0;
	obj->transform.rotation_point.y = 0.0;
	obj->transform.translate.x = 0.0;
	obj->transform.translate.y = 0.0;
	obj->transform.scale.y = 1.0;
	obj->transform.scale.x = 1.0;
	return obj;
}
progress_object_t *get_progress_defaults()
{
	progress_object_t *obj;

	obj = (progress_object_t *)malloc(sizeof(progress_object_t));
	if (!obj) {
		return NULL;
	}
	obj->params = NULL;
	obj->data = NULL;
	return obj;
}
void free_internal_bitmap(vectorizer_object_t  *vectorizer)
{
	bm_free(vectorizer->bitmap);
	vectorizer->bitmap = NULL;
}
void free_internal_state(vectorizer_object_t  *vectorizer)
{
	potrace_state_free(vectorizer->state);
	vectorizer->state = NULL;
}
void copy_fcall(data_t *data, zend_fcall_info *fci, zend_fcall_info_cache *fci_cache)
{
	if (&data->fci.function_name) {
		zval_ptr_dtor(&data->fci.function_name);
	}
	memcpy(&data->fci, fci, sizeof(zend_fcall_info));
	memcpy(&data->fci_cache, fci_cache, sizeof(zend_fcall_info_cache));
	Z_TRY_ADDREF_P(&data->fci.function_name);
}
void php_options_initialize_constants()
{
#define SVGMAGICK_REGISTER_CONST_LONG(const_name, value)\
	zend_declare_class_constant_long(php_options_sc_entry, const_name, sizeof(const_name)-1, (long)value);
	/* Constants defined in potracelib.h */
	SVGMAGICK_REGISTER_CONST_LONG("TURNPOLICY_BLACK", POTRACE_TURNPOLICY_BLACK);
	SVGMAGICK_REGISTER_CONST_LONG("TURNPOLICY_WHITE", POTRACE_TURNPOLICY_WHITE);
	SVGMAGICK_REGISTER_CONST_LONG("TURNPOLICY_LEFT", POTRACE_TURNPOLICY_LEFT);
	SVGMAGICK_REGISTER_CONST_LONG("TURNPOLICY_RIGHT", POTRACE_TURNPOLICY_RIGHT);
	SVGMAGICK_REGISTER_CONST_LONG("TURNPOLICY_MINORITY", POTRACE_TURNPOLICY_MINORITY);
	SVGMAGICK_REGISTER_CONST_LONG("TURNPOLICY_MAJORITY", POTRACE_TURNPOLICY_MAJORITY);
	SVGMAGICK_REGISTER_CONST_LONG("TURNPOLICY_RANDOM", POTRACE_TURNPOLICY_RANDOM);
#undef IMAGICK_REGISTER_CONST_LONG
}
int create_bitmap(potrace_bitmap_t **ptr_bitmap, MagickWand *magick_wand)
{
	potrace_bitmap_t *bitmap = NULL;
	int width = MagickGetImageWidth(magick_wand);
	int height = MagickGetImageHeight(magick_wand);
	if (width <= 0 || height <= 0) {
		return 1;
	}
	bitmap = bm_new(width, height);
	if (!bitmap) {
		return 2;
	}
	*ptr_bitmap = bitmap;
	return 0;
}
int black_white_predicate_rgb(const PixelWand *pixel, const void *args)
{
	double threshold = *((double *)args);
	double sum = 0.0;
	sum += PixelGetRed(pixel);
	sum += PixelGetGreen(pixel);
	sum += PixelGetBlue(pixel);
	sum /= 3;
	return (sum < threshold) ? 1 : 0;
}
int black_white_predicate_rgba(const PixelWand *pixel, const void *args)
{
	double threshold = *((double *)args);
	double sum = 1.0;
	sum -= PixelGetAlpha(pixel);
	sum += PixelGetRed(pixel);
	sum += PixelGetGreen(pixel);
	sum += PixelGetBlue(pixel);
	sum /= 4;
	return (sum < threshold) ? 1 : 0;
}
int color_predicate(const PixelWand *pixel, const void *args)
{
	double da, dr, dg, db;
	double eps = 2e-3;
	color_t *args_ptr = (color_t *)args;

	dr = fabs(PixelGetRed(pixel) - args_ptr->red);
	dg = fabs(PixelGetGreen(pixel) - args_ptr->green);
	db = fabs(PixelGetBlue(pixel) - args_ptr->blue);
	da = fabs(PixelGetAlpha(pixel) - args_ptr->alpha);

	return (
		(da < eps) &&
		(dr < eps) &&
		(dg < eps) &&
		(db < eps)
		);
}
int stack_color_predicate(const PixelWand *pixel, const void *args)
{
	double  da, dr, dg, db;
	double eps = 2e-3;
	color_t *args_ptr = (color_t *)args;

	dr = args_ptr->red - PixelGetRed(pixel);
	dg = args_ptr->green - PixelGetGreen(pixel);
	db = args_ptr->blue - PixelGetBlue(pixel);
	da = args_ptr->alpha - PixelGetAlpha(pixel);

	return (
		(da < eps) &&
		(dr < eps) &&
		(dg < eps) &&
		(db < eps)
		);
}
int fill_bitmap(
	potrace_bitmap_t *bitmap,
	MagickWand *magick_wand,
	const void *predicate_args,
	int(*predicate)(const PixelWand *pixel, const void *predicate_args)
)
{
	PixelIterator *iterator = NULL;
	PixelWand **pixels = NULL;
	int x, y;
	double r, g, b;
	int value;

	// Get a new pixel iterator 
	iterator = NewPixelIterator(magick_wand);
	if (!iterator) {
		return 1;
	}
	for (y = 0; y < bitmap->h; y++) {
		// Get the next row of the image as an array of PixelWands
		pixels = PixelGetNextIteratorRow(iterator, &x);
		// Set the row of wands
		for (x = 0; x < bitmap->w; x++) {
			// Make callback to know, is pixel black or white?
			value = predicate(pixels[x], predicate_args);
			BM_UPUT(bitmap, x, y, value);
		}
	}
	// Clean up
	iterator = DestroyPixelIterator(iterator);
	
	return 0;
}

