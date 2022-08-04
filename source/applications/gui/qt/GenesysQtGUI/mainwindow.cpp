#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogBreakpoint.h"
#include "../../../../kernel/simulator/SinkModelComponent.h"
#include "../../../../kernel/simulator/Attribute.h"
#include "ModelGraphicsScene.h"
#include <string>
#include <fstream>
#include <streambuf>
#include <QMessageBox>
#include <QFileDialog>
#include <QGraphicsScene>
#include <Qt>
#include <qt5/QtWidgets/qgraphicsitem.h>
#include <QGraphicsScene>

// @TODO: Should not be hardcoded
#include "../../../../plugins/components/Create.h"
#include "../../../../plugins/components/Delay.h"
#include "../../../../plugins/data/Variable.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	// Genesys Simulator
	simulator = new Simulator();
	simulator->getTracer()->setTraceLevel(TraceManager::Level::L9_mostDetailed);
	simulator->getTracer()->addTraceHandler<MainWindow>(this, &MainWindow::_simulatorTraceHandler);
	simulator->getTracer()->addTraceErrorHandler<MainWindow>(this, &MainWindow::_simulatorTraceErrorHandler);
	simulator->getTracer()->addTraceReportHandler<MainWindow>(this, &MainWindow::_simulatorTraceReportsHandler);
	simulator->getTracer()->addTraceSimulationHandler<MainWindow>(this, &MainWindow::_simulatorTraceSimulationHandler);
	_insertFakePlugins();
	simulator->getTracer()->setTraceLevel(TraceManager::Level::L7_internal);
	// Docks
	//ui->dockWidgetContentsPlugin->setMinimumHeight(250);
	//ui->dockWidgetContentsPlugin->setMaximumWidth(230);
	tabifyDockWidget(ui->dockWidgetConsole, ui->dockWidgetPropertyEditor);
	// plugins
	ui->treeWidget_Plugins->sortByColumn(0, Qt::AscendingOrder);
	// Text Code Editor // @todo No need for programming
	//QVBoxLayout* layout = dynamic_cast<QVBoxLayout*> (ui->tabModelText->layout());
	//ui->TextCodeEditor = new CodeEditor(ui->tabModelText);
	//layout->addWidget(ui->TextCodeEditor);
	//connect(ui->TextCodeEditor, SIGNAL(textChanged()), this, SLOT(on_ui->TextCodeEditor_textChanged()));
	// Tables
	QStringList headers;
	headers << tr("Time") << tr("Component") << tr("Entity");
	ui->tableWidget_Simulation_Event->setHorizontalHeaderLabels(headers);
	ui->tableWidget_Simulation_Event->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	headers.clear();
	headers << tr("Enabled") << tr("Based On") << tr("Break in Value");
	ui->tableWidget_Breakpoints->setHorizontalHeaderLabels(headers);
	ui->tableWidget_Breakpoints->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	headers.clear();
	headers << tr("Name") << tr("Dimentions") << tr("Values");
	ui->tableWidget_Variables->setHorizontalHeaderLabels(headers);
	ui->tableWidget_Variables->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	headers.clear();
	headers << tr("Number") << tr("Name") << tr("Type"); // << and each attribute as a column
	ui->tableWidget_Entities->setHorizontalHeaderLabels(headers);
	ui->tableWidget_Entities->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableWidget_Simulation_Event->setContentsMargins(1, 0, 1, 0);
	// Trees
	QTreeWidgetItem *treeHeader = ui->treeWidgetComponents->headerItem();
	treeHeader->setText(0, "Id");
	treeHeader->setText(1, "Type");
	treeHeader->setText(2, "Name");
	treeHeader->setText(3, "Properties");
	treeHeader->setExpanded(true);
	treeHeader = ui->treeWidgetDataDefnitions->headerItem();
	treeHeader->setText(0, "Id");
	treeHeader->setText(1, "Type");
	treeHeader->setText(2, "Name");
	treeHeader->setText(3, "Properties");
	treeHeader->setExpanded(true);
	// ModelGraphic
	ui->graphicsView->setParentWidget(ui->centralwidget);
	ui->graphicsView->setSimulator(simulator);
	_zoomValue = ui->horizontalSlider_ZoomGraphical->maximum() / 2;
	// set current tabs
	ui->tabWidgetCentral->setCurrentIndex(CONST.TabCentralModelIndex);
	ui->tabWidgetModel->setCurrentIndex(CONST.TabModelSimLangIndex);
	ui->tabWidgetSimulation->setCurrentIndex(CONST.TabSimulationBreakpointsIndex);
	ui->tabWidgetReports->setCurrentIndex(CONST.TabReportReportIndex);
	//
	_actualizeActions();
	// graphicsView
	_initModelGraphicsView();
	//// FOR TESTS ONLY
	//on_actionNew_triggered();
	//ui->tabWidget_Model->setCurrentIndex(CONST.TabModelGraphicEditIndex);
}

MainWindow::~MainWindow() {
	delete ui;
}



//-----------------------------------------------------------------

bool MainWindow::_saveGraphicalModel(std::string filename) {
	std::ofstream savefile;
	savefile.open(filename, std::ofstream::out);
	savefile << "#Genegys Graphic Model" << std::endl;
	std::string line;
	line = "0\tView\t";
	line += "zoom=" + std::to_string(ui->horizontalSlider_ZoomGraphical->value());
	line += ", grid=10, rule=0, snap=0, viewpoint=(0,0)";
	savefile << line << std::endl;
	ModelGraphicsScene* scene = (ModelGraphicsScene*) (ui->graphicsView->scene());
	for (QGraphicsItem* item : *scene->getGraphicalModelComponents()) {
		GraphicalModelComponent* gmc = (GraphicalModelComponent*) item;
		line = std::to_string(gmc->getComponent()->getId()) + "\t" + Util::TypeOf<ModelComponent>() + "\t" + "position=(" + std::to_string(gmc->scenePos().x()) + "," + std::to_string(gmc->scenePos().y()) + ")";
		savefile << line << std::endl;
	}
	savefile.close();
	//QString data = QString::fromStdString(dot);
	//QStringList strList = data.split(QRegExp("[\n]"), QString::SkipEmptyParts);
	//for (unsigned int i = 0; i < strList.size(); i++) {
	//	savefile << strList.at(i).toStdString() << std::endl;
	//}

}

bool MainWindow::_loadGraphicalModel(std::string filename) {

}
//-----------------------------------------------------------------


//-----------------
// View
//-----------------

void MainWindow::_actualizeActions() {
	bool opened = simulator->getModels()->current() != nullptr;
	bool running = false;
	bool paused = false;
	if (opened) {
		running = simulator->getModels()->current()->getSimulation()->isRunning();
		paused = simulator->getModels()->current()->getSimulation()->isPaused();
	}
	//
	ui->graphicsView->setEnabled(opened);
	ui->tabWidgetCentral->setEnabled(opened);
	// model
	ui->actionSave->setEnabled(opened);
	ui->actionClose->setEnabled(opened);
	ui->actionCheck->setEnabled(opened);
	ui->menuSimulation->setEnabled(opened);
	//edit
	ui->toolBarEdit->setEnabled(opened);
	ui->menuEdit->setEnabled(opened);
	// simulation
	ui->actionStart->setEnabled(opened && !running);
	ui->actionStep->setEnabled(opened && !running);
	ui->actionStop->setEnabled(opened && (running || paused));
	ui->actionPause->setEnabled(opened && running);
	ui->actionResume->setEnabled(opened && paused);
	// debug
	ui->tableWidget_Breakpoints->setEnabled(opened);
	ui->tableWidget_Entities->setEnabled(opened);
	ui->tableWidget_Variables->setEnabled(opened);
	// tool bars
	ui->toolBarAnimation->setEnabled(opened);
	ui->toolBarView->setEnabled(opened);
	ui->toolBarGraphicalModel->setEnabled(opened);
	// sliders
	ui->horizontalSlider_ZoomGraphical->setEnabled(opened);
	if (_modelWasOpened && !opened) {
		_clearModelEditors();
	}
	_modelWasOpened = opened;
}

void MainWindow::_actualizeTabPanes() {
	bool opened = simulator->getModels()->current() != nullptr;
	if (opened) {
		unsigned int index = ui->tabWidgetCentral->currentIndex();
		if (index == CONST.TabCentralModelIndex) {
			index = ui->tabWidgetModel->currentIndex();
			if (ui->tabWidgetModel->currentIndex() == CONST.TabModelDiagramIndex) {
				_createModelImage();
			} else if (index == CONST.TabModelSimLangIndex) {
				this->_actualizeModelSimLanguage();
			} else if (index == CONST.TabModelCppCodeIndex) {
				_actualizeModelCppCode();
			} else if (index == CONST.TabModelComponentsIndex) {
				_actualizeModelComponents(true);
			} else if (index == CONST.TabModelDataDefinitionsIndex) {
				_actualizeModelDataDefinitions(true);
			}
		} else if (index == CONST.TabCentralModelIndex) {
			index = ui->tabWidgetSimulation->currentIndex();
			if (index == CONST.TabSimulationBreakpointsIndex) {
				_actualizeDebugBreakpoints(true);
			} else if (index == CONST.TabSimulationEntitiesIndex) {
				_actualizeDebugEntities(true);
			} else if (index == CONST.TabSimulationVariablesIndex) {
				_actualizeDebugVariables(true);
			}
		} else if (index == CONST.TabCentralReportsIndex) {
			index = ui->tabWidgetReports->currentIndex();
		}
	}
}

void MainWindow::_actualizeModelTextHasChanged(bool hasChanged) {
	if (_textModelHasChanged != hasChanged) {
	}
	_textModelHasChanged = hasChanged;
}

void MainWindow::_actualizeSimulationEvents(SimulationEvent * re) {
	int row = ui->tableWidget_Simulation_Event->rowCount();
	ui->tableWidget_Simulation_Event->setRowCount(row + 1);
	QTableWidgetItem * newItem;
	newItem = new QTableWidgetItem(QString::fromStdString(std::to_string(re->getCurrentEvent()->getTime())));
	ui->tableWidget_Simulation_Event->setItem(row, 0, newItem);
	newItem = new QTableWidgetItem(QString::fromStdString(re->getCurrentEvent()->getComponent()->getName()));
	ui->tableWidget_Simulation_Event->setItem(row, 1, newItem);
	newItem = new QTableWidgetItem(QString::fromStdString(re->getCurrentEvent()->getEntity()->show()));
	ui->tableWidget_Simulation_Event->setItem(row, 2, newItem);
	QCoreApplication::processEvents();
}

void MainWindow::_actualizeDebugVariables(bool force) {
	QCoreApplication::processEvents();
	if (force || ui->tabWidgetSimulation->currentIndex() == CONST.TabSimulationVariablesIndex) {
		ui->tableWidget_Variables->setRowCount(0);
		List<ModelDataDefinition*>* variables = simulator->getModels()->current()->getDataManager()->getDataDefinitionList(Util::TypeOf<Variable>());
		int row = 0;
		ui->tableWidget_Variables->setRowCount(variables->size());
		Variable* variable;
		for (ModelDataDefinition* varData : *variables->list()) {
			variable = dynamic_cast<Variable*> (varData);
			QTableWidgetItem* newItem;
			newItem = new QTableWidgetItem(QString::fromStdString(variable->getName()));
			ui->tableWidget_Variables->setItem(row, 0, newItem);
			newItem = new QTableWidgetItem(QString::fromStdString(list2str(variable->getDimensionSizes())));
			ui->tableWidget_Variables->setItem(row, 1, newItem);
			newItem = new QTableWidgetItem(QString::fromStdString(map2str(variable->getValues())));
			ui->tableWidget_Variables->setItem(row, 2, newItem);
		}
	}
}

void MainWindow::_actualizeDebugEntities(bool force) {
	QCoreApplication::processEvents();
	if (force || ui->tabWidgetSimulation->currentIndex() == CONST.TabSimulationEntitiesIndex) {
		List<ModelDataDefinition*>* entities = simulator->getModels()->current()->getDataManager()->getDataDefinitionList(Util::TypeOf<Entity>());
		List<ModelDataDefinition*>* attributes = simulator->getModels()->current()->getDataManager()->getDataDefinitionList(Util::TypeOf<Attribute>());
		Entity* entity;
		int row = 0;
		int column = 3;
		QTableWidgetItem* newItem;
		if (ui->tableWidget_Entities->columnCount() < attributes->size() + 3) {
			ui->tableWidget_Entities->setColumnCount(3 + attributes->size());
			for (ModelDataDefinition* attribData : *attributes->list()) {
				newItem = new QTableWidgetItem(QString::fromStdString(attribData->getName()));
				ui->tableWidget_Entities->setHorizontalHeaderItem(column++, newItem);
			}
		}
		ui->tableWidget_Entities->setRowCount(0);
		ui->tableWidget_Entities->setRowCount(entities->size());
		for (ModelDataDefinition* entData : *entities->list()) {
			entity = dynamic_cast<Entity*> (entData);
			//			ui->tableWidget_Entities->setRowCount(row);
			//std::cout << row << " - " << entity->entityNumber() << " - " << entity->getName() << " - " << entity->getEntityTypeName() << std::endl;
			newItem = new QTableWidgetItem(QString::fromStdString(std::to_string(entity->entityNumber())));
			ui->tableWidget_Entities->setItem(row, 0, newItem);
			newItem = new QTableWidgetItem(QString::fromStdString(entity->getName()));
			ui->tableWidget_Entities->setItem(row, 1, newItem);
			newItem = new QTableWidgetItem(QString::fromStdString(entity->getEntityTypeName()));
			ui->tableWidget_Entities->setItem(row, 2, newItem);
			int column = 3;
			for (ModelDataDefinition* attribData : *attributes->list()) {
				newItem = new QTableWidgetItem(QString::fromStdString(std::to_string(entity->getAttributeValue(attribData->getName()))));
				ui->tableWidget_Entities->setItem(row, column++, newItem);
			}
			row++;
		}
		QCoreApplication::processEvents();
	}
}

void MainWindow::_actualizeDebugBreakpoints(bool force) {
	QCoreApplication::processEvents();
	if (force || ui->tabWidgetSimulation->currentIndex() == CONST.TabSimulationBreakpointsIndex) {
		ui->tableWidget_Breakpoints->setRowCount(0);
		ModelSimulation* sim = simulator->getModels()->current()->getSimulation();
		int row = 0;
		for (ModelComponent* comp : *sim->getBreakpointsOnComponent()->list()) {
			ui->tableWidget_Breakpoints->setRowCount(row + 1);
			QTableWidgetItem* newItem;
			newItem = new QTableWidgetItem("True");
			ui->tableWidget_Breakpoints->setItem(row, 0, newItem);
			newItem = new QTableWidgetItem("Component");
			ui->tableWidget_Breakpoints->setItem(row, 1, newItem);
			newItem = new QTableWidgetItem(QString::fromStdString(comp->getName()));
			ui->tableWidget_Breakpoints->setItem(row, 2, newItem);
			row++;
		}
		for (Entity* entity : *sim->getBreakpointsOnEntity()->list()) {
			ui->tableWidget_Breakpoints->setRowCount(++row);
			QTableWidgetItem* newItem;
			newItem = new QTableWidgetItem("Entity");
			ui->tableWidget_Breakpoints->setItem(row, 1, newItem);
			newItem = new QTableWidgetItem(QString::fromStdString(entity->getName()));
			ui->tableWidget_Breakpoints->setItem(row, 2, newItem);
		}
		for (double time : *sim->getBreakpointsOnTime()->list()) {
			ui->tableWidget_Breakpoints->setRowCount(++row);
			QTableWidgetItem* newItem;
			newItem = new QTableWidgetItem("Time");
			ui->tableWidget_Breakpoints->setItem(row, 1, newItem);
			newItem = new QTableWidgetItem(QString::fromStdString(std::to_string(time)));
			ui->tableWidget_Breakpoints->setItem(row, 2, newItem);
		}
	}
}

void MainWindow::_actualizeModelComponents(bool force) {
	Model* m = simulator->getModels()->current();
	ui->treeWidgetComponents->clear();
	if (m == nullptr) {
		return;
	}
	for (ModelComponent* comp : *m->getComponents()->getAllComponents()) {
		QList<QTreeWidgetItem *> items = ui->treeWidgetComponents->findItems(QString::fromStdString(std::to_string(comp->getId())), Qt::MatchExactly | Qt::MatchRecursive, 0);
		if (items.size() == 0) {
			QTreeWidgetItem* treeComp = new QTreeWidgetItem(ui->treeWidgetComponents);
			treeComp->setText(0, QString::fromStdString(std::to_string(comp->getId())));
			treeComp->setText(1, QString::fromStdString(comp->getClassname()));
			treeComp->setText(2, QString::fromStdString(comp->getName()));
			std::string properties = "";
			for (auto prop : *comp->getProperties()) {
				properties += prop->getName() + ":" + std::to_string(prop->getValue()) + ", ";
			}
			properties = properties.substr(0, properties.length() - 2);
			treeComp->setText(3, QString::fromStdString(properties));
		}
	}
	ui->treeWidgetComponents->resizeColumnToContents(0);
	ui->treeWidgetComponents->resizeColumnToContents(1);
	ui->treeWidgetComponents->resizeColumnToContents(2);
}

void MainWindow::_actualizeModelDataDefinitions(bool force) {
	Model* m = simulator->getModels()->current();
	ui->treeWidgetDataDefnitions->clear();
	if (m == nullptr) {
		return;
	}
	for (std::string dataTypename : *m->getDataManager()->getDataDefinitionClassnames()) {
		for (ModelDataDefinition* comp : *m->getDataManager()->getDataDefinitionList(dataTypename)->list()) {
			QList<QTreeWidgetItem *> items = ui->treeWidgetDataDefnitions->findItems(QString::fromStdString(std::to_string(comp->getId())), Qt::MatchExactly | Qt::MatchRecursive, 0);
			if (items.size() == 0) {
				QTreeWidgetItem* treeComp = new QTreeWidgetItem(ui->treeWidgetDataDefnitions);
				treeComp->setText(0, QString::fromStdString(std::to_string(comp->getId())));
				treeComp->setText(1, QString::fromStdString(comp->getClassname()));
				treeComp->setText(2, QString::fromStdString(comp->getName()));
				std::string properties = "";
				for (auto prop : *comp->getProperties()) {
					properties += prop->getName() + ":" + std::to_string(prop->getValue()) + ", ";
				}
				properties = properties.substr(0, properties.length() - 2);
				treeComp->setText(3, QString::fromStdString(properties));
			}
		}
	}
	ui->treeWidgetDataDefnitions->resizeColumnToContents(0);
	ui->treeWidgetDataDefnitions->resizeColumnToContents(1);
	ui->treeWidgetDataDefnitions->resizeColumnToContents(2);
}

void MainWindow::_actualizeGraphicalModel(SimulationEvent * re) {
	Event* event = re->getCurrentEvent();
	if (event != nullptr) {
		ui->graphicsView->selectModelComponent(event->getComponent());
	}
}

void MainWindow::_insertCommandInConsole(std::string text) {

	ui->textEdit_Console->setTextColor(QColor::fromRgb(0, 128, 0));
	QFont font(ui->textEdit_Console->font());
	font.setBold(true);
	ui->textEdit_Console->setFont(font);
	ui->textEdit_Console->append("\n$genesys> " + QString::fromStdString(text));
	ui->textEdit_Console->moveCursor(QTextCursor::MoveOperation::Down, QTextCursor::MoveMode::MoveAnchor);
	font.setBold(false);
	ui->textEdit_Console->setFont(font);
}

void MainWindow::_actualizeModelSimLanguage() {
	Model* m = simulator->getModels()->current();
	if (m != nullptr) {
		m->getPersistence()->setOption(ModelPersistence_if::Options::SAVEDEFAULTS, true);
		std::string tempFilename = "./temp.tmp";
		m->getPersistence()->setOption(ModelPersistence_if::Options::SAVEDEFAULTS, false);
		bool res = m->save(tempFilename);
		std::string line;
		std::ifstream file(tempFilename);
		if (file.is_open()) {
			ui->TextCodeEditor->clear();
			while (std::getline(file, line)) {
				ui->TextCodeEditor->appendPlainText(QString::fromStdString(line));
			}
			file.close();
			_textModelHasChanged = false;
		}
	}
}

void MainWindow::_clearModelEditors() {
	ui->TextCodeEditor->clear();
	ui->textEdit_Simulation->clear();
	ui->textEdit_Reports->clear();
	ui->graphicsView->clear();
	ui->plainTextEditCppCode->clear();
	ui->treeWidgetComponents->clear();
	ui->treeWidgetDataDefnitions->clear();
}

bool MainWindow::_setSimulationModelBasedOnText() {
	Model* model = simulator->getModels()->current();
	if (this->_textModelHasChanged) {
		//@TODO !!!!!!!!!!!!!!
		// simulator->getModels()->remove(model);
		// model = nullptr;
	}
	if (model == nullptr) { // only model text written in UI
		QString modelLanguage = ui->TextCodeEditor->toPlainText();
		if (!simulator->getModels()->createFromLanguage(modelLanguage.toStdString())) {
			QMessageBox::critical(this, "Check Model", "Error in the model text. See console for more information.");
		}
		model = simulator->getModels()->current();
		if (model != nullptr) {

			_setOnEventHandlers();
		}
	}
	return simulator->getModels()->current() != nullptr;
}

std::string MainWindow::_adjustDotName(std::string name) {
	std::string text = strReplace(name, "[", "_");
	text = strReplace(text, "]", "");
	return strReplace(text, ".", "_");
}

void MainWindow::_insertTextInDot(std::string text, unsigned int compLevel, unsigned int compRank, std::map<unsigned int, std::map<unsigned int, std::list<std::string>*>*>* dotmap, bool isNode) {
	if (dotmap->find(compLevel) == dotmap->end()) {
		dotmap->insert({compLevel, new std::map<unsigned int, std::list<std::string>*>()});
	}
	std::pair<unsigned int, std::map<unsigned int, std::list<std::string>*>*> dotPair = (*dotmap->find(compLevel));
	if (dotPair.second->find(compRank) == dotPair.second->end()) {
		dotPair.second->insert({compRank, new std::list<std::string>()});
	}
	std::pair<unsigned int, std::list<std::string>*> dotPair2 = *(dotPair.second->find(compRank));
	if (isNode) {
		dotPair2.second->insert(dotPair2.second->begin(), text);
	} else {

		dotPair2.second->insert(dotPair2.second->end(), text);
	}
}

void MainWindow::_recursiveCreateModelGraphicPicture(ModelDataDefinition* componentOrData, std::list<ModelDataDefinition*>* visited, std::map<unsigned int, std::map<unsigned int, std::list<std::string>*>*>* dotmap) {

	const struct DOT_STYLES {
		std::string nodeComponent = "shape=record, fontsize=12, fontcolor=black, style=filled, fillcolor=bisque";
		//std::string nodeComponentOtherLevel = "shape=record, fontsize=12, fontcolor=black, style=filled, fillcolor=goldenrod3";
		std::string edgeComponent = "style=solid, arrowhead=\"normal\" color=black, fontcolor=black, fontsize=7";
		std::string nodeDataDefInternal = "shape=record, fontsize=8, color=gray50, fontcolor=gray50";
		std::string nodeDataDefAttached = "shape=record, fontsize=10, color=gray50, fontcolor=gray50, style=filled, fillcolor=darkolivegreen3";
		std::string edgeDataDefInternal = "style=dashed, arrowhead=\"diamond\", color=gray55, fontcolor=gray55, fontsize=7";
		std::string edgeDataDefAttached = "style=dashed, arrowhead=\"ediamond\", color=gray50, fontcolor=gray50, fontsize=7";
		unsigned int rankSource = 0;
		unsigned int rankSink = 1;
		unsigned int rankComponent = 99;
		unsigned int rankComponentOtherLevel = 99;
		unsigned int rankDataDefInternal = 99;
		unsigned int rankDataDefAttached = 99;
		unsigned int rankDataDefRecursive = 99;
		unsigned int rankEdge = 99;
	} DOT;
	visited->insert(visited->end(), componentOrData);
	std::string text;
	unsigned int modellevel = simulator->getModels()->current()->getLevel();
	std::list<ModelDataDefinition*>::iterator visitedIt;
	ModelComponent* parentComponentSuperLevel = nullptr;
	unsigned int level = componentOrData->getLevel();
	if (dynamic_cast<ModelComponent*> (componentOrData) != nullptr) {
		if (level != modellevel && !ui->checkBox_ShowLevels->isChecked()) {
			// do not show the component itself, but its parent on the model level
			parentComponentSuperLevel = simulator->getModels()->current()->getComponents()->find(level);
			assert(parentComponentSuperLevel != nullptr);
			visitedIt = std::find(visited->begin(), visited->end(), parentComponentSuperLevel);
			if (visitedIt == visited->end()) {
				text = "  " + _adjustDotName(parentComponentSuperLevel->getName()) + " [" + DOT.nodeComponent + ", label=\"" + parentComponentSuperLevel->getClassname() + "|" + parentComponentSuperLevel->getName() + "\"]" + ";\n";
				_insertTextInDot(text, level, DOT.rankComponentOtherLevel, dotmap, true);
			}
		} else {
			text = "  " + _adjustDotName(componentOrData->getName()) + " [" + DOT.nodeComponent + ", label=\"" + componentOrData->getClassname() + "|" + componentOrData->getName() + "\"]" + ";\n";
			if (dynamic_cast<SourceModelComponent*> (componentOrData) != nullptr) {
				_insertTextInDot(text, level, DOT.rankSource, dotmap, true);
			} else if (dynamic_cast<SinkModelComponent*> (componentOrData) != nullptr) {
				_insertTextInDot(text, level, DOT.rankSink, dotmap, true);
			} else {
				_insertTextInDot(text, level, DOT.rankComponent, dotmap, true);
			}
		}
	}
	//
	ModelDataDefinition* data;
	std::string dataname, componentName;
	if (parentComponentSuperLevel != nullptr) {
		componentName = parentComponentSuperLevel->getName();
	} else {
		componentName = componentOrData->getName();
	}
	if (ui->checkBox_ShowInternals->isChecked()) {
		for (std::pair<std::string, ModelDataDefinition*> dataPair : *componentOrData->getInternalData()) {
			dataname = _adjustDotName(dataPair.second->getName());
			level = dataPair.second->getLevel();
			visitedIt = std::find(visited->begin(), visited->end(), dataPair.second);
			if (visitedIt == visited->end()) {
				if (dynamic_cast<ModelComponent*> (dataPair.second) == nullptr) {
					text = "  " + dataname + " [" + DOT.nodeDataDefInternal + ", label=\"" + dataPair.second->getClassname() + "|" + dataPair.second->getName() + "\"]" + ";\n";
					_insertTextInDot(text, level, DOT.rankDataDefInternal, dotmap, true);
					if (ui->checkBox_ShowRecursive->isChecked()) {
						_recursiveCreateModelGraphicPicture(dataPair.second, visited, dotmap);
					}
				}
			}
			if (dataPair.second->getLevel() == modellevel || ui->checkBox_ShowLevels->isChecked()) {
				text = "    " + dataname + "->" + _adjustDotName(componentName) + " [" + DOT.edgeDataDefInternal + ", label=\"" + dataPair.first + "\"];\n";
				_insertTextInDot(text, modellevel, DOT.rankEdge, dotmap);
			}
		}
	}
	if (ui->checkBox_ShowElements->isChecked()) {
		for (std::pair<std::string, ModelDataDefinition*> dataPair : *componentOrData->getAttachedData()) {
			dataname = _adjustDotName(dataPair.second->getName());
			level = dataPair.second->getLevel();
			visitedIt = std::find(visited->begin(), visited->end(), dataPair.second);
			if (visitedIt == visited->end()) {
				if (dynamic_cast<ModelComponent*> (dataPair.second) == nullptr) {
					text = "  " + dataname + " [" + DOT.nodeDataDefAttached + ", label=\"" + dataPair.second->getClassname() + "|" + dataPair.second->getName() + "\"]" + ";\n";
					_insertTextInDot(text, level, DOT.rankDataDefAttached, dotmap, true);
				}
				if (ui->checkBox_ShowRecursive->isChecked()) {
					_recursiveCreateModelGraphicPicture(dataPair.second, visited, dotmap);
				}
			}
			text = "    " + dataname + "->" + _adjustDotName(componentName) + " [" + DOT.edgeDataDefAttached + ", label=\"" + dataPair.first + "\"];\n";
			_insertTextInDot(text, modellevel, DOT.rankEdge, dotmap);
		}
	}
	ModelComponent* component = dynamic_cast<ModelComponent*> (componentOrData);
	if (component != nullptr) {
		level = component->getLevel();
		Connection* connection;
		for (unsigned short i = 0; i < component->getConnections()->size(); i++) {
			connection = component->getConnections()->getConnectionAtPort(i);
			visitedIt = std::find(visited->begin(), visited->end(), connection->component);
			if (visitedIt == visited->end()) {
				_recursiveCreateModelGraphicPicture(connection->component, visited, dotmap);
			}
			if (connection->component->getLevel() == modellevel || ui->checkBox_ShowLevels->isChecked()) {

				text = "    " + _adjustDotName(componentName) + "->" + _adjustDotName(connection->component->getName()) + "[" + DOT.edgeComponent + "];\n";
				_insertTextInDot(text, modellevel, DOT.rankEdge, dotmap);
			}
		}
	}
}

std::string MainWindow::_addCppCodeLine(std::string line, unsigned int indent) {
	std::string text = "";
	for (unsigned int i = 0; i < indent; i++) {
		text += "\t";
	}
	text += line + "\n";
	return text;
}

void MainWindow::_actualizeModelCppCode() {
	Model* m = simulator->getModels()->current();
	if (m != nullptr) {
		unsigned short tabs = 0;
		std::string text, text2, name;
		std::map<std::string, std::string>* code = new std::map<std::string, std::string>();
		text = _addCppCodeLine("/*");
		text += _addCppCodeLine(" * This C++ source cod was automatically generated by Genesys");
		text += _addCppCodeLine(" */");
		code->insert({"1begin", text});

		text = _addCppCodeLine("#include \"kernel/simulator/Simulator.h\"");
		List<std::string>* included = new List<std::string>();
		for (ModelComponent* comp : *m->getComponents()->getAllComponents()) {
			name = comp->getClassname();
			if (included->find(name) == included->list()->end()) {
				included->insert(name);
				text += _addCppCodeLine("#include \"plugins/components/" + name + ".h\"");
			}
		}
		for (std::string ddClassname : *m->getDataManager()->getDataDefinitionClassnames()) {
			text += _addCppCodeLine("#include \"plugins/data/" + ddClassname + ".h\"");
			for (ModelDataDefinition* modeldata : *m->getDataManager()->getDataDefinitionList(ddClassname)->list()) {
				name = modeldata->getName();
				if (name.find(".") == std::string::npos) {
					if (included->find(name) == included->list()->end()) {
						included->insert(ddClassname);
						text += _addCppCodeLine("#include \"plugins/data/" + ddClassname + "\"");
					}
				}
			}
		}
		code->insert({"2include", text});

		text = _addCppCodeLine("\nint main(int argc, char** argv) {");
		tabs++;
		text += _addCppCodeLine("// Create simulator, a model and get acess to plugins", tabs);
		text += _addCppCodeLine("Simulator* genesys = new Simulator();", tabs);
		text += _addCppCodeLine("Model* model = genesys->getModels()->newModel();", tabs);
		text += _addCppCodeLine("PluginManager* plugins = genesys->getPlugins();", tabs);
		text += _addCppCodeLine("model->getTracer()->setTraceLevel(TraceManager::TraceLevel::L9_mostDetailed);", tabs);
		code->insert({"3main", text});

		text = _addCppCodeLine("// Create model data definitions", tabs);
		for (std::string ddClassname : *m->getDataManager()->getDataDefinitionClassnames()) {
			for (ModelDataDefinition* modeldata : *m->getDataManager()->getDataDefinitionList(ddClassname)->list()) {
				name = modeldata->getName();
				if (name.find(".") == std::string::npos) {
					text += _addCppCodeLine(ddClassname + "* " + name + " = plugins->newInstance<" + ddClassname + ">(model, \"" + name + "\");", tabs);
				}
			}
		}
		code->insert({"4datadef", text});

		text = _addCppCodeLine("// Create model components", tabs);
		for (ModelComponent* comp : *m->getComponents()->getAllComponents()) {
			name = comp->getName();
			if (name.find(".") == std::string::npos) {
				text += _addCppCodeLine(comp->getClassname() + "* " + name + " = plugins->newInstance<" + comp->getClassname() + ">(model, \"" + name + "\");", tabs);
			}
		}
		code->insert({"5modelcompdef", text});

		text = _addCppCodeLine("// Connect the components in the model", tabs);
		Connection* conn;
		for (ModelComponent* comp : *m->getComponents()->getAllComponents()) {
			name = comp->getName();
			if (name.find(".") == std::string::npos) {
				for (std::pair<unsigned int, Connection*> pair : *comp->getConnections()->connections()) {//unsigned int i=0; i<comp->getConnections()->size(); i++) {
					conn = pair.second; //comp->getConnections()->getConnectionAtPort(i);
					text2 = conn->component->getName(); // + conn->second==0?"":","+std::to_string(conn->second);
					text += _addCppCodeLine(name + "->getConnections()->insertAtPort(" + std::to_string(pair.first) + "," + text2 + ");", tabs);
				}
			}
		}
		code->insert({"6modelconnect", text});

		ModelSimulation* sim = m->getSimulation();
		text = _addCppCodeLine("// Define simulation options", tabs);
		text += _addCppCodeLine("ModelSimulation* sim = model->getSimulation();", tabs);
		text += _addCppCodeLine("sim->setReplicationLength(" + std::to_string(sim->getReplicationLength()) + ")", tabs);
		text += _addCppCodeLine("sim->setReplicationLengthTimeUnit(Util::TimeUnit::" + Util::StrTimeUnitLong(sim->getReplicationLengthTimeUnit()) + ");", tabs);
		text += _addCppCodeLine("sim->setWarmUpPeriod(" + std::to_string(sim->getWarmUpPeriod()) + ");", tabs);
		text += _addCppCodeLine("sim->setWarmUpPeriodTimeUnit(Util::TimeUnit::" + Util::StrTimeUnitLong(sim->getWarmUpPeriodTimeUnit()) + ");", tabs);
		text += _addCppCodeLine("sim->setReplicationReportBaseTimeUnit(Util::TimeUnit::" + Util::StrTimeUnitLong(sim->getReplicationBaseTimeUnit()) + ");", tabs);
		code->insert({"7simulation", text});

		text = _addCppCodeLine("// simulate", tabs);
		text += _addCppCodeLine("sim->start();", tabs);
		text += _addCppCodeLine("return 0;", tabs);
		tabs--;
		text += _addCppCodeLine("}", tabs);
		code->insert({"8end", text});

		// Show
		ui->plainTextEditCppCode->clear();
		for (std::pair<std::string, std::string> codeSection : *code) {
			//ui->plainTextEditCppCode->appendPlainText(QString::fromStdString("// " + codeSection.first+"\n"));
			ui->plainTextEditCppCode->appendPlainText(QString::fromStdString(codeSection.second));
		}
	} else {

	}
}

bool MainWindow::graphicalModelHasChanged() const {
	return _graphicalModelHasChanged;
}

void MainWindow::setGraphicalModelHasChanged(bool graphicalModelHasChanged) {
	_graphicalModelHasChanged = graphicalModelHasChanged;
	_actualizeTabPanes();
}

bool MainWindow::_createModelImage() {
	bool res = this->_setSimulationModelBasedOnText();
	std::string dot = "digraph G {\n";
	dot += "  compound=true; rankdir=LR; \n";
	std::map<unsigned int, std::map<unsigned int, std::list<std::string>*>*>* dotmap = new std::map<unsigned int, std::map<unsigned int, std::list<std::string>*>*>();

	std::list<SourceModelComponent*>* sources = simulator->getModels()->current()->getComponents()->getSourceComponents();
	std::list<ModelDataDefinition*>* visited = new std::list<ModelDataDefinition*>();
	std::list<ModelDataDefinition*>::iterator visitedIt;
	for (SourceModelComponent* source : *sources) {
		visitedIt = std::find(visited->begin(), visited->end(), source);
		if (visitedIt == visited->end()) {
			_recursiveCreateModelGraphicPicture(source, visited, dotmap);
		}
	}
	std::list<ModelComponent*>* transfers = simulator->getModels()->current()->getComponents()->getTransferInComponents();
	for (ModelComponent* transfer : *transfers) {
		visitedIt = std::find(visited->begin(), visited->end(), transfer);
		if (visitedIt == visited->end()) {
			_recursiveCreateModelGraphicPicture(transfer, visited, dotmap);
		}
	}
	std::list<ModelComponent*>* allComps = simulator->getModels()->current()->getComponents()->getAllComponents();
	for (ModelComponent* comp : *allComps) {
		visitedIt = std::find(visited->begin(), visited->end(), comp);
		if (visitedIt == visited->end()) {
			_recursiveCreateModelGraphicPicture(comp, visited, dotmap);
		}
	}
	// combine all level subgraphs
	unsigned int modelLevel = simulator->getModels()->current()->getLevel();
	for (std::pair<unsigned int, std::map<unsigned int, std::list<std::string>*>*> dotpair : *dotmap) {
		if (dotpair.first == modelLevel) {
			dot += "\n  // model level\n";
			for (std::pair<unsigned int, std::list<std::string>*> dotpair2 : *dotpair.second) {
				dot += "  {\n";
				if (dotpair2.first == 0)
					dot += "     rank=min  // " + std::to_string(dotpair2.first) + "\n";
				else if (dotpair2.first == 1)
					dot += "     rank=max  // " + std::to_string(dotpair2.first) + "\n";
				else if (dotpair2.first < 10)
					dot += "     rank=same  // " + std::to_string(dotpair2.first) + "\n";
				for (std::string str : *dotpair2.second) {
					dot += "   " + str;
				}
				dot += "  }\n";
			}
		} else if (ui->checkBox_ShowLevels->isChecked()) {
			dot += "\n\n // submodel level  " + std::to_string(dotpair.first) + "\n";
			dot += " subgraph cluster_level_" + std::to_string(dotpair.first) + " {\n";
			dot += "   graph[style=filled; fillcolor=mistyrose2] label=\"" + simulator->getModels()->current()->getComponents()->find(dotpair.first)->getName() + "\";\n";
			for (std::pair<unsigned int, std::list<std::string>*> dotpair2 : *dotpair.second) {
				dot += "  {\n";
				if (dotpair2.first == 0)
					dot += "     rank=min  // " + std::to_string(dotpair2.first) + "\n";
				else if (dotpair2.first == 1)
					dot += "     rank=max  // " + std::to_string(dotpair2.first) + "\n";
				else if (dotpair2.first < 10)
					dot += "     rank=same  // " + std::to_string(dotpair2.first) + "\n";
				for (std::string str : *dotpair2.second) {
					dot += "   " + str;
				}
				dot += "  }\n";
			}
			dot += "\n }\n";
		}
	}
	dot += "}\n";
	visited->clear();
	std::string basefilename = "./.tempFixedGraphicalModelRepresentation";
	std::string dotfilename = basefilename + ".dot";
	std::string pngfilename = basefilename + ".png";
	try {
		std::ofstream savefile;
		savefile.open(dotfilename, std::ofstream::out);
		QString data = QString::fromStdString(dot);
		QStringList strList = data.split(QRegExp("[\n]"), QString::SkipEmptyParts);
		for (unsigned int i = 0; i < strList.size(); i++) {
			savefile << strList.at(i).toStdString() << std::endl;
		}
		savefile.close();
		try {
			std::remove(pngfilename.c_str());
		} catch (...) {

		}
		try {
			std::string command = "dot -Tpng " + dotfilename + " -o " + pngfilename;
			system(command.c_str());
			QPixmap pm(QString::fromStdString(pngfilename)); // <- path to image file
			//int w = ui->label_ModelGraphic->width();
			//int h = ui->label_ModelGraphic->height();
			ui->label_ModelGraphic->setPixmap(pm); //.scaled(w, h, Qt::IgnoreAspectRatio));
			ui->label_ModelGraphic->setScaledContents(false);
			try {
				//std::remove(dotfilename.c_str());
				//std::remove(pngfilename.c_str());
			} catch (...) {

			}
			return true;
		} catch (...) {
		}
	} catch (...) {
	}

	return false;
}

//-------------------------
// Simulator Trace Handlers
//-------------------------

void MainWindow::_simulatorTraceHandler(TraceEvent e) {
	std::cout << e.getText() << std::endl;
	if (e.getTracelevel() == TraceManager::Level::L1_errorFatal)
		ui->textEdit_Console->setTextColor(QColor::fromRgb(255, 0, 0));
	else if (e.getTracelevel() == TraceManager::Level::L2_results)
		ui->textEdit_Console->setTextColor(QColor::fromRgb(0, 0, 255));
	else if (e.getTracelevel() == TraceManager::Level::L3_errorRecover)
		ui->textEdit_Console->setTextColor(QColor::fromRgb(223, 0, 0));
	else if (e.getTracelevel() == TraceManager::Level::L4_warning)
		ui->textEdit_Console->setTextColor(QColor::fromRgb(128, 0, 0));
	else {

		unsigned short grayVal = 20 * (static_cast<unsigned int> (e.getTracelevel()) - 5);
		ui->textEdit_Console->setTextColor(QColor::fromRgb(grayVal, grayVal, grayVal));
	}
	ui->textEdit_Console->append(QString::fromStdString(e.getText()));
	ui->textEdit_Console->moveCursor(QTextCursor::MoveOperation::End, QTextCursor::MoveMode::MoveAnchor);
	QCoreApplication::processEvents();
}

void MainWindow::_simulatorTraceErrorHandler(TraceErrorEvent e) {

	std::cout << e.getText() << std::endl;
	ui->textEdit_Console->setTextColor(QColor::fromRgb(255, 0, 0));
	ui->textEdit_Console->append(QString::fromStdString(e.getText()));
	QCoreApplication::processEvents();
}

void MainWindow::_simulatorTraceSimulationHandler(TraceSimulationEvent e) {
	std::cout << e.getText() << std::endl;
	if (e.getText().find("Event {time=") != std::string::npos) {
		ui->textEdit_Simulation->setTextColor(QColor::fromRgb(0, 0, 128));
	} else {

		unsigned short grayVal = 20 * (static_cast<unsigned int> (e.getTracelevel()) - 5);
		ui->textEdit_Simulation->setTextColor(QColor::fromRgb(grayVal, grayVal, grayVal));
	}
	ui->textEdit_Simulation->append(QString::fromStdString(e.getText()));
	QCoreApplication::processEvents();
}

void MainWindow::_simulatorTraceReportsHandler(TraceEvent e) {

	std::cout << e.getText() << std::endl;
	ui->textEdit_Reports->append(QString::fromStdString(e.getText()));
	QCoreApplication::processEvents();
}

void MainWindow::_onReplicationStartHandler(SimulationEvent * re) {

	ModelSimulation* sim = simulator->getModels()->current()->getSimulation();
	QString text = QString::fromStdString(std::to_string(sim->getCurrentReplicationNumber())) + "/" + QString::fromStdString(std::to_string(sim->getNumberOfReplications()));
	ui->label_ReplicationNum->setText(text);
	int row = ui->tableWidget_Simulation_Event->rowCount();
	ui->tableWidget_Simulation_Event->setRowCount(row + 1);
	QTableWidgetItem* newItem;
	newItem = new QTableWidgetItem(QString::fromStdString("Replication " + std::to_string(re->getCurrentReplicationNumber())));
	ui->tableWidget_Simulation_Event->setItem(row, 2, newItem);

	QCoreApplication::processEvents();
}

void MainWindow::_onSimulationStartHandler(SimulationEvent * re) {
	_actualizeActions();
	ui->progressBarSimulation->setMaximum(simulator->getModels()->current()->getSimulation()->getReplicationLength());
	ui->tableWidget_Simulation_Event->setRowCount(0);
	ui->tableWidget_Entities->setRowCount(0);
	ui->tableWidget_Variables->setRowCount(0);
	ui->textEdit_Simulation->clear();
	ui->textEdit_Reports->clear();
	QCoreApplication::processEvents();
}

void MainWindow::_onSimulationPausedHandler(SimulationEvent * re) {

	_actualizeActions();
	QCoreApplication::processEvents();
}

void MainWindow::_onSimulationResumeHandler(SimulationEvent * re) {

	_actualizeActions();
	QCoreApplication::processEvents();
}

void MainWindow::_onSimulationEndHandler(SimulationEvent * re) {
	_actualizeActions();
	ui->tabWidgetCentral->setCurrentIndex(CONST.TabCentralReportsIndex);
	for (unsigned int i = 0; i < 50; i++) {
		QCoreApplication::processEvents();
	}
}

void MainWindow::_onProcessEventHandler(SimulationEvent * re) {
	ui->progressBarSimulation->setValue(simulator->getModels()->current()->getSimulation()->getSimulatedTime());
	_actualizeSimulationEvents(re);
	_actualizeDebugEntities(false);
	_actualizeDebugVariables(false);
	_actualizeGraphicalModel(re);
	QCoreApplication::processEvents();
}

void MainWindow::_onEntityCreateHandler(SimulationEvent* re) {

}

void MainWindow::_onEntityRemoveHandler(SimulationEvent* re) {

}
//-----------------------------------------

void MainWindow::_onSceneMouseEvent(QGraphicsSceneMouseEvent* mouseEvent) {
	QPointF pos = mouseEvent->scenePos();
	ui->labelMousePos->setText(QString::fromStdString("<" + std::to_string((int) pos.x()) + "," + std::to_string((int) pos.y()) + ">"));
}

void MainWindow::_onSceneGraphicalModelEvent(GraphicalModelEvent* event) {
	_actualizeTabPanes();
}

//-----------------------------------------

void MainWindow::sceneChanged(const QList<QRectF> &region) {
	//_graphicalModelHasChanged = true;
	//_actualizeTabPanes();
}

void MainWindow::sceneFocusItemChanged(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason) {
	//int a = 0;
}
//void sceneRectChanged(const QRectF &rect){}

void MainWindow::sceneSelectionChanged() {
	if (ui->graphicsView->selectedItems().size() == 1) {
		QGraphicsItem * item = ui->graphicsView->selectedItems().at(0);
		GraphicalModelComponent* gmc = dynamic_cast<GraphicalModelComponent*> (item);
		if (gmc != nullptr) {
			ui->treeViewPropertyEditor->setModelBlock(gmc->getComponent());
			return;
		}
	}
	ui->treeViewPropertyEditor->clear();
}

//-----------------------------------------

void MainWindow::sceneGraphicalModelChanged() {
	_actualizeTabPanes();
}

//-----------------------------------------

void MainWindow::_initModelGraphicsView() {
	((ModelGraphicsView*) (ui->graphicsView))->setSceneMouseEventHandler(this, &MainWindow::_onSceneMouseEvent);
	((ModelGraphicsView*) (ui->graphicsView))->setGraphicalModelEventHandler(this, &MainWindow::_onSceneGraphicalModelEvent);
	ui->graphicsView->showGrid(); //@TODO not here
	connect(ui->graphicsView->scene(), &QGraphicsScene::changed, this, &MainWindow::sceneChanged);
	connect(ui->graphicsView->scene(), &QGraphicsScene::focusItemChanged, this, &MainWindow::sceneFocusItemChanged);
	connect(ui->graphicsView->scene(), &QGraphicsScene::selectionChanged, this, &MainWindow::sceneSelectionChanged);
}

void MainWindow::_setOnEventHandlers() {

	simulator->getModels()->current()->getOnEvents()->addOnReplicationStartHandler(this, &MainWindow::_onReplicationStartHandler);
	simulator->getModels()->current()->getOnEvents()->addOnSimulationStartHandler(this, &MainWindow::_onSimulationStartHandler);
	simulator->getModels()->current()->getOnEvents()->addOnSimulationEndHandler(this, &MainWindow::_onSimulationEndHandler);
	simulator->getModels()->current()->getOnEvents()->addOnSimulationPausedHandler(this, &MainWindow::_onSimulationPausedHandler);
	simulator->getModels()->current()->getOnEvents()->addOnSimulationResumeHandler(this, &MainWindow::_onSimulationResumeHandler);
	simulator->getModels()->current()->getOnEvents()->addOnProcessEventHandler(this, &MainWindow::_onProcessEventHandler);
	simulator->getModels()->current()->getOnEvents()->addOnEntityCreateHandler(this, &MainWindow::_onEntityCreateHandler);
	simulator->getModels()->current()->getOnEvents()->addOnEntityRemoveHandler(this, &MainWindow::_onEntityRemoveHandler);
}

//-------------------------
// Simulator Fake Plugins
//-------------------------

void MainWindow::_insertPluginUI(Plugin * plugin) {
	if (plugin != nullptr) {
		if (plugin->isIsValidPlugin()) {
			QTreeWidgetItem *treeItemChild = new QTreeWidgetItem();
			//QTreeWidgetItem *treeItem = new QTreeWidgetItem; //(ui->treeWidget_Plugins);
			std::string plugtextAdds = "[" + plugin->getPluginInfo()->getCategory() + "]: ";
			QBrush brush;
			if (plugin->getPluginInfo()->isComponent()) {
				plugtextAdds += " Component";
				//brush.setColor(Qt::white);
				//treeItemChild->setBackground(brush);
				//treeItemChild->setBackgroundColor(Qt::white);
				treeItemChild->setIcon(0, QIcon(":/icons3/resources/icons/pack3/ico/component.ico"));
			} else {
				plugtextAdds += " DataDefinition";
				//brush.setColor(Qt::lightGray);
				//treeItemChild->setBackground(brush);
				//treeItemChild->setBackgroundColor(Qt::lightGray);
				treeItemChild->setIcon(0, QIcon(":/icons3/resources/icons/pack3/ico/calendarred.ico"));
				//treeItemChild->setFont(QFont::Style::StyleItalic);
			}
			if (plugin->getPluginInfo()->isSink()) {
				plugtextAdds += ", Sink";
				treeItemChild->setTextColor(0, Qt::blue);
				treeItemChild->setIcon(0, QIcon(":/icons3/resources/icons/pack3/ico/loadinv.ico"));
			}
			if (plugin->getPluginInfo()->isSource()) {
				plugtextAdds += ", Source";
				treeItemChild->setTextColor(0, Qt::blue);
				treeItemChild->setIcon(0, QIcon(":/icons3/resources/icons/pack3/ico/load.ico"));
			}
			if (plugin->getPluginInfo()->isReceiveTransfer()) {
				plugtextAdds += ", ReceiveTransfer";
				treeItemChild->setTextColor(0, Qt::blue);
				treeItemChild->setIcon(0, QIcon(":/icons3/resources/icons/pack3/ico/load.ico"));
			}
			if (plugin->getPluginInfo()->isSendTransfer()) {
				plugtextAdds += ", SendTransfer";
				treeItemChild->setTextColor(0, Qt::blue);
				treeItemChild->setIcon(0, QIcon(":/icons3/resources/icons/pack3/ico/loadinv.ico"));
			}
			//treeItem->setText(0,QString::fromStdString(plugtextAdds));
			plugtextAdds += "\n\nDescrption: " + plugin->getPluginInfo()->getDescriptionHelp();
			plugtextAdds += "\n\nTemplate: " + plugin->getPluginInfo()->getLanguageTemplate() + " (double click to add to model)";

			QTreeWidgetItem *treeRootItem;
			QString category;
			if (plugin->getPluginInfo()->isComponent())
				category = QString::fromStdString(plugin->getPluginInfo()->getCategory());
			else
				category = "Data Definition";
			QList<QTreeWidgetItem*> founds = ui->treeWidget_Plugins->findItems(category, Qt::MatchContains);
			if (founds.size() == 0) {
				QFont font("Nimbus Sans", 12, QFont::Bold);
				treeRootItem = new QTreeWidgetItem(ui->treeWidget_Plugins);
				treeRootItem->setText(0, category);
				QBrush bforeground(Qt::white);
				treeRootItem->setForeground(0, bforeground);
				QBrush bbackground(Qt::black);
				if (category == "Data Definition") {
					bbackground.setColor(Qt::darkRed);
				} else if (category == "Discrete Processing") {
					bbackground.setColor(Qt::darkGreen);
				} else if (category == "Decisions") {
					bbackground.setColor(Qt::darkYellow);
				} else if (category == "Grouping") {
					bbackground.setColor(Qt::magenta);
				} else if (category == "Input Output") {
					bbackground.setColor(Qt::darkCyan);
				} else if (category == "Material Handling") {
					bbackground.setColor(Qt::darkBlue);
				}
				treeRootItem->setBackground(0, bbackground);
				treeRootItem->setFont(0, font);
				treeRootItem->setExpanded(false); //(true);
				//treeRootItem->sortChildren(0, Qt::AscendingOrder);
			} else {
				treeRootItem = *founds.begin();
			}
			if (plugin->getPluginInfo()->isComponent() && !plugin->getPluginInfo()->isSendTransfer() && !plugin->getPluginInfo()->isReceiveTransfer() && !plugin->getPluginInfo()->isSink() && !plugin->getPluginInfo()->isSource()) {
				if (treeRootItem->backgroundColor(0).blue() < 32 && treeRootItem->backgroundColor(0).green() < 32 && treeRootItem->backgroundColor(0).red() < 32) {
					treeItemChild->setIcon(0, QIcon(":/icons3/resources/icons/pack3/ico/componentblack.ico"));
				} else if (treeRootItem->backgroundColor(0).red() >= treeRootItem->backgroundColor(0).blue() &&
						treeRootItem->backgroundColor(0).red() > treeRootItem->backgroundColor(0).green()) {
					treeItemChild->setIcon(0, QIcon(":/icons3/resources/icons/pack3/ico/componentred.ico"));
				} else if (treeRootItem->backgroundColor(0).blue() > treeRootItem->backgroundColor(0).red() &&
						treeRootItem->backgroundColor(0).blue() > treeRootItem->backgroundColor(0).green()) {
					treeItemChild->setIcon(0, QIcon(":/icons3/resources/icons/pack3/ico/componentblue.ico"));
				} else if (treeRootItem->backgroundColor(0).red() > treeRootItem->backgroundColor(0).blue() &&
						treeRootItem->backgroundColor(0).green() > treeRootItem->backgroundColor(0).blue()) {
					treeItemChild->setIcon(0, QIcon(":/icons3/resources/icons/pack3/ico/componentyellow.ico"));
				}
			}
			treeItemChild->setWhatsThis(0, QString::fromStdString(plugin->getPluginInfo()->getPluginTypename()));
			treeItemChild->setTextColor(0, treeRootItem->backgroundColor(0));
			treeItemChild->setText(0, QString::fromStdString(plugin->getPluginInfo()->getPluginTypename()));
			treeItemChild->setToolTip(0, QString::fromStdString(plugtextAdds));
			treeItemChild->setStatusTip(0, QString::fromStdString(plugin->getPluginInfo()->getLanguageTemplate()));
			//treeItemChild->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren);
			treeRootItem->addChild(treeItemChild);
		}
	}
}

void MainWindow::_insertFakePlugins() {
	PluginManager* pm = simulator->getPlugins();
	// TRYING SOME NEW ORGANIZATION (BASED ON ARENA 16..20)
	// ...
	//-----------------------------------------------------

	// OLD ORGANIZATION

	// model components
	// arena basic process
	(pm->insert("create.so"));
	(pm->insert("dispose.so"));
	(pm->insert("decide.so"));
	(pm->insert("batch.so"));
	(pm->insert("separate.so"));
	(pm->insert("assign.so"));
	(pm->insert("record.so"));
	(pm->insert("process.so"));
	(pm->insert("submodel.so"));
	(pm->insert("entitygroup.so"));
	(pm->insert("queue.so"));
	(pm->insert("set.so"));
	(pm->insert("resource.so"));
	(pm->insert("variable.so"));
	(pm->insert("schedule.so"));
	(pm->insert("entitygroup.so"));
	// arena advanced process
	(pm->insert("delay.so"));
	(pm->insert("dropoff.so"));
	(pm->insert("hold.so"));
	(pm->insert("match.so"));
	(pm->insert("pickup.so"));
	(pm->insert("read.so"));
	(pm->insert("write.so"));
	(pm->insert("release.so"));
	(pm->insert("remove.so"));
	(pm->insert("seize.so"));
	(pm->insert("search.so"));
	(pm->insert("signal.so"));
	(pm->insert("store.so"));
	(pm->insert("unstore.so"));
	(pm->insert("expression.so"));
	(pm->insert("failure.so"));
	(pm->insert("file.so"));
	(pm->insert("storage.so"));
	// arena transfer station
	(pm->insert("enter.so"));
	(pm->insert("leave.so"));
	(pm->insert("pickstation.so"));
	(pm->insert("route.so"));
	(pm->insert("sequence.so"));
	(pm->insert("station.so"));
	(pm->insert("label.so"));
	// arena transfer conveyour
	(pm->insert("access.so"));
	(pm->insert("exit.so"));
	(pm->insert("start.so"));
	(pm->insert("stop.so"));
	(pm->insert("conveyour.so"));
	(pm->insert("segment.so"));
	// arena transfer transport
	(pm->insert("alocate.so"));
	(pm->insert("free.so"));
	(pm->insert("halt.so"));
	(pm->insert("move.so"));
	(pm->insert("request.so"));
	(pm->insert("transporter.so"));
	(pm->insert("distance.so"));
	(pm->insert("network.so"));
	(pm->insert("networklink.so"));
	// others
	(pm->insert("dummy.so"));
	(pm->insert("lsode.so"));
	(pm->insert("biochemical.so"));
	(pm->insert("markovchain.so"));
	(pm->insert("cellularautomata.so"));
	(pm->insert("cppforg.so"));
	// now complete the information
	simulator->getPlugins()->completePluginsFieldsAndTemplates();
	for (unsigned int i = 0; i < simulator->getPlugins()->size(); i++) {
		//@TODO: now it's the opportunity to adjust template
		_insertPluginUI(simulator->getPlugins()->getAtRank(i));
	}
}

//-------------------------
// PRIVATE SLOTS
//-------------------------

void MainWindow::on_actionNew_triggered() {
	Model* m;
	if ((m = simulator->getModels()->current()) != nullptr) {
		QMessageBox::StandardButton reply = QMessageBox::question(this, "New Model", "There is a model already oppened. Do you want to close it and to create new model?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::No) {
			return;
		} else {
			this->on_actionClose_triggered();
			//return; //@TODO Ceck if needed (since will remove bellow)
		}
	}
	_insertCommandInConsole("new");
	if (m != nullptr) {
		simulator->getModels()->remove(m);
	}
	m = simulator->getModels()->newModel();
	ui->TextCodeEditor->clear();
	ui->textEdit_Simulation->clear();
	ui->textEdit_Reports->clear();
	ui->textEdit_Console->moveCursor(QTextCursor::End);
	// create a basic initial template for the model
	std::string tempFilename = "./temp.tmp";
	m->getPersistence()->setOption(ModelPersistence_if::Options::SAVEDEFAULTS, true);
	bool res = m->save(tempFilename);
	m->getPersistence()->setOption(ModelPersistence_if::Options::SAVEDEFAULTS, false);
	if (res) { // read the file saved and copy its contents to the model text editor
		std::string line;
		std::ifstream file(tempFilename);
		if (file.is_open()) {
			ui->TextCodeEditor->appendPlainText("# Genesys Model File");
			ui->TextCodeEditor->appendPlainText("# Simulator, ModelInfo and ModelSimulation");
			while (std::getline(file, line)) {
				ui->TextCodeEditor->appendPlainText(QString::fromStdString(line));
			}
			file.close();
			//QMessageBox::information(this, "New Model", "Model successfully created");
		} else {
			ui->textEdit_Console->append(QString("Error reading template model file"));
		}
		_actualizeModelTextHasChanged(true);
		_setOnEventHandlers();
	} else {
		ui->textEdit_Console->append(QString("Error saving template model file"));
	}
	_modelfilename = "";
	_actualizeActions();
	_actualizeTabPanes();
}

void MainWindow::on_actionSave_triggered() {
	QString fileName = QFileDialog::getSaveFileName(this,
			tr("Save Model"), _modelfilename,
			tr("Genesys Model (*.gen);;All Files (*)"));
	if (fileName.isEmpty())
		return;
	else {
		_insertCommandInConsole("save " + fileName.toStdString());
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly)) {
			QMessageBox::information(this, tr("Unable to access file to save"),
					file.errorString());
			return;
		}
		std::ofstream savefile;
		savefile.open(fileName.toStdString(), std::ofstream::out);
		QString data = ui->TextCodeEditor->toPlainText();
		QStringList strList = data.split(QRegExp("[\n]"), QString::SkipEmptyParts);
		for (unsigned int i = 0; i < strList.size(); i++) {

			savefile << strList.at(i).toStdString() << std::endl;
		}
		savefile.close();
		_saveGraphicalModel(fileName.toStdString() + ".gui");
		_modelfilename = fileName;
		QMessageBox::information(this, "Save Model", "Model successfully saved");
		// convert text info Model
		_setSimulationModelBasedOnText();
		//
		_actualizeModelTextHasChanged(false);
	}
	_actualizeActions();
}

void MainWindow::on_actionClose_triggered() {
	if (_textModelHasChanged || simulator->getModels()->current()->hasChanged()) {
		QMessageBox::StandardButton res = QMessageBox::question(this, "Close ModelSyS", "Model has changed. Do you want to save it?", QMessageBox::Yes | QMessageBox::No);
		if (res == QMessageBox::Yes) {
			this->on_actionSave_triggered();
			return;
		}
	}
	_insertCommandInConsole("close");
	simulator->getModels()->remove(simulator->getModels()->current());
	_actualizeActions();
	_actualizeTabPanes();
	//QMessageBox::information(this, "Close Model", "Model successfully closed");
}

void MainWindow::on_actionExit_triggered() {
	QMessageBox::StandardButton res;
	if (this->_textModelHasChanged) {
		res = QMessageBox::question(this, "Exit GenESyS", "Model has changed. Do you want to save it?", QMessageBox::Yes | QMessageBox::No);
		if (res == QMessageBox::Yes) {
			this->on_actionSave_triggered();
			return;
		}
	}
	res = QMessageBox::question(this, "Exit GenESyS", "Do you want to exit GenESyS?", QMessageBox::Yes | QMessageBox::No);
	if (res == QMessageBox::Yes) {
		QApplication::quit();
	} else {
		// it does not quit, but the window is closed. Check it. @TODO
	}
}

void MainWindow::on_actionStop_triggered() {
	_insertCommandInConsole("stop");
	simulator->getModels()->current()->getSimulation()->stop();
	_actualizeActions();
}

void MainWindow::on_actionStart_triggered() {
	_insertCommandInConsole("start");
	if (_setSimulationModelBasedOnText())
		simulator->getModels()->current()->getSimulation()->start();
}

void MainWindow::on_actionStep_triggered() {
	_insertCommandInConsole("step");

	if (_setSimulationModelBasedOnText())
		simulator->getModels()->current()->getSimulation()->step();
}

void MainWindow::on_actionPause_triggered() {

	_insertCommandInConsole("pause");
	simulator->getModels()->current()->getSimulation()->pause();
}

void MainWindow::on_actionResume_triggered() {
	_insertCommandInConsole("resume");

	if (_setSimulationModelBasedOnText())
		simulator->getModels()->current()->getSimulation()->start();
}

void MainWindow::on_actionOpen_triggered() {
	QString fileName = QFileDialog::getOpenFileName(
			this, "Open Model", "./models/",
			tr("All files (*.*);;Genesys model (*.gen)"));
	if (fileName == "") {
		return;
	}
	_insertCommandInConsole("load " + fileName.toStdString());
	// load Model (in the simulator)
	bool result = simulator->getModels()->loadModel(fileName.toStdString());
	if (result) { // now load the text into the GUI
		_clearModelEditors();
		std::string line;
		std::ifstream file(fileName.toStdString());
		if (file.is_open()) {
			while (std::getline(file, line)) {
				ui->TextCodeEditor->appendPlainText(QString::fromStdString(line));
			}
			file.close();
		} else {
			ui->textEdit_Console->append(QString("Error reading model file"));
		}
		ui->textEdit_Console->append("\n");
		_setOnEventHandlers();
		_actualizeModelTextHasChanged(false);
		_actualizeActions();
		_modelfilename = fileName;
		QMessageBox::information(this, "Open Model", "Model successfully oppened");
	} else {

		QMessageBox::warning(this, "Open Model", "Error while opening model");
	}
	_actualizeActions();
	_actualizeTabPanes();
}

//void MainWindow::on_ui->TextCodeEditor_textChanged() {
//	this->_actualizeModelTextHasChanged(true);
//}

void MainWindow::on_actionCheck_triggered() {
	_insertCommandInConsole("check");
	bool res = simulator->getModels()->current()->check();
	_actualizeActions();
	_actualizeTabPanes();
	if (res) {
		QMessageBox::information(this, "Model Check", "Model successfully checked.");
	} else {
		QMessageBox::critical(this, "Model Check", "Model has erros. See the console for more information.");
	}
}

void MainWindow::on_actionAbout_triggered() {

	QMessageBox::about(this, "About Genesys", "Genesys is a result of teaching and research activities of Professor Dr. Ing Rafael Luiz Cancian. It began in early 2002 as a way to teach students the basics and simulation techniques of systems implemented by other comercial simulation tools, such as Arena. In Genesys development he replicated all the SIMAN language, used by Arena software, and Genesys has become a clone of that tool, including its graphical interface. Genesys allowed the inclusion of new simulation components through dynamic link libraries and also the parallel execution of simulation models in a distributed environment. The development of Genesys continued until 2009, when the professor stopped teaching systems simulation classes. Ten years later the professor starts again to teach systems simulation classes and to carry out scientific research in the area. So in 2019 Genesys is reborn, with new language and programming techniques, and even more ambitious goals.");
}

void MainWindow::on_actionLicence_triggered() {

	LicenceManager* licman = simulator->getLicenceManager();
	std::string text = licman->showLicence() + "\n";
	text += licman->showLimits() + "\n";
	text += licman->showActivationCode();
	QMessageBox::about(this, "About Licence", QString::fromStdString(text));
}

void MainWindow::on_tabWidget_Model_tabBarClicked(int index) {

}

void MainWindow::on_checkBox_ShowElements_stateChanged(int arg1) {

	bool result = _createModelImage();
}

void MainWindow::on_checkBox_ShowInternals_stateChanged(int arg1) {

	bool result = _createModelImage();
}

void MainWindow::on_horizontalSlider_Zoom_valueChanged(int value) {

	double factor = ((double) value / 100.0)*(2 - 0.5) + 0.5;
	double scaleFactor = 1.0;
	Q_ASSERT(ui->label_ModelGraphic->pixmap());
	scaleFactor *= factor;
	ui->label_ModelGraphic->resize(scaleFactor * ui->label_ModelGraphic->pixmap()->size());

	//adjustScrollBar(ui->scrollArea_Graphic->horizontalScrollBar(), factor);
	//adjustScrollBar(ui->scrollArea_Graphic->verticalScrollBar(), factor);

	//void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor){
	//    scrollBar->setValue(int(factor * scrollBar->value()
	//                            + ((factor - 1) * scrollBar->pageStep()/2)));
	//}

	//zoomInAct->setEnabled(scaleFactor < 3.0);
	//zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void MainWindow::on_checkBox_ShowRecursive_stateChanged(int arg1) {

	bool result = _createModelImage();
}

void MainWindow::on_actionGet_Involved_triggered() {

	QMessageBox::about(this, "Get Inveolved", "Genesys is a free open-source simulator (and tools) available at 'https://github.com/rlcancian/Genesys-Simulator'. Help us by submiting your pull requests containing code improvements.");
}

void MainWindow::on_checkBox_ShowLevels_stateChanged(int arg1) {

	bool result = _createModelImage();
}

void MainWindow::on_tabWidget_Debug_currentChanged(int index) {
	_actualizeActions();
}

void MainWindow::on_pushButton_Breakpoint_Insert_clicked() {
	//ModelSimulation* sim = simulator->getModels()->current()->getSimulation();
	dialogBreakpoint* dialog = new dialogBreakpoint();
	dialog->setMVCModel(simulator);
	dialog->show();
	dialog->raise();
	dialog->activateWindow();
	std::string type, on;
	dialogBreakpoint::MVCResult* result = dialog->getMVCResult();
	if (result->type == "Time") {

	} else if (result->type == "Entity") {

	} else if (result->type == "Component") {

	}

	dialog->~dialogBreakpoint();
	_actualizeDebugBreakpoints(true);
}

void MainWindow::on_pushButton_Breakpoint_Remove_clicked() {
	ModelSimulation* sim = simulator->getModels()->current()->getSimulation();
}

void MainWindow::on_tabWidgetCentral_currentChanged(int index) {
	_actualizeTabPanes();
}

void MainWindow::on_tabWidgetCentral_tabBarClicked(int index) {
}

void MainWindow::on_treeWidget_Plugins_itemDoubleClicked(QTreeWidgetItem *item, int column) {
	if (ui->TextCodeEditor->isEnabled()) { // add text to modelsimulation
		/*
		if (item->toolTip(0).contains("DataDefinition")) {
			QTextCursor cursor = ui->TextCodeEditor->textCursor();
			QTextCursor cursorSaved = cursor;
			cursor.movePosition(QTextCursor::Start);
			ui->TextCodeEditor->setTextCursor(cursor);
			if (ui->TextCodeEditor->find("# Model Components")) {
				ui->TextCodeEditor->moveCursor(QTextCursor::MoveOperation::Left, QTextCursor::MoveMode::MoveAnchor);
				ui->TextCodeEditor->moveCursor(QTextCursor::MoveOperation::Up, QTextCursor::MoveMode::MoveAnchor);
				ui->TextCodeEditor->insertPlainText(item->statusTip(0) + "\n");
			} else {
				ui->TextCodeEditor->appendPlainText(item->statusTip(0));
			}
		} else {
			ui->TextCodeEditor->appendPlainText(item->statusTip(0));
		}
		 */
	} else {
		// treeRoot? Always?
		for (int i = 0; i < ui->treeWidget_Plugins->topLevelItemCount(); i++) {
			//if (ui->treeWidget_Plugins->topLevelItem(i) != item) {
			ui->treeWidget_Plugins->topLevelItem(i)->setExpanded(false);
			//} else {
			//	ui->treeWidget_Plugins->expandItem(item);
			//	//ui->treeWidget_Plugins->topLevelItem(i)->setExpanded(true);
			//}
		}
		//ui->treeWidget_Plugins->setAnimated(true);
		ui->treeWidget_Plugins->expandItem(item);
	}
}

void MainWindow::on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint) {

}

void MainWindow::on_horizontalSlider_ZoomGraphical_valueChanged(int value) {
	double factor = (value - _zoomValue)*0.002;
	_zoomValue = value;
	_gentle_zoom(1.0 + factor);
}

void MainWindow::_gentle_zoom(double factor) {
	QPointF target_scene_pos, target_viewport_pos;
	QPoint mouse_event_pos = QPoint(100, 100);
	target_viewport_pos = mouse_event_pos;
	target_scene_pos = ui->graphicsView->mapToScene(mouse_event_pos);
	ui->graphicsView->scale(factor, factor);
	ui->graphicsView->centerOn(target_scene_pos);
	QPointF delta_viewport_pos = target_viewport_pos - QPointF(ui->graphicsView->viewport()->width() / 2.0,
			ui->graphicsView->viewport()->height() / 2.0);
	QPointF viewport_center = ui->graphicsView->mapFromScene(target_scene_pos) - delta_viewport_pos;
	ui->graphicsView->centerOn(ui->graphicsView->mapToScene(viewport_center.toPoint()));
	//emit zoomed();
}

void MainWindow::on_actionConnect_triggered() {
	((ModelGraphicsView*) ui->graphicsView)->beginConnection();
}

void MainWindow::on_pushButton_Export_clicked() {

}

void MainWindow::on_tabWidgetModelLanguages_currentChanged(int index) {
	if (index == CONST.TabModelSimLangIndex) {
		if (_graphicalModelHasChanged) {
			_actualizeModelSimLanguage();
		}
	} else if (index == CONST.TabModelCppCodeIndex) {
		_actualizeModelCppCode();
	}
	_actualizeActions();

}

void MainWindow::on_actionComponent_Breakpoint_triggered() {
	if (ui->graphicsView->selectedItems().size() == 1) {
		QGraphicsItem* gi = ui->graphicsView->selectedItems().at(0);
		GraphicalModelComponent* gmc = dynamic_cast<GraphicalModelComponent*> (gi);
		if (gmc != nullptr) {
			ModelComponent* mc = gmc->getComponent();
			ModelSimulation* sim = simulator->getModels()->current()->getSimulation();
			if (sim->getBreakpointsOnComponent()->find(mc) == sim->getBreakpointsOnComponent()->list()->end()) {
				sim->getBreakpointsOnComponent()->insert(mc);
			} else {
				sim->getBreakpointsOnComponent()->remove(mc);
			}
		}
		_actualizeDebugBreakpoints(false);
	}
}

void MainWindow::on_treeWidgetComponents_itemSelectionChanged() {

}

void MainWindow::on_treeWidget_Plugins_itemClicked(QTreeWidgetItem *item, int column) {

}

void MainWindow::on_TextCodeEditor_textChanged() {
	this->_actualizeModelTextHasChanged(true);
}

void MainWindow::on_tabWidgetModel_currentChanged(int index) {
	_actualizeTabPanes();
}

void MainWindow::on_tabWidgetSimulation_currentChanged(int index) {
	_actualizeTabPanes();
}

void MainWindow::on_tabWidgetReports_currentChanged(int index) {
	_actualizeTabPanes();
}
