/*
 *	FindEntryPointVisitor.java
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
package org.csiro.infuser.processingphase;

import org.csiro.darjeeling.infuser.structure.DescendingVisitor;
import org.csiro.darjeeling.infuser.structure.Element;
import org.csiro.darjeeling.infuser.structure.elements.AbstractHeader;
import org.csiro.darjeeling.infuser.structure.elements.AbstractMethodImplementation;

public class FindEntryPointVisitor extends DescendingVisitor
{
	
	private AbstractHeader header;
	
	private boolean entrypoint_already_set = false;
	
	public FindEntryPointVisitor(AbstractHeader header)
	{
		this.header = header;
	}
	
	//@Override
	public void visit(AbstractMethodImplementation element)
	{
		if (((element.getMethodDefinition().getName().equals("main") &&
			element.getMethodDefinition().getSignature().equals("([Ljava/lang/String;)V")) || 
			(element.getMethodDefinition().getName().equals("motemain") &&
					element.getMethodDefinition().getSignature().equals("()V"))) && !entrypoint_already_set)
		{
			header.setEntryPoint(element);
			entrypoint_already_set = true;
		}				
	}

	//@Override
	public void visit(Element element)
	{
		// TODO Auto-generated method stub
		
	}
	
	
}
