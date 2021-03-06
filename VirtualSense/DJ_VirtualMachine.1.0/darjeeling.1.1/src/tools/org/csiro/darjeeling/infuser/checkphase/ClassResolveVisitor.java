/*
 *	ClassResolveVisitor.java
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
package org.csiro.darjeeling.infuser.checkphase;

import org.csiro.darjeeling.infuser.logging.Logging;
import org.csiro.darjeeling.infuser.structure.Element;
import org.csiro.darjeeling.infuser.structure.elements.AbstractClassDefinition;
import org.csiro.darjeeling.infuser.structure.elements.AbstractInfusion;

public class ClassResolveVisitor extends CheckVisitor
{
	
	private AbstractInfusion infusion;
	
	public ClassResolveVisitor(AbstractInfusion infusion)
	{
		this.infusion = infusion;
	}
	
	//@Override
	public void visit(AbstractClassDefinition element)
	{
		// call the superclass implementation to pick up the filename from the class definition
		super.visit(element);
		
		// resolve the super class
		resolveClass(element);
	}
	
	private void resolveClass(AbstractClassDefinition classDef)
	{
		// resolve superclass
		AbstractClassDefinition superClass = infusion.lookupClassByName(classDef.getSuperClassName());
		
		// if the current class is the base class, java.lang.Object, set the superclass to null
		// to avoid a circular reference (java.lang.Object should not inherit from java.lang.Object :)
		if ("java.lang.Object".equals(classDef.getName()))
			superClass = null;
		else
			if (superClass==null)
			{
				Logging.instance.error(String.format("Superclass %s not found for class %s", classDef.getSuperClassName(), classDef.getName()));
				return;
			}

		classDef.setSuperClass(superClass);
		
		// resolve interfaces interfaces
		for (String interfaceName : classDef.getInterfaceNames())
		{
			AbstractClassDefinition interf = infusion.lookupClassByName(interfaceName);
			if (interf==null)
				Logging.instance.error(String.format("Interface %s not found for class %s", interfaceName, classDef.getName()));
			else
				classDef.getInterfaces().put(interfaceName, interf);
		}
	}

	//@Override
	public void visit(Element element)
	{
	}

}
