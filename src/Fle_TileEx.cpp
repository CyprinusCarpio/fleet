#include <FLE/Fle_TileEx.hpp>

#include <FL/Fl_Window.H>

#include <cmath> //abs
#include <iostream>

#include <set>
#include <queue>

struct WidgetExtraData
{
    int m_minW;
    int m_minH;
    int m_maxW;
    int m_maxH;
    int m_prefW;
    int m_prefH;
    WidgetExtraData() 
    {
        m_minW = 0;
        m_minH = 0;
        m_maxW = 0x7FFFFFFF;
        m_maxH = 0x7FFFFFFF;
        m_prefW = 25;
        m_prefH = 25;
    }
};

enum EdgeType { VERTICAL, HORIZONTAL, NONE };

struct Edge 
{
    EdgeType type;
    int position;
    std::vector<int> leftOrTopWidgets;
    std::vector<int> rightOrBottomWidgets;
};

Fle_TileEx::Fle_TileEx(int X, int Y, int W, int H, const char *L) : Fl_Group(X, Y, W, H, L) 
{
}

Edge Fle_TileEx::find_edge_at(int X, int Y)
{
    Edge result;
    result.type = NONE;

    for(int i = 0; i < children(); i++)
    {
        Fl_Widget* w = child(i);
        
        if(std::abs(w->x() - X) <= 2 && Y >= w->y() && Y <= w->y() + w->h())
        {
            result.type = VERTICAL;
            result.position = w->x();
        }
        else if(std::abs(w->x() + w->w() - X) <= 2 && Y >= w->y() && Y <= w->y() + w->h())
        {
            result.type = VERTICAL;
            result.position = w->x() + w->w();
        }

        if(result.type == NONE)
        {
            if(std::abs(w->y() - Y) <= 2 && X >= w->x() && X <= w->x() + w->w())
            {
                result.type = HORIZONTAL;
                result.position = w->y();
            }
            else if(std::abs(w->y() + w->h() - Y) <= 2 && X >= w->x() && X <= w->x() + w->w())
            {
                result.type = HORIZONTAL;
                result.position = w->y() + w->h();
            }
        }
    }

    if(result.type != NONE)
    {
        get_widgets_at_edge(result);
    }

    if(result.leftOrTopWidgets.empty() || result.rightOrBottomWidgets.empty())
        result.type = NONE;

    return result;
}

void Fle_TileEx::get_widgets_at_edge(Edge& edge)
{
    for(int i = 0; i < children(); i++)
    {
        Fl_Widget* w = child(i);
        if(edge.type == VERTICAL)
        {
            if(std::abs(w->x() - edge.position) == 0)
                edge.rightOrBottomWidgets.push_back(i);
            else if(std::abs(w->x() + w->w() - edge.position) == 0)
                edge.leftOrTopWidgets.push_back(i);
        }
        else
        {
            if(std::abs(w->y() - edge.position) == 0)
                edge.rightOrBottomWidgets.push_back(i);
            else if(std::abs(w->y() + w->h() - edge.position) == 0)
                edge.leftOrTopWidgets.push_back(i);
        }
    }
}

void Fle_TileEx::get_max_changes_for_edge(Edge& edge, bool userDrag, int& maxShrinkLeftOrTop, int& maxGrowLeftOrTop, int& maxShrinkRightOrBottom, int& maxGrowRightOrBottom, int recurse)
{
    for(int i : edge.leftOrTopWidgets)
    {
        Fl_Widget* w = child(i);
        if(edge.type == VERTICAL)
        {
            int minW = userDrag ? m_extraData[i].m_minW : m_extraData[i].m_prefW;
            maxShrinkLeftOrTop = std::min(maxShrinkLeftOrTop, w->w() - m_extraData[i].m_minW);
            int maxW = userDrag? m_extraData[i].m_maxW : m_extraData[i].m_prefW;
            maxGrowLeftOrTop = std::min(maxGrowLeftOrTop, maxW - w->w());
        }
        else
        {
            int minH = userDrag ? m_extraData[i].m_minH : m_extraData[i].m_prefH;
            maxShrinkLeftOrTop = std::min(maxShrinkLeftOrTop, w->h() - m_extraData[i].m_minH);
            int maxH = userDrag ? m_extraData[i].m_maxH : m_extraData[i].m_prefH;
            maxGrowLeftOrTop = std::min(maxGrowLeftOrTop, maxH - w->h());
        }
    }

    for(int i : edge.rightOrBottomWidgets)
    {
        Fl_Widget* w = child(i);
        if(edge.type == VERTICAL)
        {
            int minW = userDrag ? m_extraData[i].m_minW : m_extraData[i].m_prefW;
            maxShrinkRightOrBottom = std::min(maxShrinkRightOrBottom, w->w() - minW);
            int maxW = userDrag ? m_extraData[i].m_maxW : m_extraData[i].m_prefW;
            maxGrowRightOrBottom = std::min(maxGrowRightOrBottom, maxW - w->w());
        }
        else
        {
            int minH = userDrag ? m_extraData[i].m_minH : m_extraData[i].m_prefH;
            maxShrinkRightOrBottom = std::min(maxShrinkRightOrBottom, w->h() - minH);
            int maxH = userDrag ? m_extraData[i].m_maxH : m_extraData[i].m_prefH;
            maxGrowRightOrBottom = std::min(maxGrowRightOrBottom, maxH - w->h());
        }
    }
}

Edge Fle_TileEx::get_next_viable_edge(Edge& edge, int delta, bool forShrink, bool userDrag)
{
    Edge result;
    result.type = NONE;

    if(edge.type == NONE)
        return result;
    
    if(edge.type == VERTICAL)
    {
        if(delta > 0 && edge.rightOrBottomWidgets.size() > 0)
        {
            Fl_Widget* c = child(edge.rightOrBottomWidgets[0]);
            for(int i = 0; i < edge.rightOrBottomWidgets.size(); i++)
            {
                Fl_Widget* ci = child(edge.rightOrBottomWidgets[i]);
                int ciMinW = m_extraData[edge.rightOrBottomWidgets[i]].m_minW;
                if(ci->w() == ciMinW)
                {
                    c = ci;
                    if(forShrink)
                        break;
                }
                if(ciMinW < c->w())
                {
                    c = ci;
                    if(!forShrink)
                        break;
                }
            }
            result = find_edge_at(edge.position + c->w(), c->y());
        }
        else if(delta < 0 && edge.leftOrTopWidgets.size() > 0)
        {
            Fl_Widget* c = child(edge.leftOrTopWidgets[0]);
            for(int i = 0; i < edge.leftOrTopWidgets.size(); i++)
            {
                Fl_Widget* ci = child(edge.leftOrTopWidgets[i]);
                int ciMinW = m_extraData[edge.leftOrTopWidgets[i]].m_minW;
                if(ci->w() == ciMinW)
                {
                    c = ci;
                    if(forShrink)
                        break;
                }
                if(ciMinW < c->w())
                {
                    c = ci;
                    if(!forShrink)
                        break;
                }
            }
            result = find_edge_at(edge.position - c->w(), c->y());
        }
    }
    else
    {
        if(delta > 0 && edge.rightOrBottomWidgets.size() > 0)
        {
            Fl_Widget* c = child(edge.rightOrBottomWidgets[0]);
            for(int i = 0; i < edge.rightOrBottomWidgets.size(); i++)
            {
                Fl_Widget* ci = child(edge.rightOrBottomWidgets[i]);
                int ciMinH = m_extraData[edge.rightOrBottomWidgets[i]].m_minH;
                if(ci->h() == ciMinH)
                {
                    c = ci;
                    if(forShrink)
                        break;
                }
                if(ciMinH < c->h())
                {
                    c = ci;
                    if(!forShrink)
                        break;
                }
            }
            result = find_edge_at(c->x() + 3, edge.position + c->h());
        }
        else if(delta < 0 && edge.leftOrTopWidgets.size() > 0)
        {
            Fl_Widget* c = child(edge.leftOrTopWidgets[0]);
            for(int i = 0; i < edge.leftOrTopWidgets.size(); i++)
            {
                Fl_Widget* ci = child(edge.leftOrTopWidgets[i]);
                int ciMinH = m_extraData[edge.leftOrTopWidgets[i]].m_minH;
                if(ci->h() == ciMinH)
                {
                    c = ci;
                    if(forShrink)
                        break;
                }
                if(ciMinH < c->h())
                {
                    if(!forShrink)
                        break;
                }
            }
            result = find_edge_at(c->x() + 3, edge.position - c->h());
        }
    }

    return result;
}

void Fle_TileEx::propagate_resize(Edge& edge, int& delta, bool userDrag)
{
    if(edge.type == NONE)
        return;

    int maxShrinkLeftOrTop = 0x7FFFFFFF;
    int maxGrowLeftOrTop = 0x7FFFFFFF;
    int maxShrinkRightOrBottom = 0x7FFFFFFF;
    int maxGrowRightOrBottom = 0x7FFFFFFF;
    
    get_max_changes_for_edge(edge, userDrag, maxShrinkLeftOrTop, maxGrowLeftOrTop, maxShrinkRightOrBottom, maxGrowRightOrBottom);

    if(edge.rightOrBottomWidgets.size() == 0 && !userDrag && delta > 0)
    {
        for(int i : edge.leftOrTopWidgets)
        {
            Fl_Widget* c = child(i);
            if(edge.type == VERTICAL)
                c->resize(c->x(), c->y(), c->w() + delta, c->h());
            else
                c->resize(c->x(), c->y(), c->w(), c->h() + delta);
        }
        // TODO: implement scaling up of widgets below their pref size
        
        return;
    }

    int maxChange;
    bool maxSizeHit = false;
    if(delta > 0)
    {
        if(maxGrowLeftOrTop < maxShrinkRightOrBottom)
        {
            maxChange = maxGrowLeftOrTop;
            maxSizeHit = true;
        }
        else
        {
            maxChange = maxShrinkRightOrBottom;
        }
    }
    else
    {
        if(maxShrinkLeftOrTop < maxGrowRightOrBottom)
        {
            maxChange = maxShrinkLeftOrTop;
        }
        else
        {
            maxChange = maxGrowRightOrBottom;
            maxSizeHit = true;
        }
    }
    int change = std::min(std::abs(delta), maxChange);
    if(delta < 0) change = -change;
    edge.position += change;

    delta -= change;
    if(change != 0)
    {
        if(edge.type == VERTICAL)
        {
            for(int i = 0; i < edge.leftOrTopWidgets.size(); i++)
            {
                Fl_Widget* w = child(edge.leftOrTopWidgets[i]);
                w->resize(w->x(), w->y(), w->w() + change, w->h());
                if(userDrag)
                {
                    m_extraData[i].m_prefW = w->w();
                    m_extraData[i].m_prefH = w->h();
                }
            }
            for(int i = 0; i < edge.rightOrBottomWidgets.size(); i++)
            {
                Fl_Widget* w = child(edge.rightOrBottomWidgets[i]);
                w->resize(w->x() + change, w->y(), w->w() - change, w->h());
                if(userDrag)
                {
                    m_extraData[i].m_prefW = w->w();
                    m_extraData[i].m_prefH = w->h();
                }
            }
        }
        else
        {
            for(int i = 0; i < edge.leftOrTopWidgets.size(); i++)
            {
                Fl_Widget* w = child(edge.leftOrTopWidgets[i]);
                w->resize(w->x(), w->y(), w->w(), w->h() + change);
                if(userDrag)
                {
                    m_extraData[i].m_prefW = w->w();
                    m_extraData[i].m_prefH = w->h();
                }
            }
            for(int i = 0; i < edge.rightOrBottomWidgets.size(); i++)
            {
                Fl_Widget* w = child(edge.rightOrBottomWidgets[i]);
                w->resize(w->x(), w->y() + change, w->w(), w->h() - change);
                if(userDrag)
                {
                    m_extraData[i].m_prefW = w->w();
                    m_extraData[i].m_prefH = w->h();
                }
            }
        }
    }
    if(delta != 0)
    {
        int oldDelta = delta;
        if(maxSizeHit && userDrag)
        {
            Edge prevEdge = get_next_viable_edge(edge, -delta, delta > 0, userDrag);
            propagate_resize(prevEdge, delta, userDrag);
            oldDelta -= delta;
            propagate_resize(edge, oldDelta, userDrag);
        }
        else
        {
            Edge nextEdge = get_next_viable_edge(edge, delta, true, userDrag);
            propagate_resize(nextEdge, delta, userDrag);
            oldDelta -= delta;
            propagate_resize(edge, oldDelta, userDrag);
        }
    }
}

int Fle_TileEx::handle(int event)
{
    static int ex, ey;
    static Edge edge;

    if(event == FL_ENTER)
    {
        return 1;
    }
    else if(event == FL_MOVE)
    {
        Edge e = find_edge_at(Fl::event_x(), Fl::event_y());
        if(e.type != NONE)
        {
            window()->cursor(e.type == VERTICAL ? FL_CURSOR_WE : FL_CURSOR_NS);
            return 1;
        }
        else
        {
            window()->cursor(FL_CURSOR_DEFAULT);
            return 1;
        }
    }
    else if(event == FL_PUSH)
    {
        ex = Fl::event_x();
        ey = Fl::event_y();
        edge = find_edge_at(Fl::event_x(), Fl::event_y());
        return 1;
    }
    else if(event == FL_DRAG && edge.type != NONE)
    {
        int delta = Fl::event_y() - ey;
        if(edge.type == VERTICAL)
            delta = Fl::event_x() - ex;
        propagate_resize(edge, delta);
        redraw();
        if(delta == 0)
        {
            ex = Fl::event_x();
            ey = Fl::event_y();
            return 1;
        }
    }
    return Fl_Group::handle(event);
}

void Fle_TileEx::resize(int X, int Y, int W, int H)
{
    int oldW = w();
    int oldH = h();

    Fl_Widget::resize(X, Y, W, H);

    if(oldW != W)
    {
        int delta = W - oldW;
        Edge rightEdge;
        rightEdge.type = VERTICAL;
        rightEdge.position = oldW;
        get_widgets_at_edge(rightEdge);
        propagate_resize(rightEdge, delta, false);
    }
    if(oldH != H)
    {
        int delta = H - oldH;
        Edge bottomEdge;
        bottomEdge.type = HORIZONTAL;
        bottomEdge.position = oldH;
        get_widgets_at_edge(bottomEdge);
        propagate_resize(bottomEdge, delta, false);
    }
}

void Fle_TileEx::get_min_size(int index, int &w, int &h)
{
    if(index >= 0 && index < m_extraData.size())
    {
        w = m_extraData[index].m_minW;
        h = m_extraData[index].m_minH;
    }
}

void Fle_TileEx::get_max_size(int index, int &w, int &h)
{
    if(index >= 0 && index < m_extraData.size())
    {
        w = m_extraData[index].m_maxW;
        h = m_extraData[index].m_maxH;
    }
}

void Fle_TileEx::get_preferred_size(int index, int &w, int &h)
{
    if(index >= 0 && index < m_extraData.size())
    {
        w = m_extraData[index].m_prefW;
        h = m_extraData[index].m_prefH;
    }
}

void Fle_TileEx::size_range(int index, int minW, int minH, int maxW, int maxH, int preferredW, int preferredH)
{
    if(index >= 0 && index < m_extraData.size())
    {
        m_extraData[index].m_minW = minW;
        m_extraData[index].m_minH = minH;
        m_extraData[index].m_maxW = maxW;
        m_extraData[index].m_maxH = maxH;
        m_extraData[index].m_prefW = preferredW;
        m_extraData[index].m_prefH = preferredH;
    }
}

void Fle_TileEx::size_range(Fl_Widget *widget, int minW, int minH, int maxW, int maxH, int preferredW, int preferredH)
{
    size_range(find(widget), minW, minH, maxW, maxH, preferredW, preferredH);
}

int Fle_TileEx::on_insert(Fl_Widget *widget, int index) 
{
    m_extraData.insert(m_extraData.begin() + index, WidgetExtraData());
    return index;
}

int Fle_TileEx::on_move(int index, int new_index) 
{
    std::swap(m_extraData[index], m_extraData[new_index]);
    return new_index;
}

void Fle_TileEx::on_remove(int index) 
{
    m_extraData.erase(m_extraData.begin() + index);
}