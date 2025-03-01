#include <FLE/Fle_Toolbar.hpp>

#include <FL/Fl_Box.H>

Fle_Toolbar::Fle_Toolbar(int X, int Y, int W, int H, int direction) : Fle_Orientable_Flex(X, Y, W, H, direction)
{
	end();
}

void Fle_Toolbar::add_separator()
{
	insert_separator(nullptr);
}

void Fle_Toolbar::insert_separator(Fl_Widget* before)
{
	Fl_Box* separator = new Fl_Box(0, 0, 3, 3);
	separator->box(FL_GTK_THIN_UP_FRAME);

	if (!before)
	{
		add_tool(separator);
		return;
	}

	insert_tool(separator, before);
}

void Fle_Toolbar::add_tool(Fl_Widget* widget)
{
	bool vertical = type() == Fl_Flex::VERTICAL;

	add(widget);
	fixed(widget, vertical ? widget->h() : widget->w());
}

void Fle_Toolbar::insert_tool(Fl_Widget* widget, Fl_Widget* before)
{
	bool vertical = type() == Fl_Flex::VERTICAL;

	insert(*widget, before);
	fixed(widget, vertical ? widget->h() : widget->w());
}