#include <FLE/Fle_Dock_Host.hpp>

#include <algorithm>
#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

// Constructor and destructor

Fle_Dock_Host::Fle_Dock_Host(int X, int Y, int W, int H, const char* l, int activeDirections) : Fl_Group(X, Y, W, H, l)
{
	m_workWidget = nullptr;
	m_activeDirections = activeDirections;

	m_breadthTop = 0;
	m_breadthRight = 0;
	m_breadthBottom = 0;
	m_breadthLeft = 0;
	m_workWidgetMinW = 25;
	m_workWidgetMinH = 25;
	m_oldMinW = 0;
	m_oldMinH = 0;

	m_previewX = 0;
	m_previewY = 0;
	m_previewW = 0;
	m_previewH = 0;

	m_previewColor = FL_SELECTION_COLOR;

	m_minSizeCallback = nullptr;

	end(); // child widgets cannot be added normally
}

Fle_Dock_Host::~Fle_Dock_Host()
{
}

// Private member functions

void Fle_Dock_Host::position_work_widget()
{
	m_workWidget->resize(x() + m_breadthLeft,
		y() + m_breadthTop,
		w() - m_breadthRight - m_breadthLeft,
		h() - m_breadthBottom - m_breadthTop);

	redraw();
	
	//Fl_Group::init_sizes();
}

void Fle_Dock_Host::recalculate_direction_breadths()
{
	m_breadthTop = get_direction_breadth(FLE_DOCK_TOP);
	m_breadthRight = get_direction_breadth(FLE_DOCK_RIGHT);
	m_breadthBottom = get_direction_breadth(FLE_DOCK_BOTTOM);
	m_breadthLeft = get_direction_breadth(FLE_DOCK_LEFT);
}

// This needs to be done after adding/removing/moving horizontal groups
// so as they don't overlap with the vertical groups.
void Fle_Dock_Host::reposition_vertical_groups(int _oldBreadthTop, int _oldBreadthBottom)
{
	int oldBreadthTop = m_breadthTop;
	int oldBreadthBottom = m_breadthBottom;

	recalculate_direction_breadths();

	if (_oldBreadthTop != -1)
		oldBreadthTop = _oldBreadthTop;

	if (_oldBreadthBottom != -1)
		oldBreadthBottom = _oldBreadthBottom;

	std::list<std::list<std::list<Fle_Dock_Group*>>*> directionsToFix;
	if (m_activeDirections & FLE_DOCK_LEFT) directionsToFix.push_back(&m_leftLines);
	if (m_activeDirections & FLE_DOCK_RIGHT) directionsToFix.push_back(&m_rightLines);

	for (std::list<std::list<Fle_Dock_Group*>>* direction : directionsToFix)
	{
		// We need to keep track of groups moved to another line in order
		// not to move/resize them twice
		std::list<Fle_Dock_Group*> alreadyFixed;

		for(std::list<std::list<Fle_Dock_Group*>>::iterator it = direction->begin(); it != direction->end(); it++)
		{
			std::list<Fle_Dock_Group*>* line = &(*it);

			// If the new top or bottom breadth is smaller, then just scale up
			// the top or bottom group in this line and position it correctly
			// ret val of set_size ignored because a group can't be too big
			if (oldBreadthTop > m_breadthTop)
			{
				Fle_Dock_Group* group = *line->begin();
				group->position(group->x(), m_breadthTop);
				group->set_size(oldBreadthTop - m_breadthTop + group->get_size());
				continue;
			}
			if (oldBreadthBottom > m_breadthBottom)
			{
				Fle_Dock_Group* group = line->back();
				group->set_size(oldBreadthBottom - m_breadthBottom + group->get_size());
				continue;
			}

			// If the new breadth is bigger, it may be enough to just scale
			// down the top or bottom group, if not, squeeze the line
			bool lineNeedsSqueezing = false;

			// If top breadth is expanded, we need to reposition the top group in a line
			if (oldBreadthTop < m_breadthTop)
			{
				int d = m_breadthTop - oldBreadthTop;
				Fle_Dock_Group* group = *line->begin();

				// Check if it was fixed already as part of moving it into another line
				if(std::find(alreadyFixed.begin(), alreadyFixed.end(), group) == alreadyFixed.end())
				{
					group->position(group->x(), m_breadthTop);

					if (!group->set_size(group->get_size() - d))
					{
						lineNeedsSqueezing = true;
					}
				}
			}
			if (oldBreadthBottom < m_breadthBottom)
			{
				int d = m_breadthBottom - oldBreadthBottom;
				Fle_Dock_Group* group = line->back();

				// Check if it was fixed already as part of moving it into another line
				if (std::find(alreadyFixed.begin(), alreadyFixed.end(), group) == alreadyFixed.end() && !group->set_size(group->get_size() - d))
				{
					lineNeedsSqueezing = true;
				}
			}

			if (lineNeedsSqueezing) squeeze_groups_in_line(line, true, &alreadyFixed);
		}
	}
}

void Fle_Dock_Host::squeeze_groups_in_line(std::list<Fle_Dock_Group*>* line, bool isVertical, std::list<Fle_Dock_Group*>* alreadyFixed)
{
	int availableSpaceInLine = w();
	if (isVertical)
	{
		availableSpaceInLine = h() - m_breadthTop - m_breadthBottom;
	}

	// First, check if the min size of the groups in this line exceeds the available space
	int smallestSizeSum = 0;

	for (std::list<Fle_Dock_Group*>::iterator it = line->begin(); it != line->end(); it++)
	{
		smallestSizeSum += (*it)->get_min_size();
	}

	// Check to prevent stack overFlew
	if (line->size() == 1 && smallestSizeSum > availableSpaceInLine)
	{
		// TODO: What now?
		return;
	}

	if (alreadyFixed != nullptr && smallestSizeSum > availableSpaceInLine)
	{
		// Move the smallest min size group to another line
		Fle_Dock_Group* smallestGroup = nullptr;
		for (Fle_Dock_Group* group : *line)
		{
			if (smallestGroup == nullptr) 
			{
				smallestGroup = group;
				continue;
			}
			if (group->get_min_size() < smallestGroup->get_min_size())
				smallestGroup = group;
		}

		line->erase(std::find(line->begin(), line->end(), smallestGroup));

		add_dock_group(smallestGroup, smallestGroup->get_direction(), 0);
		alreadyFixed->push_back(smallestGroup);
		
		// After we got rid of the smallest group, try this func again
		squeeze_groups_in_line(line, isVertical, alreadyFixed);
		return;
	}

	// Position the groups so that they dont overlap and
	// calculate the current sum of sizes, and scale them
	// up to their min sizes if needed
	int sizeSum = 0;
	for (Fle_Dock_Group* group : *line)
	{
		if (isVertical)
		{
			group->position(group->x(), y() + m_breadthTop + sizeSum);
		}
		else
		{
			group->position(x() + sizeSum, group->y());
		}

		// The group may have been scaled below its min size during window resizing
		if (group->get_size() < group->get_min_size()) 
		{
			group->set_size(group->get_min_size());
		}
		sizeSum += group->get_size();
	}

	int d = sizeSum - availableSpaceInLine;

	// Now try to fit everything by scaling down groups beginning
	// from the bottom/right
	for (std::list<Fle_Dock_Group*>::reverse_iterator it = line->rbegin(); it != line->rend(); it++)
	{
		Fle_Dock_Group* group = *it;
		
		int dg = group->get_size() - group->get_min_size();
		if (dg >= d)
		{
			// This one group can be sufficiently scaled down, no need
			// to scale down all the groups in this line
			group->set_size(group->get_size() - d);
			if (it == line->rbegin())
			{
				// If this was the last group we don't need to reposition anything
				return;
			}
			break;
		}

		group->set_size(group->get_min_size());

		d -= dg;
		if (d <= 0) break;
	}

	// If that wasn't enough we need to reposition all groups
	int offsetInLine = 0;
	for (std::list<Fle_Dock_Group*>::iterator it = line->begin(); it != line->end(); it++)
	{
		Fle_Dock_Group* group = *it;

		if (isVertical)
		{
			group->position(group->x(), y() + m_breadthTop + offsetInLine);
		}
		else
		{
			group->position(x() + offsetInLine, group->y());
		}
		offsetInLine += group->get_size();
	}

	line_reset_preferred_sizes(line);
}

void Fle_Dock_Host::line_change_offset_in_host(std::list<Fle_Dock_Group*>* line, int offset)
{
	int direction = (*line->begin())->get_direction();

	for (std::list<Fle_Dock_Group*>::iterator itg = line->begin(); itg != line->end(); itg++)
	{
		if (direction == FLE_DOCK_BOTTOM)
		{
			(*itg)->position((*itg)->x(), (*itg)->y() - offset);
		}
		else if (direction == FLE_DOCK_RIGHT)
		{
			(*itg)->position((*itg)->x() - offset, (*itg)->y());
		}
		else if (direction == FLE_DOCK_TOP)
		{
			(*itg)->position((*itg)->x(), (*itg)->y() + offset);
		}
		else if (direction == FLE_DOCK_LEFT)
		{
			(*itg)->position((*itg)->x() + offset, (*itg)->y());
		}
	}
}

// Returns oldBreadth - newBreadth
int Fle_Dock_Host::line_set_breadth(std::list<Fle_Dock_Group*>* line, int newBreadth, bool setToMinSize)
{
	int minBreadth = get_line_min_breadth(line);
	int oldBreadth = (*line->begin())->get_breadth();

	if (newBreadth < minBreadth)
	{
		if(!setToMinSize)
			return 0;
		for (Fle_Dock_Group* group : *line)
		{
			group->set_breadth(minBreadth);
		}
		return oldBreadth - minBreadth;
	}

	for (Fle_Dock_Group* group : *line)
	{
		group->set_breadth(newBreadth);
	}

	return oldBreadth - newBreadth;
}

bool Fle_Dock_Host::is_host_visible_at(int X, int Y)
{
	// TODO: implement
	return true;
}

void Fle_Dock_Host::remove_empty_lines(int direction)
{
	int oldBreadthTop = m_breadthTop;
	int oldBreadthBottom = m_breadthBottom;
	int oldBreadthRight = m_breadthRight;
	int oldBreadthLeft = m_breadthLeft;

	recalculate_direction_breadths();

	std::list<std::list<Fle_Dock_Group*>>* lines;

	bool isVertical = true;
	int d = 0;

	switch (direction)
	{
	case FLE_DOCK_TOP:
		lines = &m_topLines;
		isVertical = false;
		d = oldBreadthTop - m_breadthTop;
		break;
	case FLE_DOCK_RIGHT:
		lines = &m_rightLines;
		d = oldBreadthRight - m_breadthRight;
		break;
	case FLE_DOCK_BOTTOM:
		lines = &m_bottomLines;
		isVertical = false;
		d = oldBreadthBottom - m_breadthBottom;
		break;
	case FLE_DOCK_LEFT:
		lines = &m_leftLines;
		d = oldBreadthLeft - m_breadthLeft;
		break;
	}

	// Check for empty lines in that direction
	bool lineErased = false;
	for (std::list<std::list<Fle_Dock_Group*>>::iterator it = lines->begin(); it != lines->end(); it++)
	{
		if (it->empty())
		{
			it = lines->erase(it);
			lineErased = true;

			// If that was the last line in this direction we don't need to move other lines
			if (it == lines->end())
			{
				break;
			}
		}
		if (lineErased)
		{
			for (std::list<Fle_Dock_Group*>::iterator ita = it->begin(); ita != it->end(); ita++)
			{
				Fle_Dock_Group* g = *ita;
				switch (direction)
				{
				case FLE_DOCK_TOP:
					g->position(g->x(), g->y() - d);
					break;
				case FLE_DOCK_RIGHT:
					g->position(g->x() + d, g->y());
					break;
				case FLE_DOCK_BOTTOM:
					g->position(g->x(), g->y() + d);
					break;
				case FLE_DOCK_LEFT:
					g->position(g->x() - d, g->y());
					break;
				}
			}
		}
	}

	if (!isVertical)
	{
		reposition_vertical_groups(oldBreadthTop, oldBreadthBottom);
	}

	position_work_widget();
	find_edges();
}

void Fle_Dock_Host::find_edges()
{
	m_edges.clear();

	std::list<std::list<std::list<Fle_Dock_Group*>>*> directions;
	if (m_activeDirections & FLE_DOCK_RIGHT) directions.push_back(&m_rightLines);
	if (m_activeDirections & FLE_DOCK_LEFT) directions.push_back(&m_leftLines);
	if (m_activeDirections & FLE_DOCK_TOP) directions.push_back(&m_topLines);
	if (m_activeDirections & FLE_DOCK_BOTTOM) directions.push_back(&m_bottomLines);

	// Type 1: work group edges

	if (is_direction_flexible(FLE_DOCK_TOP))
		m_edges.push_back(Fle_Edge_Helper(x() + m_breadthLeft, y() + m_breadthTop, w() - m_breadthLeft - m_breadthRight, 0));
	if (is_direction_flexible(FLE_DOCK_RIGHT))
		m_edges.push_back(Fle_Edge_Helper(x() + w() - m_breadthRight, y() + m_breadthTop, h() - m_breadthTop - m_breadthBottom, 1));
	if (is_direction_flexible(FLE_DOCK_BOTTOM))
		m_edges.push_back(Fle_Edge_Helper(x() + m_breadthLeft, y() + h() - m_breadthBottom, w() - m_breadthLeft - m_breadthRight, 0));
	if (is_direction_flexible(FLE_DOCK_LEFT))
		m_edges.push_back(Fle_Edge_Helper(x() + m_breadthLeft, y() + m_breadthTop, h() - m_breadthTop - m_breadthBottom, 1));

	// Type 2: edges between groups
	for (std::list<std::list<std::list<Fle_Dock_Group*>>*>::iterator it = directions.begin(); it != directions.end(); it++)
	{
		std::list<std::list<Fle_Dock_Group*>>* direction = *it;

		for (std::list<std::list<Fle_Dock_Group*>>::iterator ita = direction->begin(); ita != direction->end(); ita++)
		{
			std::list<Fle_Dock_Group*>* line = &(*ita);
			if (line->size() <= 1) continue; // If there is only one group in this line then type 2 is N/A

			for (std::list<Fle_Dock_Group*>::reverse_iterator itb = line->rbegin(); itb != std::prev(line->rend()); itb++)
			{
				Fle_Dock_Group* g = *itb;
				//std::cout << g->label() << " " << g->get_size() << " " << g->get_min_size() << " " << (*std::next(itb))->label() << " " << (*std::next(itb))->get_size() << " " << (*std::next(itb))->get_min_size() << std::endl;
				// At least one of the groups needs to be above its min size
				if(g->get_size() > g->get_min_size() || (*std::next(itb))->get_size() > (*std::next(itb))->get_min_size())
				{
					m_edges.push_back(Fle_Edge_Helper(g->x(), g->y(), g->get_breadth(), g->get_direction() == FLE_DOCK_TOP || g->get_direction() == FLE_DOCK_BOTTOM, *std::next(itb), g));
				}
			}
		}

	}

	// Type 3: edges between lines
	for (std::list<std::list<std::list<Fle_Dock_Group*>>*>::iterator it = directions.begin(); it != directions.end(); it++)
	{
		std::list<std::list<Fle_Dock_Group*>>* direction = *it;
		if (direction->size() <= 1) continue; // If there is only one line in this direction then type 3 is N/A

		int d = (*direction->begin()->begin())->get_direction();

		// Direction needs to be flexible
		// If there are only two lines, both need to be flexible
		if (direction->size() == 2)
		{
			if (!(*direction->begin()->begin())->flexible() || !(*std::next(direction->begin())->begin())->flexible())
				continue;
		}
		else if (!is_direction_flexible(d))
			continue;

		// At least one line needs to be above it's min breadth
		bool isThereRoom = false;

		for (std::list<std::list<Fle_Dock_Group*>>::iterator ita = direction->begin(); ita != direction->end(); ita++)
		{
			std::list<Fle_Dock_Group*>* line = &(*ita);

			if ((*line->begin())->get_breadth() > get_line_min_breadth(line))
			{
				// A line that has more breadth than minimum is assumed to be flexible.
				isThereRoom = true;
				break;
			}
		}
		if (!isThereRoom)
			continue;

		for (std::list<std::list<Fle_Dock_Group*>>::iterator ita = direction->begin(); ita != std::prev(direction->end()); ita++)
		{
			std::list<Fle_Dock_Group*>* line = &(*ita);
			std::list<Fle_Dock_Group*>* nextLine = &(*std::next(ita));
			
			// Let A and B be line and nextLine. For rescaling on the edge of those lines
			// to be allowed (having cleared the above checks) one of the following must be true:
			// 1. Both are flexible
			// 2. If A is flexible but B is not, then there must be a line after B that is flexible and above min breadth.
			// -- 3. If B is flexible but A is not, then there must be a line before A that is flexible and above min breadth.

			bool valid = true;
			if (!(*line->begin())->flexible() || !(*nextLine->begin())->flexible())
			{
				valid = false;
				if ((*line->begin())->flexible())
				{
					for (std::list<std::list<Fle_Dock_Group*>>::iterator itb = std::next(ita, 2); itb != direction->end(); itb++)
					{
						if (get_line_min_breadth(&(*itb)) < (*itb->begin())->get_breadth() && (*itb->begin())->flexible())
						{
							valid = true;
							break;
						}
					}
				}
				/*else if ((*nextLine->begin())->flexible())
				{
					for (std::list<std::list<Fle_Dock_Group*>>::iterator itb = std::prev(ita);;)
					{
						if (get_line_min_breadth(&(*itb)) < (*itb->begin())->get_breadth() && (*itb->begin())->flexible())
						{
							valid = true;
							break;
						}

						if (itb == direction->begin()) break;
						itb--;
					}
				}*/
			}
			if (!valid)
				continue;

			switch (d)
			{
			case FLE_DOCK_TOP:
				m_edges.push_back(Fle_Edge_Helper(x(), (*line->begin())->y() + (*line->begin())->get_breadth(), w(), false, ita, direction));
				break;
			case FLE_DOCK_RIGHT:
				m_edges.push_back(Fle_Edge_Helper((*line->begin())->x(), m_breadthTop, h() - m_breadthTop - m_breadthBottom, true, ita, direction));
				break;
			case FLE_DOCK_BOTTOM:
				m_edges.push_back(Fle_Edge_Helper(x(),(*line->begin())->y(), w(), false, ita, direction));
				break;
			case FLE_DOCK_LEFT:
				m_edges.push_back(Fle_Edge_Helper((*line->begin())->x() + (*line->begin())->w(), m_breadthTop, h() - m_breadthTop - m_breadthBottom, true, ita, direction));
				break;
			}
			
		}
	}
}

int Fle_Dock_Host::get_flexible_directions_count()
{
	int count = 0;
	if (is_direction_flexible(FLE_DOCK_TOP)) count++;
	if (is_direction_flexible(FLE_DOCK_RIGHT)) count++;
	if (is_direction_flexible(FLE_DOCK_BOTTOM)) count++;
	if (is_direction_flexible(FLE_DOCK_LEFT)) count++;
	return count;
}

int Fle_Dock_Host::get_line_min_breadth(std::list<Fle_Dock_Group*>* line)
{
	int breadth = 0;

	for (Fle_Dock_Group* group : *line)
	{
		if (group->get_min_breadth() > breadth)
			breadth = group->get_min_breadth();
	}

	return breadth;
}

// FLTK overrides

void Fle_Dock_Host::resize(int X, int Y, int W, int H)
{
	int oldW = w();
	int oldH = h();

	Fl_Widget::resize(X, Y, W, H);

	int dW = W - oldW;
	int dH = H - oldH;

	if (m_workWidget != nullptr)
	{
		position_work_widget();
	}

	// Scaling up goes left to right, top to bottom
	// scaling down right to left, bottom to top
	// last group always needs to fill the remaining space

	resize_direction(&m_topLines, dW, 0);
	resize_direction(&m_leftLines, dH, 0);
	resize_direction(&m_rightLines, dH, dW);
	resize_direction(&m_bottomLines, dW, dH);
	// TODO: find if user is interacting with host to avoid calling find_edges when not needed
	find_edges();

	return;
}

int Fle_Dock_Host::handle(int e)
{
	// We need to handle 3 types of rescaling:
	// 1. Scaling by the edge of work group
	//    This changes the BREADTH of the closest flexible line.
	//    If there is no flexible line on that direction this type
	//    is disallowed.
	// 2. Scaling between two groups
	//    Changes the SIZE of two groups in a single line, making sure
	//    that the min size of either group is respected.
	// 3. Scaling between two lines
	//    Changes the BREADTH of all the groups in two lines,
	//    that may or may not be next to eachother.
	//    It cannot change the total breadth of a direction.

	// type 1:
	static int scalingDirection = 0;

	// type 2:
	static Fle_Dock_Group* group1 = nullptr;
	static Fle_Dock_Group* group2 = nullptr;

	// type 3:
	static std::list<std::list<Fle_Dock_Group*>>::iterator line_it;
	static std::list<std::list<Fle_Dock_Group*>>* type3direction = nullptr;

	// general scaling vars
	static int offsetX = 0;
	static int offsetY = 0;

	if (e == FL_HIDE)
	{
		if (!window()->shown())
		{
			// Hide all detached windows
			for (Fle_Dock_Group* group : m_detachedGroups)
			{
				// TODO: fix hide detached groups when host is hidden
				group->m_detachedWindow->hide();
			}
		}
	}
	if (e == FL_ENTER)
	{
		// We need FL_MOVE
		return 1;
	}
	else if (e == FL_MOVE)
	{
		// Check if we need to display a resize cursor

		int ex = Fl::event_x();
		int ey = Fl::event_y();

		window()->cursor(FL_CURSOR_DEFAULT);

		// type 1:
		int i = 0;
		for (std::list<Fle_Edge_Helper>::iterator it = m_edges.begin(); i < get_flexible_directions_count(); it++)
		{
			if (it == m_edges.end())
			{
				find_edges();
				it = m_edges.begin();
			}
			if (it->is_close(ex, ey))
			{
				window()->cursor(it->v ? FL_CURSOR_WE : FL_CURSOR_NS);
				return 1;
			}
			i++;
		}
		// type 1 end

		// type 2 and 3
		for (std::list<Fle_Edge_Helper>::iterator it = m_edges.begin(); it != m_edges.end(); it++)
		{
			if (it->group_a != nullptr && it->is_close(ex, ey)) // type 2
			{
				window()->cursor(it->v ? FL_CURSOR_WE : FL_CURSOR_NS);
				return 1;
			}

			if (it->direction != nullptr && it->is_close(ex, ey)) // type 3
			{
				window()->cursor(it->v ? FL_CURSOR_WE : FL_CURSOR_NS);
				return 1;
			}
		}
		// type 2 and 3 end

	}
	else if (e == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE)
	{
		int ex = Fl::event_x();
		int ey = Fl::event_y();

		// type 1:
		int i = 0; // direction as defined in enum
		for (std::list<Fle_Edge_Helper>::iterator it = m_edges.begin(); i < 4; it++)
		{
			// So we don't dereference something invalid
			// TODO: crashes here if all groups detached
			if (!is_direction_flexible(1 << i))
			{
				if(it != m_edges.begin())
					it--;
				i++;
				continue;
			}
			if (it->is_close(ex, ey))
			{
				scalingDirection = 1 << i;
				offsetX = ex;
				offsetY = ey;
				return 1;
			}
			i++;
		}
		// type 1 end

		// type 2 and 3:
		for (std::list<Fle_Edge_Helper>::iterator it = m_edges.begin(); it != m_edges.end(); it++)
		{
			if (it->group_a != nullptr && it->is_close(ex, ey)) // type 2
			{
				group1 = it->group_a;
				group2 = it->group_b;
				offsetX = ex;
				offsetY = ey;
				return 1;
			}
			if (it->direction != nullptr && it->is_close(ex, ey)) // type 3
			{
				line_it = it->line_it;
				type3direction = it->direction;
				offsetX = ex;
				offsetY = ey;
				return 1;
			}
		} // type 2 and 3 end
	}
	else if ((scalingDirection != 0 || group1 != nullptr || type3direction != nullptr) && e == FL_RELEASE && Fl::event_button() == FL_LEFT_MOUSE)
	{
		scalingDirection = 0;
		group1 = nullptr;
		group2 = nullptr;
		type3direction = nullptr;
		offsetX = 0;
		offsetY = 0;
		//init_sizes();
		find_edges();
	}
	else if (e == FL_DRAG)
	{
		int d = 0;
		// type 1:
		std::list<std::list<Fle_Dock_Group*>>* direction = nullptr;
		switch (scalingDirection)
		{
		case FLE_DOCK_TOP:
			direction = &m_topLines;
			d = Fl::event_y() - offsetY;
			break;
		case FLE_DOCK_LEFT:
			direction = &m_leftLines;
			d = Fl::event_x() - offsetX;
			break;
		case FLE_DOCK_RIGHT:
			direction = &m_rightLines;
			d = Fl::event_x() - offsetX;
			d *= -1;
			break;
		case FLE_DOCK_BOTTOM:
			direction = &m_bottomLines;
			d = Fl::event_y() - offsetY;
			d *= -1;
			break;
		}

		if (direction)
		{
			// Find the flexible line closest to the work group, change it's breadth
			// and reposition the non flexible groups closer to the work group if any
			for (std::list<std::list<Fle_Dock_Group*>>::reverse_iterator it = direction->rbegin(); it != direction->rend(); it++)
			{
				if ((*it->begin())->flexible())
				{
					// Change the breadths of the groups in this line
					int lineMinBreadth = 0;
					int lineBreadth = 0;
					for (std::list<Fle_Dock_Group*>::iterator itg = it->begin(); itg != it->end(); itg++)
					{
						if ((*itg)->get_min_breadth() > lineMinBreadth)
							lineMinBreadth = (*itg)->get_min_breadth();
						if ((*itg)->get_breadth() > lineBreadth)
							lineBreadth = (*itg)->get_breadth();
					}

					if (lineBreadth + d < lineMinBreadth)
					{
						// Continue on to scaling other flexible lines
						continue;
					}

					line_set_breadth(&(*it), lineBreadth + d, false);
					for (std::list<Fle_Dock_Group*>::iterator itg = it->begin(); itg != it->end(); itg++)
					{
						if (scalingDirection == FLE_DOCK_BOTTOM)
						{
							(*itg)->position((*itg)->x(), (*itg)->y() - d);
						}
						else if (scalingDirection == FLE_DOCK_RIGHT)
						{
							(*itg)->position((*itg)->x() - d, (*itg)->y());
						}
					}
					// Now reposition the groups in lines closer to the work group by reversing the loop
					if (it != direction->rbegin())
					{
						for (std::list<std::list<Fle_Dock_Group*>>::reverse_iterator itr = std::prev(it);;)
						{
							for (std::list<Fle_Dock_Group*>::iterator itg = itr->begin(); itg != itr->end(); itg++)
							{
								switch (scalingDirection)
								{
								case FLE_DOCK_TOP:
									(*itg)->position((*itg)->x(), (*itg)->y() + d);
									break;
								case FLE_DOCK_LEFT:
									(*itg)->position((*itg)->x() + d, (*itg)->y());
									break;
								case FLE_DOCK_RIGHT:
									(*itg)->position((*itg)->x() - d, (*itg)->y());
									break;
								case FLE_DOCK_BOTTOM:
									(*itg)->position((*itg)->x(), (*itg)->y() - d);
									break;
								}
							}

							if (itr == direction->rbegin()) break;
							itr--;
						}
					}
					// Now finalize the rescale
					if (scalingDirection == FLE_DOCK_TOP || scalingDirection == FLE_DOCK_BOTTOM)
					{
						reposition_vertical_groups();
					}
					else
						recalculate_direction_breadths();

					position_work_widget();
					//find_edges();
					calculate_min_size();

					offsetX = Fl::event_x();
					offsetY = Fl::event_y();

					return 1;
				}
			}
		} // type 1 end
		
		// type 2:
		if (group1 != nullptr)
		{
			if (group1->get_direction() == FLE_DOCK_RIGHT || group1->get_direction() == FLE_DOCK_LEFT)
			{
				d = Fl::event_y() - offsetY;
				// Check if new sizes would be less than min sizes
				if(group1->get_min_size() <= group1->get_size() + d && group2->get_min_size() <= group2->get_size() - d)
				{
					group1->set_size(group1->get_size() + d);
					group2->position(group2->x(), group2->y() + d);
					group2->set_size(group2->get_size() - d);

					group1->update_preferred_size();
					group2->update_preferred_size();

					offsetX = Fl::event_x();
					offsetY = Fl::event_y();

					//find_edges();

					return 1;
				}
			}
			else
			{
				d = Fl::event_x() - offsetX;
				// Check if new sizes would be less than min sizes
				if (group1->get_min_size() <= group1->get_size() + d && group2->get_min_size() <= group2->get_size() - d)
				{
					group1->set_size(group1->get_size() + d);
					group2->position(group2->x() + d, group2->y());
					group2->set_size(group2->get_size() - d);

					group1->update_preferred_size();
					group2->update_preferred_size();

					offsetX = Fl::event_x();
					offsetY = Fl::event_y();

					//find_edges();

					return 1;
				}
			}
		} // type 2 end

		// type 3:
		if (type3direction != nullptr)
		{
			int direction = (*line_it->begin())->get_direction();
			int directionBreadth = 0;

			switch (direction)
			{
			case FLE_DOCK_TOP:
				d = Fl::event_y() - offsetY;
				directionBreadth = m_breadthTop;
				break;
			case FLE_DOCK_LEFT:
				d = Fl::event_x() - offsetX;
				directionBreadth = m_breadthLeft;
				break;
			case FLE_DOCK_RIGHT:
				d = Fl::event_x() - offsetX;
				d *= -1;
				directionBreadth = m_breadthRight;
				break;
			case FLE_DOCK_BOTTOM:
				d = Fl::event_y() - offsetY;
				d *= -1;
				directionBreadth = m_breadthBottom;
				break;
			}

			// First calculate available breadth in the lines following it
			int availableBreadth = 0;
			for (std::list<std::list<Fle_Dock_Group*>>::iterator it = std::next(line_it); it != type3direction->end(); it++)
			{
				if ((*it->begin())->flexible())
				{
					availableBreadth += (*it->begin())->get_breadth() - get_line_min_breadth(&(*it));
				}
			}
			
			if(availableBreadth >= d)
			{
				for (std::list<std::list<Fle_Dock_Group*>>::iterator it = line_it; it != type3direction->end(); it++)
				{
					if (it == line_it)
					{
						// Prevent scaling below min breadth
						if (line_set_breadth(&(*it), (*it->begin())->get_breadth() + d, false) == 0)
						{
							return 1;
						}
						if (direction == FLE_DOCK_BOTTOM || direction == FLE_DOCK_RIGHT)
							line_change_offset_in_host(&(*it), d);
					}
					else
					{
						// If top or left, move first, else scale first
						if (direction == FLE_DOCK_TOP || direction == FLE_DOCK_LEFT)
							line_change_offset_in_host(&(*it), d);

						// Scale it if its flexible
						if ((*it->begin())->flexible())
						{
							int r = line_set_breadth(&(*it), (*it->begin())->get_breadth() - d, true);
							d -= r;
						}

						// If right or bottom move now
						if (direction == FLE_DOCK_BOTTOM || direction == FLE_DOCK_RIGHT)
							line_change_offset_in_host(&(*it), d);

						if (d == 0) break;
					}
				}

				offsetX = Fl::event_x();
				offsetY = Fl::event_y();

				return 1;
			}

		} // type 3 end

	} // FL_DRAG

	return Fl_Group::handle(e);
}

void Fle_Dock_Host::draw()
{
	Fl_Group::draw();

	// Draw the preview
	if(m_previewW != 0 && m_previewH != 0)
	{
		fl_color(m_previewColor);

		fl_line(m_previewX, m_previewY, m_previewX + m_previewW, m_previewY);
		fl_line(m_previewX + m_previewW, m_previewY, m_previewX + m_previewW, m_previewY + m_previewH);
		fl_line(m_previewX + m_previewW, m_previewY + m_previewH, m_previewX, m_previewY + m_previewH);
		fl_line(m_previewX, m_previewY + m_previewH, m_previewX, m_previewY);

		fl_push_clip(m_previewX, m_previewY, m_previewW, m_previewH);

		for (int X = m_previewX; X < m_previewX + m_previewW + m_previewH; X += 7) 
		{
			fl_line(X, m_previewY, X - m_previewH, m_previewY + m_previewH);
		}

		fl_pop_clip();
	}
}

// Fle_Dock_Host member functions

bool Fle_Dock_Host::add_work_widget(Fl_Widget* widget)
{
	if (m_workWidget != nullptr)
		return false;
	m_workWidget = widget;
	add(m_workWidget);
	//resizable(m_workWidget);
	position_work_widget();
	return true;
}

bool Fle_Dock_Host::add_dock_group(Fle_Dock_Group* group, int direction, int newline)
{
	// newline: 0 = try adding to a existing line 1 = add to a new line 2 - add to a new line inserted at
	// a specific place
	if (direction == 0)
	{
		m_detachedGroups.push_back(group);
		return true;
	}

	if (m_activeDirections & direction)
	{
		// Choose direction vector
		std::list<std::list<Fle_Dock_Group*>>* lines;
		switch (direction)
		{
		case FLE_DOCK_TOP:
			lines = &m_topLines;
			break;
		case FLE_DOCK_RIGHT:
			lines = &m_rightLines;
			break;
		case FLE_DOCK_BOTTOM:
			lines = &m_bottomLines;
			break;
		case FLE_DOCK_LEFT:
			lines = &m_leftLines;
			break;
		default:
			return false;
		}

		// Find suitable line
		std::list<Fle_Dock_Group*>* line = nullptr;

		// We need to keep track of the space to see if we need to make a new line
		// so as not to force a dock group to resize below it's minimal size
		int availableSpaceInLine = 0;

		if (direction == FLE_DOCK_LEFT || direction == FLE_DOCK_RIGHT)
		{
			// Top and bottom directions may take away some space
			availableSpaceInLine = h() - m_breadthTop - m_breadthBottom;
		}
		else
			availableSpaceInLine = w();

		int lineBreadth = 0;

		// TODO: What if minimal size of the newly added group is more than the available space?

		// Find a line with the same dock group breadth and flexibility as the new dock group
		// It also needs to have enough space provided other groups in it are set to their min
		// sizes. A group can be below it's min size due to window rescaling, in such a case we
		// need to scale it back up IF there is enough space for all of them AND the new group.
		// If not, leave that line alone and use some other one.

		if(newline == 0)
		{
			for (std::list<std::list<Fle_Dock_Group*>>::iterator it = lines->begin(); it != lines->end(); it++)
			{
				std::list<Fle_Dock_Group*>* l = &(*it);
				if (l->empty())
				{
					line = l;
					break;
				}
				if((l->back()->flexible() && group->flexible() && group->get_min_breadth() <= l->back()->get_breadth()) || (!group->flexible() && !l->back()->flexible() && group->get_breadth() == l->back()->get_breadth()))
				{
					int spaceAvailableInThisLine = availableSpaceInLine;

					for (std::list<Fle_Dock_Group*>::iterator ita = l->begin(); ita != l->end(); ita++)
					{
						spaceAvailableInThisLine -= (*ita)->get_min_size();
					}

					if (spaceAvailableInThisLine >= group->get_min_size())
					{
						line = l;
						lineBreadth = line->back()->get_breadth();
						break;
					}
				}
			}
		}
		else if (newline > 1) // insert a new line between others
		{
			std::list<std::list<Fle_Dock_Group*>>::iterator addedLine = lines->insert(std::next(lines->begin(), newline - 1), std::list<Fle_Dock_Group*>());
			line = &(*addedLine);
			lineBreadth = group->get_breadth();

			// Move the lines after this one
			for (std::list<std::list<Fle_Dock_Group*>>::iterator it = std::next(addedLine); it != lines->end(); it++)
			{
				line_change_offset_in_host(&(*it), lineBreadth);
			}
		}
		if (line == nullptr)
		{
			lines->push_back(std::list<Fle_Dock_Group*>());
			line = &lines->back();
			lineBreadth = group->get_breadth();
		}

		// Find the right offset in line for the new group
		// setting the other groups in that line to their
		// minimal size.
		int offsetInLine = 0;
		for (std::list<Fle_Dock_Group*>::iterator it = line->begin(); it != line->end(); it++)
		{
			(*it)->set_size((*it)->get_min_size());
			if (direction == FLE_DOCK_LEFT || direction == FLE_DOCK_RIGHT)
			{
				(*it)->position((*it)->x(), m_breadthTop + offsetInLine);
			}
			else if (direction == FLE_DOCK_TOP || direction == FLE_DOCK_BOTTOM)
			{
				(*it)->position(offsetInLine, (*it)->y());
			}
			offsetInLine += (*it)->get_size();
		}

		availableSpaceInLine -= offsetInLine;

		// Now find the right offset for this line
		int offsetInHost = 0;
		if (direction == FLE_DOCK_LEFT || direction == FLE_DOCK_TOP)
		{
			for (std::list<std::list<Fle_Dock_Group*>>::iterator it = lines->begin(); it != lines->end(); it++)
			{
				std::list<Fle_Dock_Group*>* l = &(*it);
				if (l == line) break;
				offsetInHost += l->back()->get_breadth();
			}
		}
		else if (direction == FLE_DOCK_RIGHT || direction == FLE_DOCK_BOTTOM)
		{
			offsetInHost = direction == FLE_DOCK_RIGHT ? w() : h();
			for (std::list<std::list<Fle_Dock_Group*>>::iterator it = lines->begin(); it != lines->end(); it++)
			{
				std::list<Fle_Dock_Group*>* l = &(*it);
				if (l == line) break;
				offsetInHost -= l->back()->get_breadth();
			}
			if (line->size() > 0)
			{
				offsetInHost -= line->back()->get_breadth();
			}
			else
				offsetInHost -= group->get_breadth();
		}

		line->push_back(group);

		add(group);

		if (direction == FLE_DOCK_LEFT || direction == FLE_DOCK_RIGHT)
		{
			group->resize(x() + offsetInHost, y() + offsetInLine + m_breadthTop, lineBreadth, availableSpaceInLine);
			recalculate_direction_breadths();
		}
		else if (direction == FLE_DOCK_TOP || direction == FLE_DOCK_BOTTOM)
		{
			group->resize(x() + offsetInLine, y() + offsetInHost, availableSpaceInLine, lineBreadth);
			reposition_vertical_groups();
		}
		
		calculate_min_size();
		find_edges();
		position_work_widget();
		return true;
	}	

	return false;
}

void Fle_Dock_Host::detach(Fle_Dock_Group* group)
{
	detach(group, true);
}

int Fle_Dock_Host::get_direction_breadth(int direction)
{
	int breadth = 0;
	if (m_activeDirections & direction)
	{
		std::list<std::list<Fle_Dock_Group*>>* lines;
		switch (direction)
		{
		case FLE_DOCK_TOP:
			lines = &m_topLines;
			break;
		case FLE_DOCK_RIGHT:
			lines = &m_rightLines;
			break;
		case FLE_DOCK_BOTTOM:
			lines = &m_bottomLines;
			break;
		case FLE_DOCK_LEFT:
			lines = &m_leftLines;
			break;
		default:
			return 0;
		}
		for (std::list<Fle_Dock_Group*>& line : *lines)
		{
			// All dock groups in one line have the same breadth by design.
			for (Fle_Dock_Group* dg : line)
			{
				breadth += dg->get_breadth();
				break;
			}
		}
	}
	return breadth;
}

bool Fle_Dock_Host::is_direction_flexible(int direction)
{
	if (m_activeDirections & direction)
	{
		// All dock groups in one line are either flexible or not.

		std::list<std::list<Fle_Dock_Group*>>* lines;

		switch (direction)
		{
		case FLE_DOCK_TOP:
			lines = &m_topLines;
			break;
		case FLE_DOCK_RIGHT:
			lines = &m_rightLines;
			break;
		case FLE_DOCK_BOTTOM:
			lines = &m_bottomLines;
			break;
		case FLE_DOCK_LEFT:
			lines = &m_leftLines;
			break;
		default:
			return false;
		}

		for (std::list<Fle_Dock_Group*>& line : *lines)
		{
			for (Fle_Dock_Group* dg : line)
			{
				if (dg->flexible() && dg->get_size() > 0) return true;
			}
		}

		return false;
	}

	return false;
}

void Fle_Dock_Host::detach(Fle_Dock_Group* group, bool addToDetached)
{
	// if addToDetached == true then we're just hiding the group

	std::list<std::list<Fle_Dock_Group*>>* direction;

	bool isVertical = true;
	bool shouldEraseLines = true;

	switch (group->get_direction())
	{
	case FLE_DOCK_TOP:
		direction = &m_topLines;
		isVertical = false;
		break;
	case FLE_DOCK_RIGHT:
		direction = &m_rightLines;
		break;
	case FLE_DOCK_BOTTOM:
		direction = &m_bottomLines;
		isVertical = false;
		break;
	case FLE_DOCK_LEFT:
		direction = &m_leftLines;
		break;
	}

	// Find the line
	std::list<Fle_Dock_Group*>* line = nullptr;
	for (std::list<std::list<Fle_Dock_Group*>>::iterator it = direction->begin(); it != direction->end(); it++)
	{
		if (std::find(it->begin(), it->end(), group) != it->end())
		{
			line = &(*it);
		}
	}

	// Try resizing neighbours, if any

	if(line->size() > 1)
	{
		Fle_Dock_Group* n;

		if (group == *line->begin())
		{
			n = *(std::next(line->begin()));

			if (isVertical)
			{
				n->position(n->x(), n->y() - group->get_size());
			}
			else
				n->position(n->x() - group->get_size(), n->y());
			
		}
		else
		{
			n = *(std::prev(std::find(line->begin(), line->end(), group)));
		}

		n->set_size(n->get_size() + group->get_size());

		shouldEraseLines = false;
	}

	line->erase(std::find(line->begin(), line->end(), group));

	remove(group);

	if(shouldEraseLines)
		remove_empty_lines(group->get_direction());

	if(addToDetached)
		m_detachedGroups.push_back(group);
	calculate_min_size();
	find_edges();
}

void Fle_Dock_Host::set_work_widget_min_size(int W, int H)
{
	m_workWidgetMinW = W;
	m_workWidgetMinH = H;

	calculate_min_size();
}

void Fle_Dock_Host::hide_group(Fle_Dock_Group* group)
{
	if (group->detached())
	{
		m_detachedGroups.erase(std::find(m_detachedGroups.begin(), m_detachedGroups.end(), group));
	}
	else
	{
		detach(group, false);
	}

	m_hiddenGroups.push_back(group);
}

void Fle_Dock_Host::show_group(Fle_Dock_Group* group)
{
	if(group->hidden())
	{
		m_hiddenGroups.erase(std::find(m_hiddenGroups.begin(), m_hiddenGroups.end(), group));

		group->m_state &= ~FLE_DOCK_HIDDEN;

		if (group->detached())
		{
			group->create_detached_window();
			add_dock_group(group, 0, 0);
		}
		else
			add_dock_group(group, group->get_direction(), 0);
	}
}

void Fle_Dock_Host::resize_direction(std::list<std::list<Fle_Dock_Group*>>* lines, int resizeDelta, int resizeDeltaSecondary)
{
	for (std::list<std::list<Fle_Dock_Group*>>::iterator it = lines->begin(); it != lines->end(); it++)
	{
		int deltaCopy = resizeDelta;
		if (deltaCopy > 0)
		{
			for (std::list<Fle_Dock_Group*>::iterator ita = it->begin(); ita != it->end(); ita++)
			{
				if (deltaCopy <= 0) break;

				Fle_Dock_Group* g = *ita;

				int deltaPref = g->get_preferred_size() - g->get_size();
				int oldSize = g->get_size();
				if (ita != std::prev(it->end()) && deltaCopy > deltaPref)
				{
					g->set_size(g->get_preferred_size());
				}
				else
				{
					g->set_size(g->get_size() + deltaCopy);
				}
				// If this group was scaled up towards its preferred size, move
				// groups after it
				if (oldSize != g->get_size() && ita != it->end())
				{
					for (std::list<Fle_Dock_Group*>::iterator itb = std::next(ita); itb != it->end(); itb++)
					{
						if(lines == &m_topLines || lines == &m_bottomLines)
						{
							(*itb)->position((*itb)->x() + (g->get_size() - oldSize), (*itb)->y());
						}
						else
						{
							(*itb)->position((*itb)->x(), (*itb)->y() + (g->get_size() - oldSize));
						}
					}
				}
				deltaCopy -= deltaPref;
			}
		}
		else if (deltaCopy < 0)
		{
			for (std::list<Fle_Dock_Group*>::reverse_iterator ita = it->rbegin(); ita != it->rend(); ita++)
			{
				if (deltaCopy >= 0) break;

				Fle_Dock_Group* g = *ita;

				int deltaMin = g->get_size() - g->get_min_size();
				int oldSize = g->get_size();
				if (std::abs(deltaCopy) > deltaMin)
				{
					g->set_size(g->get_min_size());
				}
				else
				{
					g->set_size(g->get_size() + deltaCopy);
				}
				// If this group was scaled down towards its min size, move
				// groups after it
				if (oldSize != g->get_size() && ita != it->rbegin())
				{
					for (std::list<Fle_Dock_Group*>::iterator itb = ita.base(); itb != it->end(); itb++)
					{
						if (lines == &m_topLines || lines == &m_bottomLines)
						{
							(*itb)->position((*itb)->x() - (oldSize - g->get_size()), (*itb)->y());
						}
						else
						{
							(*itb)->position((*itb)->x(), (*itb)->y() - (oldSize - g->get_size()));
						}
					}
				}
				deltaCopy += deltaMin;
			}
		}
		if (resizeDeltaSecondary != 0)
		{
			for (std::list<Fle_Dock_Group*>::iterator ita = it->begin(); ita != it->end(); ita++)
			{
				Fle_Dock_Group* g = *ita;
				if (lines == &m_bottomLines)
				{
					g->position(g->x(), g->y() + resizeDeltaSecondary);
				}
				else
					g->position(g->x() + resizeDeltaSecondary, g->y());
			}
		}
	}
}

void Fle_Dock_Host::set_min_size_callback(const std::function<void(Fle_Dock_Host* host, int, int)>& cb)
{
	m_minSizeCallback = cb;
}

void Fle_Dock_Host::set_preview_color(const Fl_Color& color)
{
	m_previewColor = color;
}

void Fle_Dock_Host::detached_drag(Fle_Dock_Group* group, int screenX, int screenY)
{
	// This gets called when a detached group is dragged, so that
	// we can calculate where to display the preview

	// this very specific combination means clear preview
	if (screenX == 2121420 && screenY == -1)
	{
		m_previewW = 0;
		m_previewH = 0;
		redraw();

		return;
	}

	if (!is_host_visible_at(screenX, screenY))
		return;

	// First check if the coords are outside this window
	if (screenX < window()->x() || screenX >= window()->x() + window()->w() || screenY < window()->y() || screenY >= window()->y() + window()->h())
	{
		if (m_previewH != 0 && m_previewW != 0)
		{
			m_previewW = 0;
			m_previewH = 0;

			redraw();
		}
		m_previewX = 0;
		m_previewY = 0;
		m_previewW = 0;
		m_previewH = 0;
		return;
	}

	// Now find where the group would be attached if user would
	// release it now
	// TODO: implement

	m_previewX = 0;
	m_previewY = 0;
	m_previewW = 100;
	m_previewH = 100;

	redraw();
}

void Fle_Dock_Host::calculate_min_size()
{
	int minW = 0;
	int minH = 0;

	// First calculate W using sizes and H using breadths
	for (std::list<std::list<Fle_Dock_Group*>>::iterator it = m_topLines.begin(); it != m_topLines.end(); it++)
	{
		int cW = 0;
		minH += (*it->begin())->get_breadth();
		for (Fle_Dock_Group* g : *it)
		{
			cW += g->get_min_size();
		}
		if (cW > minW) minW = cW;
	}
	for (std::list<std::list<Fle_Dock_Group*>>::iterator it = m_bottomLines.begin(); it != m_bottomLines.end(); it++)
	{
		int cW = 0;
		minH += (*it->begin())->get_breadth();
		for (Fle_Dock_Group* g : *it)
		{
			cW += g->get_min_size();
		}
		if (cW > minW) minW = cW;
	}

	// Next calculate W using breadths and H using sizes, making sure to add top and bottom breadth
	int cW = 0;
	for (std::list<std::list<Fle_Dock_Group*>>::iterator it = m_leftLines.begin(); it != m_leftLines.end(); it++)
	{
		int cH = 0;
		cW += (*it->begin())->get_breadth();
		for (Fle_Dock_Group* g : *it)
		{
			cH += g->get_min_size();
		}
		cH += m_breadthTop;
		cH += m_breadthBottom;
		if (cH > minH) minH = cH;
	}
	if (cW > minW) minW = cW;
	cW = 0;
	for (std::list<std::list<Fle_Dock_Group*>>::iterator it = m_rightLines.begin(); it != m_rightLines.end(); it++)
	{
		int cH = 0;
		cW += (*it->begin())->get_breadth();
		for (Fle_Dock_Group* g : *it)
		{
			cH += g->get_min_size();
		}
		cH += m_breadthTop;
		cH += m_breadthBottom;
		if (cH > minH) minH = cH;
	}
	if (cW > minW) minW = cW;

	//if (minW < m_workWidgetMinW) minW = m_workWidgetMinW;
	if (minH < m_workWidgetMinH) minH = m_workWidgetMinH;

	minW += m_workWidgetMinW;
	//minH += m_workWidgetMinH;

	if(m_minSizeCallback != nullptr && (m_oldMinW != minW || m_oldMinW != minH))
		m_minSizeCallback(this, minW, minH);

	m_oldMinW = minW;
	m_oldMinH = minH;
}

void Fle_Dock_Host::line_reset_preferred_sizes(std::list<Fle_Dock_Group*>* line)
{
	for (Fle_Dock_Group* group : *line)
	{
		group->update_preferred_size();
	}
}

// Returns true if managed to attach
int Fle_Dock_Host::try_attach(Fle_Dock_Group* group, int screenX, int screenY, bool force)
{
	// force argument is used to attach a non-detachable group even if all else fails

	if (!force && !is_host_visible_at(screenX, screenY))
		return 0;

	// Check if the coords are outside this window
	if (!force && (screenX < window()->x() || screenX >= window()->x() + window()->w() || screenY < window()->y() || screenY >= window()->y() + window()->h()))
	{
		return 0;
	}

	int X = screenX - window()->x();
	int Y = screenY - window()->y();

	// A group can be attached several ways:
	// 1. In a new line added to the end of a direction
	// 2. In a new line added between existing lines
	// 3. To a existing line:
	//    - there must be enough space in the line so that
	//      when the line is squeezed nothing will overlap
	//    - the line must have the same flexible status
	//    - if that fails, don't attach

	// TODO: check if there is enough space to add a new line
	// TODO: placing of groups in existing lines as close to the coords as possible, squshing other groups

	int addedToDirection = 0;
	bool needCalcMinSize = false;

	// Case 1:
	if (group->get_allowed_directions() & FLE_DOCK_TOP && Y > y() + m_breadthTop && Y < y() + m_breadthTop + group->get_min_breadth())
	{
		add_dock_group(group, FLE_DOCK_TOP, 1);
		addedToDirection = FLE_DOCK_TOP;
	}
	if (addedToDirection == 0 && group->get_allowed_directions() & FLE_DOCK_BOTTOM && Y > y() + h() - m_breadthBottom - group->get_min_breadth() && Y < y() + h() - m_breadthBottom)
	{
		add_dock_group(group, FLE_DOCK_BOTTOM, 1);
		addedToDirection = FLE_DOCK_BOTTOM;
	}
	if (addedToDirection == 0 && group->get_allowed_directions() & FLE_DOCK_LEFT && X > x() + m_breadthLeft && X < x() + m_breadthLeft + group->get_min_breadth())
	{
		add_dock_group(group, FLE_DOCK_LEFT, 1);
		addedToDirection = FLE_DOCK_LEFT;
	}
	if (addedToDirection == 0 && group->get_allowed_directions() & FLE_DOCK_RIGHT && X > x() + w() - m_breadthRight - group->get_min_breadth() && X < x() + w() - m_breadthRight)
	{
		add_dock_group(group, FLE_DOCK_RIGHT, 1);
		addedToDirection = FLE_DOCK_RIGHT;
	}

	// Case 2 and 3 are handled in a different function
	if (addedToDirection == 0) addedToDirection = try_attach_case2n3(FLE_DOCK_TOP, group, X, Y, needCalcMinSize);
	if (addedToDirection == 0) addedToDirection = try_attach_case2n3(FLE_DOCK_BOTTOM, group, X, Y, needCalcMinSize);
	if (addedToDirection == 0) addedToDirection = try_attach_case2n3(FLE_DOCK_LEFT, group, X, Y, needCalcMinSize);
	if (addedToDirection == 0) addedToDirection = try_attach_case2n3(FLE_DOCK_RIGHT, group, X, Y, needCalcMinSize);

	// If the group is non-detachable but attaching at those coords failed,
	// attach it forcefully to some other line in the groups direction.
	// Non detachable groups don't clear their m_direction on detaching like
	// detachable ones do.
	if (force && addedToDirection == 0)
	{
		addedToDirection = group->get_direction();
		add_dock_group(group, addedToDirection, 0);
	}

	if (addedToDirection != 0)
	{
		// Remove the group from the detached list
		m_detachedGroups.erase(std::find(m_detachedGroups.begin(), m_detachedGroups.end(), group));
		group->m_direction = addedToDirection;
		//init_sizes();
		find_edges();
		if (needCalcMinSize)
			calculate_min_size();
	}

	// Clear preview
	m_previewX = 0;
	m_previewY = 0;
	m_previewW = 0;
	m_previewH = 0;
	redraw();

	return addedToDirection;
}

int Fle_Dock_Host::try_attach_case2n3(int direction, Fle_Dock_Group* group, int X, int Y, bool& needCalcMinSize)
{
	std::list<std::list<Fle_Dock_Group*>>* lines = nullptr;
	bool isVertical = false;
	switch (direction)
	{
	case FLE_DOCK_TOP:
		lines = &m_topLines;
		break;
	case FLE_DOCK_RIGHT:
		lines = &m_rightLines;
		isVertical = true;
		break;
	case FLE_DOCK_BOTTOM:
		lines = &m_bottomLines;
		break;
	case FLE_DOCK_LEFT:
		lines = &m_leftLines;
		isVertical = true;
		break;
	}

	if (lines->empty())
	{
		return 0;
	}

	int addedToDirection = 0;
	int lineCounter = 0; //case 2
	std::list<Fle_Dock_Group*>* line = nullptr; // case 3

	for (std::list<std::list<Fle_Dock_Group*>>::iterator it = lines->begin(); it != lines->end(); it++)
	{
		if(it != std::prev(lines->end()))
		{
			int groupGripDist = 0;

			switch (direction)
			{
			case FLE_DOCK_TOP:
				groupGripDist = std::abs(((*it->begin())->y() + (*it->begin())->get_breadth()) - Y);
				break;
			case FLE_DOCK_BOTTOM:
				groupGripDist = std::abs(((*it->begin())->y() - Y));
				break;
			case FLE_DOCK_RIGHT:
				groupGripDist = std::abs(((*it->begin())->x() - X));
				break;
			case FLE_DOCK_LEFT:
				groupGripDist = std::abs(((*it->begin())->x() + (*it->begin())->get_breadth() - X));
				break;
			}
			// handle case 2 first
			if (groupGripDist <= Fle_GRIP_SIZE)
			{
				addedToDirection = direction;
				add_dock_group(group, direction, lineCounter + 2);
				break;
			}
			lineCounter++;
		}

		if ((isVertical && (X > (*it->begin())->x() && X < (*it->begin())->x() + (*it->begin())->get_breadth())) ||
			(!isVertical && (Y > (*it->begin())->y() && Y < (*it->begin())->y() + (*it->begin())->get_breadth())))
		{
			line = &(*it);
		}
	}

	// if line != nullptr then case 2 was N/A
	if (addedToDirection == 0 && line != nullptr && group->flexible() == (*line->begin())->flexible() && group->get_min_breadth() <= (*line->begin())->get_breadth())
	{
		int availableSpaceInLine = w();

		if (isVertical)
		{
			availableSpaceInLine = h() - m_breadthTop - m_breadthBottom;
		}

		for (Fle_Dock_Group* group : *line)
		{
			availableSpaceInLine -= group->get_min_size();
		}

		if (availableSpaceInLine >= group->get_min_size())
		{
			// Find the group that currently resides at those coords
			// and check if the new group should be attached before or
			// after it by comparing the coords to the middle of the group
			// and finally attach and squeeze the line
			for (std::list<Fle_Dock_Group*>::iterator it = line->begin(); it != line->end(); it++)
			{
				Fle_Dock_Group* g = *it;
				Fle_Dock_Group* otherGroup = nullptr;
				if ((!isVertical && X > g->x() && X < g->x() + (g->get_min_size() / 2)) ||
					(isVertical && Y > g->y() && Y < g->y() + (g->get_min_size() / 2)))
				{
					// Add before this group
					addedToDirection = direction;
					line->insert(it, group);
					otherGroup = *std::prev(line->end());
				}
				else if ((!isVertical && X < g->x() + g->w()) ||
						  (isVertical && Y < g->y() + g->h()))
				{
					// Add after this group
					addedToDirection = direction;
					line->insert(std::next(it), group);
					otherGroup = *line->begin();
				}
				if (addedToDirection != 0)
				{
					add(group);
					if (isVertical)
					{
						group->resize(otherGroup->x(), 0, otherGroup->get_breadth(), group->get_min_size());
					}
					else
						group->resize(0, otherGroup->y(), group->get_min_size(), otherGroup->get_breadth());
					squeeze_groups_in_line(line, isVertical);
					needCalcMinSize = true;
					break;
				}
			}
		}
	}

	return addedToDirection;
}

inline Fle_Edge_Helper::Fle_Edge_Helper(int X, int Y, int L, int V)
{
	x = X;
	y = Y;
	l = L;
	v = V;

	group_a = nullptr;
	group_b = nullptr;
	direction = nullptr;
}

inline Fle_Edge_Helper::Fle_Edge_Helper(int X, int Y, int L, int V, Fle_Dock_Group* A, Fle_Dock_Group* B)
{
	x = X;
	y = Y;
	l = L;
	v = V;
	group_a = A;
	group_b = B;
	direction = nullptr;
}

inline Fle_Edge_Helper::Fle_Edge_Helper(int X, int Y, int L, int V, std::list<std::list<Fle_Dock_Group*>>::iterator IT, std::list<std::list<Fle_Dock_Group*>>* d)
{
	x = X;
	y = Y;
	l = L;
	v = V;
	line_it = IT;
	direction = d;

	group_a = nullptr;
	group_b = nullptr;
}