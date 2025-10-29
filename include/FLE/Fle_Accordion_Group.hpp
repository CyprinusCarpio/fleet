#ifndef FLE_ACCORDION_GROUP_H
#define FLE_ACCORDION_GROUP_H

#include <FL/Fl_Group.H>

#include <FLE/Fle_Accordion.hpp>
#include <FLE/Fle_Flat_Button.hpp>

/// \class Fle_Accordion_Group
/// \brief An accordion group widget.
///
/// \par Callbacks:
/// If when() is not 0, the following callbacks are called:
/// - FL_REASON_OPENED: Called when the group is opened
/// - FL_REASON_CLOSED: Called when the group is closed
class Fle_Accordion_Group : public Fl_Group 
{
    bool m_open; ///< True if the group is open
    int m_minH; ///< The minimum height of the group
    int m_maxH; ///< The maximum height of the group
    Fl_Widget* m_child; ///< The child widget
    Fle_Flat_Button m_expandButton; ///< The expand button

    /// Returns the parent accordion
    /// \return The parent accordion
    Fle_Accordion* get_accordion() const { return (Fle_Accordion*)parent()->parent(); }

    /// Callback function for the expand button
    static void expand_button_cb(Fl_Widget* w, void* d);

protected:
    void draw() override;

public:
    /// @brief  Constructor for Fle_Accordion_Group
    /// @param C Child widget
    /// @param minH Minimum height
    /// @param maxH Maximum height
    /// @param l Label
    Fle_Accordion_Group(Fl_Widget* C, int minH, int maxH, const char* l);
    ~Fle_Accordion_Group();

    /// Opens the group
    void open();
    /// Closes the group
    void close();

    /// Checks if the group is open
    /// \return True if the group is open
    bool is_open() const { return m_open; }
    /// Returns the minimum height of the group
    /// \return The minimum height
    int min_height() const { return m_minH; }
    /// Sets the minimum height of the group
    /// \param height The new minimum height
    void min_height(int height); 
    /// Returns the maximum height of the group
    /// \return The maximum height
    int max_height() const { return m_maxH; }
    /// Sets the maximum height of the group
    /// \param height The new maximum height
    void max_height(int height);

    void resize(int X, int Y, int W, int H) override;
    int handle(int e) override;
};

#endif // FLE_ACCORDION_GROUP_H