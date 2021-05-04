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

#ifndef PHP_SVGMAGICK_SHARED_H
# define PHP_SVGMAGICK_SHARED_H

#include "php.h"

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef PHP_WIN32
# define PHP_SVGMAGICK_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHP_SVGMAGICK_API __attribute__ ((visibility("default")))
#else
# define PHP_SVGMAGICK_API
#endif

#if defined(ZTS) && defined(COMPILE_DL_SVGMAGICK)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

/* Import configure options when building
outside of the PHP source tree */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/*
Allow extensions to use the class entries
*/
PHP_SVGMAGICK_API zend_class_entry *php_options_get_class_entry();
PHP_SVGMAGICK_API zend_class_entry *php_progress_get_class_entry();
PHP_SVGMAGICK_API zend_class_entry *php_vectorizer_get_class_entry();
#endif /* PHP_SVGMAGICK_SHARED_H */
