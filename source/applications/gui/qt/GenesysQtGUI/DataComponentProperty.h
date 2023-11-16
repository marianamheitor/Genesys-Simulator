#ifndef DATACOMPONENTPROPERTY_H
#define DATACOMPONENTPROPERTY_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>


class DataComponentProperty {
public:
    DataComponentProperty();
    ~DataComponentProperty();

public:
    void open_window(QString newValue);

private:
    QWidget* _window;
    QTreeWidget* _view;
};

#endif // DATACOMPONENTPROPERTY_H
