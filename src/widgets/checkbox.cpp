#include "floah-widget/widgets/checkbox.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
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

#include "floah-widget/node_masks.h"
#include "floah-widget/panel.h"

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Checkbox::Checkbox() : Widget() {}

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

    void Checkbox::setDataSource(IBoolDataSource* source)
    {
        dataSource = source;
        staleData |= StaleData::Scenegraph;
    }

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    void Checkbox::generateLayout(Size size, Size offset)
    {
        // Create layout elements if they do not exist.
        if (!elements.root)
        {
            elements.root  = &layout->setRoot(std::make_unique<HorizontalFlow>());
            elements.box   = &elements.root->append(std::make_unique<LayoutElement>());
            elements.label = &elements.root->append(std::make_unique<LayoutElement>());
        }

        // Style layout elements.
        elements.root->getSize().setWidth(Length(1.0f));
        elements.root->getSize().setHeight(Length(1.0f));

        elements.box->getSize().setWidth(getBoxWidth());
        elements.box->getSize().setHeight(getBoxHeight());
        elements.box->getOuterMargin() = getBoxMargin();

        elements.label->getSize().setWidth(getLabelWidth());
        elements.label->getSize().setHeight(getLabelHeight());
        elements.label->getOuterMargin() = getLabelMargin();

        Widget::generateLayout(size, offset);

        // Get blocks for relevant elements.
        auto it =
          std::ranges::find_if(layoutBlocks, [&](const auto& block) { return block.id == elements.box->getId(); });
        blocks.box = &*it;
        it = std::ranges::find_if(layoutBlocks, [&](const auto& block) { return block.id == elements.label->getId(); });
        blocks.label = &*it;
    }

    void Checkbox::generateGeometry(sol::MeshManager& meshManager, FontMap& fontMap)
    {
        if (!blocks.box) throw FloahError("Cannot generate geometry. Layout was not generated yet.");

        Generator::Params params{.meshManager = meshManager, .fontMap = fontMap};
        // TODO: Update meshes if they already exist.
        {
            RectangleGenerator gen;
            gen.lower    = -0.5f * math::float2(math::min(blocks.box->bounds.width(), blocks.box->bounds.height()));
            gen.upper    = -gen.lower;
            gen.fillMode = RectangleGenerator::FillMode::Outline;
            gen.margin   = Length(2);
            gen.color    = getColor();
            meshes.box   = &gen.generate(params);
        }

        {
            RectangleGenerator gen;
            gen.lower        = -0.5f * math::float2(math::min(blocks.box->bounds.width(), blocks.box->bounds.height()));
            gen.upper        = -gen.lower;
            gen.fillMode     = RectangleGenerator::FillMode::Fill;
            gen.margin       = Length(2);
            gen.color        = getColor();
            meshes.highlight = &gen.generate(params);
        }

        {
            CircleGenerator gen;
            gen.fillMode = CircleGenerator::FillMode::Fill;
            gen.radius = 0.5f * static_cast<float>(math::min(blocks.box->bounds.width(), blocks.box->bounds.height()));
            meshes.checkmark = &gen.generate(params);
        }

        {
            TextGenerator gen;
            gen.text     = label;
            meshes.label = &gen.generate(params);
        }

        staleData = staleData & ~StaleData::Geometry;
    }

    void Checkbox::generateScenegraph(IScenegraphGenerator& generator)
    {
        if (!meshes.box) throw FloahError("Cannot generate scenegraph. Geometry was not generated yet.");


        if (!nodes.box)
        {
            // TODO: If math::float3 were directly constructible from
            // std::array<std::convertible_to<float> T, 2> and std::convertible_to<float>,
            // this could be a lot prettier:
            nodes.box = &generator.createWidgetNode(
              math::float3(blocks.box->bounds.center()[0], blocks.box->bounds.center()[1], getInputLayer()));
            nodes.highlight = &generator.createWidgetNode(
              math::float3(blocks.box->bounds.center()[0], blocks.box->bounds.center()[1], getInputLayer()));
            nodes.checkmark = &generator.createWidgetNode(
              math::float3(blocks.box->bounds.center()[0], blocks.box->bounds.center()[1], getInputLayer()));
            nodes.label = &generator.createTextNode(
              math::float3(blocks.label->bounds.x0, blocks.label->bounds.y0, getInputLayer()));

            nodes.box->addChild(std::make_unique<sol::MeshNode>(*meshes.box));
            nodes.highlight->addChild(std::make_unique<sol::MeshNode>(*meshes.highlight));
            nodes.checkmark->addChild(std::make_unique<sol::MeshNode>(*meshes.checkmark));
            nodes.label->addChild(std::make_unique<sol::MeshNode>(*meshes.label));
        }
        else
        {
            // TODO: Update nodes if they already exist.
        }

        // Set visibility of highlight.
        if (entered)
            nodes.highlight->setTypeMask(0);
        else
            nodes.highlight->setTypeMask(static_cast<uint64_t>(NodeMasks::Disabled));

        // Set visibility of checkmark.
        if (dataSource && dataSource->get())
            nodes.checkmark->setTypeMask(0);
        else
            nodes.checkmark->setTypeMask(static_cast<uint64_t>(NodeMasks::Disabled));

        staleData = staleData & ~StaleData::Scenegraph;
    }

    ////////////////////////////////////////////////////////////////
    // Input.
    ////////////////////////////////////////////////////////////////

    bool Checkbox::intersect(const int32_t x, const int32_t y) const noexcept
    {
        // Intersect with checkmark box.
        const auto       lower = math::int2{blocks.box->bounds.x0, blocks.box->bounds.y0};
        const auto       upper = math::int2{blocks.box->bounds.x1, blocks.box->bounds.y1};
        const math::AABB aabb(lower, upper);
        return inside(math::int2(x, y), aabb);
    }

    void Checkbox::onMouseEnter()
    {
        entered = true;
        staleData |= StaleData::Scenegraph;
    }

    void Checkbox::onMouseExit()
    {
        entered = false;
        staleData |= StaleData::Scenegraph;
    }

    void Checkbox::onMouseClick(const InputContext::MouseClick click)
    {
        if (click.button == InputContext::MouseButton::Left && click.action == InputContext::MouseAction::Press)
        {
            staleData |= StaleData::Scenegraph;

            if (dataSource) dataSource->toggle();
        }
    }

    ////////////////////////////////////////////////////////////////
    // Stylesheet getters.
    ////////////////////////////////////////////////////////////////

    Length Checkbox::getBoxHeight() const noexcept
    {
        const auto height = getStylesheetProperty<Length>(checkbox_box_height);
        if (height) return *height;

        const auto size = getStylesheetProperty<Size>(checkbox_box_size);
        if (size) return size->getHeight();

        return checkbox_box_height_default;
    }

    Margin Checkbox::getBoxMargin() const noexcept
    {
        const auto margin = getStylesheetProperty<Margin>(checkbox_box_margin);
        if (margin) return *margin;

        return checkbox_box_margin_default;
    }

    Length Checkbox::getBoxWidth() const noexcept
    {
        const auto width = getStylesheetProperty<Length>(checkbox_box_width);
        if (width) return *width;

        const auto size = getStylesheetProperty<Size>(checkbox_box_size);
        if (size) return size->getWidth();

        return checkbox_box_width_default;
    }

    Length Checkbox::getLabelHeight() const noexcept
    {
        const auto height = getStylesheetProperty<Length>(checkbox_label_height);
        if (height) return *height;

        const auto size = getStylesheetProperty<Size>(checkbox_label_size);
        if (size) return size->getHeight();

        return checkbox_label_height_default;
    }

    Margin Checkbox::getLabelMargin() const noexcept
    {
        const auto margin = getStylesheetProperty<Margin>(checkbox_label_margin);
        if (margin) return *margin;

        return checkbox_label_margin_default;
    }

    Length Checkbox::getLabelWidth() const noexcept
    {
        const auto width = getStylesheetProperty<Length>(checkbox_label_width);
        if (width) return *width;

        const auto size = getStylesheetProperty<Size>(checkbox_label_size);
        if (size) return size->getWidth();

        return checkbox_label_width_default;
    }

    math::float4 Checkbox::getColor() const noexcept
    {
        const auto color = getStylesheetProperty<math::float4>("color");
        if (color) return *color;

        return {1, 1, 1, 1};
    }
}  // namespace floah
