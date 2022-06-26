#include "floah-widget/widgets/checkbox.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-common/floah_error.h"
#include "floah-viz/generators/circle_generator.h"
#include "floah-viz/generators/rectangle_generator.h"
#include "floah-viz/generators/text_generator.h"
#include "sol/mesh/flat_mesh.h"
#include "sol/mesh/mesh_manager.h"
#include "sol/scenegraph/node.h"
#include "sol/scenegraph/drawable/mesh_node.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-widget/panel.h"

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Checkbox::Checkbox() : Widget()
    {
        elements.root  = &layout->setRoot(std::make_unique<HorizontalFlow>());
        elements.box   = &elements.root->append(std::make_unique<LayoutElement>());
        elements.label = &elements.root->append(std::make_unique<LayoutElement>());


        elements.root->getSize().setWidth(Length(1.0f));
        elements.root->getSize().setHeight(Length(1.0f));
        elements.box->getSize().setWidth(Length(0.2f));
        elements.box->getSize().setHeight(Length(1.0f));
        elements.label->getSize().setWidth(Length(0.8f));
        elements.label->getSize().setHeight(Length(1.0f));
    }

    Checkbox::~Checkbox() noexcept
    {
        // TODO: Destroy any allocated meshes.
        // if (meshes.box) meshManager.destroyMesh(meshes.box->getUuid());
        // if (meshes.checkmark) meshManager.destroyMesh(meshes.checkmark->getUuid());
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const std::string& Checkbox::getLabel() const noexcept { return label; }

    bool Checkbox::hasDataSource() const noexcept { return dataSource != nullptr; }

    IBoolDataSource* Checkbox::getDataSource() const noexcept { return dataSource; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Checkbox::setLabel(std::string l) { label = std::move(l); }

    void Checkbox::setDataSource(IBoolDataSource* source) { dataSource = source; }

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    void Checkbox::generateLayout(Size size, Size offset)
    {
        Widget::generateLayout(std::move(size), std::move(offset));

        // Get blocks for relevant elements.
        auto it = std::ranges::find_if(blocks, [&](const auto& block) { return block.id == elements.box->getId(); });
        elements.boxBlock = &*it;
        it = std::ranges::find_if(blocks, [&](const auto& block) { return block.id == elements.label->getId(); });
        elements.labelBlock = &*it;
    }

    void Checkbox::generateGeometry(sol::MeshManager& meshManager, FontMap& fontMap)
    {
        if (!elements.boxBlock) throw FloahError("Cannot generate geometry. Layout was not generated yet.");

        Generator::Params params{.meshManager = meshManager, .fontMap = fontMap};
        // TODO: Update meshes if they already exist.
        {
            RectangleGenerator gen;
            gen.upper    = math::float2(elements.boxBlock->bounds.width(), elements.boxBlock->bounds.height());
            gen.fillMode = RectangleGenerator::FillMode::Fill;
            gen.margin   = getMargin();
            gen.color    = getColor();
            meshes.box   = &gen.generate(params);
        }

        {
            TextGenerator gen;
            gen.text     = label;
            meshes.label = &gen.generate(params);
        }
    }

    void Checkbox::generateScenegraph(IScenegraphGenerator& generator)
    {
        if (!meshes.box) throw FloahError("Cannot generate scenegraph. Geometry was not generated yet.");

        // TODO: Update nodes if they already exist.

        auto& widgetNode = generator.createWidgetNode(
          math::float3(elements.boxBlock->bounds.x0, elements.boxBlock->bounds.y0, getInputLayer()));
        auto& labelNode = generator.createTextNode(
          math::float3(elements.labelBlock->bounds.x0, elements.labelBlock->bounds.y0, getInputLayer()));

        nodes.box   = &widgetNode.addChild(std::make_unique<sol::MeshNode>(*meshes.box));
        nodes.label = &labelNode.addChild(std::make_unique<sol::MeshNode>(*meshes.label));
    }

    ////////////////////////////////////////////////////////////////
    // Input.
    ////////////////////////////////////////////////////////////////

    bool Checkbox::intersect(const int32_t x, const int32_t y) const noexcept
    {
        const auto offset = math::int2(layout->getOffset().getWidth().get(), layout->getOffset().getHeight().get());
        const auto size   = math::int2(layout->getSize().getWidth().get(), layout->getSize().getHeight().get());
        const math::AABB aabb(offset, offset + size);
        return inside(math::int2(x, y), aabb);
    }

    ////////////////////////////////////////////////////////////////
    // Stylesheet getters.
    ////////////////////////////////////////////////////////////////

    Length Checkbox::getMargin() const noexcept
    {
        Length margin;
        if (stylesheet) return stylesheet->get("margin", margin);
        if (panel->getStylesheet()) return panel->getStylesheet()->get("margin", margin);
        return margin;
    }

    math::float4 Checkbox::getColor() const noexcept
    {
        math::float4 color(1, 1, 1, 1);
        if (stylesheet) return stylesheet->get("color", color);
        if (panel->getStylesheet()) return panel->getStylesheet()->get("color", color);
        return color;
    }
}  // namespace floah
