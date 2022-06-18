#include "floah-widget/widgets/widget.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-common/floah_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-widget/panel.h"

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Widget::Widget() : layout(std::make_unique<Layout>()) {}

    Widget::~Widget() noexcept = default;

    void Widget::destroy() { panel->destroyWidget(*this); }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Panel& Widget::getPanel() noexcept { return *panel; }

    const Panel& Widget::getPanel() const noexcept { return *panel; }

    Layer* Widget::getLayer() noexcept { return layer; }

    const Layer* Widget::getLayer() const noexcept { return layer; }

    Layout& Widget::getLayout() noexcept { return *layout; }

    const Layout& Widget::getLayout() const noexcept { return *layout; }

    LayoutElement* Widget::getPanelLayoutElement() const noexcept { return panelElement; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Widget::setPanelLayoutElement(LayoutElement& element)
    {
        if (element.getLayout() != &panel->getLayout())
            throw FloahError("Cannot attach widget to element. It is not from the panel layout.");

        panelElement = &element;
    }

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    void Widget::generateLayout(Size size, Size offset)
    {
        layout->getSize()   = std::move(size);
        layout->getOffset() = std::move(offset);
        blocks              = layout->generate();
    }

    ////////////////////////////////////////////////////////////////
    // Input.
    ////////////////////////////////////////////////////////////////

    const InputElement* Widget::getInputParent() const noexcept { return panel; }

    int32_t Widget::getInputLayer() const noexcept { return layer ? layer->depth : 0; }

}  // namespace floah
