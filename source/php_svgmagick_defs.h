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

#ifndef PHP_SVGMAGICK_DEFS_H 
#define  PHP_SVGMAGICK_DEFS_H 

#include "php_imagick_defs.h"
#include "php_imagick_shared.h"
#include "potracelib.h"
#include "bitmap.h"

/* Some extra headers */

#include "Zend/zend_exceptions.h"
#include "Zend/zend_smart_str.h"
#include "php_ini.h"
#include "Zend/zend.h"

/* Namespace */
#define PHP_SVGMAGICK_NS_NAME "SvgMagick"

/* Class names */
#define PHP_OPTIONS_SC_NAME "Options"
#define PHP_OPTIONS_EXCEPTION_SC_NAME "OptionsException"
#define PHP_PROGRESS_SC_NAME  "Progress"
#define PHP_PROGRESS_EXCEPTION_SC_NAME "ProgressException"
#define PHP_VECTORIZER_SC_NAME "Vectorizer"
#define PHP_VECTORIZER_EXCEPTION_SC_NAME "VectorizerException"


/* Structure for Options object. */
struct _php_options_object {
	potrace_param_t  *options;
	zend_object zo;
};
typedef struct _php_options_object  php_options_object;

/* Options fetching. */
static inline php_options_object *php_options_fetch_object(zend_object *obj) {
	return (php_options_object *)((char*)(obj)-XtOffsetOf(php_options_object, zo));
}

/* Options access. */
#define Z_OPTIONS_P(zv) php_options_fetch_object(Z_OBJ_P((zv)))

/* Base structures for Progress object. */
struct _data_s {
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
};
typedef struct  _data_s data_t;
struct _progress_object_s {
	potrace_progress_t *params;
	data_t *data;
};
typedef struct _progress_object_s progress_object_t;

/* Structure for Progress object. */
struct _php_progress_object {
	progress_object_t  *progress;
	zend_object zo;
};
typedef struct _php_progress_object  php_progress_object;

/* Progress fetching. */
static inline php_progress_object *php_progress_fetch_object(zend_object *obj) {
	return (php_progress_object *)((char*)(obj)-XtOffsetOf(php_progress_object, zo));
}

/* Progress access. */
#define Z_PROGRESS_P(zv) php_progress_fetch_object(Z_OBJ_P((zv)))

/* Structure for transform(rotate, scale, dx and dy) */

struct _transform_s {
	potrace_dpoint_t scale;
	potrace_dpoint_t translate;
	potrace_dpoint_t rotation_point;
	double angle;
};
typedef struct _transform_s  transform_t;

/* Vectorizer main structure */
struct _vectorizer_object_s {
	potrace_param_t  *options;
	potrace_bitmap_t *bitmap;
	potrace_state_t *state;
	transform_t transform;
	int stack_bm;
};
typedef struct _vectorizer_object_s  vectorizer_object_t;

/* Structure for Vectorizer object. */
struct _php_vectorizer_object {
	vectorizer_object_t  *vectorizer;
	zend_object zo;
};
typedef struct _php_vectorizer_object  php_vectorizer_object;

/* Vectorizer fetching. */
static inline php_vectorizer_object *php_vectorizer_fetch_object(zend_object *obj) {
	return (php_vectorizer_object *)((char*)(obj)-XtOffsetOf(php_vectorizer_object, zo));
}

/* Vectorizer access. */
#define Z_VECTORIZER_P(zv) php_vectorizer_fetch_object(Z_OBJ_P((zv)))


#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/*
ZEND_BEGIN_MODULE_GLOBALS(svgmagick)
ZEND_END_MODULE_GLOBALS(svgmagick)

ZEND_EXTERN_MODULE_GLOBALS(svgmagick)
#define SVGMAGICK_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(svgmagick, v)
*/

/* Class entries */
extern zend_class_entry *php_options_sc_entry;
extern zend_class_entry *php_options_exception_class_entry;
extern zend_class_entry *php_progress_sc_entry;
extern zend_class_entry *php_progress_exception_class_entry;
extern zend_class_entry *php_vectorizer_sc_entry;
extern zend_class_entry *php_vectorizer_exception_class_entry;

/* Options methods */
PHP_METHOD(options, setTurnPolicy);
PHP_METHOD(options, getTurnpolicy);
PHP_METHOD(options, setTurdSize);
PHP_METHOD(options, getTurdSize);
PHP_METHOD(options, setAlphaMax);
PHP_METHOD(options, getAlphaMax);
PHP_METHOD(options, setOptiCurve);
PHP_METHOD(options, getOptiCurve);
PHP_METHOD(options, setOptTolerance);
PHP_METHOD(options, getOptTolerance);

/* Progress methods */
PHP_METHOD(progress, __construct);
PHP_METHOD(progress, setLimits);
PHP_METHOD(progress, setCallback);
PHP_METHOD(progress, start);
PHP_METHOD(progress, end);

/* Vectorizer methods */
PHP_METHOD(vectorizer, __construct);
PHP_METHOD(vectorizer, createSimpleBitmap);
PHP_METHOD(vectorizer, createColorBitmap);
PHP_METHOD(vectorizer, stackBitmap);
PHP_METHOD(vectorizer, setScale);
PHP_METHOD(vectorizer, setTranslation);
PHP_METHOD(vectorizer, setRotation);
PHP_METHOD(vectorizer, trace);
PHP_METHOD(vectorizer, getAttributeContent);
PHP_METHOD(vectorizer, clear);
#endif /* PHP_SVGMAGICK_DEFS_H  */
