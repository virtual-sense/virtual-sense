/*
 *	ImmediateBytePushInstruction.java
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

import java.io.DataOutputStream;
import java.io.IOException;

import org.csiro.darjeeling.infuser.bytecode.InstructionHandle;
import org.csiro.darjeeling.infuser.bytecode.Opcode;
import org.csiro.darjeeling.infuser.structure.BaseType;

public class ImmediateBytePushInstruction extends PushInstruction
{

	public ImmediateBytePushInstruction(Opcode opcode, int value)
	{
		super(opcode, value);
	}

	@Override
	public void dump(DataOutputStream out) throws IOException
	{
		out.write(opcode.getOpcode());
		out.write(value);
	}
	
	@Override
	public BaseType getLogicalOutputType(int index, InstructionHandle handle)
	{
		return BaseType.Byte;
	}

	@Override
	public int getLength()
	{
		return 2;
	}
	
}
