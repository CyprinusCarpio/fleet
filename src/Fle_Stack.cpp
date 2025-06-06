#include <FLE/Fle_Stack.hpp>

#include <FL/Fl_Window.H>

#include <algorithm>

Fle_Stack::Fle_Stack(int X, int Y, int W, int H, Fle_Stack_Orientation direction) : Fl_Group(X, Y, W, H)
{
    m_orientation = direction;

    end();
}

void Fle_Stack::on_remove(int index)
{
    int removedSize = get_widget_actual_size(index);

    if(children() > 1)
    {
        bool spaceFilled = false;

        for(int i = index - 1; i >= 0; i--)
        {
            if(m_widgetMaxSizes[i] == 2147483647)
            {
                spaceFilled = true;
                set_widget_actual_size(i, get_widget_actual_size(i) + removedSize);
                m_widgetPreferredSizes[i] = get_widget_actual_size(i);
                break;
            }
        }
        if(!spaceFilled)
        {
            for(int i = index + 1; i < children(); i++)
            {
                if(m_widgetMaxSizes[i] == 2147483647)
                {
                    spaceFilled = true;
                    if(m_orientation == FLE_STACK_HORIZONTAL)
                    {
                        child(i)->resize(child(i)->x() - removedSize, child(i)->y(), child(i)->w() + removedSize, child(i)->h());
                    }
                    else
                    {
                        child(i)->resize(child(i)->x(), child(i)->y() - removedSize, child(i)->w(), child(i)->h() + removedSize);
                    }
                    m_widgetPreferredSizes[i] = get_widget_actual_size(i);
                    for(int j = i - 1; j >= 0; j--)
                    {
                        if(m_orientation == FLE_STACK_HORIZONTAL)
                        {
                            child(j)->resize(child(j)->x() - removedSize, child(j)->y(), child(j)->w(), child(j)->h());
                        }
                        else
                        {
                            child(j)->resize(child(j)->x(), child(j)->y() - removedSize, child(j)->w(), child(j)->h());
                        }
                    }
                    break;
                }
            }
        }
    }

    m_widgetMinSizes.erase(m_widgetMinSizes.begin() + index);
    m_widgetPreferredSizes.erase(m_widgetPreferredSizes.begin() + index);
    m_widgetMaxSizes.erase(m_widgetMaxSizes.begin() + index);

    Fl_Group::on_remove(index);
}

void Fle_Stack::position_children()
{
    if(children() == 0) return;

    int availableSpace = h();
    if(m_orientation == FLE_STACK_HORIZONTAL)
    {
        availableSpace = w();
    }

    bool needsSqueeze = false;
    int preferredSum = 0;

    for(int prefSize : m_widgetPreferredSizes)
    {
        preferredSum += prefSize;
    }

    if(preferredSum > availableSpace)
    {
        needsSqueeze = true;
    }
    int sum = 0;

    if(!needsSqueeze)
    {
        int lastFlexibleIndex = children();
        do
        {
            lastFlexibleIndex--;
        } while(m_widgetMaxSizes[lastFlexibleIndex] != 2147483647);

        for(int i = 0; i < children(); i++)
        {
            Fl_Widget* c = child(i);
            int size = m_widgetPreferredSizes[i];
            if(i == lastFlexibleIndex)
            {
                size = availableSpace - sum;
                for(int j = i + 1; j < children(); j++)
                {
                    size -= m_widgetPreferredSizes[j];
                }
            }
            
            if(m_orientation == FLE_STACK_HORIZONTAL)
            {
                c->resize(x() + sum, y(), size, h());
            }
            else
            {
                c->resize(x(), y() + sum, w(), size);
            }

            sum += size;
        }
    }
    else
    {
        int diff = preferredSum - availableSpace;

        for(int i = children() - 1; i >= 0; i--)
        {
            Fl_Widget* c = child(i);
            int change = m_widgetPreferredSizes[i] - m_widgetMinSizes[i];

            if(change > diff)
            {
                change = diff;
            }
            int size = m_widgetPreferredSizes[i] - change;
            if(m_orientation == FLE_STACK_HORIZONTAL)
            {
                c->resize(x() + w() - sum - size, y(), size, h());
            }
            else
            {
                c->resize(x(), y() + h() - sum - size, w(), size);
            }

            sum += size;
            diff -= change;
        }
    }
}

void Fle_Stack::update_preferred_sizes()
{
    for(int i = 0; i < children(); i++)
    {
        m_widgetPreferredSizes[i] = get_widget_actual_size(i);
    }
}

int Fle_Stack::get_widget_actual_size(int index)
{
    return m_orientation == FLE_STACK_HORIZONTAL ? child(index)->w() : child(index)->h();
}
void Fle_Stack::set_widget_actual_size(int index, int size)
{
    if(m_orientation == FLE_STACK_HORIZONTAL)
    {
        child(index)->resize(child(index)->x(), child(index)->y(), size, child(index)->h());
    }
    else
    {
        child(index)->resize(child(index)->x(), child(index)->y(), child(index)->w(), size);
    }
}

int Fle_Stack::handle(int e)
{
    static int offset = 0;
    static int dragging = -1;

    if(e == FL_ENTER)
    {
        return 1;
    }
    if(e == FL_MOVE)
    {
        for(int i = 0; i < children() - 1; i++)
        {
            if(m_orientation == FLE_STACK_HORIZONTAL)
            {
                if(std::abs(Fl::event_x() - (child(i)->x() + child(i)->w())) < 5)
                {
                    window()->cursor(FL_CURSOR_WE);
                    return 1;
                }
                else
                {
                    window()->cursor(FL_CURSOR_DEFAULT);
                }
            }
            else
            {
                if(std::abs(Fl::event_y() - (child(i)->y() + child(i)->h())) < 5)
                {
                    window()->cursor(FL_CURSOR_NS);
                    return 1;
                }
                else
                {
                    window()->cursor(FL_CURSOR_DEFAULT);
                }
            }
        }
    }
    if(e == FL_LEAVE)
    {
        window()->cursor(FL_CURSOR_DEFAULT);
    }
    if(e == FL_PUSH)
    {
        for(int i = 0; i < children() - 1; i++)
        {
            if(m_orientation == FLE_STACK_HORIZONTAL)
            {
                if(std::abs(Fl::event_x() - (child(i)->x() + child(i)->w())) < 5)
                {
                    offset = Fl::event_x();
                    dragging = i;
                    return 1;
                }
            }
            else
            {
                if(std::abs(Fl::event_y() - (child(i)->y() + child(i)->h())) < 5)
                {
                    offset = Fl::event_y();
                    dragging = i;
                    return 1;
                }
            }
        }
    }
    if(e == FL_DRAG)
    {
        int delta = Fl::event_y() - offset;

        if(m_orientation == FLE_STACK_HORIZONTAL)
        {
            delta = Fl::event_x() - offset;
        }

       if (delta != 0)
        {
            if (delta > 0)
            {
                int final_grower_idx = dragging; 
                while (final_grower_idx >= 0 && (m_widgetMaxSizes[final_grower_idx] - get_widget_actual_size(final_grower_idx) <= 0))
                {
                    final_grower_idx--;
                }

                if (final_grower_idx < 0)
                {
                    offset = (m_orientation == FLE_STACK_HORIZONTAL) ? Fl::event_x() : Fl::event_y();
                    return 1;
                }
                
                int shrinker_neighbor_idx = final_grower_idx; 
                int current_delta = delta;

                while (current_delta > 0 && shrinker_neighbor_idx < children() - 1)
                {
                    int possibleIncreaseGrower = m_widgetMaxSizes[final_grower_idx] - get_widget_actual_size(final_grower_idx);
                    if (possibleIncreaseGrower <= 0) break;

                    int possibleDecreaseShrinker = get_widget_actual_size(shrinker_neighbor_idx + 1) - m_widgetMinSizes[shrinker_neighbor_idx + 1];
                    if (possibleDecreaseShrinker <= 0) 
                    {
                        shrinker_neighbor_idx++;
                        continue;
                    }

                    int change = std::min(possibleIncreaseGrower, possibleDecreaseShrinker);
                    change = std::min(change, current_delta);

                    if (change <= 0) 
                    {
                        shrinker_neighbor_idx++;
                        continue;
                    }

                    set_widget_actual_size(final_grower_idx, get_widget_actual_size(final_grower_idx) + change);
                    set_widget_actual_size(shrinker_neighbor_idx + 1, get_widget_actual_size(shrinker_neighbor_idx + 1) - change);

                    current_delta -= change;

                    if (get_widget_actual_size(final_grower_idx) >= m_widgetMaxSizes[final_grower_idx]) 
                    {
                         break;
                    }
                }
            }
            else 
            {
                int amount_to_shift = -delta;

                int final_shrinker_idx = dragging;
                while (final_shrinker_idx >= 0 && (get_widget_actual_size(final_shrinker_idx) - m_widgetMinSizes[final_shrinker_idx] <= 0))
                {
                    final_shrinker_idx--;
                }

                if (final_shrinker_idx < 0)
                {
                    offset = (m_orientation == FLE_STACK_HORIZONTAL) ? Fl::event_x() : Fl::event_y();
                    return 1;
                }

                int grower_neighbor_idx = dragging;
                
                while (amount_to_shift > 0 && grower_neighbor_idx < children() - 1)
                {
                    int possibleDecreaseShrinker = get_widget_actual_size(final_shrinker_idx) - m_widgetMinSizes[final_shrinker_idx];
                    if (possibleDecreaseShrinker <= 0) break;

                    int possibleIncreaseGrower = m_widgetMaxSizes[grower_neighbor_idx + 1] - get_widget_actual_size(grower_neighbor_idx + 1);
                    if (possibleIncreaseGrower <= 0) 
                    {
                        grower_neighbor_idx++;
                        continue;
                    }

                    int change = std::min(possibleDecreaseShrinker, possibleIncreaseGrower);
                    change = std::min(change, amount_to_shift);
                    
                    if (change <= 0) 
                    {
                        grower_neighbor_idx++;
                        continue;
                    }

                    set_widget_actual_size(final_shrinker_idx, get_widget_actual_size(final_shrinker_idx) - change);
                    set_widget_actual_size(grower_neighbor_idx + 1, get_widget_actual_size(grower_neighbor_idx + 1) + change);

                    amount_to_shift -= change;

                    if (get_widget_actual_size(final_shrinker_idx) <= m_widgetMinSizes[final_shrinker_idx]) 
                    {
                        break;
                    }
                }
            }

            update_preferred_sizes();
            position_children();
            redraw();

            offset = Fl::event_y();
            if (m_orientation == FLE_STACK_HORIZONTAL)
            {
                offset = Fl::event_x();
            }

            return 1;
        }
    }
    if(e == FL_RELEASE)
    {
        dragging = -1;
        return 1;
    }

    return Fl_Group::handle(e);
}

void Fle_Stack::resize(int X, int Y, int W, int H)
{
    Fl_Group::resize(X, Y, W, H);
    position_children();
}

bool Fle_Stack::add(Fl_Widget* widget, int minSize, int preferredSize, int maxSize)
{
    return insert(widget, children(), minSize, preferredSize, maxSize);
}

bool Fle_Stack::insert(Fl_Widget* widget, int index, int minSize, int preferredSize, int maxSize)
{
    m_widgetMinSizes.insert(m_widgetMinSizes.begin() + index, minSize);
    m_widgetPreferredSizes.insert(m_widgetPreferredSizes.begin() + index, preferredSize);
    m_widgetMaxSizes.insert(m_widgetMaxSizes.begin() + index, maxSize);

    Fl_Group::insert(*widget, index);
    
    position_children();
    redraw();
    return true;
}
