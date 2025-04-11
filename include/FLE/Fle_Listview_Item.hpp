#ifndef FLE_LISTVIEW_ITEM_H
#define FLE_LISTVIEW_ITEM_H

#include <FL/Fl_Box.H>
#include <FL/Fl_Pixmap.H>

#include <string>

enum Fle_Listview_Display_Mode
{
	FLE_LISTVIEW_DISPLAY_ICONS = 1,
	FLE_LISTVIEW_DISPLAY_SMALL_ICONS,
	FLE_LISTVIEW_DISPLAY_DETAILS,
	FLE_LISTVIEW_DISPLAY_LIST
};

class Fle_Listview;

class Fle_Listview_Item
{
	friend class Fle_Listview;

	Fle_Listview_Display_Mode m_displayMode;

	std::string m_name;
	std::string m_displayName;
	bool m_selected;
	bool m_focused;
	Fl_Color m_textcolor;
	Fl_Color m_bgcolor;
	Fl_Pixmap* m_smallIcon;
	Fl_Pixmap* m_bigIcon;
	Fle_Listview* m_listview;
	int m_x;
	int m_y;
	int m_w;
	int m_h;

	void set_display_name();

protected:
	virtual void set_display_mode(Fle_Listview_Display_Mode mode);
	virtual bool is_greater(Fle_Listview_Item* other, int property);
	virtual void draw_property(int property, int X, int Y, int W, int H);
	virtual bool is_inside_drag_area(int X, int Y);
	virtual void get_text_xywh(int& X, int& Y, int& W, int& H);

	void set_selected(bool selected);
	void set_focus(bool focus);

	virtual void draw_item(int index);


public:
	Fle_Listview_Item(const char* name);

	Fle_Listview* get_listview() const;
	
	bool is_selected() const;
	
	void textcolor(Fl_Color color);
	Fl_Color textcolor() const;
	void bgcolor(Fl_Color color);
	Fl_Color bgcolor() const;
	void set_icon(Fl_Pixmap* small, Fl_Pixmap* big);
	void set_name(std::string newname);
	const std::string& get_name() const;
	void resize(int X, int Y, int W, int H);

	int get_label_width() const;

	int x() const;
	int y() const;
	int w() const { return m_w; }
	int h() const { return m_h; }
};

#endif