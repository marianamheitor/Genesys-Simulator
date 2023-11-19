#ifndef PROPERTY_H
#define PROPERTY_H

#include "SimulationControlAndResponse.h"
#include "ModelComponent.h"

class PropertyEditorGenesys {
public: // constructor and destructor
    PropertyEditorGenesys() = default;
    virtual ~PropertyEditorGenesys() = default;
public: // get
    std::list<ModelComponent*> getElements();
public:
    void changeProperty(SimulationControl* property, std::string value, bool remove=false);
    SimulationControl* findProperty(std::string id, std::string attribute);
    void addElement(ModelComponent* component);
private: // attributes
    std::list<ModelComponent*> _elements;
};

#endif
