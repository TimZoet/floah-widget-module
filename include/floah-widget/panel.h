#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <unordered_map>
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

#include "floah-widget/layer.h"
#include "floah-widget/generators/scenegraph_generator.h"
#include "floah-widget/widgets/widget.h"

namespace floah
{
    class Panel : public InputElement
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Panel() = delete;

        explicit Panel(InputContext& context);

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

        /**
         * \brief Get the input context.
         * \return Input context.
         */
        [[nodiscard]] InputContext& getInputContext() noexcept;

        /**
         * \brief Get the input context.
         * \return Input context.
         */
        [[nodiscard]] const InputContext& getInputContext() const noexcept;

        /**
         * \brief Get the panel stylesheet.
         * \return Stylesheet or nullptr.
         */
        [[nodiscard]] Stylesheet* getStylesheet() noexcept;

        /**
         * \brief Get the panel stylesheet.
         * \return Stylesheet or nullptr.
         */
        [[nodiscard]] const Stylesheet* getStylesheet() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the panel stylesheet.
         * \param sheet Stylesheet or nullptr.
         */
        void setStylesheet(Stylesheet* sheet) noexcept;

        ////////////////////////////////////////////////////////////////
        // Layers.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new layer.
         * \param layerName Name of layer.
         * \param depth Depth of layer.
         * \return Layer.
         */
        Layer& createLayer(std::string layerName, int32_t depth);

        /**
         * \brief Retrieve a layer by name.
         * \param layerName Layer name.
         * \return Layer.
         */
        [[nodiscard]] Layer& getLayer(const std::string& layerName) const;

        /**
         * \brief Destroy a layer. Automatically removes all widgets in this layer from it.
         * \param layerName Name of layer.
         */
        void destroyLayer(const std::string& layerName);

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
            addWidgetImpl(std::move(widget), nullptr);
            return ref;
        }

        /**
         * \brief Add a widget to this panel.
         * \tparam T Widget type.
         * \param widget Widget.
         * \param layer Optional layer to add widget to.
         * \return Widget.
         */
        template<std::derived_from<Widget> T>
        T& addWidget(std::unique_ptr<T> widget, Layer& layer)
        {
            T& ref = *widget;
            addWidgetImpl(std::move(widget), &layer);
            return ref;
        }

        /**
         * \brief Destroy a widget, completely removing it from this panel.
         * \param widget Widget.
         */
        void destroyWidget(Widget& widget);

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
        void generateGeometry(sol::MeshManager& meshManager, FontMap& fontMap) const;

        /**
         * \brief Generate the scenegraph.
         */
        void generateScenegraph(IScenegraphGenerator& generator) const;

        ////////////////////////////////////////////////////////////////
        // Input.
        ////////////////////////////////////////////////////////////////

        // TODO: Support sorting of panels by implementing this method (and whatever else is needed for that).
        // [[nodiscard]] int32_t getInputLayer() const noexcept override;

        [[nodiscard]] bool intersect(int32_t x, int32_t y) const noexcept override;

    private:
        void addWidgetImpl(WidgetPtr widget, Layer* layer);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Panel layout.
         */
        LayoutPtr layout;

        /**
         * \brief List of layers in this panel.
         */
        std::unordered_map<std::string, std::unique_ptr<Layer>> layers;

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
         * \brief Panel stylesheet.
         */
        Stylesheet* stylesheet = nullptr;
    };
}  // namespace floah
