// The Firmament project
// Copyright (c) 2016 Ionel Gog <ionel.gog@cl.cam.ac.uk>

#ifndef FIRMAMENT_SCHEDULING_NET_COST_MODEL_H
#define FIRMAMENT_SCHEDULING_NET_COST_MODEL_H

#include <set>
#include <string>
#include <utility>
#include <vector>

#include "base/common.h"
#include "base/types.h"
#include "misc/map-util.h"
#include "scheduling/common.h"
#include "scheduling/knowledge_base.h"
#include "scheduling/flow/cost_model_interface.h"

namespace firmament {

class NetCostModel : public CostModelInterface {
 public:
  NetCostModel(shared_ptr<ResourceMap_t> resource_map,
               shared_ptr<TaskMap_t> task_map,
               shared_ptr<KnowledgeBase> knowledge_base);
  // Costs pertaining to leaving tasks unscheduled
  Cost_t TaskToUnscheduledAggCost(TaskID_t task_id);
  Cost_t UnscheduledAggToSinkCost(JobID_t job_id);
  // Per-task costs (into the resource topology)
  Cost_t TaskToResourceNodeCost(TaskID_t task_id,
                                ResourceID_t resource_id);
  // Costs within the resource topology
  Cost_t ResourceNodeToResourceNodeCost(const ResourceDescriptor& source,
                                        const ResourceDescriptor& destination);
  Cost_t LeafResourceNodeToSinkCost(ResourceID_t resource_id);
  // Costs pertaining to preemption (i.e. already running tasks)
  Cost_t TaskContinuationCost(TaskID_t task_id);
  Cost_t TaskPreemptionCost(TaskID_t task_id);
  // Costs to equivalence class aggregators
  Cost_t TaskToEquivClassAggregator(TaskID_t task_id, EquivClass_t tec);
  pair<Cost_t, uint64_t> EquivClassToResourceNode(
      EquivClass_t tec,
      ResourceID_t res_id);
  pair<Cost_t, uint64_t> EquivClassToEquivClass(EquivClass_t tec1,
                                                EquivClass_t tec2);
  // Get the type of equiv class.
  vector<EquivClass_t>* GetTaskEquivClasses(TaskID_t task_id);
  vector<ResourceID_t>* GetOutgoingEquivClassPrefArcs(EquivClass_t tec);
  vector<ResourceID_t>* GetTaskPreferenceArcs(TaskID_t task_id);
  vector<EquivClass_t>* GetEquivClassToEquivClassesArcs(EquivClass_t tec);
  void AddMachine(ResourceTopologyNodeDescriptor* rtnd_ptr);
  void AddTask(TaskID_t task_id);
  void RemoveMachine(ResourceID_t res_id);
  void RemoveTask(TaskID_t task_id);
  FlowGraphNode* GatherStats(FlowGraphNode* accumulator, FlowGraphNode* other);
  void PrepareStats(FlowGraphNode* accumulator);
  FlowGraphNode* UpdateStats(FlowGraphNode* accumulator, FlowGraphNode* other);

 private:
  EquivClass_t GetMachineEC(const string& machine_name, uint64_t ec_index);
  inline const TaskDescriptor& GetTask(TaskID_t task_id) {
    TaskDescriptor* td = FindPtrOrNull(*task_map_, task_id);
    CHECK_NOTNULL(td);
    return *td;
  }

  shared_ptr<ResourceMap_t> resource_map_;
  // The task map used in the rest of the system
  shared_ptr<TaskMap_t> task_map_;
  // A knowledge base instance that we will refer to for job runtime statistics.
  shared_ptr<KnowledgeBase> knowledge_base_;
  unordered_map<TaskID_t, uint64_t> task_bw_requirement_;
  unordered_map<EquivClass_t, uint64_t> ec_bw_requirement_;
  unordered_map<ResourceID_t, vector<EquivClass_t>, boost::hash<ResourceID_t>>
    ecs_for_machines_;
  unordered_map<EquivClass_t, ResourceID_t> ec_to_machine_;
  unordered_map<EquivClass_t, uint64_t> ec_to_index_;
};

}  // namespace firmament

#endif  // FIRMAMENT_SCHEDULING_NET_COST_MODEL_H
