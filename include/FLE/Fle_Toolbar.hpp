#ifndef FLE_TOOLBAR_H
#define FLE_TOOLBAR_H

#include <FLE/Fle_Orientable_Flex.hpp>

/// \class Fle_Toolbar
/// \brief A simple toolbar widget.
/// 
/// Various widgets can be added to the toolbar, and
/// they will be placed in the order they are added.
class Fle_Toolbar : public Fle_Orientable_Flex
{
public:
	Fle_Toolbar(int X, int Y, int W, int H, int direction);

	/// Add a separator
	void add_separator();

	/// Insert a separator
	/// \param before Widget to insert separator before
	void insert_separator(Fl_Widget* before);

	/// Add a widget
	/// \param widget Pointer to a widget
	void add_tool(Fl_Widget* widget);

	/// Insert a widget
	/// \param widget Pointer to a widget
	/// \param before Widget to insert before
	void insert_tool(Fl_Widget* widget, Fl_Widget* before);
};

#endif