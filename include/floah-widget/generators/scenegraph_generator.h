#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
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

        [[nodiscard]] virtual sol::Node& createWidgetNode(math::float3 offset) = 0;

        [[nodiscard]] virtual sol::Node& createTextNode(math::float3 offset) = 0;
    };
}  // namespace floah
