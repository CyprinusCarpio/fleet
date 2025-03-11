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

Fle_Listview_Item::Fle_Listview_Item(const char* name) : Fl_Box(0, 0, 32, 32, "")
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

	box(FL_NO_BOX);
	image(defaultImgSmall);

	set_display_name();
}

void Fle_Listview_Item::set_display_name()
{
	std::string name = m_name;
	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		if(name.length() > 9)
			name.insert(name.begin() + 9, ' ');
	}

	copy_label(name.c_str());
}

Fle_Listview* Fle_Listview_Item::get_listview() const
{
	return (Fle_Listview*)parent()->parent();
}

void Fle_Listview_Item::set_selected(bool selected)
{
	if (selected == m_selected) return;

	m_selected = selected;

	if (m_selected)
	{
		box(FL_FLAT_BOX);
		labelcolor(fl_contrast(labelcolor(), FL_SELECTION_COLOR));
		color(FL_SELECTION_COLOR);
	}
	else
	{
		labelcolor(m_textcolor);
		if (m_bgcolor == 0xFFFFFFFF)
		{
			color(parent()->color());
			box(FL_FLAT_BOX);
		}
		else
		{
			color(m_bgcolor);
			box(FL_FLAT_BOX);
		}
	}

	redraw();
}

void Fle_Listview_Item::set_focus(bool focus)
{
	if (focus == m_focused) return;

	m_focused = focus;
	redraw();
}

bool Fle_Listview_Item::is_greater(Fle_Listview_Item* other, int property)
{
	if (property == -1)
	{
		return m_name > other->get_name();
	}
}

bool Fle_Listview_Item::is_selected() const
{
	return m_selected;
}

void Fle_Listview_Item::textcolor(Fl_Color color)
{
	m_textcolor = color;

	if (!m_selected)
	{
		labelcolor(m_textcolor);
		redraw();
	}
}

void Fle_Listview_Item::bgcolor(Fl_Color bgcolor)
{
	box(FL_FLAT_BOX);

	m_bgcolor = bgcolor;

	if (!m_selected)
	{
		color(m_bgcolor);
		redraw();
	}
}

void Fle_Listview_Item::set_icon(Fl_Pixmap* small, Fl_Pixmap* big)
{
	if (small == nullptr) small = defaultImgSmall;
	if (big == nullptr) small = defaultImgBig;

	m_smallIcon = small;
	m_bigIcon = big;

	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		image(m_bigIcon);
		return;
	}

	image(m_smallIcon);
}

void Fle_Listview_Item::set_name(std::string newname)
{
	m_name = std::move(newname);
}

const std::string& Fle_Listview_Item::get_name() const
{
	return m_name;
}

int Fle_Listview_Item::get_label_width() const
{
	int lx = 0, ly;
	measure_label(lx, ly);

	return lx + 5;
}

int Fle_Listview_Item::handle(int e)
{
	if (e == FL_PUSH)
	{
		get_listview()->set_selected(this, true);
	}
	return Fl_Box::handle(e);
;}


void Fle_Listview_Item::set_display_mode(Fle_Listview_Display_Mode mode)
{
	m_displayMode = mode;

	switch (m_displayMode)
	{
	case FLE_LISTVIEW_DISPLAY_ICONS:
		align(FL_ALIGN_TOP | FL_ALIGN_IMAGE_OVER_TEXT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP | FL_ALIGN_CLIP);
		image(m_bigIcon);
		break;
	case FLE_LISTVIEW_DISPLAY_SMALL_ICONS:
	case FLE_LISTVIEW_DISPLAY_DETAILS:
	case FLE_LISTVIEW_DISPLAY_LIST:
		align(FL_ALIGN_LEFT | FL_ALIGN_IMAGE_NEXT_TO_TEXT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
		image(m_smallIcon);
		break;
	}

	set_display_name();
}

void Fle_Listview_Item::draw()
{
	Fl_Box::draw();

	if (m_displayMode == FLE_LISTVIEW_DISPLAY_DETAILS && get_listview()->get_details_helper_lines())
	{
		fl_rect(x(), y(), w(), h(), FL_INACTIVE_COLOR);
	}

	if (!m_focused) return;

	if (m_displayMode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		draw_focus(box(), x(), y() + 32, w(), h() - 32, color());
	}
	else if (m_displayMode == FLE_LISTVIEW_DISPLAY_DETAILS && get_listview()->get_details_helper_lines())
	{
		draw_focus(box(), x() + 2, y() + 2, w() - 4, h() - 4, color());
	}
	else
		draw_focus();
}