#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "sol/material/fwd.h"
#include "sol/scenegraph/node.h"

namespace floah
{
    class IScenegraphGenerator
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IScenegraphGenerator();

        IScenegraphGenerator(const IScenegraphGenerator&);

        IScenegraphGenerator(IScenegraphGenerator&&) noexcept;

        virtual ~IScenegraphGenerator() noexcept;

        IScenegraphGenerator& operator=(const IScenegraphGenerator&);

        IScenegraphGenerator& operator=(IScenegraphGenerator&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create root node for a widget.
         * \return Node.
         */
        [[nodiscard]] virtual sol::Node& createWidgetNode() = 0;

        [[nodiscard]] virtual sol::Node& createTextMaterialNode(sol::Node& parent, sol::ForwardMaterialInstance& mtlInstance) = 0;

        [[nodiscard]] virtual sol::NodePtr createTransformNode(math::float3 offset) = 0;

        [[nodiscard]] virtual sol::NodePtr createMaterialNode(sol::ForwardMaterialInstance& mtlInstance) = 0;
    };
}  // namespace floah
