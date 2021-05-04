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

PHP_METHOD(options, setTurnPolicy)
{
	int turnpolicy;
	php_options_object *intern;
	zval *id;
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(turnpolicy)
	ZEND_PARSE_PARAMETERS_END();
	
	if (turnpolicy < POTRACE_TURNPOLICY_BLACK || turnpolicy > POTRACE_TURNPOLICY_RANDOM) {
		zend_throw_exception(php_options_exception_class_entry, "The turnpolicy value must match one of the values defined in Options constants", 0);
		return;
	}
	id = getThis();
	intern = Z_OPTIONS_P(id);
	
	intern->options->turnpolicy = turnpolicy;
}
PHP_METHOD(options, getTurnpolicy)
{
	php_options_object *intern;
	zval *id;
	
	ZEND_PARSE_PARAMETERS_NONE();
	
	id = getThis();
	intern = Z_OPTIONS_P(id);
	
	RETURN_LONG(intern->options->turnpolicy);
}
PHP_METHOD(options, setTurdSize)
{
	int turdsize;
	php_options_object *intern;
	zval *id;
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(turdsize)
		ZEND_PARSE_PARAMETERS_END();
	
	if (turdsize < 0) {
		zend_throw_exception(php_options_exception_class_entry, "The turdsize value must be greater than 0", 0);
		return;
	}
	id = getThis();
	intern = Z_OPTIONS_P(id);
	
	intern->options->turdsize = turdsize;
}
PHP_METHOD(options, getTurdSize)
{
	php_options_object *intern;
	zval *id;
	
	ZEND_PARSE_PARAMETERS_NONE();
	
	id = getThis();
	intern = Z_OPTIONS_P(id);
	
	RETURN_LONG(intern->options->turdsize);
}
PHP_METHOD(options, setAlphaMax)
{
	double alphamax;
	php_options_object *intern;
	zval *id;
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(alphamax)
		ZEND_PARSE_PARAMETERS_END();
	
	if (alphamax < 0.0 || alphamax > 1.3334) {
		zend_throw_exception(php_options_exception_class_entry, "The alphamax value must be between 0.0 and 1.3334", 0);
		return;
	}
	id = getThis();
	intern = Z_OPTIONS_P(id);
	
	intern->options->alphamax = alphamax;
}
PHP_METHOD(options, getAlphaMax)
{
	php_options_object *intern;
	zval *id;
	
	ZEND_PARSE_PARAMETERS_NONE();
	
	id = getThis();
	intern = Z_OPTIONS_P(id);
	
	RETURN_DOUBLE(intern->options->alphamax);
}
PHP_METHOD(options, setOptiCurve)
{
	zend_bool opticurve;
	php_options_object *intern;
	zval *id;
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_BOOL(opticurve)
		ZEND_PARSE_PARAMETERS_END();
	
	id = getThis();
	intern = Z_OPTIONS_P(id);
	intern->options->opticurve = opticurve;
}
PHP_METHOD(options, getOptiCurve)
{
	php_options_object *intern;
	zval *id;
	zend_error_handling error_handling;

	ZEND_PARSE_PARAMETERS_NONE();
	
	id = getThis();
	intern = Z_OPTIONS_P(id);
	
	if (intern->options->opticurve == 1)
		RETURN_TRUE;
	RETURN_FALSE;
}
PHP_METHOD(options, setOptTolerance)
{
	double opttolerance;
	php_options_object *intern;
	zval *id;
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(opttolerance)
		ZEND_PARSE_PARAMETERS_END();

	if (opttolerance < 0.0) {
		zend_throw_exception(php_options_exception_class_entry, "The opttolerance value must be greater than 0", 0);
		return;
	}

	id = getThis();
	intern = Z_OPTIONS_P(id);
	
	intern->options->opttolerance = opttolerance;
}
PHP_METHOD(options, getOptTolerance)
{
	php_options_object *intern;
	zval *id;
	
	ZEND_PARSE_PARAMETERS_NONE();
	
	id = getThis();
	intern = Z_OPTIONS_P(id);
	
	RETURN_DOUBLE(intern->options->opttolerance);
}
