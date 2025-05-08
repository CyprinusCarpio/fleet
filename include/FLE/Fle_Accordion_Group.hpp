#ifndef FLE_ACCORDION_GROUP_H
#define FLE_ACCORDION_GROUP_H

#include <FL/Fl_Group.H>

#include <FLE/Fle_Accordion.hpp>
#include <FLE/Fle_Flat_Button.hpp>

class Fle_Accordion_Group : public Fl_Group 
{
    bool m_open;
    int m_minH;
    int m_maxH;
    Fl_Widget* m_child;
    Fle_Flat_Button m_expandButton;

    Fle_Accordion* get_accordion() const { return (Fle_Accordion*)parent()->parent(); }

    static void expand_button_cb(Fl_Widget* w, void* d);

protected:
    void draw() override;

public:
    Fle_Accordion_Group(Fl_Widget* C, int minH, int maxH, const char* l);
    ~Fle_Accordion_Group();

    void open();
    void close();

    bool is_open() const { return m_open; }
    int min_height() const { return m_minH; }   

    void resize(int X, int Y, int W, int H) override;
    int handle(int e) override;
};

#endif // FLE_ACCORDION_GROUP_H