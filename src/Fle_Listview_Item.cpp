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
	if(m_displayName == "") m_displayName = " ";

	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		Fl_Align align;
		align = FL_ALIGN_CENTER | FL_ALIGN_INSIDE;
		int mw, mh;
		int lineLength = 0;
		int prevBytes = 0;
		std::string toDraw = m_displayName;
		bool secondLine = false;
		for (int i = 0; i < m_displayName.length(); i++)
		{
			mw = 0;
			const char byte = m_displayName[i];
			char codePoint[4] = {byte, 0, 0, 0};
			int bytes = 0;

			if ((byte & 0xE0) == 0xC0)
			{
				codePoint[1] = m_displayName[i + 1];
				i++;
				bytes = 1;
			}
			else if((byte & 0xE0) == 0xE0)
			{
				codePoint[1] = m_displayName[i + 1];
				codePoint[2] = m_displayName[i + 2];
				i += 2;
				bytes = 2;
			}
			else if((byte & 0xE0) == 0xF0)
			{
				codePoint[1] = m_displayName[i + 1];
				codePoint[2] = m_displayName[i + 2];
				codePoint[3] = m_displayName[i + 3];
				i += 3;
				bytes = 3;
			}

			prevBytes = bytes;
			
			fl_font(FL_HELVETICA, 14);
			fl_measure(codePoint, mw, mh, align);
			lineLength += mw;
			if (lineLength >= 76)
			{
				lineLength = 0;
				if (secondLine)
				{
					m_displayName.erase(m_displayName.begin() + i - bytes - prevBytes - 2, m_displayName.end());
					m_displayName.append("...");
					break;
				}
				else
				{
					m_displayName.insert(i - bytes, "\n");
					i++;
				}

				secondLine = true;
			}
		}
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

void Fle_Listview_Item::set_tooltip(std::string tooltip)
{
	m_tooltip = std::move(tooltip);
}
const std::string &Fle_Listview_Item::get_name() const
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

const std::string &Fle_Listview_Item::get_tooltip() const
{
    return m_tooltip.empty() ? m_name : m_tooltip;
}

int Fle_Listview_Item::get_label_width() const
{
	fl_font(get_listview()->labelfont(), get_listview()->labelsize());
	int lx = 0, ly;
	fl_measure(m_displayName.c_str(), lx, ly);

	return lx;
}

void Fle_Listview_Item::set_display_mode(Fle_Listview_Display_Mode mode)
{
	m_displayMode = mode;

	set_display_name();
}

void Fle_Listview_Item::draw_item(int index)
{
	int detailsMode = m_listview->get_details_mode();
	int textX, textY, textW, textH;

	get_text_xywh(textX, textY, textW, textH);

	if (m_displayMode == FLE_LISTVIEW_DISPLAY_DETAILS && detailsMode == 2 && index % 2 == 1)
	{
		fl_color(fl_color_average(m_listview->color(), FL_BACKGROUND_COLOR, 0.50f));
		fl_rectf(x(), y(), w(), h());
	}
	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		textX -= 2;
		textY -= 2;
		textW += 4;
		textH += 4;
	}
	if (m_selected) 
	{
		fl_color(FL_SELECTION_COLOR);
		fl_rectf(textX,	textY, textW, textH);
	}
	else if (m_bgcolor != 0xFFFFFFFF)
	{
		fl_color(m_bgcolor);
		fl_rectf(textX, textY, textW, textH);
	}

	// Draw icon
	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		m_bigIcon->draw(x() + 21, y());
	}
	else
	{
		m_smallIcon->draw(x(), y() + 2);
	}

	// Draw text
	fl_color(textcolor());
	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		Fl_Align align = FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_WRAP;

		fl_draw(m_displayName.c_str(), textX, textY, textW, textH, align);
	}
	else
	{
		Fl_Align align = FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP;

		if (m_listview->get_item_column_width() != 0 && get_label_width() + 16 > m_listview->get_item_column_width())
		{
			textW = m_listview->get_item_column_width() - 32;
			fl_draw("...", textX + textW, textY, 16, textH, align);
		}

		fl_draw(m_displayName.c_str(), textX, textY, textW, textH, align);
	}

	if (m_displayMode == FLE_LISTVIEW_DISPLAY_DETAILS)
	{
		if(detailsMode == 1 && index != m_listview->get_item_count() - 1)
		{
			fl_color(FL_INACTIVE_COLOR);
			fl_line(x() + 2, y() + 19, x() + w() - 4, y() + 19);
		}

		const std::vector<int>& props = m_listview->get_property_order();

		int prevWidth = 0;

		for (int i : props)
		{
			int width = m_listview->get_property_header_width(i);

			if (detailsMode == 1)
			{
				fl_color(FL_INACTIVE_COLOR);
				fl_line(x() + w() - width - prevWidth, y(), x() + w() - width - prevWidth, y() + h() - 1);
			}

			draw_property(i, x() + w() - prevWidth - width + 4, y(), width, h());

			prevWidth += width;
		}
	}
}

bool Fle_Listview_Item::is_inside_drag_area(int X, int Y)
{
	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		return X >= x() + 15 && X <= x() + w() - 15 && Y >= y() + 15 && Y <= y() + h() - 15;
	}

	return X >= x() && X <= x() + get_label_width() + 16 && Y >= y() && Y <= y() + h();
}
void Fle_Listview_Item::get_text_xywh(int& X, int& Y, int& W, int& H)
{
	X =	x();
	Y = y();
	W = w();
	H = h();

	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		Y += 32;
		H = 14;

		Fl_Align align = FL_ALIGN_TOP | FL_ALIGN_INSIDE;
		int mw, mh;
		fl_measure(m_name.c_str(), mw, mh, align);

		if (mw > 76) H = 28;
	}
	else
	{
		X += 16;
		W -= 16;

		if (m_displayMode == FLE_LISTVIEW_DISPLAY_DETAILS && m_listview->get_details_mode() == 1)
		{
			Y += 1;
			H -= 3;
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