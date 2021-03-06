/*
 *	InterpreterState.java
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
package org.csiro.darjeeling.infuser.bytecode.analysis;

import org.csiro.darjeeling.infuser.bytecode.InstructionHandle;
import org.csiro.darjeeling.infuser.bytecode.Opcode;
import org.csiro.darjeeling.infuser.bytecode.instructions.AbstractInvokeInstruction;
import org.csiro.darjeeling.infuser.structure.BaseType;
import org.csiro.darjeeling.infuser.structure.elements.AbstractMethodDefinition;

public class InterpreterState implements Comparable<InterpreterState>
{
	
	private InterpreterStack stack;

	public InterpreterState()
	{
		stack = new InterpreterStack();
	}
	
	private InterpreterState(InterpreterStack stack/*, InterpreterLocalVariableBlock localVariables*/)
	{
		this.stack = stack;
	}
	
	private void interpretInvoke(InstructionHandle handle)
	{
		AbstractInvokeInstruction invoke = (AbstractInvokeInstruction)handle.getInstruction();
		AbstractMethodDefinition methodDefinition = invoke.getMethodDefinition();
		
		// pop arguments off the stack
		for (int i=0; i<methodDefinition.getParameterCount(); i++) 
			stack.pop();
		
		// if the instruction is a virtual method call, pop the object reference too
		if (handle.getInstruction().getOpcode().isVirtualInvoke()) 
			stack.pop();
		
		// put the return value on the stack
		BaseType returnType = methodDefinition.getReturnType();
		if (returnType!=BaseType.Void)
			stack.push(handle);
	}
	
	public InterpreterState transition(InstructionHandle handle)
	{
		InterpreterState ret = clone();
		
		Opcode opcode = handle.getInstruction().getOpcode();
		
		switch (opcode)
		{
		
			case NOP:
				break;
				
			case SCONST_M1:
			case SCONST_0:
			case SCONST_1:
			case SCONST_2:
			case SCONST_3:
			case SCONST_4:
			case SCONST_5:
			case ICONST_M1:
			case ICONST_0:
			case ICONST_1:
			case ICONST_2:
			case ICONST_3:
			case ICONST_4:
			case ICONST_5:
			case ACONST_NULL:
				ret.getStack().push(handle);
				break;

			case SLOAD:
			case SLOAD_0:
			case SLOAD_1:
			case SLOAD_2:
			case SLOAD_3:
			case ILOAD:
			case ILOAD_0:
			case ILOAD_1:
			case ILOAD_2:
			case ILOAD_3:
			case ALOAD:
			case ALOAD_0:
			case ALOAD_1:
			case ALOAD_2:
			case ALOAD_3:
				ret.getStack().push(handle);
				break;

			case SSTORE:
			case SSTORE_0:
			case SSTORE_1:
			case SSTORE_2:
			case SSTORE_3:
			case ISTORE:
			case ISTORE_0:
			case ISTORE_1:
			case ISTORE_2:
			case ISTORE_3:
			case ASTORE:
			case ASTORE_0:
			case ASTORE_1:
			case ASTORE_2:
			case ASTORE_3:
				ret.getStack().pop();
				break;
				
			case IASTORE:
			case BASTORE:
			case CASTORE:
			case SASTORE:
			case AASTORE:
				ret.getStack().pop();
				ret.getStack().pop();
				ret.getStack().pop();
				break;
			
			case IALOAD:
			case BALOAD:
			case CALOAD:
			case SALOAD:
			case AALOAD:
				ret.getStack().pop();
				ret.getStack().pop();
				ret.getStack().push(handle);
				break;
			
			case INVOKESPECIAL:
			case INVOKEINTERFACE:
			case INVOKEVIRTUAL:
			case INVOKESTATIC:
				ret.interpretInvoke(handle);
				break;
				
			case BIPUSH:
				ret.getStack().push(handle);
				break;

			case BSPUSH:
				ret.getStack().push(handle);
				break;

			case SIPUSH:
				ret.getStack().push(handle);
				break;

			case SSPUSH:
				ret.getStack().push(handle);
				break;

			case IIPUSH:
				ret.getStack().push(handle);
				break;
				
			case LDS:
				ret.getStack().push(handle);
				break;
				
			case IADD:
			case ISUB:
			case IMUL:
			case IDIV:
			case IREM:
			case ISHL:
			case ISHR:
			case IUSHR:
			case IAND:
			case IOR:
			case IXOR:
				ret.getStack().pop();
				ret.getStack().pop();
				ret.getStack().push(handle);
				break;
				
			case INEG:
				ret.getStack().pop();
				ret.getStack().push(handle);
				break;
				
			case SADD:
			case SSUB:
			case SMUL:
			case SDIV:
			case SREM:
			case SSHL:
			case SSHR:
			case SUSHR:
			case SAND:
			case SOR:
			case SXOR:
				ret.getStack().pop();
				ret.getStack().pop();
				ret.getStack().push(handle);
				break;
				
			case SNEG:
				ret.getStack().pop();
				ret.getStack().push(handle);
				break;
				
			case IIFNE:
			case IIFEQ:
			case IIFLT:
			case IIFGE:
			case IIFGT:
			case IIFLE:
			case SIFNE:
			case SIFEQ:
			case SIFLT:
			case SIFGE:
			case SIFGT:
			case SIFLE:
			case IFNONNULL:
			case IFNULL:
				ret.getStack().pop();
				break;

			case IF_ACMPEQ:
			case IF_ACMPNE:
			case IF_ICMPEQ:
			case IF_ICMPGE:
			case IF_ICMPGT:
			case IF_ICMPLE:
			case IF_ICMPLT:
			case IF_ICMPNE:
			case IF_SCMPEQ:
			case IF_SCMPGE:
			case IF_SCMPGT:
			case IF_SCMPLE:
			case IF_SCMPLT:
			case IF_SCMPNE:
				ret.getStack().pop();
				ret.getStack().pop();
				break;
				
			case GETFIELD_A:
			case GETFIELD_B:
			case GETFIELD_C:
			case GETFIELD_I:
			case GETFIELD_S:
				ret.getStack().pop();
				ret.getStack().push(handle);
				break;

			case PUTFIELD_A:
			case PUTFIELD_B:
			case PUTFIELD_C:
			case PUTFIELD_I:
			case PUTFIELD_S:
				ret.getStack().pop();
				ret.getStack().pop();
				break;

			case PUTSTATIC_A:
			case PUTSTATIC_B:
			case PUTSTATIC_C:
			case PUTSTATIC_I:
			case PUTSTATIC_S:
				ret.getStack().pop();
				break;

			case GETSTATIC_A:
				ret.getStack().push(handle);
				break;
			case GETSTATIC_B:
				ret.getStack().push(handle);
				break;
			case GETSTATIC_C:
				ret.getStack().push(handle);
				break;
			case GETSTATIC_I:
				ret.getStack().push(handle);
				break;
			case GETSTATIC_S:
				ret.getStack().push(handle);
				break;
				
			case IRETURN:
			case ARETURN:
			case SRETURN:
				ret.getStack().pop();
				break;

			case NEWARRAY:
			case ANEWARRAY:
				ret.getStack().pop();
				ret.getStack().push(handle);
				break;

			case NEW:
				ret.getStack().push(handle);
				break;
				
			case ARRAYLENGTH:
				ret.getStack().pop();
				ret.getStack().push(handle);
				break;
				
			case INSTANCEOF:
				ret.getStack().pop();
				ret.getStack().push(handle);
				break;

			case CHECKCAST:
				break;
				
			case MONITORENTER:
			case MONITOREXIT:
			case ATHROW:
				ret.getStack().pop();
				break;
				
			case IPOP:
			case APOP:
				ret.getStack().pop();
				break;
				
			case IPOP2:
			case APOP2:
				ret.getStack().pop();
				ret.getStack().pop();
				break;
				
			case IDUP:
			case ADUP:
				ret.getStack().pop();
				ret.getStack().push(handle, 1);
				ret.getStack().push(handle, 0);
				break;

			case IDUP2:
			case ADUP2:
				ret.getStack().pop();
				ret.getStack().pop();
				ret.getStack().push(handle, 3);
				ret.getStack().push(handle, 2);
				ret.getStack().push(handle, 1);
				ret.getStack().push(handle, 0);
				break;

			case IDUP_X1:
			case ADUP_X1:
				ret.getStack().pop();
				ret.getStack().pop();
				ret.getStack().push(handle, 2);
				ret.getStack().push(handle, 1);
				ret.getStack().push(handle, 0);
				break;
				
			case IDUP_X2:
			case ADUP_X2:
				ret.getStack().pop();
				ret.getStack().pop();
				ret.getStack().pop();
				ret.getStack().push(handle, 3);
				ret.getStack().push(handle, 2);
				ret.getStack().push(handle, 1);
				ret.getStack().push(handle, 0);
				break;
				
			case B2C:
			case S2B:
			case S2C:
			case S2S:
			case S2I:
			case I2S:
			case I2B:
			case I2C:
				ret.getStack().pop();
				ret.getStack().push(handle);
				break;
				
			case TABLESWITCH:
			case LOOKUPSWITCH:
				ret.getStack().pop();
				break;
				
			case IINC:
			case IINC_W:
				break;
				
			case GOTO:
			case RETURN:
				// nothing
				break;
			
			default:
				throw new IllegalArgumentException("Unimplemented opcode: " + handle.getInstruction().getOpcode().getName());
				
		}		
		
		return ret;
	}
	
	public InterpreterStack getStack()
	{
		return stack;
	}
	
	public static InterpreterState merge(InterpreterState a, InterpreterState b)
	{
		InterpreterStack stack = InterpreterStack.merge(a.stack, b.stack);
		// InterpreterLocalVariableBlock localVariables = InterpreterLocalVariableBlock.merge(a.localVariables, b.localVariables);
		
		return new InterpreterState(stack/*, localVariables*/);
	}
	
	public int compareTo(InterpreterState o)
	{
		int ret;
		if ((ret=stack.compareTo(o.stack))!=0) return ret;
		//if ((ret=localVariables.compareTo(o.localVariables))!=0) return ret;
		return 0;
	}
	
	protected InterpreterState clone()
	{
		return new InterpreterState(stack.clone()/*, localVariables.clone()*/);
	}
	
//	public InterpreterLocalVariableBlock getLocalVariables()
//	{
//		return localVariables;
//	}
	
	public String toString()
	{
		return stack.toString();
	}
	
}
