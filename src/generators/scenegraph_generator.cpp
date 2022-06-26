#include "floah-widget/generators/scenegraph_generator.h"

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    IScenegraphGenerator::IScenegraphGenerator() = default;

    IScenegraphGenerator::IScenegraphGenerator(const IScenegraphGenerator&) = default;

    IScenegraphGenerator::IScenegraphGenerator(IScenegraphGenerator&&) noexcept = default;

    IScenegraphGenerator::~IScenegraphGenerator() noexcept = default;

    IScenegraphGenerator& IScenegraphGenerator::operator=(const IScenegraphGenerator&) = default;

    IScenegraphGenerator& IScenegraphGenerator::operator=(IScenegraphGenerator&&) noexcept = default;
}  // namespace floah
