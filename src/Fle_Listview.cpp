#include <FLE/Fle_Listview.hpp>

#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include <iostream>

bool intersect(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
{
	if (ax1 > ax2) std::swap(ax1, ax2);
	if (ay1 > ay2) std::swap(ay1, ay2);

	if (bx1 > bx2) std::swap(bx1, bx2);
	if (by1 > by2) std::swap(by1, by2);

	if (ax2 < bx1 || ax1 > bx2 || ay2 < by1 || ay1 > by2)
	{
		return false;
	}

	return true;
}

Fle_Listview::Fle_Listview(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l)
{
	m_displayMode = FLE_LISTVIEW_DISPLAY_LIST;
	m_scroll = new Fl_Scroll(X, Y, W, H);
	m_scroll->box(FL_NO_BOX);
	m_scroll->color(FL_BACKGROUND2_COLOR);
	m_state = FLE_LISTVIEW_DETAILS_LINES | FLE_LISTVIEW_REDRAW;
	m_headersHeight = 20;
	m_focusedItem = -1;
	m_lastSelectedItem = -1;
	m_itemsBBoxX = 0;
	m_itemsBBoxY = 0;
	m_nameDisplayText = "Name";
	m_sortedByProperty = -2;
	set_property_order({});
	set_property_widths({});

	end();

	box(FL_DOWN_BOX);
}

Fle_Listview::~Fle_Listview()
{
}

void Fle_Listview::arrange_items()
{
	if (w() == 0 || h() == 0) return;

	Fle_Listview_Display_Mode mode = get_display_mode();

	int X = x() + Fl::box_dx(box());
	int Y = y() + Fl::box_dy(box()) + (mode == FLE_LISTVIEW_DISPLAY_DETAILS ? m_headersHeight : 0);
	int W = 0;
	int H = 0;

	int widest = 0;
	int columnSum = 0;

	m_itemsBBoxX = 0;
	m_itemsBBoxY = 0;

	if(mode == FLE_LISTVIEW_DISPLAY_SMALL_ICONS || mode == FLE_LISTVIEW_DISPLAY_LIST)
	{
		for (int i = 0; i < m_scroll->children(); i++)
		{
			Fle_Listview_Item* item = (Fle_Listview_Item*)m_scroll->child(i);

			W = item->get_label_width();

			if (W > widest) widest = W;
		}

		if (widest > 200) widest = 200;
	}

	for (int i = 0; i < m_scroll->children() - 2; i++)
	{
		Fle_Listview_Item* item = (Fle_Listview_Item*)m_scroll->child(i);

		switch (mode)
		{
		case FLE_LISTVIEW_DISPLAY_ICONS:
			W = 70;
			H = 70;
			if (X + W > x() + w() - Fl::box_dx(box()) - Fl::box_dw(box()))
			{
				X = x() + Fl::box_dx(box());
				Y += 70;
			}
			break;
		case FLE_LISTVIEW_DISPLAY_SMALL_ICONS:
			H = 20;
			W = widest;
			if (X + W > x() + w() - Fl::box_dx(box()) - Fl::box_dw(box()))
			{
				X = x() + Fl::box_dx(box());
				Y += 20;
			}
			break;
		case FLE_LISTVIEW_DISPLAY_DETAILS:
			H = 20;
			W = w() - Fl::box_dx(box()) - Fl::box_dw(box());

			// At this point in time the scrollbar may or may not be visible
			// need to check if it WILL be visible
			if ((m_scroll->children() - 2) * 20 >= y() + h()) W -= Fl::scrollbar_size();
			break;
		case FLE_LISTVIEW_DISPLAY_LIST:
			W = widest;
			H = 20;
			if (Y + H > y() + h() - Fl::scrollbar_size())
			{
				X += widest;
				Y -= columnSum;
				columnSum = 0;
			}
			columnSum += H;
			break;
		}

		item->resize(X, Y, W, H);

		if (X + W > m_itemsBBoxX) m_itemsBBoxX = X + W;
		if (Y + H > m_itemsBBoxY) m_itemsBBoxY = Y + H;

		switch (mode)
		{
		case FLE_LISTVIEW_DISPLAY_ICONS:
			X += 70;
			break;
		case FLE_LISTVIEW_DISPLAY_SMALL_ICONS:
			X += widest;
			break;
		case FLE_LISTVIEW_DISPLAY_DETAILS:
		case FLE_LISTVIEW_DISPLAY_LIST:
			Y += 20;
			break;
		}
	}

	m_state &= ~FLE_LISTVIEW_NEEDS_ARRANGING;

	if(m_state & FLE_LISTVIEW_REDRAW)
		redraw();
}

void Fle_Listview::keyboard_select(int key)
{	
	if (m_focusedItem == -1) m_focusedItem = 0;

	int itemToFocus = -1;
	int itemWidth = m_scroll->child(m_focusedItem)->w();

	switch (get_display_mode())
	{
	case FLE_LISTVIEW_DISPLAY_LIST:
		if (key == FL_Right)
			itemToFocus = m_focusedItem + (int)((h() - Fl::scrollbar_size() - Fl::box_dy(box())) / 20);
		if (key == FL_Left)
			itemToFocus = m_focusedItem - (int)((h() - Fl::scrollbar_size() - Fl::box_dy(box())) / 20);
	case FLE_LISTVIEW_DISPLAY_DETAILS:
		if (key == FL_Down)
			itemToFocus = m_focusedItem + 1;
		if (key == FL_Up)
			itemToFocus = m_focusedItem - 1;
		break;
	case FLE_LISTVIEW_DISPLAY_ICONS:
	case FLE_LISTVIEW_DISPLAY_SMALL_ICONS:
		if (key == FL_Right)
			itemToFocus = m_focusedItem + 1;
		if (key == FL_Left)
			itemToFocus = m_focusedItem - 1;
		if (key == FL_Down)
			itemToFocus = m_focusedItem + (int)((w() - Fl::box_dx(box()) - Fl::box_dw(box())) / itemWidth);
		if (key == FL_Up)
			itemToFocus = m_focusedItem - (int)((w() - Fl::box_dx(box()) - Fl::box_dw(box())) / itemWidth);
		break;
	}

	if (itemToFocus < 0) return;
	if (itemToFocus > m_scroll->children() - 3) return;

	set_focused(itemToFocus);
}

void Fle_Listview::update_headers()
{

}

void Fle_Listview::drag_select(int x1, int y1, int x2, int y2)
{
	clear_selection();

	for (int i = 0; i < m_scroll->children() - 2; i++)
	{
		Fle_Listview_Item* item = get_item(i);

		if(intersect(x1, y1, x2, y2, item->x(), item->y(), item->x() + item->w(), item->y() + item->h()))
		{
			item->set_selected(true);
			m_selected.push_back(i);
		}
	}
}

void Fle_Listview::set_focused(Fle_Listview_Item* item, bool focused)
{
	set_focused(m_scroll->find(item));
}

void Fle_Listview::set_focused(int item)
{
	if (item == m_focusedItem) return;

	// Unfocus current
	if (m_focusedItem != -1) ((Fle_Listview_Item*)(m_scroll->child(m_focusedItem)))->set_focus(false);

	// item == -1 means clear focus
	if (item == -1) 
	{
		m_focusedItem = -1;
		return;
	}

	// Focus selected
	Fle_Listview_Item* i = (Fle_Listview_Item*)(m_scroll->child(item));
	m_focusedItem = item;
	i->set_focus(true);

	ensure_item_visible(i);
}

void Fle_Listview::set_selected(Fle_Listview_Item* item, bool selected)
{
	bool ctrl = Fl::event_ctrl();
	bool shift = Fl::event_shift();

	if (single_selection() || (!ctrl && !shift))
	{
		for (int i : m_selected)
		{
			get_item(i)->set_selected(false);
		}
		m_selected.clear();

		if (!selected) return;
	}
	int index = m_scroll->find(item);
	item->set_selected(selected);

	if (!selected) return;

	if (shift)
	{
		clear_selection();

		if (m_lastSelectedItem != -1)
		{
			get_item(m_lastSelectedItem)->set_selected(true);
			m_selected.push_back(m_lastSelectedItem);
			// "Bridge" from the last selected to currently selected
			for (int i = std::min(m_lastSelectedItem, index) + 1; i < std::max(m_lastSelectedItem, index); i++)
			{
				get_item(i)->set_selected(true);
				m_selected.push_back(i);
			}
		}
	}
	else
	{
		m_lastSelectedItem = index;
	}

	set_focused(index);
	
	m_selected.push_back(index);
}

void Fle_Listview::set_details_helper_lines(bool lines)
{
	int oldState = m_state;
	if (lines)
	{
		m_state |= FLE_LISTVIEW_DETAILS_LINES;
	}
	else
		m_state &= ~FLE_LISTVIEW_DETAILS_LINES;

	if (oldState != m_state) redraw();
}

bool Fle_Listview::get_details_helper_lines() const
{
	return m_state & FLE_LISTVIEW_DETAILS_LINES;
}

void Fle_Listview::single_selection(bool s)
{
	if (s)
	{
		m_state |= FLE_LISTVIEW_SINGLE_SELECTION;
	}
	else
		m_state &= ~FLE_LISTVIEW_SINGLE_SELECTION;
}

void Fle_Listview::add_property_name(std::string name)
{
	m_propertyDisplayNames.push_back(std::move(name));
}

void Fle_Listview::set_property_order(std::vector<int> order)
{
	m_propertyOrder = std::move(order);

	update_headers();
}

const std::vector<int>& Fle_Listview::get_property_order() const
{
	return m_propertyOrder;
}

void Fle_Listview::set_property_widths(std::vector<int> widths)
{
	m_propertyHeaderMinWidths = widths;
	m_propertyHeaderWidths = std::move(widths);
}

int Fle_Listview::get_property_header_width(int property) const
{
	return m_propertyHeaderWidths[property];
}

void Fle_Listview::set_name_text(std::string t)
{
	m_nameDisplayText = std::move(t);
}

bool Fle_Listview::single_selection() const
{
	return m_state & FLE_LISTVIEW_SINGLE_SELECTION;
}

void Fle_Listview::sort_items(bool ascending, int property)
{
	int n = m_scroll->children() - 2;

	// Remove focus and selections
	set_focused(-1);

	m_lastSelectedItem = -1;

	clear_selection();

	// Bubble sort swapping children
	for (int i = 0; i < n - 1; i++) 
	{
		for (int j = 0; j < n - i - 1; j++) 
		{
			Fle_Listview_Item* itemj = get_item(j);
			Fle_Listview_Item* itemjp = get_item(j + 1);
			if ((ascending && itemj->is_greater(itemjp, property)) || (!ascending && !itemj->is_greater(itemjp, property)))
			{
				m_scroll->insert(*itemjp, j);
			}
		}
	}

	arrange_items();

	m_state |= ascending ? FLE_LISTVIEW_SORTED_ASCENDING : FLE_LISTVIEW_SORTED_DESCENDING;
	m_state &= ascending ? ~FLE_LISTVIEW_SORTED_DESCENDING : ~FLE_LISTVIEW_SORTED_ASCENDING;
	m_sortedByProperty = property;
}

int Fle_Listview::get_sort_direction() const
{
	if (m_state & FLE_LISTVIEW_SORTED_DESCENDING) return 0;
	if (m_state & FLE_LISTVIEW_SORTED_ASCENDING) return 1;

	return -1;
}

int Fle_Listview::get_sorted_by_property() const
{
	return m_sortedByProperty;
}

void Fle_Listview::draw()
{
	Fle_Listview_Display_Mode mode = get_display_mode();

	fl_draw_box(FL_FLAT_BOX, x(), y(), w(), h(), color());

	if (m_state & FLE_LISTVIEW_NEEDS_ARRANGING)
		arrange_items();

	if (mode == FLE_LISTVIEW_DISPLAY_DETAILS)
	{
		fl_draw_box(box(), x(), y() + m_headersHeight, w(), h() - m_headersHeight, color());

		// Draw header
		int prevWidth = 0;
		int bdw = Fl::box_dw(box());

		int scrollbar = 0;
		// At this point in time the scrollbar may or may not be visible
		// need to check if it WILL be visible
		if ((m_scroll->children() - 2) * 20 >= y() + h()) scrollbar = Fl::scrollbar_size();

		fl_font(labelfont(), labelsize());

		for (int i = 0; i < m_propertyOrder.size(); i++)
		{
			int prop = m_propertyOrder[i];
			int propWidth = m_propertyHeaderWidths[prop];

			fl_draw_box(FL_UP_BOX, x() + w() - prevWidth - propWidth - scrollbar - bdw, y(), propWidth + (i == 0 ? bdw + scrollbar: 0), m_headersHeight, color());
			fl_color(labelcolor());
			fl_draw(m_propertyDisplayNames[prop].c_str(), x() + w() - prevWidth - propWidth - scrollbar - bdw + 4, y(), propWidth, m_headersHeight, FL_ALIGN_LEFT);

			if (m_sortedByProperty == prop)
			{
				int X = x() + w() - prevWidth - bdw - 18;
				if (i != 0) X -= scrollbar;

				fl_draw(get_sort_direction() == 1 ? "@-38UpArrow" : "@-32DnArrow", X, y(), w() - prevWidth, m_headersHeight, FL_ALIGN_LEFT, nullptr, 1);
			}

			prevWidth += propWidth;
		}

		fl_draw_box(FL_UP_BOX, x(), y(), w() - prevWidth - scrollbar - bdw, m_headersHeight, color());
		fl_color(labelcolor());
		fl_draw(m_nameDisplayText.c_str(), x() + 4, y(), w() - prevWidth, m_headersHeight, FL_ALIGN_LEFT);

		if (m_sortedByProperty == -1)
		{
			fl_draw(get_sort_direction() == 1 ? "@-38UpArrow" : "@-32DnArrow", x() + w() - prevWidth - scrollbar - bdw - 18, y(), w() - prevWidth, m_headersHeight, FL_ALIGN_LEFT, nullptr, 1);
		}
	}
	else
		fl_draw_box(box(), x(), y(), w(), h(), color());

	draw_child(*m_scroll);

	// TODO: support scollbars on the left and/or top
	if (m_scroll->scrollbar.visible() && m_scroll->hscrollbar.visible())
	{
		fl_draw_box(FL_FLAT_BOX, x() + w() - Fl::scrollbar_size() - Fl::box_dx(box()),
			        y() + h() - Fl::scrollbar_size() - Fl::box_dy(box()), Fl::scrollbar_size(), Fl::scrollbar_size(), FL_BACKGROUND_COLOR);
	}
}

void Fle_Listview::add_item(Fle_Listview_Item* item)
{
	m_scroll->add(item);

	m_state &= ~FLE_LISTVIEW_SORTED_ASCENDING;
	m_state &= ~FLE_LISTVIEW_SORTED_DESCENDING;
	m_sortedByProperty = -2;

	m_state | FLE_LISTVIEW_NEEDS_ARRANGING;
}

void Fle_Listview::remove_item(Fle_Listview_Item* item)
{
	m_scroll->remove(item);

	m_state | FLE_LISTVIEW_NEEDS_ARRANGING;
}

void Fle_Listview::clear_items()
{
	m_scroll->clear();
}

void Fle_Listview::clear_selection()
{
	for (int i : m_selected)
	{
		get_item(i)->set_selected(false);
	}
	m_selected.clear();
}

Fle_Listview_Display_Mode Fle_Listview::get_display_mode() const
{
	return m_displayMode;
}

Fle_Listview_Item* Fle_Listview::get_item(int index) const
{
	return (Fle_Listview_Item*)m_scroll->child(index);
}

const std::vector<int>& Fle_Listview::get_selected() const
{
	return m_selected;
}

void Fle_Listview::set_display_mode(Fle_Listview_Display_Mode mode)
{
	if (mode == m_displayMode) return;

	if (mode == FLE_LISTVIEW_DISPLAY_DETAILS && m_displayMode != FLE_LISTVIEW_DISPLAY_DETAILS)
	{
		m_scroll->resize(x() + Fl::box_dx(box()), y() + m_headersHeight + Fl::box_dy(box()), w() - Fl::box_dw(box()), h() - m_headersHeight - Fl::box_dh(box()));
	}
	else if(mode != FLE_LISTVIEW_DISPLAY_DETAILS && m_displayMode == FLE_LISTVIEW_DISPLAY_DETAILS)
	{
		m_scroll->resize(x() + Fl::box_dx(box()), y() + Fl::box_dy(box()), w() - Fl::box_dw(box()), h() - Fl::box_dh(box()));
	}

	m_displayMode = mode;

	for (int i = 0; i < m_scroll->children(); i++)
	{
		Fle_Listview_Item* item = (Fle_Listview_Item*)m_scroll->child(i);

		item->set_display_mode(mode);
	}

	arrange_items();
}

void Fle_Listview::ensure_item_visible(int item)
{
	ensure_item_visible((Fle_Listview_Item*)m_scroll->child(item));
}

void Fle_Listview::ensure_item_visible(Fle_Listview_Item* item)
{
	// First check if the item is visible now
	
}

void Fle_Listview::set_redraw(bool redraw)
{
	if (redraw)
	{
		m_state |= FLE_LISTVIEW_REDRAW;
	}
	else
		m_state &= ~FLE_LISTVIEW_REDRAW;

}

int Fle_Listview::handle(int e)
{
	if (e == FL_ENTER || e == FL_LEAVE) return 1;

	int ret = Fl_Group::handle(e);

	static int dragX, dragY;
	static int resizingHeaderProperty = -2;

	int ex = Fl::event_x();
	int ey = Fl::event_y();

	if (e == FL_PUSH)
	{
		if(Fl::focus() != this)
		{
			Fl::focus(this);
			if (m_scroll->children() > 2)
			{
				set_focused(0);
			}
		}
		// Clear selection
		if (Fl::belowmouse() == m_scroll)
		{
			clear_selection();
		}

		// Resize header bars by dragging
		resizingHeaderProperty = -2;
		if (ex > x() && ex < x() + w() && ey > y() && ey <= y() + m_headersHeight)
		{
			int prevWidth = 0;
			int bdw = Fl::box_dw(box());

			int scrollbar = 0;

			if (m_scroll->scrollbar.visible()) scrollbar = Fl::scrollbar_size();

			for (int i = 0; i < m_propertyOrder.size(); i++)
			{
				int prop = m_propertyOrder[i];
				int propWidth = m_propertyHeaderWidths[prop];

				if (Fl::event_inside(x() + w() - prevWidth - propWidth - scrollbar - bdw - 4, y(), 6, m_headersHeight))
				{
					resizingHeaderProperty = prop;
					window()->cursor(FL_CURSOR_WE);
					break;
				}

				prevWidth += propWidth;
			}
		}

		// Selection box
		dragX = ex;
		dragY = ey;

		return 1;
	}
	else if (e == FL_DRAG)
	{
		int scrollTo = 0;

		if(get_display_mode() != FLE_LISTVIEW_DISPLAY_DETAILS)
		{
			if (ex > x() + w())
			{
				scrollTo = m_scroll->xposition() + 3;
				if (scrollTo <= m_itemsBBoxX - (x() + w()))
				{
					m_scroll->scroll_to(scrollTo, m_scroll->yposition());
					dragX -= 3;
				}
			}
			if (ex < x())
			{
				scrollTo = m_scroll->xposition() - 3;
				if (scrollTo >= 0)
				{
					m_scroll->scroll_to(scrollTo, m_scroll->yposition());
					dragX += 3;
				}
			}
		}
		else if(resizingHeaderProperty != -2)
		{
			// Resize property header
			int diff = dragX - ex;

			int newval = m_propertyHeaderWidths[resizingHeaderProperty] + diff;

			if(newval > m_propertyHeaderMinWidths[resizingHeaderProperty])
			{
				m_propertyHeaderWidths[resizingHeaderProperty] += diff;

				dragX = ex;
				dragY = ey;

				redraw();

				return 1;
			}
		}
		if (ey > y() + h())
		{
			scrollTo = m_scroll->yposition() + 3;
			if (scrollTo <= m_itemsBBoxY - (y() + h()))
			{
				m_scroll->scroll_to(m_scroll->xposition(), scrollTo);
				dragY -= 3;
			}
		}
		if (ey < y())
		{
			scrollTo = m_scroll->yposition() - 3;
			if (scrollTo >= 0)
			{
				m_scroll->scroll_to(m_scroll->xposition(), scrollTo);
				dragY += 3;
			}
		}
		if (resizingHeaderProperty == -2 && (std::abs(dragX - ex) >= 6 || std::abs(dragY - ey) >= 6))
		{
			drag_select(dragX, dragY, ex, ey);
			window()->make_current();
			fl_overlay_rect(dragX, dragY, ex - dragX, ey - dragY);

			return 1;
		}
	}
	else if (e == FL_RELEASE)
	{
		window()->make_current();
		fl_overlay_clear();

		// Check for clicks on headers
		if (resizingHeaderProperty == -2 && get_display_mode() == FLE_LISTVIEW_DISPLAY_DETAILS)
		{
			if (ex > x() && ex < x() + w() && ey > y() && ey <= y() + m_headersHeight)
			{
				int prevWidth = 0;
				int bdw = Fl::box_dw(box());

				int scrollbar = 0;

				if (m_scroll->scrollbar.visible()) scrollbar = Fl::scrollbar_size();

				for (int i = 0; i < m_propertyOrder.size(); i++)
				{
					int prop = m_propertyOrder[i];
					int propWidth = m_propertyHeaderWidths[prop];

					if (Fl::event_inside(x() + w() - prevWidth - propWidth - scrollbar - bdw, y(), propWidth + (i == 0 ? bdw + scrollbar : 0), m_headersHeight))
					{
						// Clicked on a property
						if (m_sortedByProperty == prop)
						{
							sort_items(m_state & FLE_LISTVIEW_SORTED_DESCENDING, prop);
						}
						else
							sort_items(true, prop);
						return 1;
					}

					prevWidth += propWidth;
				}
				// Clicked on the name, not any other added properties
				if (m_sortedByProperty == -1)
				{
					sort_items(m_state & FLE_LISTVIEW_SORTED_DESCENDING, -1);
				}
				else
					sort_items(true, -1);
				return 1;
			}
		}
		if (resizingHeaderProperty != -2)
		{
			window()->cursor(FL_CURSOR_DEFAULT);
			resizingHeaderProperty = -2;

			return 1;
		}
	}
	else if (e == FL_FOCUS)
	{
		if(m_scroll->children() > 2)
		{
			set_focused(0);
		}
		return 1;
	}
	else if (e == FL_UNFOCUS)
	{
		if (m_focusedItem != -1) set_focused(-1);
		return 1;
	}
	else if (e == FL_KEYDOWN && Fl::focus() == this)
	{
		switch (Fl::event_key())
		{
		case FL_Up:
		case FL_Down:
		case FL_Right:
		case FL_Left:
			keyboard_select(Fl::event_key());
			return 1;
			break;
		case ' ':
			set_selected(get_item(m_focusedItem), true);
			break;
		case 'q':
			set_display_mode(FLE_LISTVIEW_DISPLAY_DETAILS);
			return 1;
			break;
		case 'w':
			set_display_mode(FLE_LISTVIEW_DISPLAY_LIST);
			return 1;
			break;
		case 'e':
			set_display_mode(FLE_LISTVIEW_DISPLAY_SMALL_ICONS);
			return 1;
			break;
		case 'r':
			set_display_mode(FLE_LISTVIEW_DISPLAY_ICONS);
			return 1;
			break;
		case 'a':
			sort_items(true, -1);
			break;
		case 's':
			sort_items(false, -1);
			break;
		}
	}

	return ret;
}

void Fle_Listview::resize(int X, int Y, int W, int H)
{
	int oldW = w();
	int oldH = h();

	Fl_Widget::resize(X, Y, W, H);

	int y = Y;
	int h = H;

	if (get_display_mode() == FLE_LISTVIEW_DISPLAY_DETAILS)
	{
		y += m_headersHeight;
		h -= m_headersHeight;

		for (int i = 0; i < m_scroll->children(); i++)
		{
			Fle_Listview_Item* item = (Fle_Listview_Item*)m_scroll->child(i);

			int W = w() - Fl::box_dx(box()) - Fl::box_dw(box());
			if (m_scroll->scrollbar.visible()) W -= Fl::scrollbar_size();

			item->size(W, 20);
		}
	}

	m_scroll->resize(X + Fl::box_dx(box()), y + Fl::box_dy(box()), W - Fl::box_dw(box()), h - Fl::box_dh(box()));

	if ((oldW == 0 || oldH == 0) || 
		(oldH != H && get_display_mode() == FLE_LISTVIEW_DISPLAY_LIST) ||
		(oldW != W && get_display_mode() == FLE_LISTVIEW_DISPLAY_SMALL_ICONS) ||
		(oldW != W && get_display_mode() == FLE_LISTVIEW_DISPLAY_ICONS))
	{
		arrange_items();
	}
}
