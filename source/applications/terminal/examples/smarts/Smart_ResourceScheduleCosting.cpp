/*
 * This C++ program was automatically generated by Genesys 220517 (gowndictator)
 * For your own safety, please review this file before compiling and running it.
 */

#include "Smart_ResourceScheduleCosting.h"

#include "../../../../kernel/simulator/Simulator.h"
#include "../../../../plugins/components/Create.h"
#include "../../../../plugins/components/Dispose.h"
#include "../../../../plugins/components/DummyComponent.h"
#include "../../../../plugins/components/Assign.h"
#include "../../../../plugins/components/Process.h"
#include "../../../../plugins/components/QueueableItem.h"
#include "../../../../plugins/components/Seize.h"
#include "../../../../plugins/components/Delay.h"
#include "../../../../plugins/components/Release.h"
#include "../../../../plugins/data/Queue.h"
#include "../../../../plugins/data/Resource.h"
#include "../../../../plugins/data/Variable.h"
#include "../../../../plugins/data/Schedule.h"

Smart_ResourceScheduleCosting::Smart_ResourceScheduleCosting() {
}

int Smart_ResourceScheduleCosting::main(int argc, char** argv) {
	// instantiate simulator
	Simulator* genesys = new Simulator();
        this->setDefaultTraceHandlers(genesys->getTracer());
	this->insertFakePluginsByHand(genesys);
	genesys->getTracer()->setTraceLevel(TraceManager::Level::L9_mostDetailed);
	PluginManager* plugins = genesys->getPlugins();
	
	// create model
	Model* model = genesys->getModels()->newModel();
	// model->load("model.gen")
	
        // Initialize resources
        Resource* Resource_1 = plugins->newInstance<Resource>(model, "Biller");
        Resource_1->setCapacity(1); // TODO: CAPACIDADE BASEADA NO SCHEDULE
        Resource_1->setCostBusyHour(7.75);
        Resource_1->setCostIdleHour(7.75);
        Resource_1->setCostPerUse(0.02);
        
        Resource* Resource_2 = plugins->newInstance<Resource>(model, "Mailer");
        Resource_2->setCapacity(1); // TODO: CAPACIDADE BASEADA NO SCHEDULE
        Resource_2->setCostBusyHour(5.15);
        Resource_2->setCostIdleHour(5.15);
        Resource_2->setCostPerUse(0.02);
	
        // initialize model parts

        // Create 2
	Create* Create_2 = plugins->newInstance<Create>(model, "Arrival");
        Create_2->setEntityTypeName("Arrival");
	Create_2->setTimeBetweenCreationsExpression("expo(1.5)", Util::TimeUnit::minute);
	Create_2->setEntitiesPerCreation(1);
        Create_2->setFirstCreation(0.0);
        
        // Process 3
        Process* Process_3 =  plugins->newInstance<Process>(model, "Billing");
        Process_3->getSeizeRequests()->insert(new SeizableItem(Resource_1));
	Process_3->setQueueableItem(new QueueableItem(model, "BillingQueue"));
	Process_3->setDelayExpression("tria(0.5,1,1.5)");
        Process_3->setDelayTimeUnit(Util::TimeUnit::minute);
        Process_3->setAllocationType(Util::AllocationType::ValueAdded);     
        
        // process 4
        Process* Process_4 =  plugins->newInstance<Process>(model, "MailRoom");
        Process_4->getSeizeRequests()->insert(new SeizableItem(Resource_2));
	Process_4->setQueueableItem(new QueueableItem(model, "MailRoomQueue")); // ??
	Process_4->setDelayExpression("tria(0.5,1,1.5)");
        Process_4->setDelayTimeUnit(Util::TimeUnit::minute);
        Process_4->setAllocationType(Util::AllocationType::ValueAdded);
        // dispose 1
        Dispose* Dispose_1 = plugins->newInstance<Dispose>(model, "Depart");
              
	// connect model components
	Create_2->getConnections()->insert(Process_3);
        Process_3->getConnections()->insert(Process_4);
        Process_4->getConnections()->insert(Dispose_1);
	
	// set simulation parameters
	ModelSimulation* sim = model->getSimulation();
	sim->setNumberOfReplications(300);
        model->getSimulation()->setReplicationLength(480, Util::TimeUnit::minute);
        sim->setWarmUpPeriod(24);
	sim->setWarmUpPeriodTimeUnit(Util::TimeUnit::minute);
	model->getSimulation()->setTerminatingCondition("count(Depart.CountNumberIn)>1000");
	model->save("./models/Smart_ResourceScheduleCosting.gen");
	model->getSimulation()->start();
	// run the simulation
        model->check();
        model->getDataManager()->show();
	sim->start();
	for (int i = 0; i < 1e9; i++); // give UI some time to finish std::cout
	// free memory
	delete genesys;
	
	return 0;
}