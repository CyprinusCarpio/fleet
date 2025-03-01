#include <FLE/Fle_Orientable_Flex.hpp>

#include <FLE/Fle_Events.hpp>

Fle_Orientable_Flex::Fle_Orientable_Flex(int direction) : Fl_Flex(direction)
{
	m_lastLayoutOrientation = direction;
}

Fle_Orientable_Flex::Fle_Orientable_Flex(int w, int h, int direction) : Fl_Flex(w, h, direction)
{
	m_lastLayoutOrientation = direction;
}

Fle_Orientable_Flex::Fle_Orientable_Flex(int x, int y, int w, int h, int direction) : Fl_Flex(x, y, w, h, direction)
{
	m_lastLayoutOrientation = direction;
}

int Fle_Orientable_Flex::handle(int e)
{
	if (e == FLE_EVENTS_SWITCH_ORIENTATION)
	{
		set_orientation(m_lastLayoutOrientation == 0);
	}

	return Fl_Flex::handle(e);
}

void Fle_Orientable_Flex::set_orientation(int orientation)
{
	if (m_lastLayoutOrientation == orientation)
		return;

	for (int i = 0; i < children(); i++)
	{
		Fl_Widget* c = child(i);

		if (fixed(c))
		{
			c->size(c->h(), c->w());
		}
	}

	type(orientation ? Fl_Flex::HORIZONTAL : Fl_Flex::VERTICAL);
	layout();

	m_lastLayoutOrientation = orientation;
}
