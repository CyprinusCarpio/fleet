#ifndef FLE_TILEEX_H
#define FLE_TILEEX_H

#include <FL/Fl_Group.H>

#include <vector>
#include <set>

struct WidgetExtraData;
struct Edge;
class EdgeList;
enum class EdgeType {ANY, VERTICAL, HORIZONTAL};

/// \class Fle_TileEx
/// \brief A extended alternative to Fl_Tile.
///
/// TileEx is used to maintain a tesselated layout of widgets. The child widgets must
/// fill the available space entirely, and not overlap. TileEx keeps a record of
/// minimal, preferred and maximal sizes for all child widgets. Widgets cannot go below
/// the minimal size. Widgets will attempt to scale up to their preferred sizes during
/// resizing of the TileEx, but can go above it when it's required for maintaining tesselation.
/// Widgets will not go over their maximal sizes. Dragging on the edge between widgets updates
/// their preferred sizes.
class Fle_TileEx : public Fl_Group
{
    std::vector<WidgetExtraData> m_extraData; ///< Extra data for each widget
    EdgeList* m_edges; ///< List of edges
    bool m_needToFindEdges; ///< True if edges need to be recalculated
    
    /// Finds edges between widgets
    void find_edges();

    /// Finds an edge at the given coordinates
    /// \param type The orientation of edge to find
    Edge* find_edge_at(int x, int y, EdgeType type = EdgeType::ANY);

    /// Populates edges with widget pointers
    void edges_populate_widgets();

    /// Gets the next viable edge to propagate to, left/top of the given edge.
    /// \param edge The edge to propagate from
    Edge* get_prev_edge(Edge* edge);

    /// Gets the next viable edge to propagate to, right/bottom of the given edge.
    /// \param edge The edge to propagate from
    Edge* get_next_edge(Edge* edge);

    /// Gets the maximum change in size for a given edge
    /// \param edge The edge to get the changes for
    void get_max_changes_for_edge(Edge* edge, bool resizingWindow, int& maxShrinkLeftOrTop, int& maxGrowLeftOrTop, int& maxShrinkRightOrBottom, int& maxGrowRightOrBottom);

    /// Propagates the resize on a given edge. This is the function responsible
    /// for actually resizing all widgets, both during user dragging on the edges
    /// and during resizing of the TileEx.
    ///
    /// \param edge The edge to propagate from
    /// \param delta The change in size. Passed by reference for recursive resizing
    /// \param resizingWindow Whether to update preferred sizes. True during resizing of the TileEx itself.
    /// \param previousEdge The edge used in the previous iteration. Can be null. If this iteration finds the same edge
    /// as the next edge to propagate to, we stop the recursion.
    void propagate_resize(Edge* edge, int& delta, bool resizingWindow, Edge* previousEdge = nullptr);

    /// A modified propagate_resize used to reposition widgets starting at the right/bottom edge
    /// when the TileEx is resized up.
    ///
    /// \param edge The edge to propagate from
    /// \param delta The change in size.
    /// \param processedEdges A set of edges that have already been processed. Passed by reference.
    /// \param correction A bool to prevent further propagation during a correction after a iteration is done.
    void propagate_grow(Edge* edge, int delta, std::set<Edge*>& processedEdges, bool correction = false);

    /// Helper function to resize and move widgets on a edge.
    /// \param edge The edge to resize/move
    /// \param delta The change in size
    /// \param updatePrefSizes True to update preferred sizes
    void edge_change(Edge* edge, int delta, bool updatePrefSizes);

protected:
    int on_insert(Fl_Widget*, int) override;
    int on_move(int, int) override;
    void on_remove(int) override;

    //void draw() override;
    
public:
    /// Typical CTOR
    Fle_TileEx(int X, int Y, int W, int H, const char *L = 0);
    /// DTOR.
    ~Fle_TileEx();

    /// Handles events
    int handle(int e) override;
    /// Resizes the TileEx
    void resize(int X, int Y, int W, int H) override;

    /// Gets the minimal size of the widget at the given index.
    /// \param index The index of the widget
    /// \param[out] w The width
    /// \param[out] h The height
    void get_min_size(int index, int& w, int& h);

    /// Gets the maximal size of the widget at the given index.
    /// \param index The index of the widget
    /// \param[out] w The width
    /// \param[out] h The height
    void get_max_size(int index, int& w, int& h);

    /// Gets the preferred size of the widget at the given index.
    /// \param index The index of the widget
    /// \param[out] w The width
    /// \param[out] h The height
    void get_preferred_size(int index, int& w, int& h);

    /// Sets size constraints for a widget at a given index.
    /// \param index The index of the widget
    /// \param minW The minimal width.
    /// \param minH The minimal height.
    /// \param maxW The maximal width.
    /// \param maxH The maximal height.
    /// \param preferredW The preferred width.
    /// \param preferredH The preferred height.
    void size_range(int index, int minW, int minH, int maxW, int maxH, int preferredW, int preferredH);

    /// Sets size constraints for a widget.
    /// \param widget The widget
    /// \param minW The minimal width.
    /// \param minH The minimal height.
    /// \param maxW The maximal width.
    /// \param maxH The maximal height.
    /// \param preferredW The preferred width.
    /// \param preferredH The preferred height.
    void size_range(Fl_Widget* widget, int minW, int minH, int maxW, int maxH, int preferredW, int preferredH);
};

#endif