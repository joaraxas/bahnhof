#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/element.h"

class Rendering;

namespace UI{

class Host;

class Layout : public Element
{
public:
    Layout(Host* h);
    virtual ~Layout() {};
    void addelements(std::vector<Element*> els);
    virtual void addelement(Element* el) = 0;
    void render(Rendering* r) final {};
    bool checkclick(UIVec pos) final {return false;};
};

class Box
{
protected:
    void addelement(Element* el);
    std::vector<Element*> elements;
    std::vector<UIVec> minsizes;
    int numresizableelements_x = 0;
    int numresizableelements_y = 0;
    bool resizable_x() const {return numresizableelements_x>0;};
    bool resizable_y() const {return numresizableelements_y>0;};
};

class HBox : public Layout, private Box
{
public:
    template<typename... Args> HBox(Host* h, Args&&... args) : 
        Layout(h) {addelements({args...});};
    void addelement(Element* el) override {Box::addelement(el);};
    UIVec getminimumsize() override;
    UIRect place(UIRect r) override;
    bool resizable_x() const override {return Box::resizable_x();};
    bool resizable_y() const override {return Box::resizable_y();};
};

class VBox : public Layout, private Box
{
public:
    template<typename... Args> VBox(Host* h, Args&&... args) : 
        Layout(h) {addelements({args...});};
    void addelement(Element* el) override {Box::addelement(el);};
    UIVec getminimumsize() override;
    UIRect place(UIRect r) override;
    bool resizable_x() const override {return Box::resizable_x();};
    bool resizable_y() const override {return Box::resizable_y();};
};

} // namespace UI