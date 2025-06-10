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
/// It is typically used to group related widgets together. 
class Fle_Stack : public Fl_Group 
{
    
    Fle_Stack_Orientation m_orientation; //< Orientation of the stack
    std::vector<int> m_widgetMinSizes; //< Minimum sizes of the widgets
    std::vector<int> m_widgetPreferredSizes; //< Preferred sizes of the widgets
    std::vector<int> m_widgetMaxSizes; //< Maximum sizes of the widgets

protected:
    void on_remove(int index) override;

    void position_children();
    void update_preferred_sizes();
    int get_widget_actual_size(int index);
    void set_widget_actual_size(int index, int size);

    
    public:
    Fle_Stack(int X, int Y, int W, int H, Fle_Stack_Orientation orientation = FLE_STACK_VERTICAL);
    
    int handle(int e) override;
    void resize(int X, int Y, int W, int H) override;
    
    bool add(Fl_Widget* widget, int minSize, int preferredSize, int maxSize = 2147483647);
    bool insert(Fl_Widget* widget, int index, int minSize, int preferredSize, int maxSize = 2147483647);

    void set_min_size(int index, int minSize);
    void set_preferred_size(int index, int preferredSize);
    void set_max_size(int index, int maxSize);

    int get_min_size(int index) const;
    int get_preferred_size(int index) const;
    int get_max_size(int index) const;

    int min_size_sum() const { return std::accumulate(m_widgetMinSizes.begin(), m_widgetMinSizes.end(), 0); }
    Fle_Stack_Orientation orientation() const { return m_orientation; }
};

#endif