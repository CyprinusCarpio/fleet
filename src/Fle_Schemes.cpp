#include <FLE/Fle_Schemes.hpp>

#include <FL/fl_draw.H>

void fle_define_schemes()
{
	// Scheme 1 - 3D scheme designed for good looks in both dark and light colors
	Fl::set_boxtype(FLE_SCHEME1_UP_BOX, fle_scheme1_up_box_draw, 1, 1, 2, 2);
	Fl::set_boxtype(FLE_SCHEME1_DOWN_BOX, fle_scheme1_down_box_draw, 1, 1, 2, 2);
	Fl::set_boxtype(FLE_SCHEME1_UP_FRAME, fle_scheme1_up_frame_draw, 1, 1, 2, 2);
	Fl::set_boxtype(FLE_SCHEME1_DOWN_FRAME, fle_scheme1_down_frame_draw, 1, 1, 2, 2);
	Fl::set_boxtype(FLE_SCHEME1_THIN_UP_BOX, fle_scheme1_thin_up_box_draw, 1, 1, 2, 2);
	Fl::set_boxtype(FLE_SCHEME1_THIN_DOWN_BOX, fle_scheme1_thin_down_box_draw, 1, 1, 2, 2);
	Fl::set_boxtype(FLE_SCHEME1_THIN_UP_FRAME, fle_scheme1_thin_up_frame_draw, 1, 1, 2, 2);
	Fl::set_boxtype(FLE_SCHEME1_THIN_DOWN_FRAME, fle_scheme1_thin_down_frame_draw, 1, 1, 2, 2);

	// Scheme 2 - gradient scheme designed for good looks in both dark and light colors
	Fl::set_boxtype(FLE_SCHEME2_UP_BOX, fle_scheme2_up_box_draw, 1, 2, 2, 4);
	Fl::set_boxtype(FLE_SCHEME2_DOWN_BOX, fle_scheme2_down_box_draw, 1, 2, 2, 4);
	Fl::set_boxtype(FLE_SCHEME2_UP_FRAME, fle_scheme2_up_frame_draw, 1, 2, 2, 4);
	Fl::set_boxtype(FLE_SCHEME2_DOWN_FRAME, fle_scheme2_down_frame_draw, 1, 2, 2, 4);
	Fl::set_boxtype(FLE_SCHEME2_THIN_UP_BOX, fle_scheme2_thin_up_box_draw, 1, 1, 2, 2);
	Fl::set_boxtype(FLE_SCHEME2_THIN_DOWN_BOX, fle_scheme2_thin_down_box_draw, 1, 1, 2, 2);
	Fl::set_boxtype(FLE_SCHEME2_THIN_UP_FRAME, fle_scheme2_thin_up_frame_draw, 1, 1, 2, 2);
	Fl::set_boxtype(FLE_SCHEME2_THIN_DOWN_FRAME, fle_scheme2_thin_down_frame_draw, 1, 1, 2, 2);
}

void fle_set_scheme1()
{
	Fl::scheme("gleam");

	Fl::set_boxtype(FL_UP_BOX, FLE_SCHEME1_UP_BOX);
	Fl::set_boxtype(FL_DOWN_BOX, FLE_SCHEME1_DOWN_BOX);
	Fl::set_boxtype(FL_UP_FRAME, FLE_SCHEME1_UP_FRAME);
	Fl::set_boxtype(FL_DOWN_FRAME, FLE_SCHEME1_DOWN_FRAME);
	Fl::set_boxtype(FL_THIN_UP_BOX, FLE_SCHEME1_THIN_UP_BOX);
	Fl::set_boxtype(FL_THIN_DOWN_BOX, FLE_SCHEME1_THIN_DOWN_BOX);
	Fl::set_boxtype(FL_THIN_UP_FRAME, FLE_SCHEME1_THIN_UP_FRAME);
	Fl::set_boxtype(FL_THIN_DOWN_FRAME, FLE_SCHEME1_THIN_DOWN_FRAME);
}

void fle_set_scheme2()
{
	Fl::scheme("gleam");

	Fl::set_boxtype(FL_UP_BOX, FLE_SCHEME2_UP_BOX);
	Fl::set_boxtype(FL_DOWN_BOX, FLE_SCHEME2_DOWN_BOX);
	Fl::set_boxtype(FL_UP_FRAME, FLE_SCHEME2_UP_FRAME);
	Fl::set_boxtype(FL_DOWN_FRAME, FLE_SCHEME2_DOWN_FRAME);
	Fl::set_boxtype(FL_THIN_UP_BOX, FLE_SCHEME2_THIN_UP_BOX);
	Fl::set_boxtype(FL_THIN_DOWN_BOX, FLE_SCHEME2_THIN_DOWN_BOX);
	Fl::set_boxtype(FL_THIN_UP_FRAME, FLE_SCHEME2_THIN_UP_FRAME);
	Fl::set_boxtype(FL_THIN_DOWN_FRAME, FLE_SCHEME2_THIN_DOWN_FRAME);
}

void fle_scheme1_up_box_draw(int x, int y, int w, int h, Fl_Color c)
{
	fl_rectf(x + 1, y + 1, w - 2, h - 2, Fl::box_color(c));
	fle_scheme1_up_frame_draw(x, y, w, h, c);
}

void fle_scheme1_down_box_draw(int x, int y, int w, int h, Fl_Color c)
{
	fl_rectf(x + 1, y + 1, w - 2, h - 2, Fl::box_color(c));
	fle_scheme1_down_frame_draw(x, y, w, h, c);
}

void fle_scheme1_up_frame_draw(int x, int y, int w, int h, Fl_Color c)
{
	Fl::set_box_color(fl_darker(c));
	fl_xyline(x + 1, y, x + w - 2, y);
	fl_xyline(x + 1, y + h - 1, x + w - 2, y + h - 1);
	fl_xyline(x, y + 1, x, y + h - 2);
	fl_xyline(x + w - 1, y + 1, x + w - 1, y + h - 2);

	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.5f));
	fl_xyline(x + 2, y + h - 3, x + w - 2, y + h - 3);
	fl_xyline(x + 1, y + h - 2, x + w - 2, y + h - 2);
	fl_xyline(x + w - 3, y + 3, x + w - 3, y + h - 4);
	fl_xyline(x + w - 2, y + 2, x + w - 2, y + h - 4);

	Fl::set_box_color(fl_color_average(c, fl_lighter(c), 0.45f));
	fl_xyline(x + 1, y + 1, x + w - 2, y + 1);
	fl_xyline(x + 1, y + 2, x + w - 3, y + 2);
	fl_xyline(x + 1, y + 2, x + 1, y + h - 3);
	fl_xyline(x + 2, y + 2, x + 2, y + h - 4);
}

void fle_scheme1_down_frame_draw(int x, int y, int w, int h, Fl_Color c)
{
	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.15f));
	fl_xyline(x + 1, y, x + w - 2, y);
	fl_xyline(x, y + 1, x + w - 1, y + 1);
	fl_xyline(x, y + h - 2, x + w - 1, y + h - 2);
	fl_xyline(x + 1, y + h - 1, x + w - 2, y + h - 1);
	fl_xyline(x, y + 2, x, y + h - 3);
	fl_xyline(x + 1, y + 2, x + 1, y + h - 3);
	fl_xyline(x + w - 1, y + 2, x + w - 1, y + h - 3);
	fl_xyline(x + w - 2, y + 2, x + w - 2, y + h - 3);

	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.5f));
	fl_xyline(x + 2, y + 2, x + w - 3, y + 2);
	fl_xyline(x + 2, y + 3, x + 2, y + h - 3);
}

void fle_scheme1_thin_up_box_draw(int x, int y, int w, int h, Fl_Color c)
{
	fl_rectf(x + 1, y + 1, w - 2, h - 2, Fl::box_color(c));
	fle_scheme1_thin_up_frame_draw(x, y, w, h, c);
}

void fle_scheme1_thin_down_box_draw(int x, int y, int w, int h, Fl_Color c)
{
	fl_rectf(x + 1, y + 1, w - 2, h - 2, Fl::box_color(c));
	fle_scheme1_thin_down_frame_draw(x, y, w, h, c);
}

void fle_scheme1_thin_up_frame_draw(int x, int y, int w, int h, Fl_Color c)
{
	Fl::set_box_color(fl_lighter(c));
	fl_xyline(x + 1, y, x + w - 2, y);
	fl_xyline(x, y + 1, x, y + h - 2);

	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.5f));
	fl_xyline(x + 1, y + h - 1, x + w - 2, y + h - 1);
	fl_xyline(x + w - 1, y + 1, x + w - 1, y + h - 2);
}

void fle_scheme1_thin_down_frame_draw(int x, int y, int w, int h, Fl_Color c)
{
	Fl::set_box_color(fl_darker(c));
	fl_xyline(x + 1, y, x + w - 2, y);
	fl_xyline(x, y + 1, x, y + h - 2);

	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.5f));
	fl_xyline(x + 1, y + h - 1, x + w - 2, y + h - 1);
	fl_xyline(x + w - 1, y + 1, x + w - 1, y + h - 2);
}

void fle_scheme2_up_box_draw(int x, int y, int w, int h, Fl_Color c)
{
	fl_rectf(x + 1, y + 1, w - 2, h - 2, Fl::box_color(c));
	fle_scheme2_up_frame_draw(x, y, w, h, c);
}

void fle_scheme2_down_box_draw(int x, int y, int w, int h, Fl_Color c)
{
	fl_rectf(x + 1, y + 1, w - 2, h - 2, Fl::box_color(c));
	fle_scheme2_down_frame_draw(x, y, w, h, c);
}

void fle_scheme2_up_frame_draw(int x, int y, int w, int h, Fl_Color c)
{
	Fl::set_box_color(fl_darker(c));
	fl_xyline(x + 2, y, x + w - 3, y);
	fl_xyline(x + 2, y + h - 1, x + w - 3, y + h - 1);
	fl_point(x + 1, y + 1);
	fl_point(x + 1, y + h - 2);
	fl_point(x + w - 2, y + 1);
	fl_point(x + w - 2, y + h - 2);
	fl_xyline(x, y + 2, x, y + h - 3);
	fl_xyline(x + w - 1, y + 2, x + w - 1, y + h - 3);

	Fl::set_box_color(fl_color_average(c, fl_lighter(c), 0.25f));
	fl_xyline(x + 2, y + 1, x + w - 3, y + 1);
	fl_xyline(x + 2, y + h - 2, x + w - 3, y + h - 2);

	Fl::set_box_color(fl_color_average(c, fl_lighter(c), 0.45f));
	fl_xyline(x + 1, y + 2, x + w - 2, y + 2);
	fl_xyline(x + 1, y + h - 3, x + w - 2, y + h - 3);

	Fl::set_box_color(fl_color_average(c, fl_lighter(c), 0.65f));
	fl_xyline(x + 1, y + 3, x + w - 2, y + 3);
	fl_xyline(x + 1, y + h - 4, x + w - 2, y + h - 4);

	Fl::set_box_color(fl_color_average(c, fl_lighter(c), 0.85f));
	fl_xyline(x + 1, y + 4, x + w - 2, y + 4);
	fl_xyline(x + 1, y + h - 5, x + w - 2, y + h - 5);
	fl_xyline(x + 1, y + 5, x + 1, y + h - 6);
	fl_xyline(x + w - 2, y + 5, x + w - 2, y + h - 6);
}

void fle_scheme2_down_frame_draw(int x, int y, int w, int h, Fl_Color c)
{
	Fl::set_box_color(fl_darker(c));
	fl_xyline(x + 2, y, x + w - 3, y);
	fl_xyline(x + 2, y + h - 1, x + w - 3, y + h - 1);
	fl_point(x + 1, y + 1);
	fl_point(x + 1, y + h - 2);
	fl_point(x + w - 2, y + 1);
	fl_point(x + w - 2, y + h - 2);
	fl_xyline(x, y + 2, x, y + h - 3);
	fl_xyline(x + w - 1, y + 2, x + w - 1, y + h - 3);

	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.25f));
	fl_xyline(x + 2, y + 1, x + w - 3, y + 1);
	fl_xyline(x + 2, y + h - 2, x + w - 3, y + h - 2);

	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.45f));
	fl_xyline(x + 1, y + 2, x + w - 2, y + 2);
	fl_xyline(x + 1, y + h - 3, x + w - 2, y + h - 3);

	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.65f));
	fl_xyline(x + 1, y + 3, x + w - 2, y + 3);
	fl_xyline(x + 1, y + h - 4, x + w - 2, y + h - 4);

	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.85f));
	fl_xyline(x + 1, y + 4, x + w - 2, y + 4);
	fl_xyline(x + 1, y + h - 5, x + w - 2, y + h - 5);
	fl_xyline(x + 1, y + 5, x + 1, y + h - 6);
	fl_xyline(x + w - 2, y + 5, x + w - 2, y + h - 6);
}

void fle_scheme2_thin_up_box_draw(int x, int y, int w, int h, Fl_Color c)
{
	fl_rectf(x + 1, y + 1, w - 2, h - 2, Fl::box_color(c));
	fle_scheme2_thin_up_frame_draw(x, y, w, h, c);
}

void fle_scheme2_thin_down_box_draw(int x, int y, int w, int h, Fl_Color c)
{
	fl_rectf(x + 1, y + 1, w - 2, h - 2, Fl::box_color(c));
	fle_scheme2_thin_down_frame_draw(x, y, w, h, c);
}

void fle_scheme2_thin_up_frame_draw(int x, int y, int w, int h, Fl_Color c)
{
	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.45f));
	fl_xyline(x + 2, y, x + w - 3, y);
	fl_xyline(x + 2, y + h - 1, x + w - 3, y + h - 1);
	fl_point(x + 1, y + 1);
	fl_point(x + 1, y + h - 2);
	fl_point(x + w - 2, y + 1);
	fl_point(x + w - 2, y + h - 2);
	fl_xyline(x, y + 2, x, y + h - 3);
	fl_xyline(x + w - 1, y + 2, x + w - 1, y + h - 3);

	Fl::set_box_color(fl_color_average(c, fl_lighter(c), 0.65f));
	fl_xyline(x + 2, y + 1, x + w - 3, y + 1);
	fl_xyline(x + 2, y + h - 2, x + w - 3, y + h - 2);

	Fl::set_box_color(fl_color_average(c, fl_lighter(c), 0.85f));
	fl_xyline(x + 1, y + 2, x + w - 2, y + 2);
	fl_xyline(x + 1, y + h - 3, x + w - 2, y + h - 3);
	fl_xyline(x + 1, y + 3, x + 1, y + h - 3);
	fl_xyline(x + w - 2, y + 3, x + w - 2, y + h - 3);
}

void fle_scheme2_thin_down_frame_draw(int x, int y, int w, int h, Fl_Color c)
{
	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.45f));
	fl_xyline(x + 2, y, x + w - 3, y);
	fl_xyline(x + 2, y + h - 1, x + w - 3, y + h - 1);
	fl_point(x + 1, y + 1);
	fl_point(x + 1, y + h - 2);
	fl_point(x + w - 2, y + 1);
	fl_point(x + w - 2, y + h - 2);
	fl_xyline(x, y + 2, x, y + h - 3);
	fl_xyline(x + w - 1, y + 2, x + w - 1, y + h - 3);

	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.65f));
	fl_xyline(x + 2, y + 1, x + w - 3, y + 1);
	fl_xyline(x + 2, y + h - 2, x + w - 3, y + h - 2);

	Fl::set_box_color(fl_color_average(c, fl_darker(c), 0.85f));
	fl_xyline(x + 1, y + 2, x + w - 2, y + 2);
	fl_xyline(x + 1, y + h - 3, x + w - 2, y + h - 3);
	fl_xyline(x + 1, y + 3, x + 1, y + h - 3);
	fl_xyline(x + w - 2, y + 3, x + w - 2, y + h - 3);
}
