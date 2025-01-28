#ifndef Fle_DOCK_HOST_H
#define Fle_DOCK_HOST_H

#include <FL/Fl_Widget.H>
#include <FL/Fl_Group.H>

#include <FLE/Fle_Dock_Group.hpp>

#define Fle_GRIP_SIZE 3

typedef void (Fle_Dock_Host_Min_Size_Callback)(Fle_Dock_Host*, int, int);

class Fle_Edge_Helper
{
	// TODO: maybe there's a better way than using a helper class?
protected:
	friend class Fle_Dock_Host;
	// x, y - starting point
	// y - length, going to right/bottom
	// v - vertical
	int x, y, l, v;
	Fle_Dock_Group* group_a;
	Fle_Dock_Group* group_b;

	// This defines the edge between lines line_it and line_it + 1
	std::list<std::list<Fle_Dock_Group*>>::iterator line_it;
	std::list<std::list<Fle_Dock_Group*>>* direction;

	const bool is_close(int X, int Y)
	{
		if (v && Y > y && Y < y + l && std::abs(X - x) <= Fle_GRIP_SIZE)
		{
			return true;
		}
		else if (X > x && X < x + l && std::abs(Y - y) <= Fle_GRIP_SIZE)
			return true;
		
		return false;
	}


	// Constructors are protected deliberately
	Fle_Edge_Helper(int X, int Y, int L, int V);

	Fle_Edge_Helper(int X, int Y, int L, int V, Fle_Dock_Group* A, Fle_Dock_Group* B);

	Fle_Edge_Helper(int X, int Y, int L, int V, std::list<std::list<Fle_Dock_Group*>>::iterator IT, std::list<std::list<Fle_Dock_Group*>>* D);
};

class Fle_Dock_Host : public Fl_Group
{
	// Used for:
	// - try_attach
	// - detached_drag
	// - hide_group
	// - show_group
	// - reordering m_detachedGroups on Wayland
	friend class Fle_Dock_Group;

	// TODO: a way for hidden groups to remember their attached pos

	Fl_Widget* m_workWidget;
	int m_activeDirections;
	int m_breadthTop;
	int m_breadthRight;
	int m_breadthBottom;
	int m_breadthLeft;
	int m_workWidgetMinW;
	int m_workWidgetMinH;
	int m_oldMinW;
	int m_oldMinH;
	
	// Layout is saved when detaching a non-detachable group
	// If the user fails to attach the non-detachable group,
	// the saved layout is loaded
	const int* m_nonDetachableDetachSavedLayout;

	int m_previewX, m_previewY, m_previewW, m_previewH;

	Fl_Color m_previewColor;

	Fle_Dock_Host_Min_Size_Callback* m_minSizeCallback;

	// All attached dock groups are children on this widget, and all dock groups
	// added to this host, detached or not, are stored in those lists
	// The outer list stores the lines, and inner list stores the pointers to
	// actual dock groups. All dock groups in a line have the same breadth and
	// flexible status.

	// Horizontal lines have priority over vertical lines, like so:
	// ---
	// | |
	// ---
	std::list<std::list<Fle_Dock_Group*>> m_topLines;
	std::list<std::list<Fle_Dock_Group*>> m_rightLines;
	std::list<std::list<Fle_Dock_Group*>> m_bottomLines;
	std::list<std::list<Fle_Dock_Group*>> m_leftLines;

	std::list<Fle_Dock_Group*> m_detachedGroups;
	std::list<Fle_Dock_Group*> m_hiddenGroups;

	// First entries are the edges of the work group, in the following order:
	// top, right, bottom, left
	std::list<Fle_Edge_Helper> m_edges;

	// Fle_Dock_Host member functions
	void position_work_widget();
	void recalculate_direction_breadths();
	void reposition_vertical_groups(int oldBreadthTop = -1, int oldBreadthBottom = -1); // This needs to be done after adding/removing/moving horizontal groups
	void squeeze_groups_in_line(std::list<Fle_Dock_Group*>* line, bool isVerticalGroup, std::list<Fle_Dock_Group*>* alreadyFixed = nullptr);
	void line_change_offset_in_host(std::list<Fle_Dock_Group*>* line, int offset);
	void remove_empty_lines(int direction);
	void find_edges();
	void detached_drag(Fle_Dock_Group* group, int screenX, int screenY);
	void calculate_min_size();
	void line_reset_preferred_sizes(std::list<Fle_Dock_Group*>* line);
	void resize_direction(std::list<std::list<Fle_Dock_Group*>>* lines, int resizeDelta, int resizeDeltaSecondary);
	void resize_host(int oldW, int oldH, int newW, int newH);
	void detach(Fle_Dock_Group* group, bool addToDetached);
	// The following two should only be called by the group itself
	void hide_group(Fle_Dock_Group* group);
	void show_group(Fle_Dock_Group* group);

	int get_flexible_directions_count();
	int get_line_min_breadth(std::list<Fle_Dock_Group*>* line);
	int line_set_breadth(std::list<Fle_Dock_Group*>* line, int newBreadth, bool setToMinSize);
	int get_direction_breadth(int direction);
	int try_attach(Fle_Dock_Group* group, int screenX, int screenY, bool force = false, bool preview = false); // returns direction
	int try_attach_case2n3(int direction, Fle_Dock_Group* group, int X, int Y, bool& needCalcMinSize /* out */, bool preview);

	bool is_host_visible_at(int X, int Y); // checks if the view of host at X, Y is obstructed by other wnds

protected:
	// FLTK overrides
	void draw() override;
	virtual void draw_preview(int X, int Y, int W, int H);

public:
	// Constructor and destructor
	Fle_Dock_Host(int X, int Y, int W, int H, const char* l, int activeDirections);
	~Fle_Dock_Host();

	// FLTK overrides
	void resize(int X, int Y, int W, int H) override;
	int handle(int e) override;

	// Fle_Dock_Host member functions
	bool add_work_widget(Fl_Widget* widget);
	bool is_direction_flexible(int direction);
	bool add_dock_group(Fle_Dock_Group* group, int direction, int newline);
	void detach(Fle_Dock_Group* group);
	void set_work_widget_min_size(int W, int H);

	// The callback gets called when a new min size is calculated for the dock host
	// Dev may use it to set the min size of the main window taking into account
	// other widgets that aren't part of the dock host
	void set_min_size_callback(Fle_Dock_Host_Min_Size_Callback* cb);

	// Set the color of preview displayed when user tries to attach a group
	void set_preview_color(const Fl_Color& color);

	// Layout save and load functions

	// Saves the layout of groups to a buffer. The user is responsible for freeing the buffer afterwards
	int const * const save_layout(int& size);

	// Loads the layout from a buffer.
	void load_layout(int const * const layout);
};

#endif