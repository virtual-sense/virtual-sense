/*
 *	invoke_instructions.h
 *
 *	Copyright (c) 2008 CSIRO, Delft University of Technology.
 *
 *	This file is part of Darjeeling.
 *
 *	Darjeeling is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	Darjeeling is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with Darjeeling.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Return from function
 */
static inline void RETURN()
{
	// return
	returnFromMethod();
}

/**
 * Return from int function
 */
static inline void IRETURN()
{

	// pop return value off the stack
	int32_t ret = popInt();

	// return
	returnFromMethod();

	// push return value on the runtime stack
	pushInt(ret);
}

/**
 * Return from short/byte/boolean/char function
 */
static inline void SRETURN()
{

	// pop return value off the stack
	int16_t ret = popShort();

	// return
	returnFromMethod();

	// push return value on the runtime stack
	pushShort(ret);
}

static inline void ARETURN()
{

	// pop return value off the stack
	ref_t ret = popRef();

	// return
	returnFromMethod();

	// push return value on the runtime stack
	pushRef(ret);
}


static inline void INVOKESTATIC()
{
	dj_local_id localId = dj_fetchLocalId();
	dj_global_id globalId = dj_global_id_resolve(dj_exec_getCurrentInfusion(),  localId);
	callMethod(globalId, false);
}


static inline void INVOKESPECIAL()
{
	dj_local_id localId = dj_fetchLocalId();
	dj_global_id globalId = dj_global_id_resolve(dj_exec_getCurrentInfusion(),  localId);

	callMethod(globalId, true);
}

static inline void INVOKEVIRTUAL()
{
	// fetch the method definition's global id and resolve it
	dj_local_id dj_local_id = dj_fetchLocalId();
	DEBUG_LOG("virtualM_loc_Id %u\n", dj_local_id);
	// fetch the number of arguments for the method.
	uint8_t nr_ref_args = fetch();

	DEBUG_LOG("args n %u, %p\n", nr_ref_args,refStack);
	// peek the object on the stack
	dj_object *object = REF_TO_VOIDP(peekDeepRef(nr_ref_args));

	// if null, throw exception
	if (object==nullref) //WAS NULL
	{
		DEBUG_LOG("Retreived object NULL\n", nr_ref_args,refStack);
		dj_exec_createAndThrow(BASE_CDEF_java_lang_NullPointerException);
		return;
	}
	DEBUG_LOG("Object %p\n", object);
	// check if the object is still valid
	if (dj_object_getRuntimeId(object)==CHUNKID_INVALID)
	{
		dj_exec_createAndThrow(BASE_CDEF_javax_darjeeling_vm_ClassUnloadedException);
		return;
	}

	dj_global_id resolvedMethodDefId = dj_global_id_resolve(dj_exec_getCurrentInfusion(), dj_local_id);
	DEBUG_LOG("ResolvedMDefId %u %p\n", resolvedMethodDefId, object);
	// lookup the virtual method
	dj_global_id methodImplId = dj_global_id_lookupVirtualMethod(resolvedMethodDefId, object);

	// check if method not found, and throw an error if this is the case. else, invoke the method
	if (methodImplId.infusion==NULL)
	{
		//dj_global_id id = dj_vm_getRuntimeClass(dj_exec_getVM(), dj_mem_getChunkId(object));
		//DEBUG_LOG("Shouldn't happen! %d: %d, %d\n", id.entity_id, dj_local_id.infusion_id, dj_local_id.entity_id);
		//dj_exec_throwHere(dj_vm_createSysLibObject(dj_exec_getVM(), BASE_CDEF_java_lang_VirtualMachineError));
	} else
	{
		//DEBUG_LOG("ResolvedMDefId %u\n", resolvedMethodDefId);
		callMethod(methodImplId, true);

	}
}


static inline void INVOKEINTERFACE()
{
	INVOKEVIRTUAL();
}
