#include <FLE/Fle_Listview_Item.hpp>

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/fl_draw.H>

#include <FLE/Fle_Listview.hpp>

static char* default_icon_small[] = {
"16 16 3 1",
" 	c None",
".	c #474747",
"+	c #EFEFEF",
"                ",
"  ........      ",
"  .++++++..     ",
"  .++++++.+.    ",
"  .++++++.++.   ",
"  .++++++.....  ",
"  .++++++++++.  ",
"  .++++++++++.  ",
"  .++++++++++.  ",
"  .++++++++++.  ",
"  .++++++++++.  ",
"  .++++++++++.  ",
"  .++++++++++.  ",
"  .++++++++++.  ",
"  ............  ",
"                " };

static char* default_icon_big[] = {
"32 32 3 1",
" 	c None",
".	c #474747",
"+	c #EFEFEF",
"                                ",
"                                ",
"    ................            ",
"    .+++++++++++++.+.           ",
"    .+++++++++++++.++.          ",
"    .+++++++++++++.+++.         ",
"    .+++++++++++++.++++.        ",
"    .+++++++++++++.+++++.       ",
"    .+++++++++++++.++++++.      ",
"    .+++++++++++++.+++++++.     ",
"    .+++++++++++++.++++++++.    ",
"    .+++++++++++++..........    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    .++++++++++++++++++++++.    ",
"    ........................    ",
"                                ",
"                                " };


Fl_Pixmap* defaultImgSmall = nullptr;
Fl_Pixmap* defaultImgBig = nullptr;

Fle_Listview_Item::Fle_Listview_Item(const char* name)
{
	m_selected = false;
	m_focused = false;
	m_name = name;
	m_textcolor = FL_FOREGROUND_COLOR;
	m_bgcolor = 0xFFFFFFFF;
	set_display_mode(FLE_LISTVIEW_DISPLAY_LIST);

	if (!defaultImgSmall) defaultImgSmall = new Fl_Pixmap(default_icon_small);
	if (!defaultImgBig) defaultImgBig = new Fl_Pixmap(default_icon_big);

	m_smallIcon = defaultImgSmall;
	m_bigIcon = defaultImgBig;

	set_display_name();
}

void Fle_Listview_Item::set_display_name()
{
	m_displayName = m_name;
	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		if(m_displayName.length() > 9)
			m_displayName.insert(m_displayName.begin() + 9, ' ');
	}

	
}

Fle_Listview* Fle_Listview_Item::get_listview() const
{
	return m_listview;
}

void Fle_Listview_Item::set_selected(bool selected)
{
	if (selected == m_selected) return;

	m_selected = selected;
}

void Fle_Listview_Item::set_focus(bool focus)
{
	if (focus == m_focused) return;

	m_focused = focus;
}

bool Fle_Listview_Item::is_greater(Fle_Listview_Item* other, int property)
{
	if (property == -1)
	{
		return m_name > other->get_name();
	}

	return false;
}

bool Fle_Listview_Item::is_selected() const
{
	return m_selected;
}

void Fle_Listview_Item::textcolor(Fl_Color color)
{
	m_textcolor = color;
}

Fl_Color Fle_Listview_Item::textcolor() const
{
	if (m_selected)
	{
		return fl_contrast(m_textcolor, FL_SELECTION_COLOR);
	}
	return m_textcolor;
}

void Fle_Listview_Item::bgcolor(Fl_Color bgcolor)
{
	m_bgcolor = bgcolor;
}

Fl_Color Fle_Listview_Item::bgcolor() const
{
	return m_bgcolor;
}

void Fle_Listview_Item::set_icon(Fl_Pixmap* small, Fl_Pixmap* big)
{
	if (small == nullptr) small = defaultImgSmall;
	if (big == nullptr) small = defaultImgBig;

	m_smallIcon = small;
	m_bigIcon = big;
}

void Fle_Listview_Item::set_name(std::string newname)
{
	m_name = std::move(newname);
}

const std::string& Fle_Listview_Item::get_name() const
{
	return m_name;
}

void Fle_Listview_Item::resize(int X, int Y, int W, int H)
{
	m_x = X;
	m_y = Y;
	m_w = W;
	m_h = H;
}

int Fle_Listview_Item::get_label_width() const
{
	fl_font(get_listview()->labelfont(), get_listview()->labelsize());
	int lx = 0, ly;
	fl_measure(m_displayName.c_str(), lx, ly);

	return lx + 5;
}

void Fle_Listview_Item::set_display_mode(Fle_Listview_Display_Mode mode)
{
	m_displayMode = mode;

	set_display_name();
}

void Fle_Listview_Item::draw_item(bool last)
{
	if (m_selected) 
	{
		fl_color(FL_SELECTION_COLOR);
		fl_rectf(x(), y(), w(), h());
	}
	else if (m_bgcolor != 0xFFFFFFFF)
	{
		fl_color(m_bgcolor);
		fl_rectf(x(), y(), w(), h());
	}
	Fl_Align align;
	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		align = FL_ALIGN_TOP | FL_ALIGN_IMAGE_OVER_TEXT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP | FL_ALIGN_CLIP;
	}
	else
		align = FL_ALIGN_LEFT | FL_ALIGN_IMAGE_NEXT_TO_TEXT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP;

	fl_color(textcolor());
	fl_draw(m_displayName.c_str(), x(), y(), w(), h(), align, m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS ? m_bigIcon : m_smallIcon);

	if (m_displayMode == FLE_LISTVIEW_DISPLAY_DETAILS)
	{
		if(get_listview()->get_details_helper_lines() && !last)
		{
			fl_color(FL_INACTIVE_COLOR);
			fl_line(x() + 2, y() + 19, x() + w() - 4, y() + 19);
		}

		Fle_Listview* lv = get_listview();

		const std::vector<int>& props = lv->get_property_order();

		bool lines = lv->get_details_helper_lines();
		int prevWidth = 0;

		for (int i : props)
		{
			int width = lv->get_property_header_width(i);

			if (lines)
			{
				fl_color(FL_INACTIVE_COLOR);
				fl_line(x() + w() - width - prevWidth, y(), x() + w() - width - prevWidth, y() + h() - 1);
			}

			draw_property(i, x() + w() - prevWidth - width + 4, y(), width, h());

			prevWidth += width;
		}
	}
}

void Fle_Listview_Item::draw_property(int property, int X, int Y, int W, int H)
{
}

int Fle_Listview_Item::x() const
{
	return m_x + m_listview->x() + m_listview->get_margin() - m_listview->m_hscrollbar.value();
}

int Fle_Listview_Item::y() const
{
	return m_y + m_listview->y() + m_listview->get_margin() - m_listview->m_vscrollbar.value();
}