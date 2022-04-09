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

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "floah-widget/widgets/widget.h"

namespace floah
{
    class Panel
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Panel();

        Panel(const Panel&) = delete;

        Panel(Panel&&) noexcept = delete;

        ~Panel() noexcept;

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

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

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
    };
}  // namespace floah
