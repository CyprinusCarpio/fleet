#ifndef FLE_ACCORDION_H
#define FLE_ACCORDION_H

#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>

class Fle_Accordion_Group;
/// \class Fle_Accordion
/// \brief An container widget commonly known as an accordion.
///
/// It contains a pack of groups. The groups can be individually opened or closed.
/// If the sum of heights of groups is greater than the height of the accordion,
/// a scrollbar will appear.
///
/// \par Callbacks:
/// Callbacks are handled by the individual groups.
/// \see Fle_Accordion_Group
///
/// \par Features:
/// - Single/multiple open mode can be changed with single_open()
/// - Groups can be added with add_group()
/// - Groups can be removed with remove_group()
/// - Groups can be inserted with insert_group()
/// - All groups can be closed with close_all()
class Fle_Accordion: public Fl_Scroll
{
    bool m_singleOpen;
    Fl_Pack m_pack;

public:
    /// CTOR
    Fle_Accordion(int X, int Y, int W, int H, const char* l);
    
    /// Fits the pack to the size of the accordion
    void fit_pack();
    /// Adds a group
    /// \param group The group to add
    void add_group(Fle_Accordion_Group* group);
    /// Inserts a group
    /// \param group The group to insert
    /// \param index The index
    void insert_group(Fle_Accordion_Group* group, int index);
    /// Removes a group
    /// \param group The group to remove
    void remove_group(Fle_Accordion_Group* group);
    /// Removes a group
    /// \param index The index
    void remove_group(int index);

    /// Closes all groups
    /// \param except The group, if any, not to close
    void close_all(Fle_Accordion_Group* except);

    /// Sets the single open mode  
    /// \param v True to enable single open mode
    void single_open(bool v) { m_singleOpen = v; }
    /// Checks if single open mode is enabled
    /// \return True if single open mode is enabled
    bool single_open() const { return m_singleOpen; }

    void resize(int X, int Y, int W, int H) override;
};

#endif