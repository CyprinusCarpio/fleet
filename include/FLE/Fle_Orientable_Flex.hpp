#ifndef FLE_ORIENTABLE_FLEX_H
#define FLE_ORIENTABLE_FLEX_H

#include <FL/Fl_Flex.H>

/// \class Fle_Orientable_Flex
/// \brief Flex that can be set to vertical or horizontal at runtime.
class Fle_Orientable_Flex : public Fl_Flex
{
	int m_lastLayoutOrientation; // 0 - vertical 1 - horizontal

public:
	Fle_Orientable_Flex(int direction);
	Fle_Orientable_Flex(int w, int h, int direction);
	Fle_Orientable_Flex(int x, int y, int w, int h, int direction);

	int handle(int e) override;

	/// Set orientation
	/// \param orientation 0 - vertical 1 - horizontal
	void set_orientation(int orientation);
};

#endif