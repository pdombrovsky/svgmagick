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

#include "path_converter.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline potrace_dpoint_t transform(potrace_dpoint_t point, const transform_t *trfm_ptr) {
	
	potrace_dpoint_t res;

	if (fabs(trfm_ptr->angle) > 1e-3) {

		double s = sin(trfm_ptr->angle * M_PI / 180);
		double c = cos(trfm_ptr->angle * M_PI / 180);

		double dx = point.x - trfm_ptr->rotation_point.x;
		double dy = point.y - trfm_ptr->rotation_point.y;

		res.x = dx * c - dy * s + trfm_ptr->rotation_point.x;
		res.y = dx * s + dy * c + trfm_ptr->rotation_point.y;
		
		res.x = trfm_ptr->scale.x * res.x + trfm_ptr->translate.x;
		res.y = trfm_ptr->scale.y * res.y + trfm_ptr->translate.y;
		
		return res;
	}
	res.x = trfm_ptr->scale.x * point.x + trfm_ptr->translate.x;
	res.y = trfm_ptr->scale.y * point.y + trfm_ptr->translate.y;
	return res;
}

static void curve_to(d_attr_t *d_ptr, potrace_dpoint_t point1, potrace_dpoint_t point2, potrace_dpoint_t point3)
{
	potrace_dpoint_t new_point1 = transform(point1, d_ptr->transform);
	potrace_dpoint_t new_point2 = transform(point2, d_ptr->transform);
	potrace_dpoint_t new_point3 = transform(point3, d_ptr->transform);

	if (d_ptr->prev_command != 'c') {
		smart_str_append_printf(d_ptr->content,
			"c%f %f %f %f %f %f ",
			new_point1.x - d_ptr->prev_point.x,
			new_point1.y - d_ptr->prev_point.y,
			new_point2.x - d_ptr->prev_point.x,
			new_point2.y - d_ptr->prev_point.y,
			new_point3.x - d_ptr->prev_point.x,
			new_point3.y - d_ptr->prev_point.y
		);
	}
	else {
		smart_str_append_printf(d_ptr->content,
			"%f %f %f %f %f %f ",
			new_point1.x - d_ptr->prev_point.x,
			new_point1.y - d_ptr->prev_point.y,
			new_point2.x - d_ptr->prev_point.x,
			new_point2.y - d_ptr->prev_point.y,
			new_point3.x - d_ptr->prev_point.x,
			new_point3.y - d_ptr->prev_point.y
		);
	}
	
	d_ptr->prev_point = new_point3;
	d_ptr->prev_command = 'c';
}

static void line_to(d_attr_t *d_ptr, potrace_dpoint_t point)
{
	potrace_dpoint_t new_point = transform(point, d_ptr->transform);

	if (d_ptr->prev_command != 'l') {
		smart_str_append_printf(d_ptr->content, "l%f %f ", new_point.x - d_ptr->prev_point.x, new_point.y - d_ptr->prev_point.y);
	}
	else {
		smart_str_append_printf(d_ptr->content, "%f %f ", new_point.x - d_ptr->prev_point.x, new_point.y - d_ptr->prev_point.y);
	}
	d_ptr->prev_point = new_point;
	d_ptr->prev_command = 'l';
}

static void move_to(d_attr_t *d_ptr, potrace_dpoint_t point)
{
	potrace_dpoint_t new_point = transform(point, d_ptr->transform);

	d_ptr->prev_point = new_point;
	smart_str_append_printf(d_ptr->content, "M%f %f ", new_point.x, new_point.y);
	d_ptr->prev_command = 'M';
}

static void r_move_to(d_attr_t *d_ptr, potrace_dpoint_t point)
{
	potrace_dpoint_t new_point = transform(point, d_ptr->transform);

	smart_str_append_printf(d_ptr->content, "m%f %f ", new_point.x - d_ptr->prev_point.x, new_point.y - d_ptr->prev_point.y);
	d_ptr->prev_point = new_point;
	d_ptr->prev_command = 'm';
}

static int get_path(d_attr_t *d_ptr, potrace_curve_t *curve, int abs)
{
	int i;
	potrace_dpoint_t *c;
	int m = curve->n;

	c = curve->c[m - 1];
	if (abs) {
		move_to(d_ptr, c[2]);
	}
	else {
		r_move_to(d_ptr, c[2]);
	}

	for (i = 0; i<m; i++) {
		c = curve->c[i];
		switch (curve->tag[i]) {
		case POTRACE_CORNER:
			line_to(d_ptr, c[1]);
			line_to(d_ptr, c[2]);
			break;
		case POTRACE_CURVETO:
			curve_to(d_ptr, c[0], c[1], c[2]);
			break;
		}
	}
	
	smart_str_appends(d_ptr->content, "z ");
	return 0;
}

static void write_paths(d_attr_t *d_ptr, potrace_path_t *plist)
{
	potrace_path_t *p, *q;

	for (p = plist; p; p = p->sibling) {
		get_path(d_ptr, &p->curve, 1);
		for (q = p->childlist; q; q = q->sibling) {
			get_path(d_ptr, &q->curve, 0);
		}
		for (q = p->childlist; q; q = q->sibling) {
			write_paths(d_ptr, q->childlist);
		}
	}
}

void convert_path(smart_str *content, transform_t *transform, potrace_path_t *plist)
{
	d_attr_t d = { 0 };
	d.content = content;
	d.transform = transform;
	write_paths(&d, plist);
	smart_str_0(content);
}