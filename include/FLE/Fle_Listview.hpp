#ifndef FLE_LISTVIEW_H
#define FLE_LISTVIEW_H

#include <vector>

#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>

#include <FLE/Fle_Listview_Item.hpp>

enum Fle_Listview_Flags
{
	FLE_LISTVIEW_EDIT_LABELS = 1 << 0,
	FLE_LISTVIEW_SINGLE_SELECTION = 1 << 1,
	FLE_LISTVIEW_SORTED_ASCENDING = 1 << 2,
	FLE_LISTVIEW_SORTED_DESCENDING = 1 << 3,
	FLE_LISTVIEW_DETAILS_LINES = 1 << 4,
	FLE_LISTVIEW_REDRAW = 1 << 5
};

class Fle_Listview : public Fl_Group
{
	Fle_Listview_Display_Mode m_displayMode;
	Fl_Scroll* m_scroll;

	int  m_state;
	int  m_headersHeight;
	int  m_focusedItem;
	int  m_lastSelectedItem;

	std::vector<int> m_selected;
	std::vector<std::string> m_propertyDisplayNames;
	std::vector<int> m_propertyOrder;

protected:

	virtual void arrange_items();
	virtual void keyboard_select(int key);
	void set_focused(int item);
	void ensure_item_visible(int item);
	virtual void update_headers();
	void drag_select(int x1, int y1, int x2, int y2);

	void draw() override;

public:
	Fle_Listview(int X, int Y, int W, int H, const char* l);
	~Fle_Listview();

	static std::string m_nameDisplayText;

	void add_item(Fle_Listview_Item* item);
	void remove_item(Fle_Listview_Item* item);
	void clear_items();
	void clear_selection();

	Fle_Listview_Display_Mode get_display_mode() const;
	Fle_Listview_Item* get_item(int index) const;

	const std::vector<int>& get_selected() const;

	void set_display_mode(Fle_Listview_Display_Mode mode);
	void ensure_item_visible(Fle_Listview_Item* item);
	void set_focused(Fle_Listview_Item* item, bool focused);
	void set_selected(Fle_Listview_Item* item, bool selected);
	void set_details_helper_lines(bool lines);
	bool get_details_helper_lines() const;

	void single_selection(bool se);
	bool single_selection() const;

	void add_property_name(std::string name);
	void set_property_order(std::vector<int> order);
	int  get_property_offset(int property) const;

	void sort_items(bool ascending, int property);

	void set_redraw(bool redraw);

	int handle(int e) override;
	void resize(int X, int Y, int W, int H) override;
};

#endif