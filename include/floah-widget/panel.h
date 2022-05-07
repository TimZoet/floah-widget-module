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
#include "floah-put/input_context.h"
#include "floah-put/input_element.h"
#include "sol/mesh/fwd.h"
#include "sol/scenegraph/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-widget/widgets/widget.h"

namespace floah
{
    class Panel : public InputElement
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Panel();

        Panel(const Panel&) = delete;

        Panel(Panel&&) noexcept = delete;

        ~Panel() noexcept override;

        Panel& operator=(const Panel&) = delete;

        Panel& operator=(Panel&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the panel layout.
         * \return Panel layout.
         */
        [[nodiscard]] Layout& getLayout() noexcept;

        /**
         * \brief Get the panel layout.
         * \return Panel layout.
         */
        [[nodiscard]] const Layout& getLayout() const noexcept;

        // TODO: Remove many of these getters and setters and instead require them on construction?
        // Would greatly limit the number of accidental errors due to uninited values, and simplify
        // error handling.
        [[nodiscard]] InputContext* getInputContext() noexcept;

        [[nodiscard]] InputContext* getInputContext() const noexcept;

        [[nodiscard]] sol::MeshManager* getMeshManager() noexcept;

        [[nodiscard]] const sol::MeshManager* getMeshManager() const noexcept;

        [[nodiscard]] FontMap* getFontMap() noexcept;

        [[nodiscard]] const FontMap* getFontMap() const noexcept;

        [[nodiscard]] sol::Node* getRootNode() noexcept;

        [[nodiscard]] const sol::Node* getRootNode() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setInputContext(InputContext& context) noexcept;

        void setMeshManager(sol::MeshManager& manager) noexcept;

        void setFontMap(FontMap& map) noexcept;

        void setRootNode(sol::Node& node) noexcept;

        ////////////////////////////////////////////////////////////////
        // Widgets.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Add a widget to this panel.
         * \tparam T Widget type.
         * \param widget Widget.
         * \return Widget.
         */
        template<std::derived_from<Widget> T>
        T& addWidget(std::unique_ptr<T> widget)
        {
            T& ref = *widget;
            addWidgetImpl(std::move(widget));
            return ref;
        }

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate the panel layout.
         */
        void generatePanelLayout();

        /**
         * \brief Generate the widget layouts.
         */
        void generateWidgetLayouts();

        /**
         * \brief Generate the geometry.
         */
        void generateGeometry() const;

        /**
         * \brief Generate the scenegraph.
         */
        void generateScenegraph() const;

        ////////////////////////////////////////////////////////////////
        // Input.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool intersect(int32_t x, int32_t y) const override;

    private:
        void addWidgetImpl(WidgetPtr widget);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Panel layout.
         */
        LayoutPtr layout;

        /**
         * \brief List of widgets in this panel.
         */
        std::vector<WidgetPtr> widgets;

        /**
         * \brief Layout blocks.
         */
        std::vector<Block> blocks;

        /**
         * \brief Input context.
         */
        InputContext* inputContext = nullptr;

        /**
         * \brief MeshManager.
         */
        sol::MeshManager* meshManager = nullptr;

        /**
         * \brief FontMap.
         */
        FontMap* fontMap = nullptr;

        /**
         * \brief Node to append all new nodes to.
         */
        sol::Node* rootNode = nullptr;
    };
}  // namespace floah
