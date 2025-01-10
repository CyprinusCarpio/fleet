#include <FLE/Fle_Dock_Group.hpp>

#include <FL/fl_draw.H>
#include <FL/Fl_Menu.H>

#include <FLE/Fle_Dock_Host.hpp>

#define PREVIEW_TIMEOUT 0.25f

#include <iostream>

std::string Fle_Dock_Group::m_closeText = u8"Close group";
std::string Fle_Dock_Group::m_pinText = u8"Pin";
std::string Fle_Dock_Group::m_unpinText = u8"Unpin";

// static callback function
void Fle_Dock_Group::decoration_cb(Fl_Widget* widget, void* data)
{
	Fle_Dock_Group* g = (Fle_Dock_Group*)widget->parent();

	if (data == (void*)0) // Close
	{
		g->hide_group();
	}
	else if(data == (void*)1) // Pin/Unpin
	{
		g->locked(!g->locked());
	}
}

void Fle_Dock_Group::preview_timeout_cb(void* data)
{
	Fle_Dock_Group* group = (Fle_Dock_Group*)data;

	if (!group->detached()) return;

	group->m_host->detached_drag(group, Fl::event_x_root(), Fl::event_y_root());

	group->m_previewTimeoutActive = false;
}

// private member functions
void Fle_Dock_Group::position_everything()
{
	// Decoration buttons are not present on the horizontal group
	if (m_state & FLE_DOCK_VERTICAL && m_closeButton && m_pinButton)
	{
		m_closeButton->resize(x() + w() - 17, y() + 2, 16, 16);
		m_pinButton->resize(x() + w() - 35, y() + 2, 16, 16);
	}
	if (m_bandWidget)
	{
		if (m_state & FLE_DOCK_VERTICAL)
		{
			m_bandWidget->resize(x() + m_bandOffsetX, y() + m_decorationSize + m_bandOffsetY, w() - m_bandOffsetX - m_bandOffsetW, h() - m_decorationSize - m_bandOffsetY - m_bandOffsetH);
		}
		else
		{
			int xPos = x() + m_decorationSize + get_label_width() + m_bandOffsetX + 3;
			if (!detached() && m_state & FLE_DOCK_NO_HOR_LABEL) xPos -= 3;

			m_bandWidget->resize(xPos, y() + m_bandOffsetY, w() - m_decorationSize - m_bandOffsetW - m_bandOffsetX - get_label_width() - 3, h() - m_bandOffsetH - m_bandOffsetY);
		}
	}

	redraw();
}

int Fle_Dock_Group::get_label_width()
{
	if (m_state & FLE_DOCK_HORIZONTAL && m_state & FLE_DOCK_NO_HOR_LABEL && !detached())
		return 0;

	fl_font(FL_HELVETICA, 14);
	int lx = 0, ly;
	fl_measure(label(), lx, ly);

	return lx;
}

void Fle_Dock_Group::try_attach_to_host(int X, int Y)
{
	int addedToDirection = m_host->try_attach(this, X, Y, !detachable());

	if (addedToDirection != 0)
	{
		// TODO: should dock host remove the group from the detached window?
		Fl::remove_timeout(Fle_Dock_Group::preview_timeout_cb, this);
		m_detachedWindow->hide();
		delete m_detachedWindow;
		m_detachedWindow = nullptr;

		if (m_state & FLE_DOCK_VERTICAL)
		{
			m_pinButton->activate();
		}

		m_state &= ~FLE_DOCK_DETACHED;

		position_everything();
	}
}

void Fle_Dock_Group::create_detached_window()
{
	if (m_state & FLE_DOCK_VERTICAL)
	{
		m_detachedWindow = new Fl_Double_Window(get_breadth(), get_size(), label());
		m_detachedWindow->end();
		m_detachedWindow->add(this);
		m_detachedWindow->resizable(this);
		m_detachedWindow->size_range(flexible() ? get_min_breadth() : get_breadth(), get_min_size());
		m_pinButton->deactivate();
		m_detachedWindow->position(80, 200);
	}
	if (m_state & FLE_DOCK_HORIZONTAL)
	{
		m_detachedWindow = new Fl_Double_Window(get_size(), get_breadth(), label());
		m_detachedWindow->end();
		m_detachedWindow->add(this);
		m_detachedWindow->resizable(this);
		m_detachedWindow->size_range(get_min_size(), flexible() ? get_min_breadth() : get_breadth());
		m_detachedWindow->position(380, 200);
	}

	resize(0, 0, m_detachedWindow->w(), m_detachedWindow->h());
	m_detachedWindow->clear_border();
	m_detachedWindow->set_non_modal();
	m_detachedWindow->show();
}

void Fle_Dock_Group::detach(int X, int Y)
{
	m_host->detach(this);
	create_detached_window();
	m_state |= FLE_DOCK_DETACHED;
	position_everything();
	if(detachable())
		m_direction = 0;
	m_detaching = false;
	m_detachedWindow->position(X - m_offsetX, Y - m_offsetY);

	m_movingGroup = true;
	m_detachedWindow->cursor(FL_CURSOR_MOVE);
	m_offsetX = m_detachedWindow->x() - X;
	m_offsetY = m_detachedWindow->y() - Y;
}

void Fle_Dock_Group::update_preferred_size()
{
	m_preferredSize = get_size();
}

// Constructor and destructor

Fle_Dock_Group::Fle_Dock_Group(Fle_Dock_Host* host, const char* l, int state, int direction, int allowedDirections, int minSize, int breadth, bool newline) : Fl_Group(0, 0, 0, 0, l)
{
	// TODO: parameter validation
	// TODO: clean up argument order
	// TODO: allow custom decoration breadth and appearance via a virtual method
	m_host = host;

	m_closeButton = nullptr;
	m_pinButton = nullptr;
	m_bandWidget = nullptr;
	m_detachedWindow = nullptr;

	m_state = state;
	m_minSize = minSize;
	m_preferredSize = minSize;

	m_direction = direction;
	m_allowedDirections = allowedDirections;

	m_bandOffsetX = 0;
	m_bandOffsetY = 0;
	m_bandOffsetW = 0;
	m_bandOffsetH = 0;

	m_offsetX = 0;
	m_offsetY = 0;
	m_movingGroup = false;
	m_resizingTop = false;
	m_resizingRight = false;
	m_resizingBottom = false;
	m_resizingLeft = false;
	m_detaching = false;
	m_previewTimeoutActive = false;

	box(FL_THIN_UP_BOX);
	color(FL_BACKGROUND_COLOR);
	labeltype(FL_NO_LABEL);

	m_minBreadth = breadth;

	if(m_state & FLE_DOCK_VERTICAL)
	{
		m_decorationSize = 20;
		size(breadth, m_minSize);
		m_closeButton = new Fle_Flat_Button(0, 0, 15, 15, "@-31+");
		m_closeButton->labelsize(10);
		m_closeButton->callback((Fl_Callback*)Fle_Dock_Group::decoration_cb, (void*)0);
		m_closeButton->tooltip(m_closeText.c_str());
		m_pinButton = new Fle_Flat_Button(0, 0, 15, 15, locked() ? "@-28->" : "@-22->");
		m_pinButton->callback((Fl_Callback*)Fle_Dock_Group::decoration_cb, (void*)1);
		m_pinButton->tooltip(locked() ? m_unpinText.c_str() : m_pinText.c_str());
	}
	else
	{
		m_decorationSize = 9;
		size(m_minSize, breadth);
	}

	// If the dock group is created detached, create the window
	if (detached())
	{
		create_detached_window();

		host->add_dock_group(this, 0, 0); // 0 means add to the detached groups vector
	}
	else
	{
		host->add_dock_group(this, m_direction, newline);
	}

	end(); // child widgets cannot be added normally
}

Fle_Dock_Group::~Fle_Dock_Group()
{
}

// FLTK overrides

void Fle_Dock_Group::resize(int X, int Y, int W, int H)
{
	Fl_Widget::resize(X, Y, W, H);

	position_everything();
}

int Fle_Dock_Group::handle(int e)
{
	// First let the band widget and decoration buttons process the event
	int ret = Fl_Group::handle(e);

	// Popup menu
	if (e == FL_PUSH && Fl::event_button() == FL_RIGHT_MOUSE)
	{
		Fl_Menu_Item menu[] =
		{
			{Fle_Dock_Group::m_closeText.c_str(), 0, this->decoration_cb, (void*)0},
			{m_state & FLE_DOCK_LOCKED ? Fle_Dock_Group::m_unpinText.c_str() : Fle_Dock_Group::m_pinText.c_str(), 0, this->decoration_cb, (void*)1},
			{0}
		};

		if (detached())
			menu[1].deactivate();

		bool shouldShowMenu = false;
		if (m_state & FLE_DOCK_VERTICAL && Fl::event_y() <= y() + m_decorationSize && Fl::event_x() <= x() + w() - 38)
		{
			shouldShowMenu = true;
		}
		else if (m_state & FLE_DOCK_HORIZONTAL && Fl::event_x() <= x() + m_decorationSize + get_label_width())
		{
			shouldShowMenu = true;
		}
		if (shouldShowMenu)
		{
			const Fl_Menu_Item* choice = menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
			if (choice) switch (choice->argument())
			{
			case 0:
				hide_group();
				break;
			case 1:
				locked(!locked());
				break;
			}
		}
		return 1;
	}

	if (e == FL_PUSH && Fl::event_button() == FL_LEFT_MOUSE)
	{
		m_offsetX = 0;
		m_offsetY = 0;

		// If detached, store screen coords
		if (detached())
		{
			// Dragging by the decoration
			if (m_state & FLE_DOCK_VERTICAL && Fl::event_y() <= m_decorationSize && Fl::event_x() <= w() - 38)
			{
				m_movingGroup = true;
				m_detachedWindow->cursor(FL_CURSOR_MOVE);
				m_offsetX = m_detachedWindow->x() - Fl::event_x_root();
				m_offsetY = m_detachedWindow->y() - Fl::event_y_root();
			}
			else if (m_state & FLE_DOCK_HORIZONTAL && Fl::event_x() <= m_decorationSize)
			{
				m_movingGroup = true;
				m_detachedWindow->cursor(FL_CURSOR_MOVE);
				m_offsetX = m_detachedWindow->x() - Fl::event_x_root();
				m_offsetY = m_detachedWindow->y() - Fl::event_y_root();
			}
			// Vertical groups can't be resized grabbing by the top edge,
			// and horizontal ones by the left edge.
			else if (flexible() && m_state & FLE_DOCK_HORIZONTAL && Fl::event_y() <= 4)
			{
				m_resizingTop = true;
				m_detachedWindow->cursor(FL_CURSOR_S);
				m_offsetX = Fl::event_x_root();
				m_offsetY = Fl::event_y_root();
			}
			else if (Fl::event_y() >= h() - 4 && ((m_state & FLE_DOCK_HORIZONTAL && flexible()) || m_state & FLE_DOCK_VERTICAL))
			{
				m_resizingBottom = true;
				m_detachedWindow->cursor(FL_CURSOR_S);
				m_offsetX = Fl::event_x_root();
				m_offsetY = Fl::event_y_root();
			}
			else if (flexible() && m_state & FLE_DOCK_VERTICAL && Fl::event_x() < 4)
			{
				m_resizingLeft = true;
				m_detachedWindow->cursor(FL_CURSOR_W);
				m_offsetX = Fl::event_x_root();
				m_offsetY = Fl::event_y_root();
			}
			else if (Fl::event_x() > w() - 4 && ((m_state & FLE_DOCK_VERTICAL && flexible()) || m_state & FLE_DOCK_HORIZONTAL))
			{
				m_resizingRight = true;
				m_detachedWindow->cursor(FL_CURSOR_E);
				m_offsetX = Fl::event_x_root();
				m_offsetY = Fl::event_y_root();
			}

			if (m_offsetX != 0 || m_offsetY != 0)
				return 1;
		}
		// If attached, store window coords
		if(!locked())
		{
			if ((m_state & FLE_DOCK_VERTICAL && Fl::event_y() <= y() + m_decorationSize && Fl::event_x() <= x() + w() - 38) ||
				(m_state & FLE_DOCK_HORIZONTAL && Fl::event_x() <= x() + m_decorationSize))
			{
				m_detaching = true;
				m_offsetX = Fl::event_x() - x();
				m_offsetY = Fl::event_y() - y();
				window()->cursor(FL_CURSOR_MOVE);
				return 1;
			}
		}
	}
	if (e == FL_RELEASE && Fl::event_button() == FL_LEFT_MOUSE)
	{
		if (detached())
		{
			m_detachedWindow->cursor(FL_CURSOR_DEFAULT);
			// Released from dragging, try to attach to a host
			if (m_movingGroup)
			{
				m_movingGroup = false;
				
				// Don't try attaching to host if less than PREVIEW_TIMEOUT seconds
				// have elapsed since last mouse movement
				if(!m_previewTimeoutActive)
				{
					try_attach_to_host(Fl::event_x_root(), Fl::event_y_root());
				}
				else
				{
					m_previewTimeoutActive = false;
					Fl::remove_timeout(Fle_Dock_Group::preview_timeout_cb, this);

					// If the group is not detachable, it cannot stay detached
					if (!detachable())
					{
						try_attach_to_host(Fl::event_x_root(), Fl::event_y_root());
					}
				}
			}
		}
		if (m_offsetX != 0 || m_offsetY != 0)
		{
			m_offsetX = 0;
			m_offsetY = 0;
			m_resizingTop = false;
			m_resizingRight = false;
			m_resizingBottom = false;
			m_resizingLeft = false;
			m_movingGroup = false;
			m_detaching = false;
		}
		return 1;
	}
	// TODO: make resizing detached groups more consistent with WM behaviour - diagonal resizing, etc
	if (e == FL_DRAG)
	{
		// If detached, simply move/resize parent window
		if (detached())
		{
			// First, check if we are resizing as opposed to moving
			bool resized = false;
			if (m_resizingTop)
			{
				m_detachedWindow->resize(m_detachedWindow->x(), Fl::event_y_root(), m_detachedWindow->w(), m_detachedWindow->h() - (Fl::event_y_root() - m_offsetY));
				m_offsetX = Fl::event_x_root();
				m_offsetY = Fl::event_y_root();
				resized = true;
			}
			if (m_resizingBottom)
			{
				m_detachedWindow->resize(m_detachedWindow->x(), m_detachedWindow->y(), m_detachedWindow->w(), m_detachedWindow->h() + (Fl::event_y_root() - m_offsetY));
				m_offsetX = Fl::event_x_root();
				m_offsetY = Fl::event_y_root();
				resized = true;
			}
			else if (m_resizingLeft)
			{
				m_detachedWindow->resize(Fl::event_x_root(), m_detachedWindow->y(), m_detachedWindow->w() - (Fl::event_x_root() - m_offsetX), m_detachedWindow->h());
				m_offsetX = Fl::event_x_root();
				m_offsetY = Fl::event_y_root();
				resized = true;
			}
			else if (m_resizingRight)
			{
				m_detachedWindow->resize(m_detachedWindow->x(), m_detachedWindow->y(), m_detachedWindow->w() + (Fl::event_x_root() - m_offsetX), m_detachedWindow->h());
				m_offsetX = Fl::event_x_root();
				m_offsetY = Fl::event_y_root();
				resized = true;
			}

			if (resized)
			{
				// Make sure that the detached window didn't go below the minimal size
				if (m_state & FLE_DOCK_VERTICAL)
				{
					if (m_detachedWindow->w() < m_minBreadth)
						m_detachedWindow->size(m_minBreadth, m_detachedWindow->h());
					if (m_detachedWindow->h() < m_minSize)
						m_detachedWindow->size(m_detachedWindow->w(), m_minSize);
				}
				else
				{
					if (m_detachedWindow->w() < m_minSize)
						m_detachedWindow->size(m_minSize, m_detachedWindow->h());
					if (m_detachedWindow->h() < m_minBreadth)
						m_detachedWindow->size(m_detachedWindow->w(), m_minBreadth);
				}
				return 1;
			}

			if (m_movingGroup)
			{
				m_detachedWindow->position(m_offsetX + Fl::event_x_root(), m_offsetY + Fl::event_y_root());
				// If the preview is active, remove it so that we can set it again
				// so that the preview only appears if PREVIEW_TIMEOUT seconds have
				// elapsed since last mouse movement.
				if(m_previewTimeoutActive)
				{
					Fl::remove_timeout(Fle_Dock_Group::preview_timeout_cb, this);
				}
				else
					m_host->detached_drag(this, 2121420, -1);
				Fl::add_timeout(PREVIEW_TIMEOUT, Fle_Dock_Group::preview_timeout_cb, this);
				m_previewTimeoutActive = true;
			}
			return 1;
		}
		else if (m_detaching)
		{
			// See if user moved the pointer far enough to detach
			if (std::abs(m_offsetX - Fl::event_x() + x()) >= 6 || std::abs(m_offsetY - Fl::event_y() + y()) >= 6)
			{
				detach(Fl::event_x_root(), Fl::event_y_root());
				return 1;
			}
		}
	}
	
	return ret;
}
void Fle_Dock_Group::draw()
{
	Fl_Group::draw(); // draw buttons

	if (m_state & FLE_DOCK_VERTICAL)
	{
		fl_color(labelcolor());
		fl_font(FL_HELVETICA, 14);
		fl_draw(label(), x() + 6, y() + 13);

		if(!locked())
		{
			// Draw gripper dots
			for (int X = x() + 12 + get_label_width(); X <= x() + w() - 44; X += 12)
			{
				fl_draw_box(FL_GTK_THIN_UP_BOX, X, y() + 3, 6, 6, FL_BACKGROUND_COLOR);
				fl_draw_box(FL_GTK_THIN_UP_BOX, X + 5, y() + 11, 6, 6, FL_BACKGROUND_COLOR);
			}
		}
	}
	else
	{
		if (!locked())
		{
			fl_draw_box(FL_GTK_THIN_UP_FRAME, x() + 2, y() + 2, 3, h() - 5, FL_BACKGROUND_COLOR);
			fl_draw_box(FL_GTK_THIN_UP_FRAME, x() + 6, y() + 2, 3, h() - 5, FL_BACKGROUND_COLOR);
		}

		fl_color(FL_FOREGROUND_COLOR);
		fl_font(FL_HELVETICA, 14);
		if(detached() || (m_state & FLE_DOCK_NO_HOR_LABEL) == false)
		{
			fl_color(labelcolor());
			fl_draw(label(), x() + m_decorationSize + 3, y(), get_label_width() + 3, h(), FL_ALIGN_LEFT);
		}
	}

	m_bandWidget->draw();

}

// Fle_Dock_Group public member functions

// The oX, oY, oW, oH are relative offsets from the XYWH of the dock group client area.
// Client area = this widget area - decoration area.
bool Fle_Dock_Group::add_band_widget(Fl_Widget* widget, int oX, int oY, int oW, int oH)
{
	if (m_bandWidget != nullptr)
		return false;

	// Check for default args
	if (oX == -1) oX = m_state & FLE_DOCK_VERTICAL ? 4 : 0;
	if (oY == -1) oY = m_state & FLE_DOCK_VERTICAL ? 0 : 4;
	if (oW == -1) oW = 4;
	if (oH == -1) oH = 4;
	m_bandWidget = widget;
	// The band widget may touch the decoration
	m_bandOffsetX = std::max(oX, m_state & FLE_DOCK_VERTICAL ? 2 : 0);
	m_bandOffsetY = std::max(oY, m_state & FLE_DOCK_VERTICAL ? 0 : 2);
	m_bandOffsetW = std::max(oW, 2);
	m_bandOffsetH = std::max(oH, 2);
	add(m_bandWidget);
	position_everything();
	return true;
}

bool Fle_Dock_Group::flexible() const
{
	return m_state & FLE_DOCK_FLEXIBLE;
}

int Fle_Dock_Group::get_breadth() const
{
	return m_state & FLE_DOCK_VERTICAL ? w() : h();
}

int Fle_Dock_Group::get_min_size() const
{
	return m_minSize;
}

int Fle_Dock_Group::get_preferred_size() const
{
	return m_preferredSize;
}

int Fle_Dock_Group::get_min_breadth() const
{
	return m_minBreadth;
}

bool Fle_Dock_Group::set_size(int newsize)
{
	// If newsize is bigger than current size, allow setting it even
	// if it is smaller than minSize
	if (newsize <= get_size() && newsize < m_minSize)
		return false;
	if (m_state & FLE_DOCK_VERTICAL)
	{
		size(w(), newsize);
	}
	else
	{
		size(newsize, h());
	}

	return true;
}

bool Fle_Dock_Group::set_breadth(int newbreadth)
{
	if (m_state & FLE_DOCK_VERTICAL)
	{
		size(newbreadth, h());
	}
	else
	{
		size(w(), newbreadth);
	}

	return true;
}

int Fle_Dock_Group::get_size() const
{
	return m_state & FLE_DOCK_VERTICAL ? h() : w();
}

int Fle_Dock_Group::get_direction() const
{
	return m_direction;
}

int Fle_Dock_Group::get_allowed_directions() const
{
	return m_allowedDirections;
}

bool Fle_Dock_Group::hidden() const
{
	return m_state & FLE_DOCK_HIDDEN;
}

bool Fle_Dock_Group::detached() const
{
	return m_state & FLE_DOCK_DETACHED;
}

bool Fle_Dock_Group::detachable() const
{
	if (m_state & FLE_DOCK_LOCKED) return false;
	return m_state & FLE_DOCK_DETACHABLE;
}

bool Fle_Dock_Group::locked() const
{
	return m_state & FLE_DOCK_LOCKED;
}

void Fle_Dock_Group::locked(bool l)
{
	if (l)
	{
		m_state |= FLE_DOCK_LOCKED;
	}
	else
		m_state &= ~FLE_DOCK_LOCKED;


	if (m_state & FLE_DOCK_VERTICAL)
	{
		m_pinButton->label(locked() ? "@-28->" : "@-22->");
		m_pinButton->tooltip(locked() ? m_unpinText.c_str() : m_pinText.c_str());
	}

	redraw();
}

void Fle_Dock_Group::hide_group()
{
	if(!hidden())
	{
		m_state |= FLE_DOCK_HIDDEN;
		// If the group is detached, hide the window, but don't move the group
		// back into host
		if (detached())
		{
			m_detachedWindow->hide();
			Fl::remove_timeout(Fle_Dock_Group::preview_timeout_cb, this);
			m_host->detached_drag(this, 2121420, -1);
		}
		m_host->hide_group(this);
	}
}

void Fle_Dock_Group::show_group()
{
	if(hidden())
	{
		m_state &= ~FLE_DOCK_HIDDEN;

		if (detached())
		{
			m_host->add_dock_group(this, 0, 0);
			create_detached_window();
		}
		else
		{
			m_host->add_dock_group(this, get_direction(), 0);
		}
	}
}

Fle_Dock_Host* Fle_Dock_Group::get_host() const
{
	return m_host;
}
