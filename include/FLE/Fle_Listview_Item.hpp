#ifndef FLE_LISTVIEW_ITEM_H
#define FLE_LISTVIEW_ITEM_H

#include <FL/Fl_Box.H>
#include <FL/Fl_Pixmap.H>

#include <string>

/// \enum Fle_Listview_Display_Mode
/// Listview display modes
/// 
enum Fle_Listview_Display_Mode
{
	FLE_LISTVIEW_DISPLAY_ICONS = 1,
	FLE_LISTVIEW_DISPLAY_SMALL_ICONS,
	FLE_LISTVIEW_DISPLAY_DETAILS,
	FLE_LISTVIEW_DISPLAY_LIST
};

class Fle_Listview;

/** \class Fle_Listview_Item
	
	Listview item class. This class is used to represent a listview item
	and manages it's various properties such as name, display name, icons, text
	and background colors. For all but the simplest use cases, a subclass of 
	Fle_Listview_Item should be used. In the subclass, you can add several new
	properties and override the draw_property() method to draw them inside the
	rectangle calculated by the listview. Also, it is required to override the
	is_greater() method to compare items according to the new properties that
	have been added.
**/ 
class Fle_Listview_Item
{
	friend class Fle_Listview;

	Fle_Listview_Display_Mode m_displayMode;

	std::string m_name; ///< Internal name of the item
	std::string m_displayName; ///< Display name
	bool m_selected; ///< Whether the item is selected
	bool m_focused; ///< Whether the item is focused
	Fl_Color m_textcolor; ///< Text color
	Fl_Color m_bgcolor;  ///< Background color
	Fl_Pixmap* m_smallIcon; ///< 16x16 icon
	Fl_Pixmap* m_bigIcon; ///< 32x32 icon
	Fle_Listview* m_listview; ///< Pointer to the listview
	int m_x;
	int m_y;
	int m_w;
	int m_h;

	void set_display_name();

protected:
	/// Set the display mode. You can use a custom one.
	virtual void set_display_mode(Fle_Listview_Display_Mode mode);
	/// Whether this item is greater than the other item. This
	/// needs to be overriden in a subclass.
	virtual bool is_greater(Fle_Listview_Item* other, int property);
	/// Draw a property. Usually this is overridden in a subclass.
	virtual void draw_property(int property, int X, int Y, int W, int H);
	/// Returns true if a click at the given coordinates should
	/// initiate a drag operation.
	virtual bool is_inside_drag_area(int X, int Y);
	/// Get the rectangle containing the item's text.
	virtual void get_text_xywh(int& X, int& Y, int& W, int& H);

	/// Set whether the item is selected.
	void set_selected(bool selected);

	/// Set whether the item is focused.
	void set_focus(bool focus);
	/// Change the drawing location in relation to the listview.
	void resize(int X, int Y, int W, int H);

	/// Draw the item.
	virtual void draw_item(int index);


public:
	/// Constructs a parent-less item and sets it's name and display name.
	/// 
	/// \param name Name of the item
	Fle_Listview_Item(const char* name);
	/// Gets the listview.
	///
	/// \return Pointer to the listview
	Fle_Listview* get_listview() const;
	
	/// Get whether the item is selected.
	///
	/// \return Whether the item is selected
	bool is_selected() const;
	
	/// Set the item text color
	///
	/// \param color Text color
	void textcolor(Fl_Color color);
	/// Get the item text color
	///
	/// \return Text color
	Fl_Color textcolor() const;
	/// Set the item background color
	///
	/// \param color Background color
	void bgcolor(Fl_Color color);
	/// Get the item background color
	///
	/// \return Background color
	Fl_Color bgcolor() const;
	/// Set the item icon, both 16x16 and 32x32.
	/// This does not make a copy of the icons.
	/// 
	/// \param small 16x16 icon
	/// \param big 32x32 icon
	void set_icon(Fl_Pixmap* small, Fl_Pixmap* big);
	/// Set the item name.
	///
	/// \param newname New name
	void set_name(std::string newname);
	/// Get the item name
	///
	/// \return Name
	const std::string& get_name() const;

	int get_label_width() const;

	int x() const;
	int y() const;
	int w() const { return m_w; }
	int h() const { return m_h; }
};

#endif