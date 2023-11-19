#include "DataComponentProperty.h"

DataComponentProperty::DataComponentProperty(PropertyEditorGenesys* editor, SimulationControl* property) {
    _window = new QWidget;
    _view = new QTreeWidget(_window);
    _add = new QPushButton("Add", _window);
    _remove = new QPushButton("Remove", _window);
    _confirmation = new QInputDialog(_window);

    _add->move(270,15);
    _remove->move(270,50);

    QObject::connect(_add, &QPushButton::clicked, this, [this, editor, property]{addElement(editor, property);});
    QObject::connect(_remove, &QPushButton::clicked, this, [this, editor, property]{removeElement(editor, property);});
};

DataComponentProperty::~DataComponentProperty() {
    delete _window;
    delete _view;
    delete _add;
    delete _remove;
    delete _confirmation;
};

void DataComponentProperty::open_window() {
    _window->show();
};

void DataComponentProperty::addElement(PropertyEditorGenesys* editor, SimulationControl* property) {
    // add element
    QTreeWidgetItem* newItem = new QTreeWidgetItem(_view);
    _view->addTopLevelItem(newItem);

    QString newValue = _confirmation->getText(_confirmation, "Item", "Enter the name:");
    newItem->setText(0,newValue);

    // change property
    editor->changeProperty(property, newValue.toStdString(), false);
};

void DataComponentProperty::removeElement(PropertyEditorGenesys* editor, SimulationControl* property) {
    // select element to remove
    QTreeWidgetItem* selectedItem = _view->currentItem();
    int column = _view->currentColumn();
    QString itemValue = selectedItem->text(column);

    // remove the selected element
    delete selectedItem;

    // change property
    editor->changeProperty(property, itemValue.toStdString(), true);
};
