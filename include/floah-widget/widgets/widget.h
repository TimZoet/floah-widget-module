#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-layout/layout.h"
#include "floah-put/input_element.h"
#include "floah-viz/font_map.h"
#include "floah-viz/stylesheet.h"
#include "sol/mesh/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-widget/generators/scenegraph_generator.h"

namespace floah
{
    struct Layer;
    class Panel;
    class Widget;

    using PanelPtr  = std::unique_ptr<Panel>;
    using WidgetPtr = std::unique_ptr<Widget>;

    class Widget : public InputElement
    {
        friend class Panel;

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Widget();

        Widget(const Widget&) = delete;

        Widget(Widget&&) noexcept = delete;

        ~Widget() noexcept override;

        Widget& operator=(const Widget&) = delete;

        Widget& operator=(Widget&&) noexcept = delete;

        /**
         * \brief Destroy this widget (internally calls panel->destroyWidget(*this)).
         */
        void destroy();

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the panel this widget is in.
         * \return Panel.
         */
        [[nodiscard]] Panel& getPanel() noexcept;

        /**
         * \brief Get the panel this widget is in.
         * \return Panel.
         */
        [[nodiscard]] const Panel& getPanel() const noexcept;

        /**
         * \brief Get the optional layer this widget is in.
         * \return Layer.
         */
        [[nodiscard]] Layer* getLayer() noexcept;

        /**
         * \brief Get the optional layer this widget is in.
         * \return Layer.
         */
        [[nodiscard]] const Layer* getLayer() const noexcept;

        /**
         * \brief Get the widget layout.
         * \return Widget layout.
         */
        [[nodiscard]] Layout& getLayout() noexcept;

        /**
         * \brief Get the widget layout.
         * \return Widget layout.
         */
        [[nodiscard]] const Layout& getLayout() const noexcept;

        /**
         * \brief Get the element in the panel layout this widget is attached to.
         * \return Element.
         */
        [[nodiscard]] LayoutElement* getPanelLayoutElement() const noexcept;

        /**
         * \brief Get the widget stylesheet.
         * \return Stylesheet or nullptr.
         */
        [[nodiscard]] Stylesheet* getStylesheet() noexcept;

        /**
         * \brief Get the widget stylesheet.
         * \return Stylesheet or nullptr.
         */
        [[nodiscard]] const Stylesheet* getStylesheet() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the element in the panel layout this widget is attached to.
         * \param element Element.
         */
        void setPanelLayoutElement(LayoutElement& element);

        /**
         * \brief Set the widget stylesheet.
         * \param sheet Stylesheet or nullptr.
         */
        void setStylesheet(Stylesheet* sheet) noexcept;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        virtual void generateLayout(Size size, Size offset);

        virtual void generateGeometry(sol::MeshManager& meshManager, FontMap& fontMap) = 0;

        virtual void generateScenegraph(IScenegraphGenerator& generator) = 0;

        ////////////////////////////////////////////////////////////////
        // Input.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] const InputElement* getInputParent() const noexcept override;

        [[nodiscard]] int32_t getInputLayer() const noexcept override;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Panel this widget is in.
         */
        Panel* panel = nullptr;

        /**
         * \brief Optional layer this widget is in.
         */
        Layer* layer = nullptr;

        /**
         * \brief Widget layout.
         */
        LayoutPtr layout;

        // TODO: It is arguably a bit weird that we have a whole uuid system and then still use the pointer here.
        // We know that the panel layout is persistent, so it is guaranteed that this pointer will remain valid, but still...
        /**
         * \brief Element in the panel layout this widget is attached to.
         */
        LayoutElement* panelElement = nullptr;

        std::vector<Block> blocks;

        /**
         * \brief Widget stylesheet.
         */
        Stylesheet* stylesheet = nullptr;
    };
}  // namespace floah
