#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-data/i_bool_data_source.h"
#include "floah-layout/element.h"
#include "floah-layout/elements/horizontal_flow.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-widget/widgets/widget.h"

namespace floah
{
    class Checkbox : public Widget
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Checkbox();

        Checkbox(const Checkbox&) = delete;

        Checkbox(Checkbox&&) noexcept = delete;

        ~Checkbox() noexcept override;

        Checkbox& operator=(const Checkbox&) = delete;

        Checkbox& operator=(Checkbox&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual const std::string& getLabel() const noexcept;

        [[nodiscard]] bool hasDataSource() const noexcept;

        [[nodiscard]] virtual IBoolDataSource* getDataSource() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        virtual void setLabel(std::string l);

        virtual void setDataSource(IBoolDataSource* source);

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        void generateLayout(Size size, Size offset) override;

        void generateGeometry(sol::MeshManager& meshManager, FontMap& fontMap) override;

        void generateScenegraph(sol::Node& parentNode) override;

        ////////////////////////////////////////////////////////////////
        // Input.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool intersect(int32_t x, int32_t y) const override;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::string label;

        struct
        {
            HorizontalFlow* root       = nullptr;
            Element*        box        = nullptr;
            Element*        label      = nullptr;
            Block*          boxBlock   = nullptr;
            Block*          labelBlock = nullptr;
        } elements;

        struct
        {
            sol::IMesh* box       = nullptr;
            sol::IMesh* checkmark = nullptr;
            sol::IMesh* label     = nullptr;
        } meshes;

        struct
        {
            sol::MeshNode* box       = nullptr;
            sol::MeshNode* checkmark = nullptr;
            sol::MeshNode* label     = nullptr;
        } nodes;

        IBoolDataSource* dataSource = nullptr;
    };
}  // namespace floah
