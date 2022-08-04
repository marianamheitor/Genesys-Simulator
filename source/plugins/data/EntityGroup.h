/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Group.h
 * Author: rlcancian
 *
 * Created on 12 de Junho de 2019, 19:00
 */

#ifndef ENTITYGROUP_H
#define ENTITYGROUP_H

#include "../../kernel/simulator/ModelDataDefinition.h"
#include "../../kernel/simulator/Entity.h"
#include "../../kernel/util/List.h"
#include <map>

/*
 The EntityGroup represents a map of entities grouped togheter somehow.
 * The entities in this group are kept into a internal queue.
 * Usually one entity representing all grouped entities is generated by ModelComponents and the EntityGroup it represented is accessed through an attribute.
 * The Key of this map is the ID of the representative entity
 */
class EntityGroup : public ModelDataDefinition {
public:
	EntityGroup(Model* model, std::string name = "");
	virtual ~EntityGroup();
public:
	virtual std::string show();
public:
	static PluginInformation* GetPluginInformation();
	static ModelDataDefinition* LoadInstance(Model* model, PersistenceRecord *fields);
	static ModelDataDefinition* NewInstance(Model* model, std::string name = "");
public:
	void insertElement(unsigned int idKey, Entity* modeldatum);
	void removeElement(unsigned int idKey, Entity* modeldatum);
	List<Entity*>* getGroup(unsigned int idKey);
public:
	void initBetweenReplications();
protected:
	virtual bool _loadInstance(PersistenceRecord *fields);
	virtual void _saveInstance(PersistenceRecord *fields, bool saveDefaultValues);
	virtual bool _check(std::string* errorMessage);
	virtual void _createInternalAndAttachedData();
private:
	void _initCStats();
private: //1::n
	std::map<unsigned int, List<Entity*>*>* _groupMap = new std::map<unsigned int, List<Entity*>*>();
private: // child inner modeldatum
	StatisticsCollector* _cstatNumberInGroup = nullptr;
};

#endif /* ENTITYGROUP_H */

