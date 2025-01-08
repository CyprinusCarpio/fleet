#include <FLE/Fle_Flat_Button.hpp>

#include <FL/fl_draw.H>

void Fle_Flat_Button::draw()
{
	fl_draw_box(value() ? FL_THIN_DOWN_BOX : m_mouseOver ? FL_THIN_UP_BOX : FL_FLAT_BOX, x(), y(), w(), h(), color());
	draw_label();
}

Fle_Flat_Button::Fle_Flat_Button(int X, int Y, int W, int H, const char* L) : Fl_Button(X, Y, W, H, L)
{
	box(FL_FLAT_BOX);
}

int Fle_Flat_Button::handle(int e)
{
	if (active() && e == FL_ENTER)
	{
		m_mouseOver = true;
		redraw();
		return 1;
	}
	else if (active() && e == FL_LEAVE)
	{
		m_mouseOver = false;
		redraw();
		return 1;
	}

	return Fl_Button::handle(e);
}
