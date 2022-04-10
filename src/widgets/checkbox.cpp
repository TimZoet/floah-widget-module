#include "floah-widget/widgets/checkbox.h"

namespace floah
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Checkbox::Checkbox() : Widget()
    {
        elements.rootElement  = &layout->setRoot(std::make_unique<HorizontalFlow>());
        elements.boxElement   = &elements.rootElement->append(std::make_unique<Element>());
        elements.labelElement = &elements.rootElement->append(std::make_unique<Element>());

        elements.rootElement->getSize().setWidth(Length(1.0f));
        elements.rootElement->getSize().setHeight(Length(1.0f));
        elements.boxElement->getSize().setWidth(Length(0.2f));
        elements.boxElement->getSize().setHeight(Length(1.0f));
        elements.labelElement->getSize().setWidth(Length(0.8f));
        elements.labelElement->getSize().setHeight(Length(1.0f));
    }

    Checkbox::~Checkbox() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const std::string& Checkbox::getLabel() const noexcept { return label; }

    bool Checkbox::hasDataSource() const noexcept { return dataSource != nullptr; }

    IBoolDataSource* Checkbox::getDataSource() const noexcept { return dataSource; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Checkbox::setLabel(std::string l) { label = std::move(l); }

    void Checkbox::setDataSource(IBoolDataSource* source) { dataSource = source; }

}  // namespace floah
