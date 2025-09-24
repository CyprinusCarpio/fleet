#ifndef FLE_LISTVIEW_H
#define FLE_LISTVIEW_H

#include <vector>

#include <FL/Fl_Group.H>
#include <FL/Fl_Scrollbar.H>

#include <FLE/Fle_Listview_Item.hpp>

/// \enum Fle_Listview_Flags
/// Listview state flags
enum Fle_Listview_Flags
{
	FLE_LISTVIEW_EDIT_LABELS = 1 << 0, ///< Can item names be edited by the user
	FLE_LISTVIEW_SINGLE_SELECTION = 1 << 1, ///< Can only one item be selected
	FLE_LISTVIEW_SORTED_ASCENDING = 1 << 2, ///< Is the list sorted ascending
	FLE_LISTVIEW_SORTED_DESCENDING = 1 << 3, ///< Is the list sorted descending
	FLE_LISTVIEW_DETAILS_NONE = 1 << 4, ///< Don't show any helper lines in details mod
	FLE_LISTVIEW_DETAILS_LINES = 1 << 5, ///< Show helper lines in details mode
	FLE_LISTVIEW_DETAILS_HIGHLIGHT = 1 << 6, ///< Highlight lines
	FLE_LISTVIEW_REDRAW = 1 << 7, ///< Whether the listview is allowed to redraw
	FLE_LISTVIEW_NEEDS_ARRANGING = 1 << 8, ///< Whether the items needs to be rearranged
	FLE_LISTVIEW_DND = 1 << 9, ///< Whether the listview allows drag and drop
	FLE_LISTVIEW_INDICES_INVALIDATED = 1 << 10, ///< Stored indices may be invalidated during CB
};

/// \enum Fle_Listview_Reason
/// Listview callback reasons
enum Fle_Listview_Reason
{
	FLE_LISTVIEW_REASON_NONE = FL_REASON_UNKNOWN, ///< Unknown reason
	FLE_LISTVIEW_REASON_SELECTED = FL_REASON_SELECTED, ///< Item has been selected
	FLE_LISTVIEW_REASON_DESELECTED = FL_REASON_DESELECTED, ///< Item has been deselected
	FLE_LISTVIEW_REASON_RESELECTED = FL_REASON_RESELECTED, ///< Item has been reselected
	FLE_LISTVIEW_REASON_ADDED = FL_REASON_OPENED, ///< Item has been added
	FLE_LISTVIEW_REASON_REMOVED = FL_REASON_CLOSED, ///< Item has been removed
	FLE_LISTVIEW_REASON_DND_END = FL_REASON_DRAGGED, ///< DND operation has ended
	FLE_LISTVIEW_REASON_DND_START = FL_REASON_USER ///< DND operation has started
};

/** \class Fle_Listview
	\brief 
	Listview class is used to display a list of items. It is somewhat similar
	to Fl_Browser, but it supports more display modes and has more features.

	By default, the listview has 4 display modes:
	- FLE_LISTVIEW_DISPLAY_ICONS: items are displayed as 32x32 icons
	- FLE_LISTVIEW_DISPLAY_SMALL_ICONS: items are displayed vertically as 16x16 icons
	- FLE_LISTVIEW_DISPLAY_DETAILS: various properties of items are displayed
	- FLE_LISTVIEW_DISPLAY_LIST: items are displayed horizontally as 16x16 icons

	\par Features:

	- Items can be added with add_item()
	- Items can be removed with remove_item()
	- Items can be sorted in ascending or descending order with sort_items()
	- Items can be inserted with insert_item()
	- Items can be selected both in single and in multiple selection mode
	- Selected items can be removed with remove_selected()
	- Drag and drop can be enabled/disabled with dnd()

	\par Details mode:
	
	Details mode, also known as report mode in other toolkits, is a display mode
	in which the items are displayed one in a line, with various properties.
	A header is also displayed at the top, which can be used to resize the
	various properties. Individual lines can be highlighted in two ways:
	- FLE_LISTVIEW_DETAILS_HIGHLIGHT: Highlight lines
	- FLE_LISTVIEW_DETAILS_LINES: Show helper lines

	or

	- FLE_LISTVIEW_DETAILS_NONE: Don't show any helper lines.

	\par Callback:

	Callback is run if when() is FL_WHEN_CHANGED. Example callback function:

	\code
	void listview_cb(Fl_Widget* w, void* data) 
	{
		Fle_Listview* lv = (Fle_Listview*)w;
		Fle_Listview_Item* i = lv->get_callback_item();
		Fle_Listview_Reason reason = (Fle_Listview_Reason)Fl::callback_reason();

		switch (reason)
		{
			case FLE_LISTVIEW_REASON_SELECTED:
				std::cout << "Selected " << i->get_name() << std::endl;
				break;
			case FLE_LISTVIEW_REASON_REMOVED:
				std::cout << "Removed " << i->get_name() << std::endl;
				break;
		}
	}
	\endcode

	\par Drag and Drop:

	Drag and drop is optional, and it is enabled by default.
	To disable drag and drop, call dnd(false). DND operations require the use
	of callbacks. DND operations rely on the concept of 'drag zones', which are
	rectangles inside individual item rectangles that typically span just the
	icon and the text, but not the empty space around the item. This is done so
	that both dragNdrop and box selection is possible.

	Typical callback for DND:
	\code
	void listview_cb(Fl_Widget* w, void* data)
	{
		Fle_Listview* lv = (Fle_Listview*)w;
		Fle_Listview_Item* i = lv->get_callback_item();
		Fle_Listview_Reason reason = (Fle_Listview_Reason)Fl::callback_reason();

		switch (reason)
		{
			case FLE_LISTVIEW_REASON_DND_START:
		    std::cout << "dnd start\n";
		    Fl::copy("test", 4);
			break;
		case FLE_LISTVIEW_REASON_DND_END:
			std::cout << "dnd end\n";
		    if(lv->get_item_drag_at(Fl::event_x(), Fl::event_y()) != nullptr)
		        std::cout << "dragged on item\n";

		    Fle_Listview_Item * item = new Test_Listview_Item(Fl::event_text(), 4, "Drag");
		    item->bgcolor(FL_RED);
		    lv->add_item(item);
			break;
		}
	}
	\endcode

	\par Usage:

	\code
	Fle_Listview* lv = new Fle_Listview(10, 10, 400, 300, "Listview");
	lv->callback(listview_cb);
	lv->set_display_mode(FLE_LISTVIEW_DISPLAY_ICONS);
	lv->add_item(new Fle_Listview_Item("Item1"));
	lv->add_item(new Fle_Listview_Item("Item2"));
	lv->add_item(new Fle_Listview_Item("Item3"));
	\endcode
 **/
class Fle_Listview : public Fl_Group
{
	Fle_Listview_Display_Mode m_displayMode;

	int m_state; //< Internal state of the listview
	int m_headersHeight; //< Height of the header section
	int m_focusedItem; //< Index of the focused item
	int m_lastSelectedItem; //< Index of the last selected item
	int m_sortedByProperty; //< Index of the sorted property
	int m_nameHeaderMinWidth; //< Minimum width of the name header
	int m_margin; //< Top, bottom, left and right margin
	int m_itemsBBoxX; //< X coordinate of the items bounding box
	int m_itemsBBoxY; //< Y coordinate of the items bounding box
	int m_columnWidth; //< Width of a single column of items

	Fle_Listview_Item* m_callbackItem; //< Item involved in a callback

	std::vector<Fle_Listview_Item*> m_items; //< Vector of items
	std::vector<int> m_selected; //< Vector of selected indices
	std::vector<std::string> m_propertyDisplayNames; //< Vector of property display names
	std::vector<int> m_propertyOrder; //< Vector of property order
	std::vector<int> m_propertyHeaderWidths; //< Vector of property header widths
	std::vector<int> m_propertyHeaderMinWidths; //< Vector of property header minimum widths

	Fl_Color m_headersColor; //< Color of the header section

	std::string m_nameDisplayText; //< Display text of the name header

	/// Scrollbars callback
	static void scr_callback(Fl_Widget* w, void* data);
	/// Utility function for item callbacks
	void do_callback_for_item(Fle_Listview_Item* item, Fle_Listview_Reason reason);
	/// Internal redraw function
	void listview_redraw();

protected:

	/// Positions all the items
	virtual void arrange_items();
	/// Internal function to handle keyboard input
	virtual void keyboard_select(int key);
	/// Internal function to get grid coordinates at x, y
	virtual void get_grid_xy_at(int x, int y, int& gridX, int& gridY);
	/// Set item focused
	void set_focused(int item);
	/// Set item focus status
	void set_focused(Fle_Listview_Item* item, bool focused);
	/// Handle user selection
	void handle_user_selection(Fle_Listview_Item* item, bool selected, bool scrollTo, bool keepSelected);
	/// Ensure item is visible in the viewport
	void ensure_item_visible(int item);
	/// Selects items in a given rectangle
	void drag_select(int x1, int y1, int x2, int y2);
	/// Updates scrollbars according to the current listview state
	void update_scrollbars();
	/// Recalculates internal item column width
	virtual void recalc_item_column_width();

	/// Virtual method to draw the background
	virtual void draw_background();
	/// Override to draw the listview
	void draw() override;

public:
	Fl_Scrollbar m_hscrollbar; //< Horizontal scrollbar
	Fl_Scrollbar m_vscrollbar; //< Vertical scrollbar

	/// CTOR
	Fle_Listview(int X, int Y, int W, int H, const char* l);
	/// DTOR
	~Fle_Listview();

	/// Adds an item
	/// 
	/// \param item Pointer to the item
	void add_item(Fle_Listview_Item* item);
	/// Inserts an item
	///
	/// \param item Pointer to the item
	void insert_item(Fle_Listview_Item* item, int index);
	/// Removes an item
	/// This does not delete the item.
	/// 
	/// \param item Pointer to the item
	void remove_item(Fle_Listview_Item* item);
	/// Removes an item at given index
	/// This does delete the item.
	/// 
	/// \param index Index of the item
	void remove_item(int index);
	/// Removes all selected items
	/// This does delete all selected items.
	void remove_selected();
	/// Removes all items
	/// This does delete all items.
	void clear_items();

	/// Returns the number of items
	///
	/// \return Number of items
	int get_item_count() const;
	/// Returns the number of selected items
	///
	/// \return Number of selected items
	int get_selected_count() const;

	/// Returns the current display mode
	///
	/// \return Current display mode
	Fle_Listview_Display_Mode get_display_mode() const;
	/// Returns the item at given index
	///
	/// \param index Index of the item
	/// \return Pointer to the item
	Fle_Listview_Item* get_item(int index) const;
	/// Returns the item, if any at given coordinates
	///
	/// \param x X coordinate
	/// \param y Y coordinate
	/// \return Pointer to the item
	Fle_Listview_Item* get_item_at(int x, int y) const;
	/// Returns the item, if any at the drag zone at given coordinates
	///
	/// \param x X coordinate
	/// \param y Y coordinate
	/// \return Pointer to the item
	Fle_Listview_Item* get_item_drag_at(int x, int y) const;
	/// Returns the item, if any, associated with the current callback
	///
	/// \return Pointer to the callback item
	Fle_Listview_Item* get_callback_item() const;

	/// Get vector of selected indices
	///
	/// \return Vector of selected item indices
	const std::vector<int>& get_selected() const;

	/// Sets the display mode
	///
	/// \param mode Display mode
	void set_display_mode(Fle_Listview_Display_Mode mode);
	/// Ensures the item is visible
	///
	/// \param item Pointer to the item
	void ensure_item_visible(Fle_Listview_Item* item);
	/// Deselects all items
	///
	/// \param otherThan Index of the item that should not be deselected
	void deselect_all(int otherThan = -1);
	/// Sets the selection status of an item
	///
	/// \param index Index of the item
	/// \param selected Selection status
	void select_item(int index, bool selected);
	/// Sets the selection status of an item
	///
	/// \param item Pointer to the item
	/// \param selected Selection status
	void select_item(Fle_Listview_Item* item, bool selected);
	/// Set display mode
	///
	/// \param mode Display mode
	void set_details_mode(int mode);
	/// Get mode of details display mode
	///
	/// \return Details display mode
	int get_details_mode() const;
	/// Set header color
	///
	/// \param c Color
	void set_headers_color(Fl_Color c);

	/// Set single selection
	///
	/// \param se Single selection
	void single_selection(bool se);
	/// Get single selection
	///
	/// \return Single selection enabled
	bool single_selection() const;

	/// Set drag and drop
	///
	/// \param dnd Drag and drop enabled
	void dnd(bool dnd);
	/// Get drag and drop
	///
	/// \return Drag and drop enabled
	bool dnd() const { return m_state & FLE_LISTVIEW_DND; }

	/// Add property
	///
	/// \param name Name of the property
	void add_property_name(std::string name);
	/// Set property order
	///
	/// \param order Property order
	void set_property_order(std::vector<int> order);
	/// Get property order
	///
	/// \return Property order
	const std::vector<int>& get_property_order() const;
	/// Set property display widths
	///
	/// \param widths Property widths
	void set_property_widths(std::vector<int> widths);
	/// Set minimal width of name property
	///
	/// \param width Minimal width
	void set_name_min_width(int width);
	/// Get width of specified property
	///
	/// \param property Property index
	/// \return Property width
	int  get_property_header_width(int property) const;
	/// Set margin
	///
	/// \param m Margin
	void set_margin(int m);
	/// Get margin
	///
	/// \return Margin
	int  get_margin() const;
	/// Get individual item column width
	///
	/// \return Item column width
	int  get_item_column_width() const;

	/// Set name display text
	///
	/// \param t Text
	void set_name_text(std::string t);

	/// Sort items in ascending or descending order by given property
	///
	/// \param ascending True for ascending order, false for descending order
	/// \param property Property
	void sort_items(bool ascending, int property);
	/// Get sort direction (ascending or descending or none)
	///
	/// \return Sort direction: -1 = none, 0 = descending, 1 = ascending
	int get_sort_direction() const;
	/// Get sorted by property
	///
	/// \return Sorted by property
	int get_sorted_by_property() const;

	/// Allow/disallow redraw
	///
	/// \param redraw Allow redraw
	void set_redraw(bool redraw);

	/// FLTK handle override
	int handle(int e) override;
	/// FLTK resize override
	void resize(int X, int Y, int W, int H) override;
};

#endif