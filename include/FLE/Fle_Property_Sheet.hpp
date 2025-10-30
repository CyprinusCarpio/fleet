#ifndef FLE_PROPERTY_SHEET_H
#define FLE_PROPERTY_SHEET_H

#include <FL/Fl_Grid.H>

/// \class Fle_Property_Sheet
/// \brief A container widget for displaying property widgets.
/// 
/// A property sheet is a container widget that displays property widgets
/// in a grid layout. It is typically used to group widgets pertaining
/// to a specific object or entity.
class Fle_Property_Sheet: public Fl_Grid
{
    int m_nameColumnWidth; ///< Width of the name column
    int m_heightSum;       ///< Sum of the heights of all rows

protected:
    /// Overridden to fix a minor graphical issue.
    void draw() override; 

public:
    /// Typical CTOR for the property sheet.
    Fle_Property_Sheet(int X, int Y, int W, int H, const char* l = 0);

    /// Adds a widget to the property sheet with the given name.
    /// The widget's height dictates the height of the row. Other coordinates are ignored.
    /// Do not add widgets with visible labels, use the name parameter instead.
    ///
    /// \param w The widget to add
    /// \param name The name of the property. If it's an empty string, the widget will occupy both the name and value columns
    void add_property_widget(Fl_Widget* w, const char* name);
};
#endif