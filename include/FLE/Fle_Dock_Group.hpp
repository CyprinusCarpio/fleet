#ifndef Fle_DOCK_GROUP_H
#define Fle_DOCK_GROUP_H

#include <string>
#include <list>

#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>

#include <FLE/Fle_Flat_Button.hpp>

class Fle_Dock_Host;

/// \enum Fle_Dock_Directions
/// Defines the possible docking directions for a Fle_Dock_Group.
/// These enumerations specify where a dock group can be attached within a Fle_Dock_Host.
enum Fle_Dock_Directions
{
    FLE_DOCK_TOP = 1 << 0, ///< Dock to the top edge.
    FLE_DOCK_RIGHT = 1 << 1, ///< Dock to the right edge.
    FLE_DOCK_BOTTOM = 1 << 2, ///< Dock to the bottom edge.
    FLE_DOCK_LEFT = 1 << 3, ///< Dock to the left edge.
    FLE_DOCK_ALLDIRS = 15      ///< Allow docking to all edges.
};

/// \enum Fle_Dock_State
/// Defines the possible states and behaviors of a Fle_Dock_Group.
/// These flags control the appearance, orientation, and interaction capabilities of a dock group.
enum Fle_Dock_State
{
    FLE_DOCK_VERTICAL = 1 << 0, ///< Group is oriented vertically.
    FLE_DOCK_HORIZONTAL = 1 << 1, ///< Group is oriented horizontally.
    FLE_DOCK_DETACHED = 1 << 2, ///< Group is currently floating in its own window.
    FLE_DOCK_DETACHABLE = 1 << 3, ///< Group can be detached from the host.
    ///< Group can be resized perpendicular to its orientation when docked.
    FLE_DOCK_FLEXIBLE = 1 << 4,
    FLE_DOCK_LOCKED = 1 << 5, ///< Group cannot be moved or detached.
    FLE_DOCK_HIDDEN = 1 << 6, ///< Group is currently hidden.
    ///< Label is not displayed when the group is attached horizontally.
    FLE_DOCK_NO_HOR_LABEL = 1 << 7,
    ///< Group behaves like a toolbar, attachable both horizontally and vertically.
    ///< Cannot share a vertical line with non-toolbar groups. 
    FLE_DOCK_TOOLBAR = 1 << 8
};

/// \class Fle_Detached_Window
/// \brief A specialized, internal Fl_Double_Window for hosting detached Fle_Dock_Group widgets.
///
/// This class primarily addresses compatibility issues on Wayland, 
/// where detached windows need special handling to avoid unwanted resizing behavior
/// when the parent window changes.  It ensures that the detached group maintains its
/// intended size and layout independently of the main application window, acting
/// as a wrapper to manage the detached state correctly.
class Fle_Detached_Window : public Fl_Double_Window
{
private:

    friend class Fle_Dock_Group;
#ifdef FLTK_USE_WAYLAND
    bool m_resizeAllowed; ///< Internal flag to control resizing.
    float m_screenScale;  ///< Stores the screen scale factor for Wayland display handling. 
#endif

    /// Constructor for the detached window.
    /// \param W Initial width of the window.
    /// \param H Initial height of the window.
    /// \param l Optional window title.
    Fle_Detached_Window(int W, int H, const char* l);

    /// Handles FLTK events for the detached window.
    /// Overridden to provide custom event handling, especially for Wayland specific behaviors like scaling and mouse events.
    /// \param e The FLTK event code.
    /// \return 1 if the event was handled, 0 otherwise.
    int handle(int e) override;
    /// Resizes the detached window.
    /// Overridden to manage Wayland-specific resizing constraints and screen scale changes.
    /// \param X The new X coordinate.
    /// \param Y The new Y coordinate.
    /// \param W The new width.
    /// \param H The new height.
    void resize(int X, int Y, int W, int H) override;

    /// Internal resize function allowing controlled resizing, bypassing Wayland restrictions when needed. 
    /// \param X The new X coordinate.
    /// \param Y The new Y coordinate.
    /// \param W The new width.
    /// \param H The new height.
    void fle_resize(int X, int Y, int W, int H);
};

/// \class Fle_Dock_Group
/// \brief Represents a dockable group widget within a Fle_Dock_Host.
///
/// Fle_Dock_Group is a container widget (derived from Fl_Group) that can be attached
/// to the edges of a Fle_Dock_Host or detached into its own floating window
/// (Fle_Detached_Window). It manages its own state (docked/detached, orientation, locked, hidden),
/// appearance (decoration, label, gripper), and interaction (dragging, resizing, pinning).
/// It typically contains a primary content widget ("band widget") and provides
/// standard controls like close and pin buttons. The behavior and appearance can be
/// customized using various state flags (Fle_Dock_State).
class Fle_Dock_Group : public Fl_Group
{
    /// Used for:
    /// - changing m_direction after attaching but before finding lines
    /// - removing Fle_DOCK_HIDDEN state in show_group
    /// - set_size
    /// - set_breadth
    /// - update_preferred_size
    /// - saving/loading group data from a layout buffer
    /// - detach and delete_detached_wnd when loading layout
    /// - update_decoration_btns when loading layout
    /// - set_vertical when attaching a toolbar group
    friend class Fle_Dock_Host;
protected:

    int m_ID;                 ///< Unique identifier for the dock group. 
    int m_state;              ///< Bitmask representing the current state (see Fle_Dock_State). 
    int m_direction;          ///< Current docking direction (see Fle_Dock_Directions) if attached, 0 if detached. 
    int m_allowedDirections;  ///< Bitmask of allowed docking directions. 
    int m_minSize;            ///< Minimum size (width if horizontal, height if vertical). 
    int m_preferredSize;      ///< Preferred size, often used for layout saving/restoring. 
    int m_minBreadth;         ///< Minimum breadth (height if horizontal, width if vertical). 
    int m_decorationSize;     ///< Size (height or width) of the decoration area (grip/title bar). 
    /// Band widget offsets from edges of client area 
    int m_bandOffsetX;        ///< Horizontal offset of the band widget within the client area. 
    int m_bandOffsetY;        ///< Vertical offset of the band widget within the client area. 
    int m_bandOffsetW;        ///< Width adjustment offset for the band widget. 
    int m_bandOffsetH;        ///< Height adjustment offset for the band widget. 
    /// Dragging, scaling, detaching etc 
    int m_offsetX;            ///< Horizontal offset used during drag operations. 
    int m_offsetY;            ///< Vertical offset used during drag operations. 
    bool m_movingGroup;       ///< Flag indicating if the group is currently being moved (dragged). 
    int m_resizing;           ///< Bitmask indicating which edges are being resized (uses Fle_Dock_Directions). 
    bool m_detaching;         ///< Flag indicating if a detach operation is in progress. 
    bool m_previewTimeoutActive; ///< Flag indicating if the docking preview timeout is active. 

    Fle_Flat_Button* m_closeButton;    ///< Button to hide the group. 
    Fle_Flat_Button* m_pinButton;      ///< Button to lock/unlock the group's position in the host.
    Fl_Widget* m_bandWidget;           ///< The main content widget hosted within this group. 
    Fle_Detached_Window* m_detachedWindow; ///< Pointer to the floating window when detached. 

    Fle_Dock_Host* m_host;             ///< Pointer to the parent Fle_Dock_Host. 

    /// Static callback function for decoration buttons (Close, Pin).
    /// \param widget The button that triggered the callback.
    /// \param data User data (0 for Close, 1 for Pin/Unpin).
    static void decoration_cb(Fl_Widget* widget, void* data);
    /// Static callback function for the docking preview timeout.
    /// \param data Pointer to the Fle_Dock_Group instance.
    static void preview_timeout_cb(void* data);
    /// Draws the dock group, including decorations (grip, title, buttons) and the band widget.
    /// Overridden from Fl_Group.
    void draw() override;

    /// Repositions internal elements (decoration buttons, band widget) based on current size and state.
    virtual void position_everything();
    /// Calculates the width required to draw the group's label. 
    /// \return The width of the label text in pixels.
    virtual int get_label_width();
    /// Attempts to attach the group to its host at the given screen coordinates. 
    /// \param X Screen X coordinate.
    /// \param Y Screen Y coordinate.
    void try_attach_to_host(int X, int Y);
    /// Creates the Fle_Detached_Window when the group is detached.
    void create_detached_window();
    /// Destroys the Fle_Detached_Window when the group is re-attached or hidden. 
    void delete_detached_wnd();
    /// Detaches the group from its host and places it in a new Fle_Detached_Window at the specified screen coordinates.
    /// \param X Screen X coordinate for the new window.
    /// \param Y Screen Y coordinate for the new window.
    void detach(int X, int Y);
    /// Updates the preferred size based on the current size. Called during interactions like resizing. 
    void update_preferred_size();
    /// Updates the state and appearance of decoration buttons (e.g., pin icon). 
    void update_decoration_btns();
    /// Sets the orientation (vertical or horizontal), adjusting state and layout accordingly.
    /// \param vertical True for vertical orientation, false for horizontal.
    void set_vertical(bool vertical);
    /// Sets the size (width if horizontal, height if vertical). 
    /// \param newsize The new size value.
    /// \return True if the size was set, false if newsize is below the minimum size.
    bool set_size(int newsize);
    /// Sets the breadth (height if horizontal, width if vertical). 
    /// \param newbreadth The new breadth value.
    /// \return Always returns true.
    bool set_breadth(int newbreadth);

public:
    static std::string m_closeText; ///< Localizable text for the "Close" tooltip/menu item.
    static std::string m_pinText;   ///< Localizable text for the "Pin" tooltip/menu item.
    static std::string m_unpinText; ///< Localizable text for the "Unpin" tooltip/menu item.

    /// Constructor for Fle_Dock_Group.
    /// \param host Pointer to the parent Fle_Dock_Host.
    /// \param id A unique integer identifier for this group.
    /// \param label The text label displayed in the group's decoration.
    /// \param state Initial state flags (see Fle_Dock_State).
    /// \param direction Initial docking direction (see Fle_Dock_Directions).
    /// \param allowedDirections Bitmask of directions where this group is allowed to dock.
    /// \param minSize Minimum size (width for horizontal, height for vertical).
    /// \param breadth Initial breadth (height for horizontal, width for vertical).
    /// \param newLine If true, forces the group to start in a new line within the host. Defaults to false.
    Fle_Dock_Group(Fle_Dock_Host* host, int id, const char* label, int state, int direction, int allowedDirections, int minSize, int breadth, bool newLine = false);
    /// Destructor.
    ~Fle_Dock_Group();

    /// Resizes the dock group widget.
    /// Overridden from Fl_Group to handle internal layout updates. 
    /// \param X New X coordinate.
    /// \param Y New Y coordinate.
    /// \param W New width.
    /// \param H New height.
    void resize(int X, int Y, int W, int H) override;
    /// Handles FLTK events for the dock group.
    /// Overridden from Fl_Group to manage dragging, detaching, resizing, context menus, and interaction with the host.
    /// \param e The FLTK event code.
    /// \return 1 if the event was handled, otherwise delegates to Fl_Group::handle().
    int handle(int e) override;
    /// Adds the primary content widget (band widget) to the group.
    /// Only one band widget can be added. 
    /// \param widget The widget to add as the main content.
    /// \param oX Horizontal offset within the client area (default: determined by state).
    /// \param oY Vertical offset within the client area (default: determined by state).
    /// \param oW Width adjustment offset (subtracted from available width, default: 4).
    /// \param oH Height adjustment offset (subtracted from available height, default: 4).
    /// \return True if the widget was added successfully, false if a band widget already exists. 
    bool add_band_widget(Fl_Widget* widget, int oX = -1, int oY = -1, int oW = -1, int oH = -1);
    /// Checks if the group is flexible (resizable perpendicular to its orientation). 
    /// \return True if the FLE_DOCK_FLEXIBLE flag is set.
    bool flexible() const;
    /// Gets the breadth of the group (height if horizontal, width if vertical). 
    /// \return The breadth in pixels.
    int get_breadth() const;
    /// Gets the minimum allowed size (width if horizontal, height if vertical). 
    /// \return The minimum size in pixels.
    int get_min_size() const;
    /// Gets the preferred size (width if horizontal, height if vertical). 
    /// \return The preferred size in pixels.
    int get_preferred_size() const;
    /// Gets the minimum allowed breadth (height if horizontal, width if vertical). 
    /// \return The minimum breadth in pixels.
    int get_min_breadth() const;
    /// Gets the size of the group (width if horizontal, height if vertical). 
    /// \return The size in pixels.
    int get_size() const;
    /// Gets the current docking direction if attached. 
    /// \return The docking direction (Fle_Dock_Directions enum value) or 0 if detached.
    int get_direction() const;
    /// Gets the allowed docking directions. 
    /// \return A bitmask of Fle_Dock_Directions values.
    int get_allowed_directions() const;
    /// Gets the unique ID of the group. 
    /// \return The integer ID.
    int get_id() const;
    /// Checks if the group is currently hidden. 
    /// \return True if the FLE_DOCK_HIDDEN flag is set.
    bool hidden() const;
    /// Checks if the group is currently detached (floating). 
    /// \return True if the FLE_DOCK_DETACHED flag is set.
    bool detached() const;
    /// Checks if the group can be detached by the user. 
    /// A locked group is never detachable.
    /// \return True if the FLE_DOCK_DETACHABLE flag is set and the group is not locked.
    bool detachable() const;
    /// Checks if the group is locked (cannot be moved or detached). 
    /// \return True if the FLE_DOCK_LOCKED flag is set.
    bool locked() const;
    /// Checks if the group is a toolbar type. 
    /// \return True if the FLE_DOCK_TOOLBAR flag is set.
    bool is_toolbar() const;
    /// Sets the locked state of the group. 
    /// \param l True to lock the group, false to unlock.
    void locked(bool l);
    /// Hides the group.
    /// If attached, it's removed from the host's layout. If detached, its window is hidden.
    /// Notifies the host via Fle_Dock_Host::hide_group().
    void hide_group();
    /// Shows a previously hidden group. 
    /// If it was attached, the host attempts to re-add it. If it was detached, its window is shown.
    /// Notifies the host via Fle_Dock_Host::show_group().
    void show_group();

    /// Gets a pointer to the parent Fle_Dock_Host.
    /// \return Pointer to the Fle_Dock_Host.
    Fle_Dock_Host* get_host() const;
};

#endif