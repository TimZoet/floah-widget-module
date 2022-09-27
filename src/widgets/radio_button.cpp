#include "floah-widget/widgets/radio_button.h"

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
#include "sol/scenegraph/forward/forward_material_node.h"

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

    RadioButton::RadioButton() : Widget() {}

    RadioButton::RadioButton(RadioButton& main) : Widget(), mainButton(&main)
    {
        if (main.mainButton) throw FloahError("Cannot create RadioButton. Other button is not a main button.");

        mainButton->siblings.push_back(this);
    }

    RadioButton::~RadioButton() noexcept
    {
        if (mainButton) mainButton->siblings.erase(std::ranges::find(mainButton->siblings, this));
        if (dataSource) dataSource->removeDataListener(*this);
        // TODO: Destroy any allocated meshes, nodes, etc.
        // if (meshes.box) meshManager.destroyMesh(meshes.box->getUuid());
        // if (meshes.checkmark) meshManager.destroyMesh(meshes.checkmark->getUuid());
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const std::string& RadioButton::getLabel() const noexcept { return label; }

    bool RadioButton::hasDataSource() const noexcept { return dataSource != nullptr; }

    IBoolDataSource* RadioButton::getDataSource() const noexcept { return dataSource; }

    bool RadioButton::isMainButton() const noexcept { return mainButton == nullptr; }

    RadioButton* RadioButton::getMainButton() const noexcept { return mainButton; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void RadioButton::setLabel(std::string l) { label = std::move(l); }

    void RadioButton::setDataSource(IBoolDataSource* source)
    {
        if (replaceDataSource(&dataSource, source)) staleData |= StaleData::Scenegraph;
    }

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    void RadioButton::generateLayout(Size size, Size offset)
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

    void RadioButton::generateGeometry(sol::MeshManager& meshManager, FontMap& fontMap)
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

    void RadioButton::generateScenegraph(IScenegraphGenerator& generator)
    {
        if (!meshes.box) throw FloahError("Cannot generate scenegraph. Geometry was not generated yet.");


        if (!nodes.root)
        {
            nodes.root = &generator.createWidgetNode(panel->getPanelNode());

            auto& widgetMtlNode = nodes.root->addChild(std::make_unique<sol::ForwardMaterialNode>());
            widgetMtlNode.setMaterial(getWidgetMaterial());

            auto& textMtlNode = generator.createTextMaterialNode(*nodes.root, *getTextMaterial());

            // TODO: If math::float3 were directly constructible from
            // std::array<std::convertible_to<float> T, 2> and std::convertible_to<float>,
            // this could be a lot prettier:

            auto& widgetTansformNode = generator.createWidgetTransformNode(
              widgetMtlNode,
              math::float3(blocks.box->bounds.center()[0], blocks.box->bounds.center()[1], getInputLayer()));
            widgetTansformNode.getAsNode().addChild(std::make_unique<sol::MeshNode>(*meshes.box));
            nodes.highlight =
              &widgetTansformNode.getAsNode().addChild(std::make_unique<sol::MeshNode>(*meshes.highlight));
            nodes.checkmark =
              &widgetTansformNode.getAsNode().addChild(std::make_unique<sol::MeshNode>(*meshes.checkmark));

            auto& labelTransformNode = generator.createWidgetTransformNode(
              textMtlNode, math::float3(blocks.label->bounds.x0, blocks.label->bounds.y0, getInputLayer()));
            labelTransformNode.getAsNode().addChild(std::make_unique<sol::MeshNode>(*meshes.label));
        }
        else
        {
            // TODO: Update nodes if they already exist.
        }

        // Set visibility of highlight.
        if (state.entered)
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

    bool RadioButton::intersect(const math::int2 point) const noexcept
    {
        // Intersect with checkmark box.
        const auto       lower = math::int2{blocks.box->bounds.x0, blocks.box->bounds.y0};
        const auto       upper = math::int2{blocks.box->bounds.x1, blocks.box->bounds.y1};
        const math::AABB aabb(lower, upper);
        return inside(point, aabb);
    }

    InputContext::MouseEnterResult RadioButton::onMouseEnter(const InputContext::MouseEnterEvent&)
    {
        state.entered = true;
        staleData |= StaleData::Scenegraph;
        return {};
    }

    InputContext::MouseExitResult RadioButton::onMouseExit(const InputContext::MouseExitEvent&)
    {
        state.entered = false;
        staleData |= StaleData::Scenegraph;
        return {};
    }

    InputContext::MouseClickResult RadioButton::onMouseClick(const InputContext::MouseClickEvent& click)
    {
        if (click.button == InputContext::MouseButton::Left && click.action == InputContext::MouseAction::Press)
        {
            if (mainButton)
                mainButton->setMain(*this);
            else
                setMain(*this);
        }

        return {};
    }

    ////////////////////////////////////////////////////////////////
    // DataListener.
    ////////////////////////////////////////////////////////////////

    void RadioButton::onDataSourceUpdate(DataSource&) { staleData |= StaleData::Scenegraph; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void RadioButton::setMain(RadioButton& setButton)
    {
        if (!setButton.dataSource) return;

        // Set value to true for button that was clicked and false for all others.

        if (dataSource) { dataSource->set(this == &setButton); }

        for (auto* b : siblings)
        {
            if (b->dataSource) { b->dataSource->set(b == &setButton); }
        }
    }

    ////////////////////////////////////////////////////////////////
    // Stylesheet getters.
    ////////////////////////////////////////////////////////////////

    Length RadioButton::getBoxHeight() const noexcept
    {
        const auto height = getStylesheetProperty<Length>(radiobutton_box_height);
        if (height) return *height;

        const auto size = getStylesheetProperty<Size>(radiobutton_box_size);
        if (size) return size->getHeight();

        return radiobutton_box_height_default;
    }

    Margin RadioButton::getBoxMargin() const noexcept
    {
        const auto margin = getStylesheetProperty<Margin>(radiobutton_box_margin);
        if (margin) return *margin;

        return radiobutton_box_margin_default;
    }

    Length RadioButton::getBoxWidth() const noexcept
    {
        const auto width = getStylesheetProperty<Length>(radiobutton_box_width);
        if (width) return *width;

        const auto size = getStylesheetProperty<Size>(radiobutton_box_size);
        if (size) return size->getWidth();

        return radiobutton_box_width_default;
    }

    Length RadioButton::getLabelHeight() const noexcept
    {
        const auto height = getStylesheetProperty<Length>(radiobutton_label_height);
        if (height) return *height;

        const auto size = getStylesheetProperty<Size>(radiobutton_label_size);
        if (size) return size->getHeight();

        return radiobutton_label_height_default;
    }

    Margin RadioButton::getLabelMargin() const noexcept
    {
        const auto margin = getStylesheetProperty<Margin>(radiobutton_label_margin);
        if (margin) return *margin;

        return radiobutton_label_margin_default;
    }

    Length RadioButton::getLabelWidth() const noexcept
    {
        const auto width = getStylesheetProperty<Length>(radiobutton_label_width);
        if (width) return *width;

        const auto size = getStylesheetProperty<Size>(radiobutton_label_size);
        if (size) return size->getWidth();

        return radiobutton_label_width_default;
    }

    sol::ForwardMaterialInstance* RadioButton::getTextMaterial() const noexcept
    {
        auto mtl = getStylesheetProperty<sol::ForwardMaterialInstance*>(radiobutton_material_text);
        if (mtl) return *mtl;
        mtl = getStylesheetProperty<sol::ForwardMaterialInstance*>(material_text);
        if (mtl) return *mtl;
        return nullptr;
    }

    sol::ForwardMaterialInstance* RadioButton::getWidgetMaterial() const noexcept
    {
        auto mtl = getStylesheetProperty<sol::ForwardMaterialInstance*>(radiobutton_material_widget);
        if (mtl) return *mtl;
        mtl = getStylesheetProperty<sol::ForwardMaterialInstance*>(material_widget);
        if (mtl) return *mtl;
        return nullptr;
    }

    math::float4 RadioButton::getColor() const noexcept
    {
        const auto color = getStylesheetProperty<math::float4>("color");
        if (color) return *color;

        return {1, 1, 1, 1};
    }
}  // namespace floah
