#ifndef FLE_TOOLBAR_H
#define FLE_TOOLBAR_H

#include <FLE/Fle_Orientable_Flex.hpp>

class Fle_Toolbar : public Fle_Orientable_Flex
{
public:
	Fle_Toolbar(int X, int Y, int W, int H, int direction);

	void add_separator();
	void insert_separator(Fl_Widget* before);
	void add_tool(Fl_Widget* widget);
	void insert_tool(Fl_Widget* widget, Fl_Widget* before);
};

#endif