#ifndef DATACOMPONENTPROPERTY_H
#define DATACOMPONENTPROPERTY_H

#include <iostream>

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QInputDialog>

#include "../../../../kernel/simulator/PropertyGenesys.h"


class DataComponentProperty : public QObject {
public:
    DataComponentProperty(PropertyEditorGenesys* editor, SimulationControl* property);
    ~DataComponentProperty();

public:
    void open_window();

private Q_SLOTS:
    void addElement(PropertyEditorGenesys* editor, SimulationControl* property);
    void removeElement(PropertyEditorGenesys* editor, SimulationControl* property);

private:
    QWidget* _window;
    QTreeWidget* _view;
    QPushButton* _add;
    QPushButton* _remove;
    QInputDialog* _confirmation;
};

#endif // DATACOMPONENTPROPERTY_H
