#include "floah-widget/widgets/checkbox.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-layout/utils/floah_error.h"
#include "floah-viz/generators/circle_generator.h"
#include "floah-viz/generators/rectangle_generator.h"
#include "floah-viz/generators/text_generator.h"
#include "sol/mesh/flat_mesh.h"
#include "sol/mesh/mesh_manager.h"
#include "sol/scenegraph/node.h"
#include "sol/scenegraph/drawable/mesh_node.h"

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Checkbox::Checkbox() : Widget()
    {
        elements.root  = &layout->setRoot(std::make_unique<HorizontalFlow>());
        elements.box   = &elements.root->append(std::make_unique<Element>());
        elements.label = &elements.root->append(std::make_unique<Element>());


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

            gen.lower    = math::float2(elements.boxBlock->bounds.x0, elements.boxBlock->bounds.y0);
            gen.upper    = math::float2(elements.boxBlock->bounds.x1, elements.boxBlock->bounds.y1);
            gen.fillMode = RectangleGenerator::FillMode::Fill;
            gen.margin   = Length(0.1f);
            meshes.box   = &gen.generate(params);
        }

        {
            TextGenerator gen;
            gen.text     = label;
            gen.position = math::float2(elements.labelBlock->bounds.x0, elements.labelBlock->bounds.y0);
            meshes.label = &gen.generate(params);
        }
    }

    void Checkbox::generateScenegraph(sol::Node& parentNode)
    {
        if (!meshes.box) throw FloahError("Cannot generate scenegraph. Geometry was not generated yet.");

        // TODO: Update nodes if they already exist.
        nodes.box = &parentNode.addChild(std::make_unique<sol::MeshNode>(*meshes.box));
        //nodes.checkmark = &parentNode.addChild(std::make_unique<sol::MeshNode>(*meshes.checkmark));
        nodes.label = &parentNode.addChild(std::make_unique<sol::MeshNode>(*meshes.label));
    }

    ////////////////////////////////////////////////////////////////
    // Input.
    ////////////////////////////////////////////////////////////////

    bool Checkbox::intersect(const int32_t x, const int32_t y) const
    {
        const auto offset = math::int2(layout->getOffset().getWidth().get(), layout->getOffset().getHeight().get());
        const auto size   = math::int2(layout->getSize().getWidth().get(), layout->getSize().getHeight().get());
        const math::AABB aabb(offset, offset + size);
        return inside(math::int2(x, y), aabb);
    }

}  // namespace floah
