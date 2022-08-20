#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-data/i_integral_value_data_source.h"
#include "floah-data/i_list_data_source.h"
#include "floah-layout/layout_element.h"
#include "floah-layout/elements/horizontal_flow.h"
#include "floah-layout/elements/vertical_flow.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-widget/widgets/widget.h"

namespace floah
{
    class Dropdown : public Widget
    {
    public:
        static constexpr char dropdown_flow[]            = "dropdown.flow";
        static constexpr char dropdown_box_height[]      = "dropdown.box.height";
        static constexpr char dropdown_box_margin[]      = "dropdown.box.margin";
        static constexpr char dropdown_box_size[]        = "dropdown.box.size";
        static constexpr char dropdown_box_width[]       = "dropdown.box.width";
        static constexpr char dropdown_items_height[]    = "dropdown.items.height";
        static constexpr char dropdown_items_max[]       = "dropdown.items.max";
        static constexpr char dropdown_label_alignment[] = "dropdown.label.alignment";
        static constexpr char dropdown_label_height[]    = "dropdown.label.height";
        static constexpr char dropdown_label_margin[]    = "dropdown.label.margin";
        static constexpr char dropdown_label_size[]      = "dropdown.label.size";
        static constexpr char dropdown_label_width[]     = "dropdown.label.width";
        static constexpr char dropdown_material_text[]   = "dropdown.material.text";
        static constexpr char dropdown_material_widget[] = "dropdown.material.widget";
        // dropdown_flow_default

        static constexpr Length dropdown_box_height_default = Length(1.0f);
        static constexpr Margin dropdown_box_margin_default = Margin();
        static constexpr Size   dropdown_box_size_default   = Size(0.1f, 1.0f);
        static constexpr Length dropdown_box_width_default  = Length(0.1f);
        // dropdown_checkmark_margin_default
        // dropdown_label_alignment_default
        static constexpr Length dropdown_items_height_default = Length(1.0f);
        static constexpr size_t dropdown_items_max_default    = 20;
        static constexpr Length dropdown_label_height_default = Length(1.0f);
        static constexpr Margin dropdown_label_margin_default = Margin();
        static constexpr Size   dropdown_label_size_default   = Size(0.9f, 1.0f);
        static constexpr Length dropdown_label_width_default  = Length(0.9f);

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Dropdown();

        Dropdown(const Dropdown&) = delete;

        Dropdown(Dropdown&&) noexcept = delete;

        ~Dropdown() noexcept override;

        Dropdown& operator=(const Dropdown&) = delete;

        Dropdown& operator=(Dropdown&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual const std::string& getLabel() const noexcept;

        [[nodiscard]] bool hasItemsDataSource() const noexcept;

        [[nodiscard]] bool hasIndexDataSource() const noexcept;

        [[nodiscard]] virtual IListDataSource* getItemsDataSource() const noexcept;

        [[nodiscard]] virtual IIntegralValueDataSource* getIndexDataSource() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        virtual void setLabel(std::string l);

        virtual void setItemsDataSource(IListDataSource* source);

        virtual void setIndexDataSource(IIntegralValueDataSource* source);

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

        void onMouseEnter() override;

        void onMouseExit() override;

        [[nodiscard]] InputContext::MouseClickResult onMouseClick(InputContext::MouseClick click) override;

    protected:
        ////////////////////////////////////////////////////////////////
        // Stylesheet getters.
        ////////////////////////////////////////////////////////////////

        // [[nodiscard]] ? getFlow() const noexcept;



        [[nodiscard]] Length getBoxHeight() const noexcept;

        [[nodiscard]] Margin getBoxMargin() const noexcept;

        [[nodiscard]] Length getBoxWidth() const noexcept;

        // [[nodiscard]] Margin getCheckmarkMargin() const noexcept;

        [[nodiscard]] Length getItemsHeight() const noexcept;

        [[nodiscard]] size_t getItemsMax() const noexcept;

        // [[nodiscard]] ? getLabelAlignment() const noexcept;

        [[nodiscard]] Length getLabelHeight() const noexcept;

        [[nodiscard]] Margin getLabelMargin() const noexcept;

        [[nodiscard]] Length getLabelWidth() const noexcept;

        [[nodiscard]] sol::ForwardMaterialInstance* getTextMaterial() const noexcept;

        [[nodiscard]] sol::ForwardMaterialInstance* getWidgetMaterial() const noexcept;

        [[nodiscard]] math::float4 getColor() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::string label;

        struct
        {
            VerticalFlow*   root   = nullptr;
            HorizontalFlow* active = nullptr;
            LayoutElement*  box    = nullptr;
            LayoutElement*  label  = nullptr;
            LayoutElement*  items  = nullptr;

        } elements;

        struct
        {
            Block* box   = nullptr;
            Block* label = nullptr;
            Block* items = nullptr;
        } blocks;

        struct
        {
            sol::IMesh*              box       = nullptr;
            sol::IMesh*              highlight = nullptr;
            sol::IMesh*              value     = nullptr;
            sol::IMesh*              label     = nullptr;
            std::vector<sol::IMesh*> items;
            sol::IMesh*              itemsBack      = nullptr;
            sol::IMesh*              itemsHighlight = nullptr;
        } meshes;

        struct
        {
            sol::Node* root                    = nullptr;
            sol::Node* highlight               = nullptr;
            sol::Node* widgetItems             = nullptr;
            sol::Node* itemsHighlightTransform = nullptr;
            sol::Node* textItems               = nullptr;
        } nodes;

        IListDataSource* itemsDataSource = nullptr;

        IIntegralValueDataSource* indexDataSource = nullptr;

        struct
        {
            bool entered = false;
            bool opened  = false;
        } state;
    };
}  // namespace floah
