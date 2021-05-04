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
#include "path_converter.h"

PHP_METHOD(vectorizer, __construct)
{
	zval *id;
	php_vectorizer_object *intern;
	php_options_object *opt_intern;
	zval *options_object = NULL;
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(options_object, php_options_sc_entry)
		ZEND_PARSE_PARAMETERS_END();

	id = getThis();
	intern = Z_VECTORIZER_P(id);
	
	opt_intern = Z_OPTIONS_P(options_object);
	if (opt_intern->options == NULL) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "Instance of Options class doesn't exist", 0);
		return;
	}
	intern->vectorizer->options = opt_intern->options;
}
PHP_METHOD(vectorizer, stackBitmap)
{
	zend_bool make_stack;
	php_vectorizer_object *intern;
	zval *id;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_BOOL(make_stack)
		ZEND_PARSE_PARAMETERS_END();

	id = getThis();
	intern = Z_VECTORIZER_P(id);
	intern->vectorizer->stack_bm = make_stack;
}
static inline void createBitmap(vectorizer_object_t *vectorizer, zval *imagick_id, const void *predicate_args, int(*predicate)(const PixelWand *pixel, const void *predicate_args))
{
	php_imagick_object *imagick_intern;
	int err = 0;

	imagick_intern = Z_IMAGICK_P(imagick_id);
	if (imagick_intern->magick_wand == NULL) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "Instance of Imagick class doesn't exist", 0);
		return;
	}

	int number_images = MagickGetNumberImages(imagick_intern->magick_wand);
	if (number_images == 0) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "Can't process empty Imagick object", 0);
		return;
	}

	/* Clear current bitmap if exists */
	if (vectorizer->bitmap != NULL) {
		free_internal_bitmap(vectorizer);
	}
	/* Create bitmap here, if result is NULL than something goes wrong, and we should throw exception */
	err = create_bitmap(&vectorizer->bitmap, imagick_intern->magick_wand);
	if (err == 1) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "Invalid image sizes", 0);
		return;
	}
	if (err == 2) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "An error occured while allocating bitmap", 0);
		return;
	}
	err = fill_bitmap(vectorizer->bitmap, imagick_intern->magick_wand, predicate_args, predicate);
	if (err == 1) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "An error occured while allocating PixelIterator", 0);
		return;
	}
}
PHP_METHOD(vectorizer, createSimpleBitmap)
{
	zval *id;
	zval *imagick_id = NULL;
	php_vectorizer_object *intern;
	b_w_args_t args = { 0.5 , 1.0 };
	
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_OBJECT_OF_CLASS(imagick_id, php_imagick_get_class_entry())
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(args.threshold)
		Z_PARAM_DOUBLE(args.opacity)
		ZEND_PARSE_PARAMETERS_END();
	
	int count = ZEND_NUM_ARGS();
	if (count >= 2) {
		if (args.threshold < 0.0 || args.threshold > 1.0) {
			zend_throw_exception(php_vectorizer_exception_class_entry, "The threshold value must be between 0.0 and 1.0", 0);
			return;
		}

	}  
	if (count == 3) {
		if (args.opacity < 0.0 || args.opacity > 1.0) {
			zend_throw_exception(php_vectorizer_exception_class_entry, "The opacity value must be between 0.0 and 1.0", 0);
			return;
		}
	}
	
	id = getThis();
	intern = Z_VECTORIZER_P(id);
	createBitmap(intern->vectorizer, imagick_id, &args, &black_white_predicate);
}
PHP_METHOD(vectorizer, createColorBitmap)
{
	zval *id;
	zval *imagick_id = NULL;
	php_vectorizer_object *intern;
	color_t args ;
	args.alpha = 1.0;
	double eps = 1e-3;

	ZEND_PARSE_PARAMETERS_START(4, 5)
		Z_PARAM_OBJECT_OF_CLASS(imagick_id, php_imagick_get_class_entry())
		Z_PARAM_DOUBLE(args.red)
		Z_PARAM_DOUBLE(args.green)
		Z_PARAM_DOUBLE(args.blue)
	    Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(args.alpha)
		ZEND_PARSE_PARAMETERS_END();

	if (args.red < -eps || args.red > (1.0 + eps)) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "The 'red' value must be between 0.0 and 1.0", 0);
		return;
	}
	if (args.green < -eps || args.green > (1.0 + eps)) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "The 'green' value must be between 0.0 and 1.0", 0);
		return;
	}
	if (args.blue < -eps || args.blue > (1.0 + eps)) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "The 'blue' value must be between 0.0 and 1.0", 0);
		return;
	}

	int count = ZEND_NUM_ARGS();

	if (count == 5) {
		if (args.alpha < -eps || args.alpha >(1.0 + eps)) {
			zend_throw_exception(php_vectorizer_exception_class_entry, "The 'alpha' value must be between 0.0 and 1.0", 0);
			return;
		}
	}

	id = getThis();
	intern = Z_VECTORIZER_P(id);
	if (intern->vectorizer->stack_bm) {
		createBitmap(intern->vectorizer, imagick_id, &args, &stack_color_predicate);
	}
	else {
		createBitmap(intern->vectorizer, imagick_id, &args, &color_predicate);
	}
	
}
PHP_METHOD(vectorizer, setScale)
{
	zval *id;
	php_vectorizer_object *intern;
	double scaleX = 1.0;
	double scaleY = 1.0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_DOUBLE(scaleX)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(scaleY)
		ZEND_PARSE_PARAMETERS_END();

	if (scaleX < 0.0) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "The scaleX value must be greater than 0.0", 0);
		return;
	}

	int count = ZEND_NUM_ARGS();

	if (count == 1) {
		scaleY = scaleX;
	} else {
		if (scaleY < 0.0) {
			zend_throw_exception(php_vectorizer_exception_class_entry, "The scaleY value must be greater than 0.0", 0);
			return;
		}
	}

	id = getThis();
	intern = Z_VECTORIZER_P(id);
	intern->vectorizer->transform.scale.x = scaleX;
	intern->vectorizer->transform.scale.y = scaleY;
}
PHP_METHOD(vectorizer, setTranslation)
{
	zval *id;
	php_vectorizer_object *intern;
	double dX = 0.0;
	double dY = 0.0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_DOUBLE(dX)
		Z_PARAM_DOUBLE(dY)
		ZEND_PARSE_PARAMETERS_END();

	id = getThis();
	intern = Z_VECTORIZER_P(id);

	intern->vectorizer->transform.translate.x = dX;
	intern->vectorizer->transform.translate.y = dY;
}
PHP_METHOD(vectorizer, setRotation)
{
	zval *id;
	php_vectorizer_object *intern;
	double x = 0.0;
	double y = 0.0;
	double angle = 0.0;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_DOUBLE(x)
		Z_PARAM_DOUBLE(y)
		Z_PARAM_DOUBLE(angle)
		ZEND_PARSE_PARAMETERS_END();

	id = getThis();
	intern = Z_VECTORIZER_P(id);

	intern->vectorizer->transform.rotation_point.x = x;
	intern->vectorizer->transform.rotation_point.y = y;
	intern->vectorizer->transform.angle = angle;
}
PHP_METHOD(vectorizer, trace)
{
	zval *id;
	php_vectorizer_object *intern;

	ZEND_PARSE_PARAMETERS_NONE();

	id = getThis();
	intern = Z_VECTORIZER_P(id);
	/* if bitmap not exist, throw exception */
	if (intern->vectorizer->bitmap == NULL) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "Unable to trace, bitmap is missing", 0);
		return;
	}
	/* Clear current state if exists */
	if (intern->vectorizer->state != NULL) {
		free_internal_state(intern->vectorizer);
	}
	intern->vectorizer->state = potrace_trace(intern->vectorizer->options, intern->vectorizer->bitmap);
	if (!intern->vectorizer->state || intern->vectorizer->state->plist == NULL || intern->vectorizer->state->status != POTRACE_STATUS_OK) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

PHP_METHOD(vectorizer, getAttributeContent)
{
	zval *id;
	php_vectorizer_object *intern;

	ZEND_PARSE_PARAMETERS_NONE();

	id = getThis();
	intern = Z_VECTORIZER_P(id);
	/* if state is wrong or plist not exist, throw exception */
	if (!intern->vectorizer->state || intern->vectorizer->state->plist == NULL || intern->vectorizer->state->status != POTRACE_STATUS_OK) {
		zend_throw_exception(php_vectorizer_exception_class_entry, "Unable to get 'd' attribute content, bitmap is untraced", 0);
		return;
	}
	
	smart_str content = { 0 };

	convert_path(&content, &intern->vectorizer->transform, intern->vectorizer->state->plist);
	
	if (content.s == NULL) {
		RETURN_NULL();
	}
	RETVAL_STR_COPY(content.s);
	smart_str_free(&content);
}
PHP_METHOD(vectorizer, clear)
{
	zval *id;
	php_vectorizer_object *intern;

	ZEND_PARSE_PARAMETERS_NONE();

	id = getThis();
	intern = Z_VECTORIZER_P(id);
	/* Clear current bitmap if exists */
	if (intern->vectorizer->bitmap != NULL) {
		free_internal_bitmap(intern->vectorizer);
	}
	/* Clear current state if exists */
	if (intern->vectorizer->state != NULL) {
		free_internal_state(intern->vectorizer);
	}
}
