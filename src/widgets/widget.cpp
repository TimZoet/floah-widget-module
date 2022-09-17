#include "floah-widget/widgets/widget.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "floah-common/floah_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-data/data_source.h"
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

    Stylesheet* Widget::getStylesheet() noexcept { return stylesheet; }

    const Stylesheet* Widget::getStylesheet() const noexcept { return stylesheet; }

    Stylesheet* Widget::getPanelStylesheet() noexcept { return panel->getStylesheet(); }

    const Stylesheet* Widget::getPanelStylesheet() const noexcept { return panel->getStylesheet(); }

    Widget::StaleData Widget::getStaleData() const noexcept { return staleData; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Widget::setPanelLayoutElement(LayoutElement& element)
    {
        if (element.getLayout() != &panel->getLayout())
            throw FloahError("Cannot attach widget to element. It is not from the panel layout.");

        panelElement = &element;
    }

    void Widget::setStylesheet(Stylesheet* sheet) noexcept { stylesheet = sheet; }

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    void Widget::generateLayout(const Size size, const Size offset)
    {
        layout->getSize()   = size;
        layout->getOffset() = offset;
        layoutBlocks        = layout->generate();

        staleData           = staleData & ~StaleData::Layout;
    }

    ////////////////////////////////////////////////////////////////
    // Input.
    ////////////////////////////////////////////////////////////////

    const InputElement* Widget::getInputParent() const noexcept { return panel; }

    int32_t Widget::getInputLayer() const noexcept { return layer ? layer->depth : 0; }

    math::int2 Widget::getInputOffset() const noexcept { return panel->getInputOffset(); }

}  // namespace floah
