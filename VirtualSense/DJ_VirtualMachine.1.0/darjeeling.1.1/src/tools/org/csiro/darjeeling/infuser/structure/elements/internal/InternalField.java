/*
 *	InternalField.java
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
package org.csiro.darjeeling.infuser.structure.elements.internal;

import org.csiro.darjeeling.infuser.structure.ElementVisitor;
import org.csiro.darjeeling.infuser.structure.Flags;
import org.csiro.darjeeling.infuser.structure.GlobalId;
import org.csiro.darjeeling.infuser.structure.elements.AbstractClassDefinition;
import org.csiro.darjeeling.infuser.structure.elements.AbstractField;

public class InternalField extends AbstractField
{

	private InternalField(String name, String descriptor, int size, Flags flags, AbstractClassDefinition parentClass)
	{
		super(name, descriptor, size, flags, parentClass);
	}
	
	public static InternalField fromField(org.apache.bcel.classfile.Field field, 
			AbstractClassDefinition parentClass, InternalInfusion infusion)
	{
		InternalField ret;
		String signature = field.getSignature();
		int size = classify(signature).getSize();
		
		ret = new InternalField(
				field.getName(),
				field.getSignature(),
				size,
				Flags.fromAcessFlags(field),
				parentClass
				);
		
		ret.setGlobalID(new GlobalId(infusion.getHeader().getInfusionName(),0));
		
		return ret;		
	}	

	//@Override
	public void accept(ElementVisitor visitor)
	{
		visitor.visit(this);
	}
	
}
