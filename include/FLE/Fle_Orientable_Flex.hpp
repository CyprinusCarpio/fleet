#ifndef FLE_ORIENTABLE_FLEX_H
#define FLE_ORIENTABLE_FLEX_H

#include <FL/Fl_Flex.h>

class Fle_Orientable_Flex : public Fl_Flex
{
	int m_lastLayoutOrientation; // 0 - vertical 1 - horizontal

public:
	Fle_Orientable_Flex(int direction);
	Fle_Orientable_Flex(int w, int h, int direction);
	Fle_Orientable_Flex(int x, int y, int w, int h, int direction);

	int handle(int e) override;

	void set_orientation(int orientation);
};

#endif