#include "DataComponentProperty.h"

DataComponentProperty::DataComponentProperty() {
    _window = new QWidget;
    _view = new QTreeWidget(_window);
};

DataComponentProperty::~DataComponentProperty() {
    delete _window;
    delete _view;
};

void DataComponentProperty::open_window(QString newValue) {
    // add element
    QTreeWidgetItem* newItem = new QTreeWidgetItem(_view);
    _view->addTopLevelItem(newItem);
    newItem->setText(0,newValue);
    
    _window->show();
}
