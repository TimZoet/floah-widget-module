#include "floah-widget/widgets/widget.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-layout/utils/floah_error.h"

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

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Panel& Widget::getPanel() noexcept { return *panel; }

    const Panel& Widget::getPanel() const noexcept { return *panel; }

    Layout& Widget::getLayout() noexcept { return *layout; }

    const Layout& Widget::getLayout() const noexcept { return *layout; }

    Element* Widget::getPanelLayoutElement() const noexcept { return panelElement; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Widget::setPanelLayoutElement(Element& element)
    {
        if (element.getLayout() != &panel->getLayout())
            throw FloahError("Cannot attach widget to element. It is not from the panel layout.");

        panelElement = &element;
    }
}  // namespace floah
