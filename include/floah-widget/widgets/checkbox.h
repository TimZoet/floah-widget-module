#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-data/i_bool_data_source.h"
#include "floah-layout/layout_element.h"
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
        static constexpr char checkbox_flow[]             = "checkbox.flow";
        static constexpr char checkbox_box_height[]       = "checkbox.box.height";
        static constexpr char checkbox_box_margin[]       = "checkbox.box.margin";
        static constexpr char checkbox_box_size[]         = "checkbox.box.size";
        static constexpr char checkbox_box_width[]        = "checkbox.box.width";
        static constexpr char checkbox_checkmark_margin[] = "checkbox.checkmark.margin";
        static constexpr char checkbox_label_alignment[]  = "checkbox.label.alignment";
        static constexpr char checkbox_label_height[]     = "checkbox.label.height";
        static constexpr char checkbox_label_margin[]     = "checkbox.label.margin";
        static constexpr char checkbox_label_size[]       = "checkbox.label.size";
        static constexpr char checkbox_label_width[]      = "checkbox.label.width";
        // checkbox_flow_default
        static constexpr Length checkbox_box_height_default = Length(1.0f);
        static constexpr Margin checkbox_box_margin_default = Margin();
        static constexpr Size   checkbox_box_size_default   = Size(0.1f, 1.0f);
        static constexpr Length checkbox_box_width_default  = Length(0.1f);
        // checkbox_checkmark_margin_default
        // checkbox_label_alignment_default
        static constexpr Length checkbox_label_height_default = Length(1.0f);
        static constexpr Margin checkbox_label_margin_default = Margin();
        static constexpr Size   checkbox_label_size_default   = Size(0.9f, 1.0f);
        static constexpr Length checkbox_label_width_default  = Length(0.9f);

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

        void generateScenegraph(IScenegraphGenerator& generator) override;

        ////////////////////////////////////////////////////////////////
        // Input.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool intersect(int32_t x, int32_t y) const noexcept override;

        void onMouseClick(InputContext::MouseClick click) override;

    protected:
        ////////////////////////////////////////////////////////////////
        // Stylesheet getters.
        ////////////////////////////////////////////////////////////////

        // [[nodiscard]] ? getFlow() const noexcept;

        [[nodiscard]] Length getBoxHeight() const noexcept;

        [[nodiscard]] Margin getBoxMargin() const noexcept;

        [[nodiscard]] Length getBoxWidth() const noexcept;

        // [[nodiscard]] Margin getCheckmarkMargin() const noexcept;

        // [[nodiscard]] ? getLabelAlignment() const noexcept;

        [[nodiscard]] Length getLabelHeight() const noexcept;

        [[nodiscard]] Margin getLabelMargin() const noexcept;

        [[nodiscard]] Length getLabelWidth() const noexcept;

        [[nodiscard]] math::float4 getColor() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::string label;

        struct
        {
            HorizontalFlow* root       = nullptr;
            LayoutElement*  box        = nullptr;
            LayoutElement*  label      = nullptr;
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
            sol::Node* box       = nullptr;
            sol::Node* checkmark = nullptr;
            sol::Node* label     = nullptr;
        } nodes;

        IBoolDataSource* dataSource = nullptr;
    };
}  // namespace floah
