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

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::string label;

        struct
        {
            HorizontalFlow* rootElement = nullptr;

            Element* boxElement = nullptr;

            Element* labelElement = nullptr;
        } elements;

        IBoolDataSource* dataSource = nullptr;
    };
}  // namespace floah
