#include "floah-widget/panel.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "floah-common/floah_error.h"
#include "math/include_all.h"

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Panel::Panel(InputContext& context) : InputElement(), layout(std::make_unique<Layout>()), inputContext(&context)
    {
        inputContext->addElement(*this);
    }

    Panel::~Panel() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Layout& Panel::getLayout() noexcept { return *layout; }

    const Layout& Panel::getLayout() const noexcept { return *layout; }

    InputContext& Panel::getInputContext() noexcept { return *inputContext; }

    const InputContext& Panel::getInputContext() const noexcept { return *inputContext; }

    Stylesheet* Panel::getStylesheet() noexcept { return stylesheet; }

    const Stylesheet* Panel::getStylesheet() const noexcept { return stylesheet; }

    sol::Node* Panel::getPanelNode() noexcept { return nullptr; }

    const sol::Node* Panel::getPanelNode() const noexcept { return nullptr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Panel::setStylesheet(Stylesheet* sheet) noexcept { stylesheet = sheet; }

    ////////////////////////////////////////////////////////////////
    // Layers.
    ////////////////////////////////////////////////////////////////

    Layer& Panel::createLayer(std::string layerName, const int32_t depth)
    {
        if (layerName.empty()) throw FloahError("Cannot create a layer with an empty name.");
        if (layers.contains(layerName))
            throw FloahError(std::format("Cannot create layer {}. A layer with this name already exists.", layerName));

        return *layers.try_emplace(std::move(layerName), std::make_unique<Layer>(depth)).first->second;
    }

    Layer& Panel::getLayer(const std::string& layerName) const
    {
        const auto it = layers.find(layerName);
        if (it == layers.end())
            throw FloahError(std::format("Cannot get layer {}. A layer with this name does not exist.", layerName));

        return *it->second;
    }

    void Panel::destroyLayer(const std::string& layerName)
    {
        const auto it = layers.find(layerName);
        if (it == layers.end())
            throw FloahError(std::format("Cannot destroy layer {}. A layer with this name does not exist.", layerName));

        layers.erase(it);

        // Remove widgets from layer.
        for (const auto& w : widgets)
            if (w->layer == it->second.get()) w->layer = nullptr;
    }

    ////////////////////////////////////////////////////////////////
    // Widgets.
    ////////////////////////////////////////////////////////////////

    void Panel::destroyWidget(Widget& widget)
    {
        if (&widget.getPanel() != this) throw FloahError("Cannot destroy widget. It is not part of this panel.");

        inputContext->removeElement(widget);

        const auto it = std::ranges::find_if(widgets, [&widget](const auto& w) { return w.get() == &widget; });
        widgets.erase(it);
    }

    void Panel::addWidgetImpl(WidgetPtr widget, Layer* layer)
    {
        auto& ref = *widgets.emplace_back(std::move(widget));
        ref.panel = this;
        ref.layer = layer;
        inputContext->addElement(ref);
    }

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    void Panel::generatePanelLayout() { blocks = layout->generate(); }

    void Panel::generateWidgetLayouts()
    {
        for (auto& w : widgets | std::views::filter([](const auto& widget) {
                           return any(widget->getStaleData() & Widget::StaleData::Layout);
                       }))
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

    void Panel::generateGeometry(sol::MeshManager& meshManager, FontMap& fontMap)
    {
        for (const auto& w : widgets | std::views::filter([](const auto& widget) {
                                 return any(widget->getStaleData() & Widget::StaleData::Geometry);
                             }))
            w->generateGeometry(meshManager, fontMap);
    }

    void Panel::generateScenegraph(IScenegraphGenerator& generator)
    {
        for (const auto& w : widgets | std::views::filter([](const auto& widget) {
                                 return any(widget->getStaleData() & Widget::StaleData::Scenegraph);
                             }))
            w->generateScenegraph(generator);
    }

    ////////////////////////////////////////////////////////////////
    // Input.
    ////////////////////////////////////////////////////////////////

    bool Panel::intersect(const math::int2 point) const noexcept
    {
        const auto offset = math::int2(layout->getOffset().getWidth().get(), layout->getOffset().getHeight().get());
        const auto size   = math::int2(layout->getSize().getWidth().get(), layout->getSize().getHeight().get());
        const math::AABB aabb(offset, size);
        return inside(point, aabb);
    }

}  // namespace floah
