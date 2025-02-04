/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Queue.h
 * Author: rafael.luiz.cancian
 *
 * Created on 21 de Agosto de 2018, 17:12
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "../../kernel/simulator/ModelDataDefinition.h"
#include "../../kernel/util/List.h"
#include "../../kernel/simulator/Entity.h"
#include "../../kernel/simulator/ModelDataManager.h"
#include "../../kernel/simulator/StatisticsCollector.h"
#include "../../kernel/simulator/Plugin.h"
#include "../../kernel/simulator/ModelComponent.h"

class Waiting {
public:
	Waiting(Entity* entity, double timeStartedWaiting, ModelComponent* thisComponent, unsigned int thisComponentOutputPort = 0) {
		_entity = entity;
		_thisComponent = thisComponent;
		_timeStartedWaiting = timeStartedWaiting;
		_thisComponentOutputPort = thisComponentOutputPort;
	}

	virtual ~Waiting() = default;
public:
	virtual std::string show() {
		return //ModelDataDefinition::show()+
		",entity=" + std::to_string(_entity->getId()) +
				",component=\"" + _thisComponent->getName() + "\"" +
				",inputPort=\"" + std::to_string(_thisComponentOutputPort) + "\"" +
				",timeStartedWaiting=" + std::to_string(_timeStartedWaiting);
	}
public:
	double getTimeStartedWaiting() const {
		return _timeStartedWaiting;
	}
	ModelComponent* geComponent() const {
		return _thisComponent;
	}
	Entity* getEntity() const {
		return _entity;
	}
	unsigned int geComponentOutputPort() const {
		return _thisComponentOutputPort;
	}
private:
	Entity* _entity;
	ModelComponent* _thisComponent;
	double _timeStartedWaiting;
	unsigned int _thisComponentOutputPort;
};

/*!
 Queue module
DESCRIPTION
This data module may be utilized to change the ranking rule for a specified queue.
The default ranking rule for all queues is First In, First Out unless otherwise specified
in this module. There is an additional field that allows the queue to be defined as
shared.
TYPICAL USES
* Stack of work waiting for a resource at a Process module
* Holding area for documents waiting to be collated at a Batch module
Prompt Description
Name The name of the queue whose characteristics are being defined.
This name must be unique.
Type Ranking rule for the queue, which can be based on an attribute.
Types include First In, First Out; Last In, First Out; Lowest
Attribute Value (first); and Highest Attribute Value (first). A
low attribute value would be 0 or 1, while a high value may be
200 or 300.
Attribute Name Attribute that will be evaluated for the Lowest Attribute Value or
Highest Attribute Value types. Entities with lowest or highest
values of the attribute will be ranked first in the queue, with ties
being broken using the First In, First Out rule.
Shared Check box that determines whether a specific queue is used in
multiple places within the simulation model. Shared queues can
only be used for seizing resources (for example, with the Seize
module from the Advanced Process panel).
Report Statistics Specifies whether or not statistics will be collected automatically
and stored in the report database for this queue.
 */
class Queue : public ModelDataDefinition {
public:

	enum class OrderRule : int {
		FIFO = 0, LIFO = 1, HIGHESTVALUE = 2, SMALLESTVALUE = 3, num_elements = 4
	};
public:
	static std::string convertEnumToStr(OrderRule rule);
public:
	Queue(Model* model, std::string name = "");
	virtual ~Queue();
public:
	virtual std::string show();
public: // static
	static PluginInformation* GetPluginInformation();
	static ModelDataDefinition* LoadInstance(Model* model, PersistenceRecord *fields);
	static ModelDataDefinition* NewInstance(Model* model, std::string name = "");
public:
	void insertElement(Waiting* modeldatum);
	void removeElement(Waiting* modeldatum);
	unsigned int size();
	Waiting* first();
	Waiting* getAtRank(unsigned int rank);
	void setAttributeName(std::string _attributeName);
	std::string getAttributeName() const;
	void setOrderRule(OrderRule _orderRule);
	Queue::OrderRule getOrderRule() const;
	void setOrderRuleInt(int orderRule);
	int getOrderRuleInt() const;
public: // to implement SIMAN functions
	double sumAttributesFromWaiting(Util::identification attributeID); // use to implement SIMAN SAQUE function
	double getAttributeFromWaitingRank(unsigned int rank, Util::identification attributeID);
	//public:
	//	void initBetweenReplications();
protected: // must be overriden
	virtual bool _loadInstance(PersistenceRecord *fields);
	virtual void _saveInstance(PersistenceRecord *fields, bool saveDefaultValues);
protected: // could be overriden
	virtual bool _check(std::string* errorMessage);
	virtual void _initBetweenReplications();
	virtual void _createInternalAndAttachedData();
	virtual ParserChangesInformation* _getParserChangesInformation();

private:
	void _initCStats();
private:
	List<Waiting*>* _list = new List<Waiting*>();
	double _lastTimeNumberInQueueChanged;
private: //1::1

	const struct DEFAULT_VALUES {
		const OrderRule orderRule = OrderRule::FIFO;
		const std::string attributeName = "";
	} DEFAULT;
	OrderRule _orderRule = DEFAULT.orderRule;
	std::string _attributeName = DEFAULT.attributeName;
private: // inner internal elements
	StatisticsCollector* _cstatNumberInQueue = nullptr;
	StatisticsCollector* _cstatTimeInQueue;
};

#endif /* QUEUE_H */

