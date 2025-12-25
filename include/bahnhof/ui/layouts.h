#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/element.h"

class Rendering;

namespace UI{

class Host;

class Layout : public Element
{
public:
    template<typename... Args> Layout(Host* h, Args&&... args) : 
        Element(h), elements{std::forward<Args>(args)...} 
        {setpadding(UIVec{0,0});};
    virtual ~Layout() {};
    virtual UIVec getminimumsize() override = 0;
    void render(Rendering* r) final {};
    bool checkclick(UIVec pos) final {return false;};
    void addelements(std::vector<Element*> els);
    virtual void addelement(Element* el);
protected:
    std::vector<Element*> elements;
};

class HBox : public Layout
{
public:
    template<typename... Args> HBox(Host* h, Args&&... args) : 
        Layout(h, std::forward<Args>(args)...) 
        {getminimumsize();};
    UIRect place(UIRect r) override;
    UIVec getminimumsize() override;
    void addelement(Element* el) override;
    bool resizable_x() const override {return numresizableelements_x>0;};
    bool resizable_y() const override {return anyresizableelement_y;};
private:
    std::vector<Coord> minwidths;
    int numresizableelements_x = 0;
    bool anyresizableelement_y = false;
};

class VBox : public Layout
{
public:
    template<typename... Args> VBox(Host* h, Args&&... args) : 
        Layout(h, std::forward<Args>(args)...) 
        {getminimumsize();};
    UIRect place(UIRect r) override;
    UIVec getminimumsize() override;
    void addelement(Element* el) override;
    bool resizable_x() const override {return anyresizableelement_x;};
    bool resizable_y() const override {return numresizableelements_y>0;};
private:
    std::vector<Coord> minheights;
    bool anyresizableelement_x = false;
    int numresizableelements_y = 0;
};

} // namespace UI