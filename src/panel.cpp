#include "floah-widget/panel.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Panel::Panel() : layout(std::make_unique<Layout>()) {}

    Panel::~Panel() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Layout& Panel::getLayout() noexcept { return *layout; }

    const Layout& Panel::getLayout() const noexcept { return *layout; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////
    // Widgets.
    ////////////////////////////////////////////////////////////////

    void Panel::addWidgetImpl(WidgetPtr widget)
    {
        auto& ref = *widgets.emplace_back(std::move(widget));
        ref.panel = this;
    }

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    void Panel::generateLayout()
    {
        blocks = layout->generate();

        for (auto& w : widgets)
        {
            // Look for element in panel layout widget is attached to.
            const auto it = std::ranges::find_if(blocks, [&](const Block block) {
                auto* elem = w->getPanelLayoutElement();
                if (!elem) return false;
                return elem->getId() == block.id;
            });

            // If widget was attached to an element, generate its layout.
            if (it != blocks.end())
                w->generateLayout(Size(Length(it->bounds.width()), Length(it->bounds.height())),
                                  Size(Length(it->bounds.x0), Length(it->bounds.y0)));
            // TODO: Clear layout otherwise?
        }
    }
}  // namespace floah
