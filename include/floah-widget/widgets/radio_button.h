#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

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
    class RadioButton : public Widget
    {
    public:
        static constexpr char radiobutton_flow[]             = "radiobutton.flow";
        static constexpr char radiobutton_box_height[]       = "radiobutton.box.height";
        static constexpr char radiobutton_box_margin[]       = "radiobutton.box.margin";
        static constexpr char radiobutton_box_size[]         = "radiobutton.box.size";
        static constexpr char radiobutton_box_width[]        = "radiobutton.box.width";
        static constexpr char radiobutton_checkmark_margin[] = "radiobutton.checkmark.margin";
        static constexpr char radiobutton_label_alignment[]  = "radiobutton.label.alignment";
        static constexpr char radiobutton_label_height[]     = "radiobutton.label.height";
        static constexpr char radiobutton_label_margin[]     = "radiobutton.label.margin";
        static constexpr char radiobutton_label_size[]       = "radiobutton.label.size";
        static constexpr char radiobutton_label_width[]      = "radiobutton.label.width";
        static constexpr char radiobutton_material_text[]    = "radiobutton.material.text";
        static constexpr char radiobutton_material_widget[]  = "radiobutton.material.widget";
        // radiobutton_flow_default
        static constexpr Length radiobutton_box_height_default = Length(1.0f);
        static constexpr Margin radiobutton_box_margin_default = Margin();
        static constexpr Size   radiobutton_box_size_default   = Size(0.1f, 1.0f);
        static constexpr Length radiobutton_box_width_default  = Length(0.1f);
        // radiobutton_checkmark_margin_default
        // radiobutton_label_alignment_default
        static constexpr Length radiobutton_label_height_default = Length(1.0f);
        static constexpr Margin radiobutton_label_margin_default = Margin();
        static constexpr Size   radiobutton_label_size_default   = Size(0.9f, 1.0f);
        static constexpr Length radiobutton_label_width_default  = Length(0.9f);

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RadioButton();

        RadioButton(RadioButton& main);

        RadioButton(const RadioButton&) = delete;

        RadioButton(RadioButton&&) noexcept = delete;

        ~RadioButton() noexcept override;

        RadioButton& operator=(const RadioButton&) = delete;

        RadioButton& operator=(RadioButton&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual const std::string& getLabel() const noexcept;

        [[nodiscard]] bool hasDataSource() const noexcept;

        [[nodiscard]] virtual IBoolDataSource* getDataSource() const noexcept;

        /**
         * \brief Returns whether this is the main button of one or more radio buttons,
         * i.e. it has no main button assigned itself.
         * \return True if main button, false otherwise.
         */
        [[nodiscard]] bool isMainButton() const noexcept;

        /**
         * \brief Get the main button of this radio button's group.
         * \return Pointer to main button, or nullptr if this is the main button.
         */
        [[nodiscard]] RadioButton* getMainButton() const noexcept;

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

        [[nodiscard]] bool intersect(math::int2 point) const noexcept override;

        [[nodiscard]] InputContext::MouseEnterResult onMouseEnter(const InputContext::MouseEnterEvent& enter) override;

        [[nodiscard]] InputContext::MouseExitResult onMouseExit(const InputContext::MouseExitEvent& exit) override;

        [[nodiscard]] InputContext::MouseClickResult onMouseClick(const InputContext::MouseClickEvent& click) override;

        ////////////////////////////////////////////////////////////////
        // DataListener.
        ////////////////////////////////////////////////////////////////

        void onDataSourceUpdate(DataSource& source) override;

    protected:
        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void setMain(RadioButton& setButton);

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

        [[nodiscard]] sol::ForwardMaterialInstance* getTextMaterial() const noexcept;

        [[nodiscard]] sol::ForwardMaterialInstance* getWidgetMaterial() const noexcept;

        [[nodiscard]] math::float4 getColor() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::string label;

        struct
        {
            HorizontalFlow* root  = nullptr;
            LayoutElement*  box   = nullptr;
            LayoutElement*  label = nullptr;
        } elements;

        struct
        {
            Block* box   = nullptr;
            Block* label = nullptr;
        } blocks;

        struct
        {
            sol::IMesh* box       = nullptr;
            sol::IMesh* highlight = nullptr;
            sol::IMesh* checkmark = nullptr;
            sol::IMesh* label     = nullptr;
        } meshes;

        struct
        {
            sol::Node* root      = nullptr;
            sol::Node* highlight = nullptr;
            sol::Node* checkmark = nullptr;
        } nodes;

        IBoolDataSource* dataSource = nullptr;

        /**
         * \brief Main button in the radio button group. Only set if this is not the main button.
         */
        RadioButton* mainButton = nullptr;

        /**
         * \brief Sibling/child buttons. Only set if this is the main button.
         */
        std::vector<RadioButton*> siblings;

        struct
        {
            bool entered = false;
        } state;
    };
}  // namespace floah
