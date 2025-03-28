#ifndef FLE_LISTVIEW_H
#define FLE_LISTVIEW_H

#include <vector>

#include <FL/Fl_Group.H>
#include <FL/Fl_Scrollbar.H>

#include <FLE/Fle_Listview_Item.hpp>

enum Fle_Listview_Flags
{
	FLE_LISTVIEW_EDIT_LABELS = 1 << 0,
	FLE_LISTVIEW_SINGLE_SELECTION = 1 << 1,
	FLE_LISTVIEW_SORTED_ASCENDING = 1 << 2,
	FLE_LISTVIEW_SORTED_DESCENDING = 1 << 3,
	FLE_LISTVIEW_DETAILS_NONE = 1 << 4,
	FLE_LISTVIEW_DETAILS_LINES = 1 << 5,
	FLE_LISTVIEW_DETAILS_HIGHLIGHT = 1 << 6,
	FLE_LISTVIEW_REDRAW = 1 << 7,
	FLE_LISTVIEW_NEEDS_ARRANGING = 1 << 8,
	FLE_LISTVIEW_DND = 1 << 9
};

enum Fle_Listview_Reason
{
	FLE_LISTVIEW_REASON_NONE = FL_REASON_UNKNOWN,
	FLE_LISTVIEW_REASON_SELECTED = FL_REASON_SELECTED,
	FLE_LISTVIEW_REASON_DESELECTED = FL_REASON_DESELECTED,
	FLE_LISTVIEW_REASON_RESELECTED = FL_REASON_RESELECTED,
	FLE_LISTVIEW_REASON_ADDED = FL_REASON_OPENED,
	FLE_LISTVIEW_REASON_REMOVED = FL_REASON_CLOSED,
	FLE_LISTVIEW_REASON_DND_END = FL_REASON_DRAGGED,
	FLE_LISTVIEW_REASON_DND_START = FL_REASON_USER
};

class Fle_Listview : public Fl_Group
{
	Fle_Listview_Display_Mode m_displayMode;

	int m_state;
	int m_headersHeight;
	int m_focusedItem;
	int m_lastSelectedItem;
	int m_sortedByProperty;
	int m_nameHeaderMinWidth;
	int m_margin;
	int m_itemsBBoxX;
	int m_itemsBBoxY;

	Fle_Listview_Item* m_callbackItem;

	std::vector<Fle_Listview_Item*> m_items;
	std::vector<int> m_selected;
	std::vector<std::string> m_propertyDisplayNames;
	std::vector<int> m_propertyOrder;
	std::vector<int> m_propertyHeaderWidths;
	std::vector<int> m_propertyHeaderMinWidths;

	Fl_Color m_headersColor;

	std::string m_nameDisplayText;

	static void scr_callback(Fl_Widget* w, void* data);
	void do_callback_for_item(Fle_Listview_Item* item, Fle_Listview_Reason reason);
	void listview_redraw();

protected:

	virtual void arrange_items();
	virtual void keyboard_select(int key);
	virtual void get_grid_xy_at(int x, int y, int& gridX, int& gridY);
	void set_focused(int item);
	void set_focused(Fle_Listview_Item* item, bool focused);
	void handle_user_selection(Fle_Listview_Item* item, bool selected, bool scrollTo, bool keepSelected);
	void ensure_item_visible(int item);
	void drag_select(int x1, int y1, int x2, int y2);
	void update_scrollbars();

	void draw() override;

public:
	Fl_Scrollbar m_hscrollbar;
	Fl_Scrollbar m_vscrollbar;

	Fle_Listview(int X, int Y, int W, int H, const char* l);
	~Fle_Listview();

	void add_item(Fle_Listview_Item* item);
	void insert_item(Fle_Listview_Item* item, int index);
	void remove_item(Fle_Listview_Item* item);
	void remove_item(int index);
	void remove_selected();
	void clear_items();

	int get_item_count() const;
	int get_selected_count() const;

	Fle_Listview_Display_Mode get_display_mode() const;
	Fle_Listview_Item* get_item(int index) const;
	Fle_Listview_Item* get_item_at(int x, int y) const;
	Fle_Listview_Item* get_callback_item() const;

	const std::vector<int>& get_selected() const;

	void set_display_mode(Fle_Listview_Display_Mode mode);
	void ensure_item_visible(Fle_Listview_Item* item);
	void deselect_all(int otherThan = -1);
	void select_item(int index, bool selected);
	void select_item(Fle_Listview_Item* item, bool selected);
	void set_details_mode(int mode);
	int get_details_mode() const;
	void set_headers_color(Fl_Color c);

	void single_selection(bool se);
	bool single_selection() const;

	void dnd(bool dnd);
	bool dnd() const { return m_state & FLE_LISTVIEW_DND; }

	void add_property_name(std::string name);
	void set_property_order(std::vector<int> order);
	const std::vector<int>& get_property_order() const;
	void set_property_widths(std::vector<int> widths);
	void set_name_min_width(int width);
	int  get_property_header_width(int property) const;
	void set_margin(int m);
	int  get_margin() const;

	void set_name_text(std::string t);

	void sort_items(bool ascending, int property);
	int get_sort_direction() const;
	int get_sorted_by_property() const;

	void set_redraw(bool redraw);

	int handle(int e) override;
	void resize(int X, int Y, int W, int H) override;
};

#endif