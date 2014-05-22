/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2013 Blender Foundation.
 * All rights reserved.
 *
 * Original Author: Joshua Leung
 * Contributor(s): None Yet
 *
 * ***** END GPL LICENSE BLOCK *****
 */

#ifndef __DEPSNODE_COMPONENT_H__
#define __DEPSNODE_COMPONENT_H__

#include "MEM_guardedalloc.h"

#include "DEG_depsgraph.h"
#include "depsnode.h"

#include "depsgraph_util_map.h"
#include "depsgraph_util_set.h"

struct ID;
struct bPoseChannel;

struct Depsgraph;
struct DepsgraphCopyContext;
struct OperationDepsNode;
struct BoneComponentDepsNode;

/* ID Component - Base type for all components */
struct ComponentDepsNode : public DepsNode {
	typedef unordered_map<string, OperationDepsNode *> OperationMap;
	
	ComponentDepsNode();
	
	void init(const ID *id, const string &subdata);
	void copy(DepsgraphCopyContext *dcc, const ComponentDepsNode *src);
	~ComponentDepsNode();
	
	OperationDepsNode *find_operation(const string &name) const;
	/* Create a new node for representing an operation and add this to graph
	 * ! If an existing node is found, it will be modified. This helps when node may 
	 *   have been partially created earlier (e.g. parent ref before parent item is added)
	 *
	 * < type: Operation node type (corresponding to context/component that it operates in)
	 * < optype: Role that operation plays within component (i.e. where in eval process)
	 * < op: The operation to perform
	 * < name: Identifier for operation - used to find/locate it again
	 */
	OperationDepsNode *add_operation(eDepsNode_Type type, eDepsOperation_Type optype, 
	                                 DepsEvalOperationCb op, const string &name);
	void remove_operation(const string &name);
	void clear_operations();
	
	void tag_update(Depsgraph *graph);
	
	/* Evaluation Context Management .................. */
	
	/* Initialise component's evaluation context used for the specified purpose */
	virtual bool eval_context_init(eEvaluationContextType context_type) { return false; }
	/* Free data in component's evaluation context which is used for the specified purpose 
	 * NOTE: this does not free the actual context in question
	 */
	virtual void eval_context_free(eEvaluationContextType context_type) {}
	
	IDDepsNode *owner;
	
	OperationMap operations;    /* inner nodes for this component */
	OperationDepsNode *entry_operation;
	OperationDepsNode *exit_operation;
	
	/* (DEG_OperationsContext) array of evaluation contexts to be passed to evaluation functions for this component. 
	 *                         Only the requested context will be used during any particular evaluation
	 */
	void *contexts[DEG_MAX_EVALUATION_CONTEXTS];
	
	// XXX: a poll() callback to check if component's first node can be started?
};

/* ---------------------------------------- */

struct ParametersComponentDepsNode : public ComponentDepsNode {
	DEG_DEPSNODE_DECLARE;
};

struct AnimationComponentDepsNode : public ComponentDepsNode {
	DEG_DEPSNODE_DECLARE;
};

struct TransformComponentDepsNode : public ComponentDepsNode {
	DEG_DEPSNODE_DECLARE;
};

struct ProxyComponentDepsNode : public ComponentDepsNode {
	DEG_DEPSNODE_DECLARE;
};

struct GeometryComponentDepsNode : public ComponentDepsNode {
	DEG_DEPSNODE_DECLARE;
};

struct SequencerComponentDepsNode : public ComponentDepsNode {
	DEG_DEPSNODE_DECLARE;
};

/* Pose Evaluation - Sub-data needed */
struct PoseComponentDepsNode : public ComponentDepsNode {
	typedef unordered_map<string, BoneComponentDepsNode *> BoneComponentMap;
	
	BoneComponentDepsNode *find_bone_component(const string &name) const;
	BoneComponentDepsNode *add_bone_component(const string &name);
	void remove_bone_component(const string &name);
	void clear_bone_components();
	
	void init(const ID *id, const string &subdata);
	void copy(DepsgraphCopyContext *dcc, const PoseComponentDepsNode *src);
	~PoseComponentDepsNode();
	
	BoneComponentMap bone_hash; /* hash for quickly finding bone components */
	
	DEG_DEPSNODE_DECLARE;
};

/* Bone Component */
struct BoneComponentDepsNode : public ComponentDepsNode {
	void init(const ID *id, const string &subdata);
	
	PoseComponentDepsNode *pose_owner;
	struct bPoseChannel *pchan;     /* the bone that this component represents */
	
	DEG_DEPSNODE_DECLARE;
};

void DEG_register_component_depsnodes();

#endif // __DEPSNODE_COMPONENT_H__
