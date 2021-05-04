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
PHP_METHOD(progress, __construct)
{
	zval *id;
	php_progress_object *intern;
	php_options_object *opt_intern;
	zval *options_object = NULL;
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(options_object, php_options_sc_entry)
		ZEND_PARSE_PARAMETERS_END();
	
	id = getThis();
	intern = Z_PROGRESS_P(id);
	
	opt_intern = Z_OPTIONS_P(options_object);
	if (opt_intern->options == NULL) {
		zend_throw_exception(php_progress_exception_class_entry, "Instance of Options class doesn't exist", 0);
		return;
	}
	intern->progress->params = &opt_intern->options->progress;
}

PHP_METHOD(progress, setLimits)
{
	double minimum, maximum;
	double epsilon;
	php_progress_object *intern;
	zval *id;
	
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_DOUBLE(minimum)
		Z_PARAM_DOUBLE(maximum)
		Z_PARAM_OPTIONAL
		Z_PARAM_DOUBLE(epsilon)
		ZEND_PARSE_PARAMETERS_END();
	
	if (minimum > maximum) {
		zend_throw_exception(php_progress_exception_class_entry, "The minimum value must be less than maximum value", 0);
		return;
	}
	int count = ZEND_NUM_ARGS();
	if (count == 3) {
		if ((maximum - minimum) < epsilon) {
			zend_throw_exception(php_progress_exception_class_entry, "The epsilon value must be less than difference between maximum and minimum ", 0);
			return;
		}
	}
	id = getThis();
	intern = Z_PROGRESS_P(id);
	
	if (intern->progress->params == NULL) {
		zend_throw_exception(php_progress_exception_class_entry, "Instance of Options class doesn't initialized properly", 0);
		return;
	}
	intern->progress->params->min = minimum;
	intern->progress->params->max = maximum;
	if (count == 3) {
		intern->progress->params->epsilon = epsilon;
	}
}
PHP_METHOD(progress, setCallback)
{
	zval *id;
	php_progress_object *intern;
	
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC(fci, fci_cache)
		ZEND_PARSE_PARAMETERS_END();
	
	id = getThis();
	intern = Z_PROGRESS_P(id);
	
	if (intern->progress->params == NULL) {
		zend_throw_exception(php_progress_exception_class_entry, "Instance of Options class doesn't initialized properly", 0);
		return;
	}
	if (intern->progress->data == NULL) {
		intern->progress->data = get_data_defaults();
	}
	fci.param_count = 1;
	fci.no_separation = 1;
	copy_fcall(intern->progress->data, &fci, &fci_cache);
	
}
static void php_progress(double d, void *data)
{
	zval args[1];		/* Arguments to userland function */
	zval retval;
	data_t *f_data = (data_t *)data;
	int result = SUCCESS;

	ZVAL_DOUBLE(&args[0], d);
	f_data->fci.params = args;
	f_data->fci.retval = &retval;
	/* Call the userland function */
	result = zend_call_function(&f_data->fci, &f_data->fci_cache);
	if (result == SUCCESS) {
		zval_ptr_dtor(&retval);
	}
	zval_ptr_dtor(&args[0]);
	if (result == FAILURE) {
		zend_throw_exception(php_progress_exception_class_entry, "An error occured while performing callback function", 0);
		return;
	}
}
PHP_METHOD(progress, start)
{
	zval *id;
	php_progress_object *intern;
	
	ZEND_PARSE_PARAMETERS_NONE();
	
	id = getThis();
	intern = Z_PROGRESS_P(id);
	
	if (intern->progress->data == NULL) {
		zend_throw_exception(php_progress_exception_class_entry, "Callback function doesn't initialized properly", 0);
		return;
	}
	intern->progress->params->callback = &php_progress;
	intern->progress->params->data = (void *)intern->progress->data;
	php_progress(intern->progress->params->min, intern->progress->params->data);
}
PHP_METHOD(progress, end)
{
	zval *id;
	php_progress_object *intern;
	
	ZEND_PARSE_PARAMETERS_NONE();
	
	id = getThis();
	intern = Z_PROGRESS_P(id);
	
	if (intern->progress->data == NULL) {
		zend_throw_exception(php_progress_exception_class_entry, "Callback function doesn't initialized properly", 0);
		return;
	}
	php_progress(intern->progress->params->max, intern->progress->params->data);
	intern->progress->params->data = NULL;
}
