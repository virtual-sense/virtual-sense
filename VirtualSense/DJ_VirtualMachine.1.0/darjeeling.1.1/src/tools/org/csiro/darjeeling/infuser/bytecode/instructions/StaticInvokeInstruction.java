/*
 *	StaticInvokeInstruction.java
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
package org.csiro.darjeeling.infuser.bytecode.instructions;

import org.csiro.darjeeling.infuser.bytecode.Opcode;
import org.csiro.darjeeling.infuser.structure.LocalId;
import org.csiro.darjeeling.infuser.structure.elements.AbstractMethodDefinition;

public class StaticInvokeInstruction extends AbstractInvokeInstruction
{

	public StaticInvokeInstruction(Opcode opcode, LocalId localId, AbstractMethodDefinition methodDefinition)
	{
		super(opcode, localId, methodDefinition);
	}

}
