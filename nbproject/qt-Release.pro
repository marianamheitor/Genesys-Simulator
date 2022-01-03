# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Release/GNU-Linux
TARGET = 2019_2022_GenESyS
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += release 
PKGCONFIG +=
QT = core gui widgets
SOURCES += kernel/simulator/Attribute.cpp kernel/simulator/ComponentManager.cpp kernel/simulator/ConnectionManager.cpp kernel/simulator/Counter.cpp kernel/simulator/ElementManager.cpp kernel/simulator/Entity.cpp kernel/simulator/EntityType.cpp kernel/simulator/Event.cpp kernel/simulator/ExperimentManager.cpp kernel/simulator/ExperimentManagerDefaultImpl1.cpp kernel/simulator/LicenceManager.cpp kernel/simulator/Model.cpp kernel/simulator/ModelCheckerDefaultImpl1.cpp kernel/simulator/ModelComponent.cpp kernel/simulator/ModelElement.cpp kernel/simulator/ModelInfo.cpp kernel/simulator/ModelManager.cpp kernel/simulator/ModelPersistenceDefaultImpl1.cpp kernel/simulator/ModelSimulation.cpp kernel/simulator/OnEventManager.cpp kernel/simulator/ParserChangesInformation.cpp kernel/simulator/ParserDefaultImpl1.cpp kernel/simulator/ParserDefaultImpl2.cpp kernel/simulator/ParserManager.cpp kernel/simulator/Plugin.cpp kernel/simulator/PluginConnectorDummyImpl1.cpp kernel/simulator/PluginInformation.cpp kernel/simulator/PluginManager.cpp kernel/simulator/SimulationControl.cpp kernel/simulator/SimulationExperiment.cpp kernel/simulator/SimulationReporterDefaultImpl1.cpp kernel/simulator/SimulationResponse.cpp kernel/simulator/SimulationScenario.cpp kernel/simulator/Simulator.cpp kernel/simulator/SinkModelComponent.cpp kernel/simulator/SourceModelComponent.cpp kernel/simulator/StatisticsCollector.cpp kernel/simulator/TraceManager.cpp kernel/simulator/parserBisonFlex/Genesys++-driver.cpp kernel/simulator/parserBisonFlex/Genesys++-scanner.cpp kernel/simulator/parserBisonFlex/GenesysParser.cpp kernel/simulator/parserBisonFlex/obj_t.cpp kernel/statistics/CollectorDatafileDefaultImpl1.cpp kernel/statistics/CollectorDefaultImpl1.cpp kernel/statistics/SamplerDefaultImpl1.cpp kernel/statistics/SorttFile.cpp kernel/statistics/StatisticsDataFileDefaultImpl.cpp kernel/statistics/StatisticsDefaultImpl1.cpp kernel/util/Util.cpp main.cpp plugins/components/Access.cpp plugins/components/Assign.cpp plugins/components/Batch.cpp plugins/components/CellularAutomata.cpp plugins/components/Create.cpp plugins/components/Decide.cpp plugins/components/Delay.cpp plugins/components/Dispose.cpp plugins/components/DropOff.cpp plugins/components/Dummy.cpp plugins/components/Enter.cpp plugins/components/Exit.cpp plugins/components/Hold.cpp plugins/components/LSODE.cpp plugins/components/Leave.cpp plugins/components/MarkovChain.cpp plugins/components/Match.cpp plugins/components/OLD_ODEelement.cpp plugins/components/PickStation.cpp plugins/components/PickUp.cpp plugins/components/Process.cpp plugins/components/QueueableItem.cpp plugins/components/Record.cpp plugins/components/Release.cpp plugins/components/Remove.cpp plugins/components/Route.cpp plugins/components/Search.cpp plugins/components/SeizableItem.cpp plugins/components/Seize.cpp plugins/components/Separate.cpp plugins/components/Signal.cpp plugins/components/Start.cpp plugins/components/Stop.cpp plugins/components/Store.cpp plugins/components/Submodel.cpp plugins/components/Unstore.cpp plugins/components/Write.cpp plugins/elements/EntityGroup.cpp plugins/elements/Failure.cpp plugins/elements/File.cpp plugins/elements/Formula.cpp plugins/elements/Queue.cpp plugins/elements/Resource.cpp plugins/elements/Schedule.cpp plugins/elements/Sequence.cpp plugins/elements/Set.cpp plugins/elements/Station.cpp plugins/elements/Storage.cpp plugins/elements/Variable.cpp tests/testProbabilityDistribution.cpp tools/HypothesisTesterDefaultImpl1.cpp tools/IntegratorDefaultImpl1.cpp tools/ProbabilityDistribution.cpp userInterfaces/BaseConsoleGenesysApplication.cpp userInterfaces/examples/AnElectronicAssemblyAndTestSystem.cpp userInterfaces/examples/Example_AssignWrite3Seizes.cpp userInterfaces/examples/Example_Delay.cpp userInterfaces/examples/Example_Dummy.cpp userInterfaces/examples/Example_HoldSignal.cpp userInterfaces/examples/Example_ModelInfoModelSimulation.cpp userInterfaces/examples/Example_Process.cpp userInterfaces/examples/Example_ProcessSet.cpp userInterfaces/examples/Example_RouteStation.cpp userInterfaces/examples/Example_SeizeDelayRelease.cpp userInterfaces/examples/Example_SeizeDelayReleaseMany.cpp userInterfaces/examples/Example_Sequence.cpp userInterfaces/examples/FullSimulationOfComplexModel.cpp userInterfaces/examples/book/Book_Cap02_Example01.cpp userInterfaces/gui/qt/GenesysQtGUI.cpp userInterfaces/gui/qt/dialogs/dialogabout.cpp userInterfaces/gui/qt/dialogs/dialogmodelinformation.cpp userInterfaces/gui/qt/forms/formPlugins.cpp userInterfaces/gui/qt/mainWindow/mainwindow.cpp userInterfaces/gui/qt/mapkernelandui.cpp userInterfaces/terminal/GenesysTerminalApp.cpp
HEADERS += Traits.h kernel/TraitsKernel.h kernel/simulator/Attribute.h kernel/simulator/ComponentManager.h kernel/simulator/ConnectionManager.h kernel/simulator/Counter.h kernel/simulator/DefineGetterSetter.h kernel/simulator/ElementManager.h kernel/simulator/ElementManager_if.h kernel/simulator/Entity.h kernel/simulator/EntityType.h kernel/simulator/Event.h kernel/simulator/ExperimentManager.h kernel/simulator/ExperimentManagerDefaultImpl1.h kernel/simulator/ExperimetManager_if.h kernel/simulator/LicenceManager.h kernel/simulator/Model.h kernel/simulator/ModelCheckerDefaultImpl1.h kernel/simulator/ModelChecker_if.h kernel/simulator/ModelComponent.h kernel/simulator/ModelElement.h kernel/simulator/ModelInfo.h kernel/simulator/ModelManager.h kernel/simulator/ModelPersistenceDefaultImpl1.h kernel/simulator/ModelPersistence_if.h kernel/simulator/ModelSimulation.h kernel/simulator/OnEventManager.h kernel/simulator/ParserChangesInformation.h kernel/simulator/ParserDefaultImpl1.h kernel/simulator/ParserDefaultImpl2.h kernel/simulator/ParserManager.h kernel/simulator/Parser_if.h kernel/simulator/PersistentObject_base.h kernel/simulator/Plugin.h kernel/simulator/PluginConnectorDummyImpl1.h kernel/simulator/PluginConnector_if.h kernel/simulator/PluginInformation.h kernel/simulator/PluginManager.h kernel/simulator/ScenarioExperiment_if.h kernel/simulator/SimulationControl.h kernel/simulator/SimulationExperiment.h kernel/simulator/SimulationReporterDefaultImpl1.h kernel/simulator/SimulationReporter_if.h kernel/simulator/SimulationResponse.h kernel/simulator/SimulationScenario.h kernel/simulator/Simulator.h kernel/simulator/SinkModelComponent.h kernel/simulator/SourceModelComponent.h kernel/simulator/StatisticsCollector.h kernel/simulator/TraceManager.h kernel/simulator/parserBisonFlex/Genesys++-driver.h kernel/simulator/parserBisonFlex/GenesysParser.h kernel/simulator/parserBisonFlex/obj_t.h kernel/statistics/CollectorDatafileDefaultImpl1.h kernel/statistics/CollectorDatafile_if.h kernel/statistics/CollectorDefaultImpl1.h kernel/statistics/Collector_if.h kernel/statistics/SamplerDefaultImpl1.h kernel/statistics/Sampler_if.h kernel/statistics/SorttFile.h kernel/statistics/StatisticsDataFileDefaultImpl.h kernel/statistics/StatisticsDataFile_if.h kernel/statistics/StatisticsDefaultImpl1.h kernel/statistics/Statistics_if.h kernel/util/Exact.h kernel/util/List.h kernel/util/Util.h plugins/components/Access.h plugins/components/Assign.h plugins/components/Batch.h plugins/components/CellularAutomata.h plugins/components/Create.h plugins/components/Decide.h plugins/components/Delay.h plugins/components/Dispose.h plugins/components/DropOff.h plugins/components/Dummy.h plugins/components/Enter.h plugins/components/Exit.h plugins/components/Hold.h plugins/components/LSODE.h plugins/components/Leave.h plugins/components/MarkovChain.h plugins/components/Match.h plugins/components/OLD_ODEelement.h plugins/components/PickStation.h plugins/components/PickUp.h plugins/components/Process.h plugins/components/QueueableItem.h plugins/components/Record.h plugins/components/Release.h plugins/components/Remove.h plugins/components/Route.h plugins/components/Search.h plugins/components/SeizableItem.h plugins/components/Seize.h plugins/components/Separate.h plugins/components/Signal.h plugins/components/Start.h plugins/components/Stop.h plugins/components/Store.h plugins/components/Submodel.h plugins/components/Unstore.h plugins/components/Write.h plugins/elements/EntityGroup.h plugins/elements/Failure.h plugins/elements/File.h plugins/elements/Formula.h plugins/elements/Queue.h plugins/elements/Resource.h plugins/elements/Schedule.h plugins/elements/Sequence.h plugins/elements/Set.h plugins/elements/Station.h plugins/elements/Storage.h plugins/elements/Variable.h tools/HypothesisTesterDefaultImpl1.h tools/HypothesisTester_if.h tools/IntegratorDefaultImpl1.h tools/Integrator_if.h tools/ProbabilityDistribution.h userInterfaces/BaseConsoleGenesysApplication.h userInterfaces/GenesysApplication_if.h userInterfaces/examples/AnElectronicAssemblyAndTestSystem.h userInterfaces/examples/Example_AssignWrite3Seizes.h userInterfaces/examples/Example_Delay.h userInterfaces/examples/Example_Dummy.h userInterfaces/examples/Example_HoldSignal.h userInterfaces/examples/Example_ModelInfoModelSimulation.h userInterfaces/examples/Example_Process.h userInterfaces/examples/Example_ProcessSet.h userInterfaces/examples/Example_RouteStation.h userInterfaces/examples/Example_SeizeDelayRelease.h userInterfaces/examples/Example_SeizeDelayReleaseMany.h userInterfaces/examples/Example_Sequence.h userInterfaces/examples/FullSimulationOfComplexModel.h userInterfaces/examples/book/Book_Cap02_Example01.h userInterfaces/gui/qt/GenesysQtGUI.h userInterfaces/gui/qt/dialogs/dialogabout.h userInterfaces/gui/qt/dialogs/dialogmodelinformation.h userInterfaces/gui/qt/forms/formPlugins.h userInterfaces/gui/qt/mainWindow/mainwindow.h userInterfaces/gui/qt/mapkernelandui.h userInterfaces/terminal/GenesysShell_if.h userInterfaces/terminal/GenesysTerminalApp.h
FORMS += userInterfaces/gui/qt/dialogs/dialogabout.ui userInterfaces/gui/qt/dialogs/dialogmodelinformation.ui userInterfaces/gui/qt/forms/formPlugins.ui userInterfaces/gui/qt/mainWindow/mainwindow.ui
RESOURCES += userInterfaces/gui/qt/guiresources.qrc
TRANSLATIONS +=
OBJECTS_DIR = build/Release/GNU-Linux
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += 
LIBS += 
