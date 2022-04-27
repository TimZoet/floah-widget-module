#include "floah-widget/panel.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-layout/utils/floah_error.h"
#include "math/include_all.h"

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Panel::Panel() : InputElement(), layout(std::make_unique<Layout>()) {}

    Panel::~Panel() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Layout& Panel::getLayout() noexcept { return *layout; }

    const Layout& Panel::getLayout() const noexcept { return *layout; }

    InputContext* Panel::getInputContext() noexcept { return inputContext; }

    InputContext* Panel::getInputContext() const noexcept { return inputContext; }

    sol::MeshManager* Panel::getMeshManager() noexcept { return meshManager; }

    const sol::MeshManager* Panel::getMeshManager() const noexcept { return meshManager; }

    sol::Node* Panel::getRootNode() noexcept { return rootNode; }

    const sol::Node* Panel::getRootNode() const noexcept { return rootNode; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Panel::setInputContext(InputContext& context) noexcept { inputContext = &context; }

    void Panel::setMeshManager(sol::MeshManager& manager) noexcept { meshManager = &manager; }

    void Panel::setRootNode(sol::Node& node) noexcept { rootNode = &node; }

    ////////////////////////////////////////////////////////////////
    // Widgets.
    ////////////////////////////////////////////////////////////////

    void Panel::addWidgetImpl(WidgetPtr widget)
    {
        auto& ref = *widgets.emplace_back(std::move(widget));
        ref.panel = this;
        inputContext->addElement(ref);
    }

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    void Panel::generatePanelLayout() { blocks = layout->generate(); }

    void Panel::generateWidgetLayouts()
    {
        for (auto& w : widgets)
        {
            // Look for element in panel layout widget is attached to.
            const auto it = std::ranges::find_if(blocks, [&](const Block& block) {
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

    void Panel::generateGeometry() const
    {
        if (!meshManager) throw FloahError("Cannot generate geometry: no mesh manager assigned to panel.");

        for (const auto& w : widgets) w->generateGeometry(*meshManager);
    }

    void Panel::generateScenegraph() const
    {
        if (!rootNode) throw FloahError("Cannot generate scenegraph: no root node assigned to panel.");

        for (const auto& w : widgets) w->generateScenegraph(*rootNode);
    }

    ////////////////////////////////////////////////////////////////
    // Input.
    ////////////////////////////////////////////////////////////////

    bool Panel::intersect(const int32_t x, const int32_t y) const
    {
        const auto offset = math::int2(layout->getOffset().getWidth().get(), layout->getOffset().getHeight().get());
        const auto size   = math::int2(layout->getSize().getWidth().get(), layout->getSize().getHeight().get());
        const math::AABB aabb(offset, size);
        return inside(math::int2(x, y), aabb);
    }

}  // namespace floah
