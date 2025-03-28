#ifndef Fle_DOCK_GROUP_H
#define Fle_DOCK_GROUP_H

#include <string>
#include <list>

#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>

#include <FLE/Fle_Flat_Button.hpp>

class Fle_Dock_Host;

enum Fle_Dock_Directions
{
	FLE_DOCK_TOP = 1 << 0,
	FLE_DOCK_RIGHT = 1 << 1,
	FLE_DOCK_BOTTOM = 1 << 2,
	FLE_DOCK_LEFT = 1 << 3,
	FLE_DOCK_ALLDIRS = 15
};

enum Fle_Dock_State
{
	FLE_DOCK_VERTICAL = 1 << 0,
	FLE_DOCK_HORIZONTAL = 1 << 1,
	FLE_DOCK_DETACHED = 1 << 2,
	FLE_DOCK_DETACHABLE = 1 << 3,
	// can the group be resized horizontally if it's left or right of the work widget,
	// or resized vertically if it's top or bottom of the work widget
	FLE_DOCK_FLEXIBLE = 1 << 4,
	FLE_DOCK_LOCKED = 1 << 5,
	FLE_DOCK_HIDDEN = 1 << 6,
	// Don't display the label in a attached horizontal group
	FLE_DOCK_NO_HOR_LABEL = 1 << 7,
	// A toolbar group is a group that can be attached both horizontally and vertically
	// It has the appearance of a horizontal group. It cannot be in the same vertical line
	// as non-toolbar groups.
	FLE_DOCK_TOOLBAR = 1 << 8
};

class Fle_Detached_Window : public Fl_Double_Window
{
	// This class is needed for proper function of
	// detachable groups on Wayland. Detachable
	// groups on Wayland are sub-windows of the top-most
	// window and therefore subject to it's resizing which
	// if not for this wrapper class would mess up
	// the size and layout of detached groups
private:

	friend class Fle_Dock_Group;
#ifdef FLTK_USE_WAYLAND
	bool m_resizeAllowed;
	float m_screenScale;
#endif

	Fle_Detached_Window(int W, int H, const char* l);

	int handle(int e) override;
	void resize(int X, int Y, int W, int H) override;

	void fle_resize(int X, int Y, int W, int H);
};

class Fle_Dock_Group : public Fl_Group
{
	// Used for:
	// - changing m_direction after attaching but before finding lines
	// - removing Fle_DOCK_HIDDEN state in show_group
	// - set_size
	// - set_breadth
	// - update_preferred_size
	// - saving/loading group data from a layout buffer
	// - detach and delete_detached_wnd when loading layout
	// - update_decoration_btns when loading layout
	// - set_vertical when attaching a toolbar group
	friend class Fle_Dock_Host;

protected:

	int m_ID;
	int m_state;
	int m_direction;
	int m_allowedDirections;
	int m_minSize;
	int m_preferredSize;
	int m_minBreadth;
	int m_decorationSize;

	// Band widget offsets from edges of client area
	int m_bandOffsetX;
	int m_bandOffsetY;
	int m_bandOffsetW;
	int m_bandOffsetH;

	// Dragging, scaling, detaching etc
	int m_offsetX;
	int m_offsetY;
	bool m_movingGroup;
	int m_resizing;
	bool m_detaching;
	bool m_previewTimeoutActive;

	Fle_Flat_Button* m_closeButton;
	Fle_Flat_Button* m_pinButton;

	Fl_Widget* m_bandWidget;
	Fle_Detached_Window* m_detachedWindow;

	Fle_Dock_Host* m_host;

	// callback
	static void decoration_cb(Fl_Widget* widget, void* data);
	static void preview_timeout_cb(void* data);

	// FLTK overrides
	void draw() override;

	// protected member functions
	virtual void position_everything();
	virtual int get_label_width();
	void try_attach_to_host(int X, int Y); // Screen coords
	void create_detached_window();
	void delete_detached_wnd();
	void detach(int X, int Y); // Screen coords
	void update_preferred_size();
	void update_decoration_btns();
	void set_vertical(bool vertical);
	bool set_size(int newsize); // ret false if size is smaller than min size
	bool set_breadth(int newbreadth);

public:
	static std::string m_closeText;
	static std::string m_pinText;
	static std::string m_unpinText;

	// Constructor and destructor
	Fle_Dock_Group(Fle_Dock_Host* host, int id, const char* label, int state, int direction, int allowedDirections, int minSize, int breadth, bool newLine = false);
	~Fle_Dock_Group();

	// FLTK overrides
	void resize(int X, int Y, int W, int H) override;
	int handle(int e) override;

	// Fle_Dock_Group member functions
	bool add_band_widget(Fl_Widget* widget, int oX = -1, int oY = -1, int oW = -1, int oH = -1);
	bool flexible() const;
	int get_breadth() const;
	int get_min_size() const;
	int get_preferred_size() const;
	int get_min_breadth() const;
	int get_size() const;
	int get_direction() const;
	int get_allowed_directions() const;
	int get_id() const;
	bool hidden() const;
	bool detached() const;
	bool detachable() const;
	bool locked() const;
	bool is_toolbar() const;
	void locked(bool l);
	void hide_group();
	void show_group();

	Fle_Dock_Host* get_host() const;
};

#endif