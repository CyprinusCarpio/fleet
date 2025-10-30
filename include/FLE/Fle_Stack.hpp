#ifndef FLE_STACK_H
#define FLE_STACK_H

#include <FL/Fl_Group.H>

#include <vector>
#include <numeric>

enum Fle_Stack_Orientation
{
    FLE_STACK_HORIZONTAL = 0, 
    FLE_STACK_VERTICAL = 1
};

/// \class Fle_Stack
/// \brief A container widget that stacks children vertically or horizontally.
///
/// A stack widget is a container that arranges its children in a vertical or horizontal stack.
/// It is typically used to group related widgets together. It's very similar to Fl_Flex, the
/// main difference is the ability to set minimum, preferred and maximum sizes for each child.
/// The preferred size is effectively the maximum size during child widget positioning, except 
/// for cases where the sum of  preferred sizes of all widgets is less than the available space. 
/// User dragging on the edges updates the preferred sizes.
class Fle_Stack : public Fl_Group 
{
    
    Fle_Stack_Orientation m_orientation; //< Orientation of the stack
    std::vector<int> m_widgetMinSizes; //< Minimum sizes of the widgets
    std::vector<int> m_widgetPreferredSizes; //< Preferred sizes of the widgets
    std::vector<int> m_widgetMaxSizes; //< Maximum sizes of the widgets

protected:
    /// Overridden to remove the extra data associated with the widget
    /// and also to reposition other widgets so as to maintain tesselation.
    void on_remove(int index) override;

    /// Positions all children according to their min, preferred and max sizes
    void position_children();
    /// Updates the preferred sizes of all widgets to their current sizes.
    void update_preferred_sizes();
    /// Returns the actual size of the widget at the given index
    int get_widget_actual_size(int index);
    /// Sets the actual size of the widget at the given index
    void set_widget_actual_size(int index, int size);

    
    public:
    /// Fle_Stack constructor.
    ///
    /// \param X The x position of the stack.
    /// \param Y The y position of the stack.
    /// \param W The width of the stack.
    /// \param H The height of the stack.
    /// \param orientation The orientation of the stack.
    Fle_Stack(int X, int Y, int W, int H, Fle_Stack_Orientation orientation = FLE_STACK_VERTICAL);
    
    /// Handles FLTK events for the stack.
    int handle(int e) override;

    /// Handles resizing of the widget.
    void resize(int X, int Y, int W, int H) override;
    
    /// Adds a widget to the stack.
    ///
    /// \param widget The widget to add.
    /// \param minSize The minimum size of the widget.
    /// \param preferredSize The preferred size of the widget.
    /// \param maxSize (Optional) The maximum size of the widget.
    bool add(Fl_Widget* widget, int minSize, int preferredSize, int maxSize = 2147483647);

    /// Inserts a widget into the stack.
    ///
    /// \param widget The widget to insert.
    /// \param index The index at which to insert the widget.
    /// \param minSize The minimum size of the widget.
    /// \param preferredSize The preferred size of the widget.
    /// \param maxSize (Optional) The maximum size of the widget.
    bool insert(Fl_Widget* widget, int index, int minSize, int preferredSize, int maxSize = 2147483647);

    /// Sets the minimum size of the widget at the given index.
    ///
    /// \param index The index of the widget.
    /// \param minSize The minimum size of the widget.
    void set_min_size(int index, int minSize);

    /// Sets the preferred size of the widget at the given index.
    ///
    /// \param index The index of the widget.
    /// \param preferredSize The preferred size of the widget.
    void set_preferred_size(int index, int preferredSize);

    /// Sets the maximum size of the widget at the given index.
    ///
    /// \param index The index of the widget.
    /// \param maxSize The maximum size of the widget.
    void set_max_size(int index, int maxSize);

    /// Gets the minimum size of the widget at the given index.
    ///
    /// \param index The index of the widget.
    /// \return The minimum size of the widget.
    int get_min_size(int index) const;

    /// Gets the preferred size of the widget at the given index.
    ///
    /// \param index The index of the widget.
    /// \return The preferred size of the widget.
    int get_preferred_size(int index) const;

    /// Gets the maximum size of the widget at the given index.
    ///
    /// \param index The index of the widget.
    /// \return The maximum size of the widget.
    int get_max_size(int index) const;

    /// Gets the sum of the minimum sizes of all widgets.
    /// 
    /// \return The sum of the minimum sizes of all widgets.
    int min_size_sum() const { return std::accumulate(m_widgetMinSizes.begin(), m_widgetMinSizes.end(), 0); }

    /// Gets the orientation of the stack.
    /// 
    /// \return The orientation of the stack.
    Fle_Stack_Orientation orientation() const { return m_orientation; }
};

#endif