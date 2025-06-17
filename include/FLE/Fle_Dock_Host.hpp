#ifndef Fle_DOCK_HOST_H
#define Fle_DOCK_HOST_H

#include <FL/Fl_Widget.H>
#include <FL/Fl_Group.H>

#include <FLE/Fle_Dock_Group.hpp>

#define Fle_GRIP_SIZE 3 ///< Defines the sensitive pixel width/height for edge detection (resizing grips). 

/// Callback function type for notifying when the host's minimum size changes.
/// \param host Pointer to the Fle_Dock_Host whose minimum size changed.
/// \param minW The new calculated minimum width.
/// \param minH The new calculated minimum height.
typedef void (Fle_Dock_Host_Min_Size_Callback)(Fle_Dock_Host*, int, int);

/// \class Fle_Edge_Helper
/// \brief Internal helper class to represent interactive edges for resizing.
///
/// This class stores information about a draggable edge within the Fle_Dock_Host.
/// Edges can exist between the work area and docked groups, between two adjacent
/// groups in the same line, or between two adjacent lines of groups. It simplifies
/// event handling for resizing operations by encapsulating the edge's position,
/// orientation, and the associated groups or lines.
class Fle_Edge_Helper
{
	/// TODO: maybe there's a better way than using a helper class? 
protected:
	friend class Fle_Dock_Host;
	/// x, y - starting point 
	/// l - length, going to right/bottom  (Note: Corrected comment from 'y - length')
	/// v - vertical 
	int x, y, l;
	bool v; ///< True if the edge is vertical, false if horizontal.
	Fle_Dock_Group* group_a; ///< Pointer to the first group involved in a group-group edge resize.  Null otherwise.
	Fle_Dock_Group* group_b; ///< Pointer to the second group involved in a group-group edge resize.  Null otherwise.

	/// This defines the edge between lines line_it and line_it + 1 
	std::list<std::list<Fle_Dock_Group*>>::iterator line_it; ///< Iterator pointing to the first line in a line-line edge resize. 
	std::list<std::list<Fle_Dock_Group*>>* direction; ///< Pointer to the list of lines for a line-line edge resize. 

	/// Checks if the given coordinates are close to this edge.
	/// \param X The X coordinate to check.
	/// \param Y The Y coordinate to check.
	/// \return True if the coordinates are within Fle_GRIP_SIZE pixels of the edge.
	const bool is_close(int X, int Y)
	{
		if (v && Y >= y && Y < y + l && std::abs(X - x) <= Fle_GRIP_SIZE) // Corrected Y range check
		{
			return true;
		}
		else if (!v && X >= x && X < x + l && std::abs(Y - y) <= Fle_GRIP_SIZE) // Corrected Y range check and orientation check
			return true;

		return false;
	}


	/// Constructors are protected deliberately 
	/// Constructor for work area edges.
	/// \param X Starting X coordinate.
	/// \param Y Starting Y coordinate.
	/// \param L Length of the edge.
	/// \param V True if vertical, false if horizontal.
	Fle_Edge_Helper(int X, int Y, int L, bool V);
	/// Constructor for edges between two groups.
	/// \param X Starting X coordinate.
	/// \param Y Starting Y coordinate.
	/// \param L Length/Breadth associated with the edge (often group breadth).
	/// \param V True if vertical, false if horizontal.
	/// \param A Pointer to the first group.
	/// \param B Pointer to the second group.
	Fle_Edge_Helper(int X, int Y, int L, bool V, Fle_Dock_Group* A, Fle_Dock_Group* B);
	/// Constructor for edges between two lines.
	/// \param X Starting X coordinate.
	/// \param Y Starting Y coordinate.
	/// \param L Length of the edge.
	/// \param V True if vertical, false if horizontal.
	/// \param IT Iterator to the first line.
	/// \param D Pointer to the list containing the lines.
	Fle_Edge_Helper(int X, int Y, int L, bool V, std::list<std::list<Fle_Dock_Group*>>::iterator IT, std::list<std::list<Fle_Dock_Group*>>* D);
};

/** \class Fle_Dock_Host
	\brief Manages a central work area and surrounding dockable areas.
	
	Fle_Dock_Host is the main container for Fle_Dock_Group widgets. It provides
	docking areas (top, bottom, left, right) where groups can be attached.
	Groups can be arranged in multiple "lines" in each direction. The host
	manages the layout, resizing, docking, undocking, and hiding/showing of these groups.
	It typically contains a central, non-dockable "work widget" that occupies the
	remaining space. The host handles mouse events for resizing edges between groups,
	lines, and the work area, and facilitates drag-and-drop docking/undocking.

	\par API Features:

	- Adding dock groups with add_dock_group()
	- Adding work widget with add_work_widget()
	- Detaching groups with detach()
	- Setting min size for the work widget with set_work_widget_min_size()
	- Setting min size callback with set_min_size_callback()
	- Saving layout with save_layout()
	- Loading layout with load_layout()
	- Setting preview rectangle color with set_preview_color()
	- Checking whether a direction is flexible with is_direction_flexible()

	\par Min size callback:

	The min size callback is called when the minimum size of the dock host,
	calculated with regards to all the docked groups, changes. This can be used
	to update the minimum size of the application window or other widgets.

	Example code:
	\code
	void min_size_cb(Fle_Dock_Host* host, int W, int H)
	{
	    host->window()->size_range(W, H);
	}
	\endcode

	\par Usage:

	Example dock host with several groups:
	\code
	Fle_Dock_Host* dh = new Fle_Dock_Host(0, 0, 600, 600, "Dock Host", FLE_DOCK_ALLDIRS);

	vertGroup1 = new Fle_Dock_Group(dh, ID_FOLDERS, "Folders", FLE_DOCK_DETACHABLE | FLE_DOCK_FLEXIBLE, FLE_DOCK_LEFT, FLE_DOCK_RIGHT, 120, 180, false);
	Fl_Tree* tree = make_tree();
	vertGroup1->add_band_widget(tree);

	Fle_Dock_Group* horGroup = new Fle_Dock_Group(dh, ID_ADDRESS, "Address: ", FLE_DOCK_DETACHABLE | FLE_DOCK_FLEXIBLE, FLE_DOCK_TOP, FLE_DOCK_BOTTOM, 180, 26, true);
	Fl_Group* addressGroup = make_address_group();
	horGroup->add_band_widget(addressGroup, 0, 1, 1, 1);

	listview = make_listview();

	dh->add_work_widget(listview);
	\endcode
 **/
class Fle_Dock_Host : public Fl_Group
{
	/// Used for:
	/// - try_attach
	/// - detached_drag
	/// - hide_group
	/// - show_group
	/// - reordering m_detachedGroups on Wayland
	friend class Fle_Dock_Group;
	// TODO: a way for hidden groups to remember their attached pos 

	Fl_Widget* m_workWidget;     ///< The central, non-dockable widget. 
	int m_activeDirections;      ///< Bitmask of directions where docking is currently allowed (see Fle_Dock_Directions). 
	int m_breadthTop;            ///< Total height of all docked groups in the top area. 
	int m_breadthRight;          ///< Total width of all docked groups in the right area. 
	int m_breadthBottom;         ///< Total height of all docked groups in the bottom area. 
	int m_breadthLeft;           ///< Total width of all docked groups in the left area. 
	int m_workWidgetMinW;        ///< Minimum width constraint for the work widget. 
	int m_workWidgetMinH;        ///< Minimum height constraint for the work widget. 
	int m_oldMinW;               ///< Previously calculated minimum width for the host. 
	int m_oldMinH;               ///< Previously calculated minimum height for the host. 
	float m_previewTimeout;      ///< Time in seconds before the docking preview rectangle is shown.
	/// Layout is saved when detaching a non-detachable group 
	/// If the user fails to attach the non-detachable group,
	/// the saved layout is loaded 
	const int* m_nonDetachableDetachSavedLayout; ///< Buffer storing layout before detaching a non-detachable group. 
	int m_previewX, m_previewY, m_previewW, m_previewH; ///< Coordinates and dimensions for the docking preview rectangle. 

	Fl_Color m_previewColor;     ///< Color used to draw the docking preview rectangle. 

	Fle_Dock_Host_Min_Size_Callback* m_minSizeCallback; ///< Pointer to the minimum size change callback function. 

	/// All attached dock groups are children on this widget, and all dock groups
	/// added to this host, detached or not, are stored in those lists
	/// The outer list stores the lines, and inner list stores the pointers to
	/// actual dock groups.  All dock groups in a line have the same breadth and
	/// flexible status. 
	/// Horizontal lines have priority over vertical lines, like so: 
	/// ---
	/// | |
	/// ---
	std::list<std::list<Fle_Dock_Group*>> m_topLines;     ///< Lines of groups docked at the top. 
	std::list<std::list<Fle_Dock_Group*>> m_rightLines;   ///< Lines of groups docked on the right. 
	std::list<std::list<Fle_Dock_Group*>> m_bottomLines;  ///< Lines of groups docked at the bottom. 
	std::list<std::list<Fle_Dock_Group*>> m_leftLines;    ///< Lines of groups docked on the left. 

	std::list<Fle_Dock_Group*> m_detachedGroups; ///< List of currently detached (floating) groups. 
	std::list<Fle_Dock_Group*> m_hiddenGroups;   ///< List of currently hidden groups. 

	/// First entries are the edges of the work group, in the following order:
	/// top, right, bottom, left 
	std::list<Fle_Edge_Helper> m_edges; ///< List of all interactive resize edges within the host. 

	/// Positions and resizes the central work widget based on surrounding docked group breadths. 
	void position_work_widget();
	/// Recalculates the total breadth occupied by docked groups in each direction. 
	void recalculate_direction_breadths();
	/// Repositions groups docked on the left and right sides, adjusting for changes in top/bottom breadths. 
	/// \param oldBreadthTop Optional previous top breadth for delta calculation.
	/// \param oldBreadthBottom Optional previous bottom breadth for delta calculation.
	void reposition_vertical_groups(int oldBreadthTop = -1, int oldBreadthBottom = -1);
	/// Adjusts the sizes of groups within a line to fit available space, respecting minimum sizes. May move groups to new lines if necessary. 
	/// \param line Pointer to the list of groups in the line.
	/// \param isVerticalGroup True if the line contains vertically oriented groups (left/right docking).
	/// \param alreadyFixed Optional list to track groups moved during squeezing to prevent double processing.
	void squeeze_groups_in_line(std::list<Fle_Dock_Group*>* line, bool isVerticalGroup, std::list<Fle_Dock_Group*>* alreadyFixed = nullptr);
	/// Shifts the position of all groups within a line horizontally or vertically. 
	/// \param line Pointer to the list of groups in the line.
	/// \param offset The amount to shift (positive or negative).
	void line_change_offset_in_host(std::list<Fle_Dock_Group*>* line, int offset);
	/// Removes lines that become empty after detaching/hiding groups and adjusts layout. 
	/// \param direction The docking direction (Fle_Dock_Directions) to check for empty lines.
	void remove_empty_lines(int direction);
	/// Recalculates and rebuilds the list of all interactive resize edges (m_edges). 
	void find_edges();
	/// Handles the dragging of a detached group over the host, displaying a docking preview. 
	/// \param group The group being dragged.
	/// \param screenX Current screen X coordinate of the cursor.
	/// \param screenY Current screen Y coordinate of the cursor.
	void detached_drag(Fle_Dock_Group* group, int screenX, int screenY);
	/// Calculates the minimum required size for the host based on docked groups and work widget minimums. 
	void calculate_min_size();
	/// Resets the preferred size for each group in a line, typically to their current size after interaction. 
	/// \param line Pointer to the list of groups in the line.
	void line_reset_preferred_sizes(std::list<Fle_Dock_Group*>* line);
	/// Resizes groups within a specific direction's lines in response to host resize. 
	/// \param lines Pointer to the list of lines for the direction.
	/// \param resizeDelta Change in size along the line's primary axis.
	/// \param resizeDeltaSecondary Change in size perpendicular to the line's primary axis (affects position).
	void resize_direction(std::list<std::list<Fle_Dock_Group*>>* lines, int resizeDelta, int resizeDeltaSecondary);
	/// Handles the overall resizing of the host and adjusts the layout of all docked groups. 
	/// \param oldW Previous host width.
	/// \param oldH Previous host height.
	/// \param newW New host width.
	/// \param newH New host height.
	void resize_host(int oldW, int oldH, int newW, int newH);
	/// Detaches a group from the host layout. Called internally by Fle_Dock_Group or during layout changes. 
	/// \param group The group to detach.
	/// \param addToDetached If true, adds the group to the m_detachedGroups list; if false, assumes it's being hidden.
	void detach(Fle_Dock_Group* group, bool addToDetached);
	// The following two should only be called by the group itself 
	
	/// Hides a group, removing it from the layout and adding it to the hidden list. Called by Fle_Dock_Group::hide_group(). 
	/// \param group The group to hide.
	void hide_group(Fle_Dock_Group* group);
	/// Shows a hidden group, attempting to re-add it to the layout. Called by Fle_Dock_Group::show_group(). 
	/// \param group The group to show.
	void show_group(Fle_Dock_Group* group);

	/// Counts how many docking directions currently contain at least one flexible line. 
	/// \return The count of flexible directions.
	int get_flexible_directions_count();
	/// Calculates the minimum required breadth for a specific line based on its groups. 
	/// \param line Pointer to the list of groups in the line.
	/// \return The maximum minimum breadth among all groups in the line.
	int get_line_min_breadth(std::list<Fle_Dock_Group*>* line);
	/// Sets the breadth for all groups within a line. 
	/// \param line Pointer to the list of groups in the line.
	/// \param newBreadth The desired new breadth.
	/// \param setToMinSize If true and newBreadth is less than the minimum, sets breadth to the minimum.
	/// \return The difference between the old breadth and the new breadth (old - new), or 0 if no change occurred due to minimum constraints.
	int line_set_breadth(std::list<Fle_Dock_Group*>* line, int newBreadth, bool setToMinSize);
	/// Calculates the total breadth occupied by all lines in a given direction. 
	/// \param direction The docking direction (Fle_Dock_Directions).
	/// \return The total breadth in pixels.
	int get_direction_breadth(int direction);
	/// Attempts to attach a group at the given screen coordinates, either finding a suitable line/position or creating a new line. 
	/// \param group The group to attach.
	/// \param screenX Screen X coordinate for attachment attempt.
	/// \param screenY Screen Y coordinate for attachment attempt.
	/// \param force If true, forces attachment even if normal logic fails (used for non-detachable groups). Defaults to false.
	/// \param preview If true, only calculates and displays the preview rectangle without actually attaching. Defaults to false.
	/// \return The direction (Fle_Dock_Directions) where the group was attached, or 0 if attachment failed. 
	int try_attach(Fle_Dock_Group* group, int screenX, int screenY, bool force = false, bool preview = false);
	/// Internal helper for try_attach, handling attachment cases involving existing lines (adding between lines or adding to an existing line). 
	/// \param direction The target docking direction.
	/// \param group The group to attach.
	/// \param X Host-relative X coordinate.
	/// \param Y Host-relative Y coordinate.
	/// \param needCalcMinSize Output parameter; set to true if minimum host size might need recalculation.
	/// \param preview If true, only calculates and displays the preview rectangle.
	/// \return The direction (Fle_Dock_Directions) where the group was attached, or 0 if attachment failed.
	int try_attach_case2n3(int direction, Fle_Dock_Group* group, int X, int Y, bool& needCalcMinSize /* out */, bool preview);
	/// Checks if the host area at the given screen coordinates is obstructed by other top-level windows. 
	/// \param X Screen X coordinate.
	/// \param Y Screen Y coordinate.
	/// \return True if the host is visible at the coordinates, false otherwise.
	bool is_host_visible_at(int X, int Y);

protected:
	/// Draws the host, including the docking preview if active.
	/// Overridden from Fl_Group. 
	void draw() override;
	/// Draws the docking preview rectangle. Can be overridden for custom preview appearance. 
	/// \param X Preview X coordinate.
	/// \param Y Preview Y coordinate.
	/// \param W Preview width.
	/// \param H Preview height.
	virtual void draw_preview(int X, int Y, int W, int H);
public:
	/// Constructor for Fle_Dock_Host.
	/// \param X X coordinate of the host widget.
	/// \param Y Y coordinate of the host widget.
	/// \param W Width of the host widget.
	/// \param H Height of the host widget.
	/// \param l Optional label (typically null).
	/// \param activeDirections Initial bitmask of directions where docking is allowed.
	Fle_Dock_Host(int X, int Y, int W, int H, const char* l, int activeDirections);
	/// Destructor.
	~Fle_Dock_Host();
	/// Resizes the host widget.
	/// Overridden from Fl_Group to trigger internal layout recalculation via resize_host(). 
	/// \param X New X coordinate.
	/// \param Y New Y coordinate.
	/// \param W New width.
	/// \param H New height.
	void resize(int X, int Y, int W, int H) override;
	/// Handles FLTK events for the host.
	/// Overridden from Fl_Group to manage resizing interactions via edges.
	/// \param e The FLTK event code.
	/// \return 1 if the event was handled, otherwise delegates to Fl_Group::handle().
	int handle(int e) override;
	/// Adds the central work widget to the host. 
	/// Only one work widget can be added.
	/// \param widget The widget to set as the central work area.
	/// \return True if added successfully, false if a work widget already exists.
	bool add_work_widget(Fl_Widget* widget);
	/// Checks if a given docking direction contains at least one flexible line. 
	/// \param direction The direction to check (Fle_Dock_Directions).
	/// \return True if the direction has flexible lines, false otherwise.
	bool is_direction_flexible(int direction);
	/// Adds a Fle_Dock_Group to the host.
	/// This is the primary method for introducing new dockable groups.
	/// \param group The group to add.
	/// \param direction The initial docking direction (or 0 to add as initially detached).
	/// \param newline Controls line placement: 0=try existing line, 1=new line at end, 2+=insert new line at specific index.
	/// \return True if the group was added successfully to the specified direction, false otherwise.
	bool add_dock_group(Fle_Dock_Group* group, int direction, int newline);
	/// Public interface to detach a group, triggering its transition to a floating window.
	/// \param group The group to detach.
	void detach(Fle_Dock_Group* group);
	/// Sets the minimum width and height constraints for the central work widget.
	/// \param W Minimum width.
	/// \param H Minimum height.
	void set_work_widget_min_size(int W, int H);

	/// The callback gets called when a new min size is calculated for the dock host 
	/// Dev may use it to set the min size of the main window taking into account
	/// other widgets that aren't part of the dock host 
	/// Sets a callback function to be invoked when the host's minimum size changes.
	/// \param cb Pointer to the callback function.
	void set_min_size_callback(Fle_Dock_Host_Min_Size_Callback* cb);

	/// Set the color of preview displayed when user tries to attach a group 
	/// Sets the color used for drawing the docking preview rectangle.
	/// \param color The Fl_Color value.
	void set_preview_color(const Fl_Color& color);
	/// Sets the timeout (in seconds) for the preview rectangle.
	/// \param timeout The timeout value in seconds.
	void set_preview_timeout(float timeout) { m_previewTimeout = timeout; }
	/// Gets the preview timeout.
	/// \return The timeout value in seconds.
	float get_preview_timeout() const { return m_previewTimeout; }

	/// Layout save and load functions 

	/// Saves the layout of groups to a buffer.  The user is responsible for freeing the buffer afterwards 
	/// Saves the current layout (positions, sizes, states of all groups) into an integer buffer.
	/// \param size Output parameter; will be set to the size of the allocated buffer in integers.
	/// \return A pointer to the newly allocated integer buffer containing the layout data. The caller must delete[] this buffer.
	int const* const save_layout(int& size);
	/// Loads the layout from a buffer. 
	/// Restores a previously saved layout from an integer buffer.
	/// \param layout Pointer to the buffer containing layout data created by save_layout().
	void load_layout(int const* const layout);
};

#endif