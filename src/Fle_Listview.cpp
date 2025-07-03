#include <FLE/Fle_Listview.hpp>

#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include <algorithm>
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

void Fle_Listview::scr_callback(Fl_Widget* w, void* data)
{
	Fle_Listview* lv = (Fle_Listview*)w->parent();
	bool horizontal = data == (void*)1;
	
	// Update only if currently scrolled past max
	if (horizontal)
	{
		if (lv->m_hscrollbar.value() > lv->m_itemsBBoxX - lv->w())
			lv->update_scrollbars();
	}
	else
	{
		if(lv->m_vscrollbar.value() > lv->m_itemsBBoxY - lv->h())
			lv->update_scrollbars();
	}


	lv->listview_redraw();
}

Fle_Listview::Fle_Listview(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l),
m_hscrollbar(0, 0, 0, 0, 0), m_vscrollbar(0, 0, 0, 0, 0)
{
	m_state = FLE_LISTVIEW_DETAILS_HIGHLIGHT | FLE_LISTVIEW_REDRAW | FLE_LISTVIEW_DND;
	m_headersHeight = 20;
	m_focusedItem = -1;
	m_lastSelectedItem = -1;
	m_itemsBBoxX = 0;
	m_itemsBBoxY = 0;
	m_nameDisplayText = "Name";
	m_nameHeaderMinWidth = 100;
	m_sortedByProperty = -2;
	m_margin = 5;
	m_headersColor = FL_BACKGROUND_COLOR;
	m_hscrollbar.type(FL_HORIZONTAL);
	m_hscrollbar.when(FL_WHEN_CHANGED);
	m_vscrollbar.when(FL_WHEN_CHANGED);
	m_hscrollbar.callback(scr_callback, (void*)1);
	m_vscrollbar.callback(scr_callback, (void*)0);
	set_property_order({});
	set_property_widths({});
	set_display_mode(FLE_LISTVIEW_DISPLAY_DETAILS);

	end();
	when(FL_WHEN_CHANGED);

	box(FL_DOWN_BOX);
	color(FL_BACKGROUND2_COLOR);
}

Fle_Listview::~Fle_Listview()
{
}

void Fle_Listview::arrange_items()
{
	if (w() == 0 || h() == 0) return;

	Fle_Listview_Display_Mode mode = get_display_mode();

	int X = 0;
	int Y = (mode == FLE_LISTVIEW_DISPLAY_DETAILS ? m_headersHeight : 0);
	int W = 0;
	int H = 0;

	recalc_item_column_width();
	int widest = m_columnWidth;
	int columnSum = 0;

	m_itemsBBoxX = 0;
	m_itemsBBoxY = 0;

	for (int i = 0; i < m_items.size(); i++)
	{
		Fle_Listview_Item* item = m_items[i];

		switch (mode)
		{
		case FLE_LISTVIEW_DISPLAY_ICONS:
			W = 70;
			H = 70;
			if (X + W > w() - Fl::scrollbar_size() - (2 * m_margin))
			{
				X = 0;
				Y += 70;
			}
			break;
		case FLE_LISTVIEW_DISPLAY_SMALL_ICONS:
			H = 20;
			W = widest;
			if (X + W > w() - Fl::scrollbar_size() - (2 * m_margin))
			{
				X = 0;
				Y += 20;
			}
			break;
		case FLE_LISTVIEW_DISPLAY_DETAILS:
			H = 20;
			W = w() - (2 * m_margin);

			// At this point in time the scrollbar may or may not be visible
			// need to check if it WILL be visible
			if (m_items.size() * 20 >= h()) W -= Fl::scrollbar_size();
			break;
		case FLE_LISTVIEW_DISPLAY_LIST:
			W = widest;
			H = 20;
			if (Y + H > h() - Fl::scrollbar_size() - (2 * m_margin))
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

	update_scrollbars();

	listview_redraw();
}

void Fle_Listview::keyboard_select(int key)
{	
	if(m_items.size() <= 1) return;

	if (m_focusedItem == -1) m_focusedItem = 0;

	int itemToFocus = -1;
	int itemWidth = m_items[m_focusedItem]->w();

	switch (get_display_mode())
	{
	case FLE_LISTVIEW_DISPLAY_LIST:
		if (key == FL_Right)
			itemToFocus = m_focusedItem + (int)((h() - (2 * m_margin) - Fl::scrollbar_size()) / 20);
		if (key == FL_Left)
			itemToFocus = m_focusedItem - (int)((h() - (2 * m_margin) - Fl::scrollbar_size()) / 20);
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
			itemToFocus = m_focusedItem + (int)((w() - (2 * m_margin) - Fl::scrollbar_size()) / itemWidth);
		if (key == FL_Up)
			itemToFocus = m_focusedItem - (int)((w() - (2 * m_margin) - Fl::scrollbar_size()) / itemWidth);
		break;
	}

	if (itemToFocus < 0) return;
	if (itemToFocus > m_items.size() - 1) return;

	set_focused(itemToFocus);
}

void Fle_Listview::get_grid_xy_at(int X, int Y, int& gridX, int& gridY)
{
	gridX = 0;
	gridY = 0;
	
	int scrX = m_hscrollbar.value();
	int scrY = m_vscrollbar.value();

	Fle_Listview_Display_Mode mode = get_display_mode();

	if (mode == FLE_LISTVIEW_DISPLAY_DETAILS)
	{
		gridY = (Y - y() - m_margin + scrY) / 20;
	}
	else if (mode == FLE_LISTVIEW_DISPLAY_ICONS)
	{
		gridX = (X - x() - m_margin + scrX) / 70;
		gridY = (Y - y() - m_margin + scrY) / 70;
	}
	else
	{
		gridX = (X - x() - m_margin + scrX) / get_item(0)->w();
		gridY = (Y - y() - m_margin + scrY) / 20;
	}
}

void Fle_Listview::drag_select(int x1, int y1, int x2, int y2)
{
	if(!Fl::event_ctrl())
		m_selected.clear();

	for (int i = 0; i < m_items.size(); i++)
	{
		Fle_Listview_Item* item = get_item(i);

		if(intersect(x1, y1, x2, y2, item->x(), item->y(), item->x() + item->w(), item->y() + item->h()))
		{
			if (item->is_selected())
			{
				m_selected.push_back(i);
				
				// reselect callback
				if (when() & FL_WHEN_CHANGED)
					do_callback_for_item(item, FLE_LISTVIEW_REASON_RESELECTED);
			}
			else
			{
				item->set_selected(true);
				m_selected.push_back(i);

				if (when() & FL_WHEN_CHANGED)
					do_callback_for_item(item, FLE_LISTVIEW_REASON_SELECTED);
			}
		}
		else if (item->is_selected() && !Fl::event_ctrl())
		{
			item->set_selected(false);
			if (when() & FL_WHEN_CHANGED)
				do_callback_for_item(item, FLE_LISTVIEW_REASON_DESELECTED);
		}
	}
}

void Fle_Listview::update_scrollbars()
{
	if (get_display_mode() == FLE_LISTVIEW_DISPLAY_LIST)
	{
		m_vscrollbar.hide();
		if (m_itemsBBoxX + m_hscrollbar.value() > w() || m_hscrollbar.value() != 0)
		{
			m_hscrollbar.show();
			m_hscrollbar.resize(x() + Fl::box_dx(box()), y() + h() - Fl::scrollbar_size() - Fl::box_dy(box()), w() - Fl::box_dw(box()), Fl::scrollbar_size());
			m_hscrollbar.value(m_hscrollbar.value(), w(), 0, m_itemsBBoxX + (2 * m_margin));
		}
		else
			m_hscrollbar.hide();
	}
	else if (get_display_mode() == FLE_LISTVIEW_DISPLAY_DETAILS)
	{
		m_hscrollbar.hide();
		if (m_itemsBBoxY > h() - m_headersHeight + m_vscrollbar.value() || m_vscrollbar.value() != 0)
		{
			m_vscrollbar.show();
			m_vscrollbar.resize(x() + w() - Fl::scrollbar_size() - Fl::box_dx(box()), y() + Fl::box_dy(box()) + m_headersHeight, Fl::scrollbar_size(), h() - Fl::box_dh(box()) - m_headersHeight);
			m_vscrollbar.value(m_vscrollbar.value(), h() - m_headersHeight, 0, m_itemsBBoxY - m_headersHeight + (2 * m_margin));
		}
		else
			m_vscrollbar.hide();
	}
	else
	{
		m_hscrollbar.hide();
		if (m_itemsBBoxY > h() + m_vscrollbar.value() || m_vscrollbar.value() != 0)
		{
			m_vscrollbar.show();
			m_vscrollbar.resize(x() + w() - Fl::scrollbar_size() - Fl::box_dx(box()), y() + Fl::box_dy(box()), Fl::scrollbar_size(), h() - Fl::box_dh(box()));
			m_vscrollbar.value(m_vscrollbar.value(), h(), 0, m_itemsBBoxY + (2 * m_margin));
		}
		else
			m_vscrollbar.hide();
	}
}

void Fle_Listview::recalc_item_column_width()
{
	int W;
	switch (get_display_mode())
	{
	default:
		m_columnWidth = 0;
		break;
	case FLE_LISTVIEW_DISPLAY_DETAILS:
		m_columnWidth = w() - Fl::scrollbar_size() - (2 * m_margin);
		for (int i = 0; i < m_propertyOrder.size(); i++)
		{
			m_columnWidth -= get_property_header_width(m_propertyOrder[i]);
		}

		break;
	case FLE_LISTVIEW_DISPLAY_SMALL_ICONS:
	case FLE_LISTVIEW_DISPLAY_LIST:
		for (int i = 0; i < m_items.size(); i++)
		{
			Fle_Listview_Item* item = m_items[i];

			W = item->get_label_width() + 16;

			if (W > m_columnWidth) m_columnWidth = W;
		}

		if (m_columnWidth > 200) m_columnWidth = 200;
		break;
	}
}

void Fle_Listview::set_focused(Fle_Listview_Item* item, bool focused)
{
	set_focused(std::distance(m_items.begin(), std::find(m_items.begin(), m_items.end(), item)));
}

void Fle_Listview::set_focused(int item)
{
	// item == -1 means clear focus
	if (item == -1) 
	{
		m_focusedItem = -1;
		listview_redraw();
		return;
	}

	if (item == m_focusedItem || item > m_items.size() - 1) return;

	// Unfocus current
	if (m_focusedItem != -1) m_items[m_focusedItem]->set_focus(false);

	// Focus selected
	Fle_Listview_Item* i = m_items[item];
	m_focusedItem = item;
	i->set_focus(true);

	ensure_item_visible(i);
}

void Fle_Listview::handle_user_selection(Fle_Listview_Item* item, bool selected, bool scrollTo, bool keepSelected)
{
	bool ctrl = Fl::event_ctrl();
	bool shift = Fl::event_shift();
	int index = std::distance(m_items.begin(), std::find(m_items.begin(), m_items.end(), item));

	if (!keepSelected && (single_selection() || (!ctrl && !shift)))
	{
		deselect_all(selected ? index : -1);
	}

	if(shift && m_lastSelectedItem == -1) m_lastSelectedItem = 0;

	if (shift && !single_selection())
	{
		for (std::vector<int>::iterator it = m_selected.begin(); it != m_selected.end();)
		{
			if (*it >= std::min(m_lastSelectedItem, index + 1) && *it <= std::max(m_lastSelectedItem, index))
			{
				// Keep this selected
				it++;
				continue;
			}

			Fle_Listview_Item* item = get_item(*it);
			item->set_selected(false);

			if (when() & FL_WHEN_CHANGED)
				do_callback_for_item(item, FLE_LISTVIEW_REASON_DESELECTED);

			it = m_selected.erase(it);
			if(it == m_selected.end()) break;
		}
		// "Bridge" from the last selected to currently selected
		for (int i = std::min(m_lastSelectedItem, index); i <= std::max(m_lastSelectedItem, index); i++)
		{
			select_item(i, true);
		}
	}
	else
	{
		select_item(index, selected);
		m_lastSelectedItem = index;
	}

	if(scrollTo)
		set_focused(index);
	
	listview_redraw();
}

void Fle_Listview::set_details_mode(int mode)
{
	int oldState = m_state;
	
	m_state &= ~FLE_LISTVIEW_DETAILS_NONE;
	m_state &= ~FLE_LISTVIEW_DETAILS_LINES;
	m_state &= ~FLE_LISTVIEW_DETAILS_HIGHLIGHT;

	m_state |= mode;

	if (oldState != m_state) listview_redraw();
}

int Fle_Listview::get_details_mode() const
{
	return m_state & FLE_LISTVIEW_DETAILS_NONE ? 0 : m_state & FLE_LISTVIEW_DETAILS_LINES ? 1 : 2;
}

void Fle_Listview::set_headers_color(Fl_Color c)
{
	m_headersColor = c;
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

	listview_redraw();
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

void Fle_Listview::set_name_min_width(int width)
{
	m_nameHeaderMinWidth = width;
}

int Fle_Listview::get_property_header_width(int property) const
{
	return m_propertyHeaderWidths[property];
}

void Fle_Listview::set_margin(int m)
{
	m_margin = m;

	update_scrollbars();
	listview_redraw();
}

int Fle_Listview::get_margin() const
{
	return m_margin;
}

int Fle_Listview::get_item_column_width() const
{
	return m_columnWidth;
}

void Fle_Listview::set_name_text(std::string t)
{
	m_nameDisplayText = std::move(t);
}

bool Fle_Listview::single_selection() const
{
	return m_state & FLE_LISTVIEW_SINGLE_SELECTION;
}

void Fle_Listview::dnd(bool dnd)
{
	if(dnd)
	{
		m_state |= FLE_LISTVIEW_DND;
	}
	else
		m_state &= ~FLE_LISTVIEW_DND;
}

void Fle_Listview::do_callback_for_item(Fle_Listview_Item* item, Fle_Listview_Reason reason)
{
	m_callbackItem = item;
	do_callback((Fl_Callback_Reason)reason);
}

void Fle_Listview::listview_redraw()
{
	if(m_state & FLE_LISTVIEW_REDRAW) redraw();
}

void Fle_Listview::sort_items(bool ascending, int property)
{
	// Remove focus and selections
	set_focused(-1);

	m_lastSelectedItem = -1;

	deselect_all();

	std::sort(m_items.begin(), m_items.end(), [property, ascending](Fle_Listview_Item* a, Fle_Listview_Item* b) { return ascending ? b->is_greater(a, property) : a->is_greater(b, property); });

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

void Fle_Listview::draw_background()
{
	fl_draw_box(FL_DOWN_BOX, x(), y(), w(), h(), color());
}

void Fle_Listview::draw()
{
	if(m_state & FLE_LISTVIEW_INDICES_INVALIDATED)
	{
		set_focused(-1);
		deselect_all();
		m_lastSelectedItem = -1;
		m_state &= ~FLE_LISTVIEW_INDICES_INVALIDATED;
	}
	Fle_Listview_Display_Mode mode = get_display_mode();

	draw_background();

	if (m_state & FLE_LISTVIEW_NEEDS_ARRANGING)
		arrange_items();

	if (mode == FLE_LISTVIEW_DISPLAY_DETAILS)
	{
		// Draw header
		int prevWidth = 0;
		int bdw = Fl::box_dw(box());

		int scrollbar = 0;
		// At this point in time the scrollbar may or may not be visible
		// need to check if it WILL be visible
		if (m_items.size() * 20 >= y() + h()) scrollbar = Fl::scrollbar_size();

		fl_font(labelfont(), labelsize());

		for (int i = 0; i < m_propertyOrder.size(); i++)
		{
			int prop = m_propertyOrder[i];
			int propWidth = m_propertyHeaderWidths[prop];

			fl_draw_box(FL_UP_BOX, x() + w() - prevWidth - propWidth - scrollbar - bdw, y(), propWidth + (i == 0 ? bdw + scrollbar: 0), m_headersHeight, m_headersColor);
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

		fl_draw_box(FL_UP_BOX, x(), y(), w() - prevWidth - scrollbar - bdw, m_headersHeight, m_headersColor);
		fl_color(labelcolor());
		fl_draw(m_nameDisplayText.c_str(), x() + 4, y(), w() - prevWidth, m_headersHeight, FL_ALIGN_LEFT);

		if (m_sortedByProperty == -1)
		{
			fl_draw(get_sort_direction() == 1 ? "@-38UpArrow" : "@-32DnArrow", x() + w() - prevWidth - scrollbar - bdw - 18, y(), w() - prevWidth, m_headersHeight, FL_ALIGN_LEFT, nullptr, 1);
		}

		fl_push_clip(x(), y() + m_headersHeight, w(), h() - m_headersHeight);
	}
	else
	{
		fl_push_clip(x(), y(), w(), h());
	}

	
	// Draw all items
	fl_font(labelfont(), labelsize());
	for (int i = 0; i < m_items.size(); i++)
		if(intersect(x(), y(), x() + w(), y() + h(), m_items[i]->x(), m_items[i]->y(), m_items[i]->x() + m_items[i]->w(), m_items[i]->y() + m_items[i]->h()))
			m_items[i]->draw_item(i);


	// Draw frame
	if (mode == FLE_LISTVIEW_DISPLAY_DETAILS)
	{
		fl_draw_box(fl_frame(box()), x(), y() + m_headersHeight, w(), h() - m_headersHeight, color());
	}
	else
	{
		fl_draw_box(fl_frame(box()), x(), y(), w(), h(), color());
	}

	// Draw focus rectangle
	if (m_focusedItem != -1)
	{
		Fle_Listview_Item *item = m_items[m_focusedItem];
		Fl_Color c = item->is_selected() ? FL_SELECTION_COLOR : color();
		int textX, textY, textW, textH;
		item->get_text_xywh(textX, textY, textW, textH);

		draw_focus(FL_FLAT_BOX, textX, textY, textW, textH, c);
	}

	fl_pop_clip();

	// Draw scrollbars
	draw_children();
}

void Fle_Listview::add_item(Fle_Listview_Item* item)
{
	m_items.push_back(item);
	item->m_listview = this;
	item->set_display_mode(get_display_mode());

	m_state &= ~FLE_LISTVIEW_SORTED_ASCENDING;
	m_state &= ~FLE_LISTVIEW_SORTED_DESCENDING;
	m_sortedByProperty = -2;

	m_state |= FLE_LISTVIEW_NEEDS_ARRANGING;

	if(when() & FL_WHEN_CHANGED) do_callback_for_item(item, FLE_LISTVIEW_REASON_ADDED);

	m_state |= FLE_LISTVIEW_INDICES_INVALIDATED;

	listview_redraw();
}

void Fle_Listview::insert_item(Fle_Listview_Item* item, int index)
{
	m_items.insert(m_items.begin() + index, item);
	item->m_listview = this;
	item->set_display_mode(get_display_mode());

	m_state &= ~FLE_LISTVIEW_SORTED_ASCENDING;
	m_state &= ~FLE_LISTVIEW_SORTED_DESCENDING;
	m_sortedByProperty = -2;

	m_state |= FLE_LISTVIEW_NEEDS_ARRANGING;

	if (when() & FL_WHEN_CHANGED) do_callback_for_item(item, FLE_LISTVIEW_REASON_ADDED);

	m_state |= FLE_LISTVIEW_INDICES_INVALIDATED;

	listview_redraw();
}

void Fle_Listview::remove_item(Fle_Listview_Item* item)
{
	std::vector<Fle_Listview_Item*>::iterator it = std::find(m_items.begin(), m_items.end(), item);
	if (it == std::prev(m_items.end()) && m_focusedItem == m_items.size() - 1)
	{
		set_focused(-1);
	}
	m_items.erase(it);

	m_state |= FLE_LISTVIEW_NEEDS_ARRANGING;

	if (when() & FL_WHEN_CHANGED) do_callback_for_item(item, FLE_LISTVIEW_REASON_REMOVED);

	m_state |= FLE_LISTVIEW_INDICES_INVALIDATED;

	listview_redraw();
}

void Fle_Listview::remove_item(int index)
{
	if(index > m_items.size() - 1) return;

	Fle_Listview_Item* item = get_item(index);
	remove_item(item);

	delete item;
}

void Fle_Listview::remove_selected()
{
	set_redraw(false);
	std::vector<int> selected = get_selected();
	deselect_all();
	std::sort(selected.begin(), selected.end(), [](int a, int b) { return a > b; });
	set_focused(-1);
	for (int i = 0; i < selected.size(); i++)
	{
		remove_item(selected[i]);
	}
	set_redraw(true);
	listview_redraw();
}

void Fle_Listview::clear_items()
{
	set_redraw(false);
	for(Fle_Listview_Item* item : m_items)
	{
		delete item;
	}
	m_state |= FLE_LISTVIEW_INDICES_INVALIDATED;
	m_items.clear();
	m_selected.clear();
	m_vscrollbar.value(0);
	m_hscrollbar.value(0);
	set_focused(-1);
	set_redraw(true);
	listview_redraw();
}

void Fle_Listview::deselect_all(int otherThan)
{
	if(m_selected.size() == 0) return;
	set_redraw(false);
	bool otherThanSelected = false;
	for(std::vector<int>::iterator it = m_selected.begin(); it != m_selected.end();)
	{
		Fle_Listview_Item* item = get_item(*it);
		if (*it == otherThan) 
		{
			otherThanSelected = item->is_selected();
			it++;
			continue;
		}

		it = m_selected.erase(it);

		item->set_selected(false);
		if (when() & FL_WHEN_CHANGED)
		{
			do_callback_for_item(item, FLE_LISTVIEW_REASON_DESELECTED);
		}
	}
	set_redraw(true);

	listview_redraw();
}

void Fle_Listview::select_item(int index, bool selected)
{
	if(index > m_items.size() - 1) return;
	Fle_Listview_Item* item = get_item(index);

	if (!selected && item->is_selected())
	{
		m_selected.erase(std::find(m_selected.begin(), m_selected.end(), index));
		item->set_selected(false);
		if(when() & FL_WHEN_CHANGED)
		{
			do_callback_for_item(item, FLE_LISTVIEW_REASON_DESELECTED);
		}
		listview_redraw();
		return;
	}

	bool wasSelected = item->is_selected();
	item->set_selected(true);

	if (!wasSelected)
	{
		m_selected.push_back(index);
	}

	if(when() & FL_WHEN_CHANGED)
	{
		if (wasSelected)
		{
			do_callback_for_item(item, FLE_LISTVIEW_REASON_RESELECTED);
		}
		else
		{
			do_callback_for_item(item, FLE_LISTVIEW_REASON_SELECTED);
		}
	}
	listview_redraw();
}

void Fle_Listview::select_item(Fle_Listview_Item* item, bool selected)
{
	select_item(std::distance(m_items.begin(), std::find(m_items.begin(), m_items.end(), item)), selected);
}

int Fle_Listview::get_item_count() const
{
	return m_items.size();
}

int Fle_Listview::get_selected_count() const
{
	return m_selected.size();
}

Fle_Listview_Display_Mode Fle_Listview::get_display_mode() const
{
	return m_displayMode;
}

Fle_Listview_Item* Fle_Listview::get_item(int index) const
{
	return m_items[index];
}

Fle_Listview_Item* Fle_Listview::get_item_at(int X, int Y) const
{
	if (get_display_mode() == FLE_LISTVIEW_DISPLAY_DETAILS && Y < y() + m_headersHeight)
		return nullptr;

	for (Fle_Listview_Item* item : m_items)
	{
		if(X >= item->x() && X < item->x() + item->w() && Y >= item->y() && Y < item->y() + item->h())
		{
			return item;
		}
	}

	return nullptr;
}

Fle_Listview_Item* Fle_Listview::get_item_drag_at(int x, int y) const
{
	Fle_Listview_Item* item = get_item_at(x, y);
	
	if (item && item->is_inside_drag_area(x, y))
		return item;

	return nullptr;
}

Fle_Listview_Item* Fle_Listview::get_callback_item() const
{
	return m_callbackItem;
}

const std::vector<int>& Fle_Listview::get_selected() const
{
	return m_selected;
}

void Fle_Listview::set_display_mode(Fle_Listview_Display_Mode mode)
{
	if (mode == m_displayMode) return;

	m_displayMode = mode;

	m_vscrollbar.value(0);
	m_hscrollbar.value(0);

	for (int i = 0; i < m_items.size(); i++)
	{
		m_items[i]->set_display_mode(mode);
	}

	arrange_items();
}

void Fle_Listview::ensure_item_visible(int item)
{
	ensure_item_visible(get_item(item));
}

void Fle_Listview::ensure_item_visible(Fle_Listview_Item* item)
{
	int scrollX = m_hscrollbar.value();
	int scrollY = m_vscrollbar.value();

	Fle_Listview_Display_Mode mode = get_display_mode();

	if (mode == FLE_LISTVIEW_DISPLAY_LIST)
	{
		if (item->x() < x() || item->w() > w())
		{
			m_hscrollbar.value(scrollX - (x() - item->x() + m_margin));
		}
		else if(item->x() + item->w() > x() + w())
		{
			m_hscrollbar.value(scrollX - (x() + w() - item->x() - item->w() + m_margin));
		}
	}
	else
	{
		if (item->y() < y() + (mode == FLE_LISTVIEW_DISPLAY_DETAILS ? m_headersHeight : 0) || item->h() > h())
		{
			m_vscrollbar.value(scrollY - (y() + (mode == FLE_LISTVIEW_DISPLAY_DETAILS ? m_headersHeight : 0) - item->y()));
		}
		else if (item->y() + item->h() > y() + h())
		{
			m_vscrollbar.value(scrollY - (y() + h() - item->y() - item->h()));
		}
	}

	listview_redraw();
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
	static int lastGridX, lastGridY;
	static bool itemDrag = false;

	int ex = Fl::event_x();
	int ey = Fl::event_y();
	int gridX, gridY;
	Fle_Listview_Item* atItem = get_item_at(ex, ey);
	
	if (dnd())
	{
		if (e == FL_DND_ENTER || e == FL_DND_DRAG || e == FL_DND_RELEASE)
			return 1;

		if(e == FL_PASTE)
		{
			if (when() & FL_WHEN_CHANGED)
			{
				do_callback((Fl_Callback_Reason)FLE_LISTVIEW_REASON_DND_END);
			}
			return 1;
		}
	}
	if (e == FL_PUSH)
	{
		if(Fl::focus() != this)
		{
			Fl::focus(this);
			if (m_items.size() > 0)
			{
				set_focused(0);
			}
		}
		if (atItem)
		{
			if (dnd() && atItem->is_inside_drag_area(ex, ey) && atItem->is_selected() && !Fl::event_ctrl())
			{
				itemDrag = true;
			}
			bool selected = true;
			if (atItem->is_selected() && Fl::event_ctrl()) selected = false;
			handle_user_selection(atItem, selected, true, itemDrag);
		}
		else if (ret != 1)
		{
			// Clear selection
			deselect_all();
		}

		// Resize header bars by dragging
		resizingHeaderProperty = -2;
		if (ex > x() && ex < x() + w() && ey > y() && ey <= y() + m_headersHeight)
		{
			int prevWidth = 0;
			int bdw = Fl::box_dw(box());

			int scrollbar = 0;

			if (m_vscrollbar.visible()) scrollbar = Fl::scrollbar_size();

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

		get_grid_xy_at(ex, ey, gridX, gridY);

		return 1;
	}
	else if (e == FL_MOUSEWHEEL)
	{
		if (get_display_mode() == FLE_LISTVIEW_DISPLAY_LIST && m_hscrollbar.visible())
		{
			int scrollTo = m_hscrollbar.value() + Fl::event_dy() * 10;
			if(scrollTo < 0) scrollTo = 0;
			if(scrollTo > m_itemsBBoxX - w() + (2 * m_margin)) scrollTo = m_itemsBBoxX - w() + (2 * m_margin);

			m_hscrollbar.value(scrollTo);
		}
		else if(m_vscrollbar.visible())
		{
			int scrollTo = m_vscrollbar.value() + Fl::event_dy() * 5;
			if(scrollTo < 0) scrollTo = 0;
			if(scrollTo > m_itemsBBoxY - h() + (2 * m_margin)) scrollTo = m_itemsBBoxY - h() + (2 * m_margin);

			m_vscrollbar.value(scrollTo);
		}
		listview_redraw();
		return 1;
	}
	else if (e == FL_DRAG)
	{
		int scrollTo = 0;
		get_grid_xy_at(ex, ey, gridX, gridY);

		if(get_display_mode() != FLE_LISTVIEW_DISPLAY_DETAILS)
		{
			if (ex > x() + w())
			{
				scrollTo = m_hscrollbar.value() + 3;
				if (scrollTo <= m_itemsBBoxX - w())
				{
					m_hscrollbar.value(scrollTo);
					dragX -= 3;
					update_scrollbars();
					listview_redraw();
				}
			}
			if (ex < x())
			{
				scrollTo = m_hscrollbar.value() - 3;
				if (scrollTo >= 0)
				{
					m_hscrollbar.value(scrollTo);
					dragX += 3;
					update_scrollbars();
					listview_redraw();
				}
			}
		}
		else if(resizingHeaderProperty != -2)
		{
			// Resize property header
			int diff = dragX - ex;

			int newval = m_propertyHeaderWidths[resizingHeaderProperty] + diff;

			int nameHeaderWidth = w();
			nameHeaderWidth -= newval;

			for (int i = 0; i < m_propertyOrder.size(); i++)
			{
				if (m_propertyOrder[i] != resizingHeaderProperty)
				{
					nameHeaderWidth -= m_propertyHeaderWidths[m_propertyOrder[i]];
				}
			}

			if(nameHeaderWidth >= m_nameHeaderMinWidth && newval > m_propertyHeaderMinWidths[resizingHeaderProperty])
			{
				m_propertyHeaderWidths[resizingHeaderProperty] += diff;

				dragX = ex;
				dragY = ey;

				recalc_item_column_width();
				listview_redraw();

				return 1;
			}
		}
		if (ey > y() + h())
		{
			scrollTo = m_vscrollbar.value() + 3;
			if (scrollTo <= m_itemsBBoxY -h())
			{
				m_vscrollbar.value(scrollTo);
				dragY -= 3;
				update_scrollbars();
				listview_redraw();
			}
		}
		if (ey < y())
		{
			scrollTo = m_vscrollbar.value() - 3;
			if (scrollTo >= 0)
			{
				m_vscrollbar.value(scrollTo);
				dragY += 3;
				update_scrollbars();
				listview_redraw();
			}
		}

		if (resizingHeaderProperty == -2 && (gridX != lastGridX || gridY != lastGridY) && !single_selection() && (std::abs(dragX - ex) >= 6 || std::abs(dragY - ey) >= 6))
		{
			if (itemDrag)
			{
				// Start drag of selected items
				if (when() & FL_WHEN_CHANGED)
				{
					do_callback((Fl_Callback_Reason)FLE_LISTVIEW_REASON_DND_START);
					Fl::dnd();
				}
			}
			else
			{
				drag_select(dragX, dragY, ex, ey);
				window()->make_current();
				listview_redraw();
				fl_overlay_rect(dragX, dragY, ex - dragX, ey - dragY);
				lastGridX = gridX;
				lastGridY = gridY;
			}

			return 1;
		}
	}
	else if (e == FL_RELEASE)
	{
		window()->make_current();
		fl_overlay_clear();

		if (itemDrag)
		{
			deselect_all(m_lastSelectedItem);
		}
		itemDrag = false;

		// Check for clicks on headers
		if (resizingHeaderProperty == -2 && get_display_mode() == FLE_LISTVIEW_DISPLAY_DETAILS)
		{
			if (ex > x() && ex < x() + w() && ey > y() && ey <= y() + m_headersHeight)
			{
				int prevWidth = 0;
				int bdw = Fl::box_dw(box());

				int scrollbar = 0;

				if (m_vscrollbar.visible()) scrollbar = Fl::scrollbar_size();

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
		if(m_items.size() > 0)
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
			if(m_focusedItem != -1)
				handle_user_selection(m_items[m_focusedItem], true, true, false);
			break;
		case 'a':
			if (Fl::event_ctrl() && !single_selection())
			{
				for (int i = 0; i < m_items.size(); i++)
				{
					select_item(i, true);
				}
			}
			break;
		}
	}

	return ret;
}

void Fle_Listview::resize(int X, int Y, int W, int H)
{
	Fl_Widget::resize(X, Y, W, H);

	arrange_items();
}
