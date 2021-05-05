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

#ifndef PHP_SVGMAGICK_HELPERS_H
#define PHP_SVGMAGICK_HELPERS_H

struct _b_w_args_s {
	double threshold;
	double opacity;
};
typedef struct _b_w_args_s b_w_args_t;

struct _color_s {
	double red;
	double green;
	double blue;
	double alpha;
};
typedef struct _color_s color_t;

progress_object_t *get_progress_defaults(void);

vectorizer_object_t *get_vectorizer_defaults(void);

data_t *get_data_defaults(void);

void php_options_initialize_constants();

void free_internal_bitmap(vectorizer_object_t  *vectorizer);

void copy_fcall(data_t *data, zend_fcall_info *fci, zend_fcall_info_cache *fci_cache);

int create_bitmap(potrace_bitmap_t **ptr_bitmap, MagickWand *magick_wand);

int black_white_predicate(const PixelWand *pixel, const void *args);

int color_predicate(const PixelWand *pixel, const void *args);

int stack_color_predicate(const PixelWand *pixel, const void *args);

int fill_bitmap(
	potrace_bitmap_t *bitmap,
	MagickWand *magick_wand,
	const void *predicate_args,
	int(*predicate)(const PixelWand *pixel, const void *predicate_args)
);

void free_internal_state(vectorizer_object_t  *vectorizer);

#endif /* PHP_SVGMAGICK_HELPERS_H */
