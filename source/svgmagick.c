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

#include "php_svgmagick_shared.h"
#include "php_svgmagick_defs.h"
#include "php_svgmagick.h"
#include "php_svgmagick_helpers.h"

zend_class_entry *php_options_sc_entry;
zend_class_entry *php_options_exception_class_entry;

zend_class_entry *php_progress_sc_entry;
zend_class_entry *php_progress_exception_class_entry;

zend_class_entry *php_vectorizer_sc_entry;
zend_class_entry *php_vectorizer_exception_class_entry;

/* Handlers */
static zend_object_handlers options_object_handlers;
static zend_object_handlers progress_object_handlers;
static zend_object_handlers vectorizer_object_handlers;

/* External API */
PHP_SVGMAGICK_API zend_class_entry *php_options_get_class_entry()
{
	return php_options_sc_entry;
}
PHP_SVGMAGICK_API zend_class_entry *php_progress_get_class_entry()
{
	return php_progress_sc_entry;
}
PHP_SVGMAGICK_API zend_class_entry *php_vectorizer_get_class_entry()
{
	return php_vectorizer_sc_entry;
}
/* Module has no any functions (yet?)*/
static zend_function_entry php_svgmagick_functions[] =
{
	{ NULL, NULL, NULL, 0, 0 }
};

/* Options */
ZEND_BEGIN_ARG_INFO_EX(zero_args, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(options_setTurdSize_args, 0, 0, 1)
ZEND_ARG_INFO(0, turdsize)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(options_setTurnPolicy_args, 0, 0, 1)
ZEND_ARG_INFO(0, turnpolicy)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(options_setAlphaMax_args, 0, 0, 1)
ZEND_ARG_INFO(0, alphamax)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(options_setOptiCurve_args, 0, 0, 1)
ZEND_ARG_INFO(0, opticurve)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(options_setOptTolerance_args, 0, 0, 1)
ZEND_ARG_INFO(0, opttolerance)
ZEND_END_ARG_INFO()

/* Progress */
ZEND_BEGIN_ARG_INFO_EX(progress_construct_args, 0, 0, 1)
ZEND_ARG_OBJ_INFO(0, options, SvgMagick\\Options, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(progress_setLimits_args, 0, 0, 3)
ZEND_ARG_INFO(0, minimum)
ZEND_ARG_INFO(0, maximum)
ZEND_ARG_INFO(0, epsilon)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(progress_setCallback_args, 0, 0, 1)
ZEND_ARG_CALLABLE_INFO(0, function, 0)
ZEND_END_ARG_INFO()

/* Vectorizer */
ZEND_BEGIN_ARG_INFO_EX(vectorizer_construct_args, 0, 0, 1)
ZEND_ARG_OBJ_INFO(0, options, SvgMagick\\Options, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vectorizer_createSimpleBitmap_args, 0, 0, 3)
ZEND_ARG_OBJ_INFO(0, imagick, Imagick, 0)
ZEND_ARG_INFO(0, threshold)
ZEND_ARG_INFO(0, opacity)
ZEND_END_ARG_INFO()

/* Since imagick contains a bug(???) (a function php_imagickpixel_get_class_entry(),
that should return 'php_imagickpixel_sc_entry' returns 'php_imagickdraw_sc_entry'),
we have to set the color through the components (r, g, b, a)*/
ZEND_BEGIN_ARG_INFO_EX(vectorizer_createColorBitmap_args, 0, 0, 5)
ZEND_ARG_OBJ_INFO(0, imagick, Imagick, 0)
ZEND_ARG_INFO(0, red)
ZEND_ARG_INFO(0, green)
ZEND_ARG_INFO(0, blue)
ZEND_ARG_INFO(0, alpha)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vectorizer_stackBitmap_args, 0, 0, 1)
ZEND_ARG_INFO(0, makeStack)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vectorizer_setScale_args, 0, 0, 2)
ZEND_ARG_INFO(0, scaleX)
ZEND_ARG_INFO(0, scaleY)
ZEND_ARG_INFO(0, dx)
ZEND_ARG_INFO(0, dy)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vectorizer_setTranslation_args, 0, 0, 2)
ZEND_ARG_INFO(0, dx)
ZEND_ARG_INFO(0, dy)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vectorizer_setRotation_args, 0, 0, 3)
ZEND_ARG_INFO(0, x)
ZEND_ARG_INFO(0, y)
ZEND_ARG_INFO(0, angle)
ZEND_END_ARG_INFO()

static zend_function_entry php_options_class_methods[] =
{
	PHP_ME(options, setTurnPolicy, options_setTurnPolicy_args, ZEND_ACC_PUBLIC)
	PHP_ME(options, getTurnpolicy, zero_args, ZEND_ACC_PUBLIC)
	PHP_ME(options, setTurdSize, options_setTurdSize_args, ZEND_ACC_PUBLIC)
	PHP_ME(options, getTurdSize, zero_args, ZEND_ACC_PUBLIC)
	PHP_ME(options, setAlphaMax, options_setAlphaMax_args, ZEND_ACC_PUBLIC)
	PHP_ME(options, getAlphaMax, zero_args, ZEND_ACC_PUBLIC)
	PHP_ME(options, setOptiCurve, options_setOptiCurve_args, ZEND_ACC_PUBLIC)
	PHP_ME(options, getOptiCurve, zero_args, ZEND_ACC_PUBLIC)
	PHP_ME(options, setOptTolerance, options_setOptTolerance_args, ZEND_ACC_PUBLIC)
	PHP_ME(options, getOptTolerance, zero_args, ZEND_ACC_PUBLIC)
	
    {NULL, NULL, NULL, 0, 0}
};
static zend_function_entry php_progress_class_methods[] =
{
	PHP_ME(progress, __construct, progress_construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(progress, setLimits, progress_setLimits_args, ZEND_ACC_PUBLIC)
	PHP_ME(progress, setCallback, progress_setCallback_args, ZEND_ACC_PUBLIC)
	PHP_ME(progress, start, zero_args, ZEND_ACC_PUBLIC)
	PHP_ME(progress, end, zero_args, ZEND_ACC_PUBLIC)

    {NULL, NULL, NULL, 0, 0}
};
static zend_function_entry php_vectorizer_class_methods[] =
{
	PHP_ME(vectorizer, __construct, vectorizer_construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(vectorizer, createSimpleBitmap, vectorizer_createSimpleBitmap_args, ZEND_ACC_PUBLIC)
	PHP_ME(vectorizer, createColorBitmap, vectorizer_createColorBitmap_args, ZEND_ACC_PUBLIC)
	PHP_ME(vectorizer, stackBitmap, vectorizer_stackBitmap_args, ZEND_ACC_PUBLIC)
	PHP_ME(vectorizer, setScale, vectorizer_setScale_args, ZEND_ACC_PUBLIC)
	PHP_ME(vectorizer, setTranslation, vectorizer_setTranslation_args, ZEND_ACC_PUBLIC)
	PHP_ME(vectorizer, setRotation, vectorizer_setRotation_args, ZEND_ACC_PUBLIC)
	PHP_ME(vectorizer, trace, zero_args, ZEND_ACC_PUBLIC)
	PHP_ME(vectorizer, getAttributeContent, zero_args, ZEND_ACC_PUBLIC)
	PHP_ME(vectorizer, clear, zero_args, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL, 0, 0}
};
static void php_options_object_destroy(zend_object *object)
{
	php_options_object *intern = php_options_fetch_object(object);
	zend_objects_destroy_object(&intern->zo); /* call __destruct() from userland */
}
static void php_progress_object_destroy(zend_object *object)
{
	php_progress_object *intern = php_progress_fetch_object(object);
	zend_objects_destroy_object(&intern->zo); /* call __destruct() from userland */
}
static void php_vectorizer_object_destroy(zend_object *object)
{
	php_vectorizer_object *intern = php_vectorizer_fetch_object(object);
	zend_objects_destroy_object(&intern->zo); /* call __destruct() from userland */
}
static void php_options_object_free_storage(zend_object *object)
{
	php_options_object *intern = php_options_fetch_object(object);
	if (!intern) {
		return;
	}
	if (intern->options != NULL) {
		potrace_param_free(intern->options);
		intern->options = NULL;
	}
	zend_object_std_dtor(&intern->zo);
	efree(intern);
}
static void php_progress_object_free_storage(zend_object *object)
{
	php_progress_object *intern = php_progress_fetch_object(object);
	if (!intern) {
		return;
	}
	if (intern->progress != NULL) {
		if (intern->progress->data != NULL) {
				free(intern->progress->data);
				intern->progress->data = NULL;
		}
		free(intern->progress);
		intern->progress = NULL;
	}
	zend_object_std_dtor(&intern->zo);
	efree(intern);
}
static void php_vectorizer_object_free_storage(zend_object *object)
{
	php_vectorizer_object *intern = php_vectorizer_fetch_object(object);
	if (!intern) {
		return;
	}
	if (intern->vectorizer != NULL) {
		if (intern->vectorizer->bitmap != NULL) {
			free_internal_bitmap(intern->vectorizer);
		}
		if (intern->vectorizer->state != NULL) {
			free_internal_state(intern->vectorizer);
		}
		free(intern->vectorizer);
		intern->vectorizer = NULL;
	}
	zend_object_std_dtor(&intern->zo);
	efree(intern);
}
static zend_object *php_options_object_new(zend_class_entry *class_type)
{
	php_options_object *intern;

	/* Allocate memory for it */
	intern = ecalloc(1,
		sizeof(php_options_object) +
		zend_object_properties_size(class_type));
	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);
	intern->options = potrace_param_default();
	if (intern->options == NULL) {
		zend_error(E_ERROR, "Failed to create Options object");
	}
	intern->zo.handlers = &options_object_handlers;
	return &intern->zo;
}
static zend_object *php_progress_object_new(zend_class_entry *class_type)
{
	php_progress_object *intern;

	/* Allocate memory for it */
	intern = ecalloc(1,
		sizeof(php_progress_object) +
		zend_object_properties_size(class_type));
	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);
	intern->progress = get_progress_defalts();
	if (!intern->progress) {
		zend_error(E_ERROR, "Failed to create Progress object");
	}
	intern->zo.handlers = &progress_object_handlers;
	return &intern->zo;
}
static zend_object *php_vectorizer_object_new(zend_class_entry *class_type)
{
	php_vectorizer_object *intern;

	/* Allocate memory for it */
	intern = ecalloc(1,
		sizeof(php_vectorizer_object) +
		zend_object_properties_size(class_type));
	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);
	intern->vectorizer=get_vectorizer_defalts();
	if (!intern->vectorizer) {
		zend_error(E_ERROR, "Failed to create Vectorizer object");
	}
	intern->zo.handlers = &vectorizer_object_handlers;
	return &intern->zo;
}

PHP_MINIT_FUNCTION(svgmagick)
{
	zend_class_entry ce;
	/*
	Allocate some memory
	*/
	memcpy(&options_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&progress_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	memcpy(&vectorizer_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	/*
	Initialize exceptions (Options exception)
	*/
	INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(PHP_SVGMAGICK_NS_NAME, PHP_OPTIONS_EXCEPTION_SC_NAME), NULL);
	php_options_exception_class_entry = zend_register_internal_class_ex(&ce, zend_ce_exception);

	/*
	Initialize exceptions (Progress exception)
	*/
	INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(PHP_SVGMAGICK_NS_NAME, PHP_PROGRESS_EXCEPTION_SC_NAME), NULL);
	php_progress_exception_class_entry = zend_register_internal_class_ex(&ce, zend_ce_exception);

	/*
	Initialize exceptions (Vectorizer exception)
	*/
	INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(PHP_SVGMAGICK_NS_NAME, PHP_VECTORIZER_EXCEPTION_SC_NAME), NULL);
	php_vectorizer_exception_class_entry = zend_register_internal_class_ex(&ce, zend_ce_exception);

	/*
	Initialize the class (Options)
	*/
	INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(PHP_SVGMAGICK_NS_NAME, PHP_OPTIONS_SC_NAME), php_options_class_methods);
	ce.create_object = php_options_object_new;
	options_object_handlers.offset = XtOffsetOf(php_options_object, zo);
	options_object_handlers.free_obj = php_options_object_free_storage;
	options_object_handlers.dtor_obj = php_options_object_destroy;
	php_options_sc_entry = zend_register_internal_class(&ce);
	
	php_options_initialize_constants();

	/*
	Initialize the class (Progress)
	*/
	INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(PHP_SVGMAGICK_NS_NAME, PHP_PROGRESS_SC_NAME), php_progress_class_methods);
	ce.create_object = php_progress_object_new;
	progress_object_handlers.offset = XtOffsetOf(php_progress_object, zo);
	progress_object_handlers.free_obj = php_progress_object_free_storage;
	progress_object_handlers.dtor_obj = php_progress_object_destroy;
	php_progress_sc_entry = zend_register_internal_class(&ce);

	/*
	Initialize the class (Vectorizer)
	*/
	INIT_CLASS_ENTRY(ce, ZEND_NS_NAME(PHP_SVGMAGICK_NS_NAME, PHP_VECTORIZER_SC_NAME), php_vectorizer_class_methods);
	ce.create_object = php_vectorizer_object_new;
	vectorizer_object_handlers.offset = XtOffsetOf(php_vectorizer_object, zo);
	vectorizer_object_handlers.free_obj = php_vectorizer_object_free_storage;
	vectorizer_object_handlers.dtor_obj = php_vectorizer_object_destroy;
	php_vectorizer_sc_entry = zend_register_internal_class(&ce);

	return SUCCESS;
}


PHP_MINFO_FUNCTION(svgmagick)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "svgmagick module", "enabled");
	php_info_print_table_row(2, "svgmagick module version", PHP_SVGMAGICK_VERSION);
	php_info_print_table_row(2, "svgmagick namespace", "SvgMagick");
	php_info_print_table_row(2, "svgmagick classes", "Options, Progress, Vectorizer");

	php_info_print_table_header(2, "Class", "Options");
	php_info_print_table_header(2, "Method", "Info");
	php_info_print_table_row(2, "setTurnPolicy(int Options::TURNPOLICY_{NAME})","Default value is Options::TURNPOLICY_MINORITY");
	php_info_print_table_row(2, "getTurnpolicy()", "int");
	php_info_print_table_row(2, "setTurdSize(int new_turdsize)", "Default value is 2");
	php_info_print_table_row(2, "getTurdSize()", "int");
	php_info_print_table_row(2, "setAlphaMax(double new_alphamax)", "Default value is 1.0");
	php_info_print_table_row(2, "getAlphaMax()", "double");
	php_info_print_table_row(2, "setOptiCurve(bool new_opticurve)", "Default value is 1 (true)");
	php_info_print_table_row(2, "gettOptiCurve()", "bool");
	php_info_print_table_row(2, "setOptTolerance(double new_opttolerance)", "Default value is 0.2");
	php_info_print_table_row(2, "getOptTolerance()", "double");

	php_info_print_table_header(2, "Class", "Progress");
	php_info_print_table_header(2, "Method", "Info");
	php_info_print_table_row(2, "__construct(Options obj)", "Binds Progress instance with Options instance");
	php_info_print_table_row(2, "setLimits(double minimum, double maximm, optional double epsilon)", "Default values are 0.0, 1.0 and 0.0");
	php_info_print_table_row(2, "setCallback(callback function)", "Default value is NULL");
	php_info_print_table_row(2, "start()", "Start call callback function");
	php_info_print_table_row(2, "end()", "Last call callback function");

	php_info_print_table_header(2, "Class", "Vectorizer");
	php_info_print_table_header(2, "Method", "Info");
	php_info_print_table_row(2, "__construct(Options obj)", "Binds Vectorizer instance with Options instance");
	php_info_print_table_row(2, "createSimpleBitmap(Imagick obj, optional double treshold, optional double opacity)", "Creates a bitmap of an image by treating colors as shades of gray. Default treshold is 0.5, default opacity is 1.0");
	php_info_print_table_row(2, "createColorBitmap(Imagick obj, double red, double green, double blue, optional double alpha)", "Creates a bitmap of an image by given color components");
	php_info_print_table_row(2, "stackBitmap(bool makeStack)", "Default value is 0 (false)");
	php_info_print_table_row(2, "setScale(double scaleX, optional double scaleY)", "Sets scale factor. if scaleY is not passed, its value is considered equal to scaleX. Defaults are 1.0");
	php_info_print_table_row(2, "setTranslate(double dX, double dX)", "Sets the translation vector. Defaults are 0.0");
	php_info_print_table_row(2, "setRotate(double x, double y, double angle)", "Sets rotation point and angle. Defaults are 0.0");
	php_info_print_table_row(2, "trace()", "Trace current bitmap. if successful return true, otherwise - false");
	php_info_print_table_row(2, "getAttributeContent()", "Returns a string containing the value of the 'd' attribute or null");
	php_info_print_table_row(2, "clear()", "Clears the memory allocated for bitmap and internal trace data, if it exists");
	
	php_info_print_table_header(2, "Constant", "Value");
	php_info_print_table_row(2, "Options::TURNPOLICY_BLACK", "0");
	php_info_print_table_row(2, "Options::TURNPOLICY_WHITE", "1");
	php_info_print_table_row(2, "Options::TURNPOLICY_LEFT", "2");
	php_info_print_table_row(2, "Options::TURNPOLICY_RIGHT", "3");
	php_info_print_table_row(2, "Options::TURNPOLICY_MINORITY", "4");
	php_info_print_table_row(2, "Options::TURNPOLICY_MAJORITY", "5");
	php_info_print_table_row(2, "Options::TURNPOLICY_RANDOM", "6");
	php_info_print_table_end();
}


PHP_MSHUTDOWN_FUNCTION(svgmagick)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(svgmagick)
{
    #if defined(COMPILE_DL_SVGMAGICK) && defined(ZTS)
	    ZEND_TSRMLS_CACHE_UPDATE();
    #endif
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(svgmagick)
{
	return SUCCESS;
}

static const zend_module_dep svgmagick_deps[] = {
	ZEND_MOD_REQUIRED("imagick")
	ZEND_MOD_END
};

zend_module_entry svgmagick_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	svgmagick_deps,
	PHP_SVGMAGICK_EXTNAME,
	NULL,                          /* Functions */
	PHP_MINIT(svgmagick),
	PHP_MSHUTDOWN(svgmagick),     /* MSHUTDOWN */
	PHP_RINIT(svgmagick),         /* RINIT */
	PHP_RSHUTDOWN(svgmagick),     /* RSHUTDOWN */
	PHP_MINFO(svgmagick),         /* MINFO */
	PHP_SVGMAGICK_VERSION,
	STANDARD_MODULE_PROPERTIES
};
#ifdef COMPILE_DL_SVGMAGICK
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(svgmagick)
#endif
