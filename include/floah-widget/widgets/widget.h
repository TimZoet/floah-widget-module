#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "floah-data/data_listener.h"
#include "floah-layout/layout.h"
#include "floah-put/input_element.h"
#include "floah-viz/font_map.h"
#include "floah-viz/stylesheet.h"
#include "floah-viz/scenegraph/scenegraph_generator.h"
#include "sol/mesh/fwd.h"

namespace floah
{
    struct Layer;
    class Panel;
    class Widget;

    using PanelPtr  = std::unique_ptr<Panel>;
    using WidgetPtr = std::unique_ptr<Widget>;

    class Widget : public InputElement, public DataListener
    {
        friend class Panel;

    public:
        static constexpr char material_text[]   = "material.text";
        static constexpr char material_widget[] = "material.widget";

        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class StaleData
        {
            Layout     = 1,
            Geometry   = 2,
            Scenegraph = 4,
            All        = Layout | Geometry | Scenegraph
        };

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

        /**
         * \brief Get the panel stylesheet.
         * \return Stylesheet or nullptr.
         */
        [[nodiscard]] Stylesheet* getPanelStylesheet() noexcept;

        /**
         * \brief Get the panel stylesheet.
         * \return Stylesheet or nullptr.
         */
        [[nodiscard]] const Stylesheet* getPanelStylesheet() const noexcept;

        /**
         * \brief Get the data that is stale and needs to be regenerated.
         * \return StaleData.
         */
        [[nodiscard]] StaleData getStaleData() const noexcept;

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

        /**
         * \brief Replace current data source with new data soruce. Automatically takes care of removing and adding data listener.
         * \tparam T DataSource.
         * \param current Current data source.
         * \param replacement New data source.
         * \return True if value was updated, false if current and replacement were the same.
         */
        template<typename T>
        bool replaceDataSource(T** current, T* replacement)
        {
            if (*current == replacement) return false;
            if (*current) (*current)->removeDataListener(*this);
            *current = replacement;
            if (*current) (*current)->addDataListener(*this);
            return true;
        }

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
        // Stylesheet getter.
        ////////////////////////////////////////////////////////////////

        template<typename T, typename N>
        [[nodiscard]] std::optional<T> getStylesheetProperty(N name) const
        {
            // Try to retrieve property from widget stylesheet.
            if (stylesheet)
            {
                const auto opt = stylesheet->get<T>(name);
                if (opt) return *opt;
            }

            // Try to retrieve property from panel stylesheet.
            if (getPanelStylesheet())
            {
                const auto opt = getPanelStylesheet()->get<T>(name);
                if (opt) return *opt;
            }

            return {};
        }

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

        /**
         * \brief Generated layout blocks.
         */
        std::vector<Block> layoutBlocks;

        /**
         * \brief Widget stylesheet.
         */
        Stylesheet* stylesheet = nullptr;

        StaleData staleData = StaleData::All;
    };
}  // namespace floah
