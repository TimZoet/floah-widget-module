#include "floah-widget/widgets/dropdown.h"

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

    Dropdown::Dropdown() : Widget() {}

    Dropdown::~Dropdown() noexcept
    {
        if (meshes.box)
        {
            auto& meshManager = meshes.box->getMeshManager();
            meshManager.destroyMesh(meshes.box->getUuid());
            meshManager.destroyMesh(meshes.highlight->getUuid());
            meshManager.destroyMesh(meshes.value->getUuid());
            meshManager.destroyMesh(meshes.label->getUuid());
            meshManager.destroyMesh(meshes.itemsBack->getUuid());
            meshManager.destroyMesh(meshes.itemsHighlight->getUuid());
            std::ranges::for_each(meshes.items,
                                  [](auto* mesh) { mesh->getMeshManager().destroyMesh(mesh->getUuid()); });
        }

        // TODO: Destroy nodes.

        if (indexDataSource) indexDataSource->removeDataListener(*this);
        if (itemsDataSource) itemsDataSource->removeDataListener(*this);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const std::string& Dropdown::getLabel() const noexcept { return label; }

    bool Dropdown::hasItemsDataSource() const noexcept { return itemsDataSource != nullptr; }

    bool Dropdown::hasIndexDataSource() const noexcept { return indexDataSource != nullptr; }

    IListDataSource* Dropdown::getItemsDataSource() const noexcept { return itemsDataSource; }

    IIntegralValueDataSource* Dropdown::getIndexDataSource() const noexcept { return indexDataSource; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Dropdown::setLabel(std::string l) { label = std::move(l); }

    void Dropdown::setItemsDataSource(IListDataSource* source)
    {
        if (replaceDataSource(&itemsDataSource, source))
        {
            staleData |= StaleData::Geometry | StaleData::Scenegraph;
            state.isValueMeshState = true;
            state.isItemsMeshStale = true;
        }
    }

    void Dropdown::setIndexDataSource(IIntegralValueDataSource* source)
    {
        if (replaceDataSource(&indexDataSource, source))
        {
            staleData |= StaleData::Geometry | StaleData::Scenegraph;
            state.isValueMeshState = true;
            state.isItemsMeshStale = true;
        }
    }

    ////////////////////////////////////////////////////////////////
    // Generate.
    ////////////////////////////////////////////////////////////////

    void Dropdown::generateLayout(Size size, Size offset)
    {
        // Create layout elements if they do not exist.
        if (!elements.root)
        {
            elements.root   = &layout->setRoot(std::make_unique<VerticalFlow>());
            elements.active = &elements.root->append(std::make_unique<HorizontalFlow>());
            elements.box    = &elements.active->append(std::make_unique<LayoutElement>());
            elements.label  = &elements.active->append(std::make_unique<LayoutElement>());
            elements.items  = &elements.root->append(std::make_unique<LayoutElement>());
        }

        // Style layout elements.
        elements.root->getSize().setWidth(Length(1.0f));
        elements.root->getSize().setHeight(Length(1.0f));
        elements.active->getSize().setWidth(Length(1.0f));
        elements.active->getSize().setHeight(Length(1.0f));

        elements.box->getSize().setWidth(getBoxWidth());
        elements.box->getSize().setHeight(getBoxHeight());
        elements.box->getOuterMargin() = getBoxMargin();

        elements.label->getSize().setWidth(getLabelWidth());
        elements.label->getSize().setHeight(getLabelHeight());
        elements.label->getOuterMargin() = getLabelMargin();

        elements.items->getSize().setWidth(getBoxWidth());
        elements.items->getSize().setHeight(getItemsHeight() * getItemsMax());

        Widget::generateLayout(size, offset);

        // Get blocks for relevant elements.
        auto it =
          std::ranges::find_if(layoutBlocks, [&](const auto& block) { return block.id == elements.box->getId(); });
        blocks.box = &*it;
        it = std::ranges::find_if(layoutBlocks, [&](const auto& block) { return block.id == elements.label->getId(); });
        blocks.label = &*it;
        it = std::ranges::find_if(layoutBlocks, [&](const auto& block) { return block.id == elements.items->getId(); });
        blocks.items = &*it;
    }

    void Dropdown::generateGeometry(sol::MeshManager& meshManager, FontMap& fontMap)
    {
        if (!blocks.box) throw FloahError("Cannot generate geometry. Layout was not generated yet.");

        Generator::Params params{.meshManager = meshManager, .fontMap = fontMap};

        if (!meshes.box)
        {
            RectangleGenerator gen;
            gen.lower    = -0.5f * math::float2(blocks.box->bounds.width(), blocks.box->bounds.height());
            gen.upper    = -gen.lower;
            gen.fillMode = RectangleGenerator::FillMode::Outline;
            gen.margin   = Length(2);
            gen.color    = getColor();
            meshes.box   = &gen.generate(params);
        }

        if (!meshes.highlight)
        {
            RectangleGenerator gen;
            gen.lower        = -0.5f * math::float2(blocks.box->bounds.width(), blocks.box->bounds.height());
            gen.upper        = -gen.lower;
            gen.fillMode     = RectangleGenerator::FillMode::Fill;
            gen.margin       = Length(2);
            gen.color        = getColor();
            meshes.highlight = &gen.generate(params);
        }

        if (!meshes.value || state.isValueMeshState)
        {
            state.isValueMeshState = false;

            // Destroy old mesh.
            if (meshes.value) meshes.value->getMeshManager().destroyMesh(meshes.value->getUuid());

            TextGenerator gen;
            gen.text     = itemsDataSource->getString(indexDataSource->get<size_t>());
            meshes.value = &gen.generate(params);
        }

        if (!meshes.label)
        {
            TextGenerator gen;
            gen.text     = label;
            meshes.label = &gen.generate(params);
        }

        if (state.opened && (meshes.items.empty() || state.isItemsMeshStale))
        {
            state.isItemsMeshStale = false;

            // Destroy old meshes.
            std::ranges::for_each(meshes.items,
                                  [](auto* mesh) { mesh->getMeshManager().destroyMesh(mesh->getUuid()); });

            meshes.items.resize(math::min(itemsDataSource->getSize(), getItemsMax()));

            TextGenerator gen;
            for (size_t i = 0; i < meshes.items.size(); i++)
            {
                gen.text        = itemsDataSource->getString(i + state.scroll);
                meshes.items[i] = &gen.generate(params);
            }
        }

        if (!meshes.itemsBack)
        {
            RectangleGenerator gen;
            gen.lower        = -0.5f * math::float2(blocks.items->bounds.width(), blocks.items->bounds.height());
            gen.upper        = -gen.lower;
            gen.fillMode     = RectangleGenerator::FillMode::Fill;
            gen.margin       = Length(2);
            gen.color        = math::float4(0.5f, 0.5f, 0.5f, 1.0f);  //getColor();
            meshes.itemsBack = &gen.generate(params);
        }

        if (!meshes.itemsHighlight)
        {
            RectangleGenerator gen;
            gen.lower = math::float2(0);
            gen.upper =
              math::float2(static_cast<float>(blocks.items->bounds.width()),
                           static_cast<float>(blocks.items->bounds.height()) / static_cast<float>(getItemsMax()));
            gen.fillMode          = RectangleGenerator::FillMode::Outline;
            gen.margin            = Length(2);
            gen.color             = getColor();
            meshes.itemsHighlight = &gen.generate(params);
        }

        staleData = staleData & ~StaleData::Geometry;
    }

    void Dropdown::generateScenegraph(IScenegraphGenerator& generator)
    {
        if (!meshes.box) throw FloahError("Cannot generate scenegraph. Geometry was not generated yet.");

        if (!nodes.root)
        {
            // TODO: If math::float3 were directly constructible from
            // std::array<std::convertible_to<float> T, 2> and std::convertible_to<float>,
            // this could be a lot prettier:
            nodes.root = &generator.createWidgetNode(panel->getPanelNode());

            auto& widgetMtlNode = nodes.root->addChild(std::make_unique<sol::ForwardMaterialNode>());
            widgetMtlNode.setMaterial(getWidgetMaterial());

            auto& textMtlNode = generator.createTextMaterialNode(*nodes.root, *getTextMaterial());

            auto& widgetTansformNode = generator.createWidgetTransformNode(
              widgetMtlNode,
              math::float3(blocks.box->bounds.center()[0], blocks.box->bounds.center()[1], getInputLayer()));
            widgetTansformNode.getAsNode().addChild(std::make_unique<sol::MeshNode>(*meshes.box));
            nodes.highlight =
              &widgetTansformNode.getAsNode().addChild(std::make_unique<sol::MeshNode>(*meshes.highlight));

            auto& valueTransformNode = generator.createWidgetTransformNode(
              textMtlNode, math::float3(blocks.box->bounds.x0, blocks.box->bounds.y0, getInputLayer()));
            nodes.value = &valueTransformNode.getAsNode().addChild(std::make_unique<sol::MeshNode>(*meshes.value));

            auto& labelTransformNode = generator.createWidgetTransformNode(
              textMtlNode, math::float3(blocks.label->bounds.x0, blocks.label->bounds.y0, getInputLayer()));
            labelTransformNode.getAsNode().addChild(std::make_unique<sol::MeshNode>(*meshes.label));

            nodes.widgetItems = &widgetMtlNode.addChild(std::make_unique<sol::Node>());
            auto& backTansformNode =
              generator.createWidgetTransformNode(*nodes.widgetItems,
                                                  math::float3(static_cast<float>(blocks.items->bounds.center()[0]),
                                                               static_cast<float>(blocks.items->bounds.center()[1]),
                                                               static_cast<float>(getInputLayer()) - 0.2f));
            backTansformNode.getAsNode().addChild(std::make_unique<sol::MeshNode>(*meshes.itemsBack));

            nodes.itemsHighlightTransform = &generator.createWidgetTransformNode(*nodes.widgetItems, math::float3(0));
            nodes.itemsHighlightTransform->getAsNode().addChild(
              std::make_unique<sol::MeshNode>(*meshes.itemsHighlight));

            nodes.textItems = &textMtlNode.addChild(std::make_unique<sol::Node>());
            const auto h    = static_cast<float>(blocks.items->bounds.height()) / static_cast<float>(getItemsMax());
            for (size_t i = 0; i < getItemsMax(); i++)
            {
                auto& trans = generator.createWidgetTransformNode(
                  *nodes.textItems,
                  math::float3(static_cast<float>(blocks.items->bounds.x0),
                               static_cast<float>(blocks.items->bounds.y0) + static_cast<float>(i) * h,
                               static_cast<float>(getInputLayer() + 1)));

                if (i < meshes.items.size())
                    trans.getAsNode().addChild(std::make_unique<sol::MeshNode>(*meshes.items[i]));
                else
                    trans.getAsNode().addChild(std::make_unique<sol::MeshNode>());
            }
        }
        else
        {
            nodes.value->setMesh(meshes.value);

            size_t i = 0;
            for (auto& child : nodes.textItems->getChildren())
            {
                auto& transformNode = dynamic_cast<ITransformNode&>(*child);
                auto& meshNode      = dynamic_cast<sol::MeshNode&>(*transformNode.getAsNode().getChildren()[0]);
                transformNode.setZ(static_cast<float>(getInputLayer() + 1));
                meshNode.setMesh(i < meshes.items.size() ? meshes.items[i] : nullptr);
                i++;
            }
        }

        // Set visibility of highlight.
        if (state.entered && !state.opened)
            nodes.highlight->setTypeMask(0);
        else
            nodes.highlight->setTypeMask(static_cast<uint64_t>(NodeMasks::Disabled));

        // Set visiblity of dropdown items.
        if (state.opened)
        {
            nodes.textItems->setTypeMask(0);
            nodes.widgetItems->setTypeMask(0);

            // Move highlight to item cursor is hovering over.
            if (state.hightlight == -1)
                nodes.itemsHighlightTransform->getAsNode().setTypeMask(static_cast<uint64_t>(NodeMasks::Disabled));
            else
            {
                nodes.itemsHighlightTransform->getAsNode().setTypeMask(0);

                const float offset = static_cast<float>(state.hightlight) *
                                     static_cast<float>(blocks.items->bounds.height()) /
                                     static_cast<float>(getItemsMax());
                nodes.itemsHighlightTransform->setOffset(
                  math::float3(static_cast<float>(blocks.items->bounds.x0),
                               static_cast<float>(blocks.items->bounds.y0) + offset,
                               static_cast<float>(getInputLayer()) - 0.1f));
            }
        }
        else
        {
            nodes.textItems->setTypeMask(static_cast<uint64_t>(NodeMasks::Disabled));
            nodes.widgetItems->setTypeMask(static_cast<uint64_t>(NodeMasks::Disabled));
        }

        staleData = staleData & ~StaleData::Scenegraph;
    }

    ////////////////////////////////////////////////////////////////
    // Input.
    ////////////////////////////////////////////////////////////////

    bool Dropdown::intersect(const math::int2 point) const noexcept
    {
        if (state.opened)
        {
            // Intersect with items.
            const auto       lower = math::int2{blocks.items->bounds.x0, blocks.items->bounds.y0};
            const auto       upper = math::int2{blocks.items->bounds.x1, blocks.items->bounds.y1};
            const math::AABB aabb(lower, upper);
            if (inside(point, aabb)) return true;
        }

        // Intersect with value box.
        const auto       lower = math::int2{blocks.box->bounds.x0, blocks.box->bounds.y0};
        const auto       upper = math::int2{blocks.box->bounds.x1, blocks.box->bounds.y1};
        const math::AABB aabb(lower, upper);
        return inside(point, aabb);
    }

    InputContext::MouseEnterResult Dropdown::onMouseEnter(const InputContext::MouseEnterEvent&)
    {
        state.entered = true;
        staleData |= StaleData::Scenegraph;
        return {};
    }

    InputContext::MouseExitResult Dropdown::onMouseExit(const InputContext::MouseExitEvent&)
    {
        state.entered = false;
        staleData |= StaleData::Scenegraph;
        return {};
    }

    InputContext::MouseClickResult Dropdown::onMouseClick(const InputContext::MouseClickEvent& click)
    {
        if (click.button == InputContext::MouseButton::Left && click.action == InputContext::MouseAction::Press)
        {
            if (state.opened)
            {
                state.opened = false;
                staleData |= StaleData::Scenegraph;

                // Update index (if at all possible).
                if (!indexDataSource || !itemsDataSource || state.hightlight == -1) return {.claim = false};
                indexDataSource->set(state.hightlight + state.scroll);

                return {.claim = false};
            }

            state.opened = true;
            staleData |= StaleData::Geometry | StaleData::Scenegraph;
            return {.claim = true};
        }

        return {.claim = state.opened};
    }

    InputContext::MouseMoveResult Dropdown::onMouseMove(const InputContext::MouseMoveEvent& move)
    {
        staleData |= StaleData::Scenegraph;

        if (state.opened)
        {
            state.hightlight = -1;

            // Move highlight to item being hovered over.
            const auto       lower = math::int2{blocks.items->bounds.x0, blocks.items->bounds.y0};
            const auto       upper = math::int2{blocks.items->bounds.x1, blocks.items->bounds.y1};
            const math::AABB aabb(lower, upper);
            if (inside(move.current, aabb))
            {
                const float y = static_cast<float>(move.current.y - blocks.items->bounds.y0) /
                                static_cast<float>(blocks.items->bounds.height());
                state.hightlight = static_cast<int32_t>(y * static_cast<float>(getItemsMax()));

                // TODO: Include scroll offset.
                // Limit to visible items.
                if (itemsDataSource && static_cast<size_t>(state.hightlight) >= itemsDataSource->getSize())
                    state.hightlight = -1;
            }
        }

        return {};
    }

    InputContext::MouseScrollResult Dropdown::onMouseScroll(const InputContext::MouseScrollEvent& scroll)
    {
        // Only scroll when opened.
        if (!state.opened) return {};

        const auto oldScroll = state.scroll;
        state.scroll -= scroll.scroll.y;
        calculateScroll();

        if (state.scroll != oldScroll)
        {
            staleData |= StaleData::Geometry | StaleData::Scenegraph;
            state.isItemsMeshStale = true;
        }

        return {};
    }

    ////////////////////////////////////////////////////////////////
    // DataListener.
    ////////////////////////////////////////////////////////////////

    void Dropdown::onDataSourceUpdate(DataSource&)
    {
        staleData |= StaleData::Geometry | StaleData::Scenegraph;
        state.isValueMeshState = true;
        state.isItemsMeshStale = true;
    }

    ////////////////////////////////////////////////////////////////
    // Stylesheet getters.
    ////////////////////////////////////////////////////////////////

    Length Dropdown::getBoxHeight() const noexcept
    {
        const auto height = getStylesheetProperty<Length>(dropdown_box_height);
        if (height) return *height;

        const auto size = getStylesheetProperty<Size>(dropdown_box_size);
        if (size) return size->getHeight();

        return dropdown_box_height_default;
    }

    Margin Dropdown::getBoxMargin() const noexcept
    {
        const auto margin = getStylesheetProperty<Margin>(dropdown_box_margin);
        if (margin) return *margin;

        return dropdown_box_margin_default;
    }

    Length Dropdown::getBoxWidth() const noexcept
    {
        const auto width = getStylesheetProperty<Length>(dropdown_box_width);
        if (width) return *width;

        const auto size = getStylesheetProperty<Size>(dropdown_box_size);
        if (size) return size->getWidth();

        return dropdown_box_width_default;
    }

    size_t Dropdown::getItemsMax() const noexcept
    {
        const auto items = getStylesheetProperty<size_t>(dropdown_items_max);
        if (items) return std::max(static_cast<size_t>(1), *items);

        return std::max(static_cast<size_t>(1), dropdown_items_max_default);
    }

    Length Dropdown::getItemsHeight() const noexcept
    {
        const auto height = getStylesheetProperty<Length>(dropdown_items_height);
        if (height) return *height;

        return dropdown_items_height_default;
    }

    Length Dropdown::getLabelHeight() const noexcept
    {
        const auto height = getStylesheetProperty<Length>(dropdown_label_height);
        if (height) return *height;

        const auto size = getStylesheetProperty<Size>(dropdown_label_size);
        if (size) return size->getHeight();

        return dropdown_label_height_default;
    }

    Margin Dropdown::getLabelMargin() const noexcept
    {
        const auto margin = getStylesheetProperty<Margin>(dropdown_label_margin);
        if (margin) return *margin;

        return dropdown_label_margin_default;
    }

    Length Dropdown::getLabelWidth() const noexcept
    {
        const auto width = getStylesheetProperty<Length>(dropdown_label_width);
        if (width) return *width;

        const auto size = getStylesheetProperty<Size>(dropdown_label_size);
        if (size) return size->getWidth();

        return dropdown_label_width_default;
    }

    sol::ForwardMaterialInstance* Dropdown::getTextMaterial() const noexcept
    {
        auto mtl = getStylesheetProperty<sol::ForwardMaterialInstance*>(dropdown_material_text);
        if (mtl) return *mtl;
        mtl = getStylesheetProperty<sol::ForwardMaterialInstance*>(material_text);
        if (mtl) return *mtl;
        return nullptr;
    }

    sol::ForwardMaterialInstance* Dropdown::getWidgetMaterial() const noexcept
    {
        auto mtl = getStylesheetProperty<sol::ForwardMaterialInstance*>(dropdown_material_widget);
        if (mtl) return *mtl;
        mtl = getStylesheetProperty<sol::ForwardMaterialInstance*>(material_widget);
        if (mtl) return *mtl;
        return nullptr;
    }

    math::float4 Dropdown::getColor() const noexcept
    {
        const auto color = getStylesheetProperty<math::float4>("color");
        if (color) return *color;

        return {1, 1, 1, 1};
    }

    ////////////////////////////////////////////////////////////////
    // Utils.
    ////////////////////////////////////////////////////////////////

    void Dropdown::calculateScroll() noexcept
    {
        const auto max = getItemsMax(), size = itemsDataSource ? itemsDataSource->getSize() : 0;
        if (size <= max) { state.scroll = 0; }
        else { state.scroll = math::clamp(state.scroll, 0, static_cast<int32_t>(size - max)); }
    }

}  // namespace floah
