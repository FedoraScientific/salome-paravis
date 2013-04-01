// PARAVIS : ParaView wrapper SALOME module
//
// Copyright (C) 2010-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : vtkWrapIDL.c
// Author : Vladimir TURIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vtkConfigure.h"
#include "vtkParse.h"
#include "vtkParseType.h"
#include "vtkParseMain.h"
#include "vtkWrapIDL.h"
#include "vtkWrap.h"

char* Copyright[] = {
  "// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,",
  "// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS",
  "//",
  "// This library is free software; you can redistribute it and/or",
  "// modify it under the terms of the GNU Lesser General Public",
  "// License as published by the Free Software Foundation; either",
  "// version 2.1 of the License.",
  "//",
  "// This library is distributed in the hope that it will be useful,",
  "// but WITHOUT ANY WARRANTY; without even the implied warranty of",
  "// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU",
  "// Lesser General Public License for more details.",
  "//",
  "// You should have received a copy of the GNU Lesser General Public",
  "// License along with this library; if not, write to the Free Software",
  "// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA",
  "//",
  "// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com",
  ""
};

#define bs 12288

HierarchyInfo *hierarchyInfo = NULL;
int numberOfWrappedFunctions = 0;
FunctionInfo *wrappedFunctions[1000];
extern FunctionInfo *currentFunction;
char *EmptyString = "";


static void add_to_sig(char *sig, const char *add, int *i)
{
  strcpy(&sig[*i],add);
  *i += (int)strlen(add);
}

int IsReturnArg(int I) {
  return (I == MAX_ARGS);
}

void AddReturnArg(char *Result, int *CurrPos) {
  add_to_sig(Result,"virtual ",CurrPos);
}

int IsIn(ValueInfo* Type) {
  return 1;
  //return ((Type % VTK_PARSE_BASE_TYPE)/0x100 < 1 || (Type % VTK_PARSE_BASE_TYPE)/0x100 > 7);
}

void AddNotReturnArg(ValueInfo* Type, char *Result, int *CurrPos) {
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  ;
#else
  if(IsIn(Type))
    add_to_sig(Result,"in ",CurrPos);
  else
    add_to_sig(Result,"inout ",CurrPos);
#endif
}

int IsFunction(ValueInfo* Type) {
  //return (Type == VTK_PARSE_FUNCTION);
  return vtkWrap_IsFunction(Type);
}

int IsConst(ValueInfo* Type) {
  //return ((Type % 0x2000) >= 0x1000);
  return vtkWrap_IsConst(Type);
}

void AddConst(char *Result, int *CurrPos) {
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  add_to_sig(Result,"const ",CurrPos);
#else
  add_to_sig(Result,"in ",CurrPos);
#endif
}


int IsClass(ValueInfo* theType) {
  //return ((theType->Type % 0x10) == 0x9);
  return vtkWrap_IsObject(theType) || vtkWrap_IsVTKObject(theType);
}

int IsString(ValueInfo* Type) {
  //return (IsChar(Type) && IsArray(Type));
  //return (IsChar(Type) && IsPtr(Type));
  return vtkWrap_IsCharPointer(Type) || vtkWrap_IsString(Type) || (strcmp(Type->Class, "vtkStdString") == 0);
}

int IsPtr(ValueInfo* Type) {
  //return ((Type % VTK_PARSE_BASE_TYPE)/0x100 == 0x1);
  return vtkWrap_IsPointer(Type) && (!IsString(Type)) && (!IsClass(Type));
}

int IsUnknown(ValueInfo* theType) {
  //return ((Type % VTK_PARSE_BASE_TYPE)/0x100 == 0x8);
  return (theType->Type & VTK_PARSE_BASE_TYPE) == VTK_PARSE_UNKNOWN;
}

void AddAtomArg(int I, ValueInfo* Type, char *TypeIDL, char *TypeCorba, char *Result, int *CurrPos) {
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  add_to_sig(Result,"CORBA::",CurrPos);
  add_to_sig(Result,TypeCorba,CurrPos);

  if (!IsReturnArg(I) && !IsConst(Type) && !IsIn(Type))
    add_to_sig(Result,"&",CurrPos);
#else
  add_to_sig(Result,TypeIDL,CurrPos);
#endif
  add_to_sig(Result," ",CurrPos);
}

int IsArray(ValueInfo* Type) {
  //return ((Type % VTK_PARSE_BASE_TYPE)/0x100 == 0x3);
  return vtkWrap_IsArray(Type);
}

void AddArrayArg(int I, ValueInfo* Type, char *TypeIDL, char *Result, int *CurrPos) {
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  if(!IsReturnArg(I) && !IsConst(Type))
    add_to_sig(Result, "const ",CurrPos);
#endif
  add_to_sig(Result, TypeIDL,CurrPos);
  add_to_sig(Result, "_array",CurrPos);
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  if(IsReturnArg(I)) {
    add_to_sig(Result, "*",CurrPos);
  } else {
    add_to_sig(Result, "&",CurrPos);
  }
#else
#endif
  add_to_sig(Result, " ",CurrPos);
}

int IsBoolean(ValueInfo* Type) {
  //return ((Type % 0x10) == 0xE);
  return vtkWrap_IsBool(Type);
}

void AddBooleanAtomArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"boolean","Boolean",Result,CurrPos);
}

int IsChar(ValueInfo* theType) {
  //return ((Type % 0x10) == 0x3 || (Type % 0x10) == 0xD);
  int aBase = theType->Type & VTK_PARSE_BASE_TYPE;
  return (aBase == VTK_PARSE_CHAR) || (aBase == VTK_PARSE_UNSIGNED_CHAR) || (aBase == VTK_PARSE_SIGNED_CHAR);
}

int IsCharArray(ValueInfo* theType) {
  return (IsChar(theType) && IsArray(theType));
}

void AddCharAtomArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"char","Char",Result,CurrPos);
}

void AddCharArrayArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddArrayArg(I,Type,"char",Result,CurrPos);
}

void AddStringArg(int I, char *Result, int *CurrPos) {
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  add_to_sig(Result,"char ",CurrPos);
#else
  add_to_sig(Result,"string ",CurrPos);
#endif
}

int IsFloat(ValueInfo* theType) {
  //return ((Type % 0x10) == 0x1);
  return (theType->Type & VTK_PARSE_BASE_TYPE) == VTK_PARSE_FLOAT;
}

void AddFloatAtomArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"float","Float",Result,CurrPos);
}

int IsFloatArray(ValueInfo* theType) {
  return (IsFloat(theType) && IsArray(theType));
}

void AddFloatArrayArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddArrayArg(I,Type,"float",Result,CurrPos);
}

int IsDouble(ValueInfo* theType) {
  //return ((Type % 0x10) == 0x7);
  return (theType->Type & VTK_PARSE_BASE_TYPE) == VTK_PARSE_DOUBLE;
}

void AddDoubleAtomArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"double","Double",Result,CurrPos);
}

int IsDoubleArray(ValueInfo*  Type) {
  return (IsDouble(Type) && IsArray(Type));
}

void AddDoubleArrayArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddArrayArg(I,Type,"double",Result,CurrPos);
}

int IsvtkIdType(ValueInfo* theType) {
  //return((Type % 0x10) == 0xA);
  return (theType->Type & VTK_PARSE_BASE_TYPE) == VTK_PARSE_ID_TYPE;
}

int IsShort(ValueInfo* theType) {
  //return ((Type % 0x10) == 0x4 || (Type % 0x10) == 0x5 || (Type % 0x10) == 0xA);
  int aVal = theType->Type & VTK_PARSE_BASE_TYPE;
  return (aVal == VTK_PARSE_SHORT) || (aVal == VTK_PARSE_UNSIGNED_SHORT);
}

int IsInt(ValueInfo* theType) {
  //return ((Type % 0x10) == 0x4 || (Type % 0x10) == 0x5 || (Type % 0x10) == 0xA);
  int aVal = theType->Type & VTK_PARSE_BASE_TYPE;
  return (aVal == VTK_PARSE_INT) || 
    (aVal == VTK_PARSE_ID_TYPE) || (aVal == VTK_PARSE_UNSIGNED_INT) ||
    (aVal == VTK_PARSE_SSIZE_T) || (aVal == VTK_PARSE_SIZE_T);
}

int IsShortInt(ValueInfo* theType) {
  //return ((Type % 0x10) == 0x4 || (Type % 0x10) == 0x5 || (Type % 0x10) == 0xA);
  return (IsShort(theType) || IsInt(theType));
}

void AddShortAtomArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"short","Short",Result,CurrPos);
}

int IsShortArray(ValueInfo* Type) {
  return (IsShort(Type) && IsArray(Type));
}

int IsIntArray(ValueInfo* Type) {
  return (IsInt(Type) && IsArray(Type));
}

void AddShortArrayArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddArrayArg(I,Type,"short",Result,CurrPos);
}

int IsLong(ValueInfo* theType) {
  //return ((Type % 0x10) == 0x6 || (Type % 0x10) == 0xB || (Type % 0x10) == 0xC);
  unsigned int aVal = theType->Type & VTK_PARSE_BASE_TYPE;
  return (aVal == VTK_PARSE_LONG) || (aVal == VTK_PARSE_UNSIGNED_LONG) || 
    (aVal == VTK_PARSE___INT64) || (aVal == VTK_PARSE_UNSIGNED___INT64) || (aVal == VTK_PARSE_LONG_LONG) || 
    (aVal == VTK_PARSE_UNSIGNED_LONG_LONG);
}

void AddLongAtomArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"long","Long",Result,CurrPos);
}

int IsLongArray(ValueInfo* Type) {
  return (IsLong(Type) && IsArray(Type));
}

void AddLongArrayArg(int I, ValueInfo* Type, char *Result, int *CurrPos) {
  AddArrayArg(I,Type,"long",Result,CurrPos);
}

void AddClassArg(int I, ValueInfo* Type, const char *Class, char *Result, int *CurrPos) {
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  add_to_sig(Result,"PARAVIS_Base",CurrPos);
  if(IsReturnArg(I) || IsConst(Type) || IsIn(Type)) {
    add_to_sig(Result,"_ptr",CurrPos);
  } else {
    add_to_sig(Result,"_ptr&",CurrPos);
  }
  add_to_sig(Result," ",CurrPos);
#else
  add_to_sig(Result,"PARAVIS_Base ",CurrPos);
#endif
}

int _IsVoid(ValueInfo* theType) {
  //return ((Type % 0x10) == 0x2);
  return (theType->Type & VTK_PARSE_BASE_TYPE) == VTK_PARSE_VOID;
}

int IsVoid(ValueInfo* Type) {
  return (_IsVoid(Type) && (!IsPtr(Type)));
}

int IsUnsigned(ValueInfo* theType) {
  unsigned int aVal = theType->Type & VTK_PARSE_BASE_TYPE;
  switch (aVal) {
  case VTK_PARSE_UNSIGNED_CHAR:
  case VTK_PARSE_UNSIGNED_INT:
  case VTK_PARSE_UNSIGNED_SHORT:
  case VTK_PARSE_UNSIGNED_LONG:
  case VTK_PARSE_UNSIGNED_ID_TYPE:
  case VTK_PARSE_UNSIGNED_LONG_LONG:
  case VTK_PARSE_UNSIGNED___INT64:
  case VTK_PARSE_UNSIGNED_CHAR_PTR:
  case VTK_PARSE_UNSIGNED_INT_PTR:
  case VTK_PARSE_UNSIGNED_SHORT_PTR:
  case VTK_PARSE_UNSIGNED_LONG_PTR:
  case VTK_PARSE_UNSIGNED_ID_TYPE_PTR:
  case VTK_PARSE_UNSIGNED_LONG_LONG_PTR:
  case VTK_PARSE_UNSIGNED___INT64_PTR:
    return 1;
  }
  return 0;
}

void AddVoid(char *Result, int *CurrPos) {
  add_to_sig(Result,"void ",CurrPos);
}

int IsVoidPtr(ValueInfo* Type) {
  return (_IsVoid(Type) && (IsPtr(Type) || IsArray(Type)));
}

void AddVoidArg(int I, char *Result, int *CurrPos, ValueInfo* Type) {
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  if(!IsReturnArg(I)) {
    if(!IsConst(Type))
      add_to_sig(Result,"const ",CurrPos);
    add_to_sig(Result,"CORBA::Any& ",CurrPos);
  } else {
    add_to_sig(Result,"CORBA::Any ",CurrPos);
  }
#else
  add_to_sig(Result,"any ",CurrPos);
#endif
}

void AddTypeArray(ValueInfo* Type, char *Result, int *CurrPos) {
  if(IsShortInt(Type))
    add_to_sig(Result,"short",CurrPos);
  if(IsLong(Type))
    add_to_sig(Result,"long",CurrPos);
  if(IsFloat(Type))
    add_to_sig(Result,"float",CurrPos);
  if(IsDouble(Type))
    add_to_sig(Result,"double",CurrPos);
  if(IsChar(Type))
    add_to_sig(Result,"char",CurrPos);
  add_to_sig(Result,"_array",CurrPos);
}

typedef struct _ReadFuncs
{
  char* Name;
  char* Signature;
  int   EndPos;
  int   NumPos;
} ReadFuncs;

ReadFuncs readFunctions[1000];
int numberOfReadFunctions = 0;

static int class_has_new = 0;

void output_type(char* result, int *currPos, int i, ValueInfo* aType, const char *Id)
{
  if (vtkWrap_IsConst(aType))
    add_to_sig(result,"const ",currPos);
    
  if(IsFloat(aType)) {
    if(IsArray(aType)) {
      add_to_sig(result,"float",currPos);
    } else {
      add_to_sig(result,"CORBA::Float",currPos);
    }
  }
  
  if(IsDouble(aType)) {
    if(IsArray(aType)) {
      add_to_sig(result,"double",currPos);
    } else {
      add_to_sig(result,"CORBA::Double",currPos);
    }
  }
  
  if(IsShortInt(aType)) {
    if(IsArray(aType)) {
      if (IsUnsigned(aType))
        add_to_sig(result,"unsigned ",currPos);
      if(IsvtkIdType(aType)) {
        add_to_sig(result,"vtkIdType",currPos);
      } else {
        add_to_sig(result,"int",currPos);
      }
    } else {
      add_to_sig(result,"CORBA::Short",currPos);
    }
  }
  
  if(IsLong(aType)) {
    if(IsArray(aType)) {
      if (IsUnsigned(aType))
        add_to_sig(result,"unsigned ",currPos);
      add_to_sig(result,"long",currPos);
    } else {
      add_to_sig(result,"CORBA::Long",currPos);
    }
  }
  
  if(IsString(aType)) {
    if(IsReturnArg(i))
      add_to_sig(result,"const ",currPos);
    if(strcmp(aType->Class, "vtkStdString") == 0) {
      add_to_sig(result,"vtkStdString",currPos);
    } else {
      add_to_sig(result,"char",currPos);
    }
  } else if(IsChar(aType)) {
    if(IsArray(aType)) {
      if (IsUnsigned(aType))
        add_to_sig(result,"unsigned ",currPos);
      add_to_sig(result,"char",currPos);
    } else {
      add_to_sig(result,"CORBA::Char",currPos);	    
    }
  }
  
  if(IsBoolean(aType)) {
    add_to_sig(result,"CORBA::Boolean",currPos);
  }
  
  if(IsVoidPtr(aType)) {
    add_to_sig(result,"void",currPos);
  }
  
  if(IsClass(aType)) {
    add_to_sig(result,"::",currPos);
    add_to_sig(result,Id,currPos);
  }
  
  if(IsArray(aType) || IsPtr(aType) || IsClass(aType) || IsString(aType)) {
    if(strcmp(aType->Class, "vtkStdString") != 0) {
      add_to_sig(result,"*",currPos);
    }
  }
}

void output_typedef(char* result, int *currPos, int i, ValueInfo* aType, const char *Id)
{
  add_to_sig(result,"  typedef ",currPos);
  output_type(result,currPos,i,aType,Id);
}

void output_temp(char* result, int *currPos, int i, ValueInfo* aType, const char *Id, int aCount)
{
  static char buf[bs];

  /* handle VAR FUNCTIONS */
  if (IsFunction(aType)) {
    return;
  }

#if defined(IDL_I_HH)
  if(IsReturnArg(i)) {
    AddReturnArg(result,currPos);
  }
#endif

  if (IsReturnArg(i) && IsVoid(aType) && !IsVoidPtr(aType)) {
    AddVoid(result,currPos);
    return;
  }

  /* for const * return types prototype with const */
  if ( IsConst(aType) || IsString(aType)) {
    if(!IsReturnArg(i))	{
      AddConst(result,currPos);
    }
  } else {
    if(!IsReturnArg(i))	{
      AddNotReturnArg(aType,result,currPos);
    }
  }

  if(IsFloat(aType)) {
    if(IsFloatArray(aType)) {
      AddFloatArrayArg(i,aType,result,currPos);
    } else {
      AddFloatAtomArg(i,aType,result,currPos);
    }
  }
  
  if(IsDouble(aType)) {
    if(IsDoubleArray(aType)) {
      AddDoubleArrayArg(i,aType,result,currPos);
    } else {
      AddDoubleAtomArg(i,aType,result,currPos);
    }
  }

  if(IsShortInt(aType)) {
    if(IsArray(aType)) {
      AddShortArrayArg(i,aType,result,currPos);
    } else {
      AddShortAtomArg(i,aType,result,currPos);
    }
  }

  if(IsLong(aType)) {
    if(IsLongArray(aType)) {
      AddLongArrayArg(i,aType,result,currPos);
    } else {
      AddLongAtomArg(i,aType,result,currPos);
    }
  }

  if(IsChar(aType)) {
    if(IsString(aType)) {
      AddStringArg(i,result,currPos);
    } else {
      if(IsCharArray(aType)) {
	AddCharArrayArg(i,aType,result,currPos);
      } else {
	AddCharAtomArg(i,aType,result,currPos);
      }
    }
  } else if (IsString(aType)) //!!! VSV
    AddStringArg(i,result,currPos);

  if(IsBoolean(aType)) {
    AddBooleanAtomArg(i,aType,result,currPos);
  }

  if(IsVoid(aType)) {
    AddVoidArg(i,result,currPos,aType);
  }

  if(IsClass(aType)) {
    AddClassArg(i,aType,Id,result,currPos);
  }

  if(IsUnknown(aType)) {
    return;
  }

  if(!IsClass(aType)) {
    if(IsPtr(aType)) {
      add_to_sig(result," *",currPos);
    }
#if defined(IDL_I_HH) || defined(IDL_I_CC)
    if(IsString(aType) && !IsIn(aType) && !IsConst(aType) && !IsReturnArg(i)) {
      add_to_sig(result,"*&",currPos);
    } else {
      if(IsString(aType) || (IsReturnArg(i) && IsVoidPtr(aType))) {
        add_to_sig(result," *",currPos);
      }
    }
#endif
  }

  if(!IsReturnArg(i)) {
    sprintf(buf,"temp%i",i);
    add_to_sig(result,buf,currPos);
  }
  
  return;
}

void read_class_functions(const char* name, const char* classname, FILE* fp)
{
  int len=0;
  int curlen=0;
  int i, j;
  int flen=0;
  //int num=0;
  int ret_str=0;
  FILE *fin;
  char buf[bs];
  char buf1[bs];
  char fname[bs];
  static char sig[bs];
  static int slen=8;
#if ! defined(IDL_I_CC)
  static int clen=15;
#else
  static int clen=7;
#endif


#if defined(IDL_I_HH)
  sprintf(buf,"PARAVIS_Gen_%s_i.hh",name);
#elif defined(IDL_I_CC)
  sprintf(buf,"PARAVIS_Gen_%s_i.cc",name);
#else
  sprintf(buf,"PARAVIS_Gen_%s.idl",name);
#endif
  if (!(fin = fopen(buf,"r"))) {
    fprintf(stderr,"Error opening input file %s\n",buf);
    exit(1);
  }
  while (fgets(sig,bs-1,fin) != 0) {
#if defined(IDL_I_CC)
    if(strncmp("#include \"PARAVIS_Gen_vtk",sig,25)==0) {
      fprintf(fp, "%s",sig);
    }
    if(strncmp("#include <vtk",sig,13)==0) {
      fprintf(fp, "%s",sig);
    }
#endif

#if ! defined(IDL_I_CC)
    if(strncmp("        //C++: ",sig,clen)==0) {
#else
    if(strncmp("//C++: ",sig,clen)==0) {
#endif
#if defined(IDL_I_CC)
      while(fgets(buf,bs-1,fin) != 0) {
	len=strlen(sig);
	strcpy(sig+len,buf);
	if(strlen(buf) == 3 && buf[0]=='/' && buf[1]=='/')
	  break;
      }
#endif
      len=strlen(sig);
      if(fgets(buf,bs-1,fin)!=NULL && strlen(buf) > 1) {
	ret_str=0;
#if defined(IDL_I_HH)
	strcpy(sig+len,buf);
	readFunctions[numberOfReadFunctions].EndPos=strlen(sig);  
	sscanf(sig+len,"%s %s %s",buf,buf1,fname);
	if(fname[0] == '*') {
	  ret_str=1;
	}
#elif defined(IDL_I_CC)
	strcpy(buf1,buf);
	sscanf(buf1,"%s %s",buf,fname);

	curlen=strlen(sig);
	sprintf(sig+curlen,"%s ",buf);
	j=strlen(buf)+1;

	if(fname[0] == '*') {
	  curlen=strlen(sig);
	  sprintf(sig+curlen," *");
	  j+=1;
	}
	curlen=strlen(sig);
	sprintf(sig+curlen,"%s_i::",classname);

	for(i = 0; i < strlen(fname)-1; i++) {
	  ret_str+=1;
	  j+=1;
	  if(fname[i] == ':' && fname[i] == ':') {
	    ret_str+=1;
	    j+=1;
	    break;
	  }
	}
	curlen=strlen(sig);
	sprintf(sig+curlen,"%s", buf1+j);

	curlen=strlen(sig);
	readFunctions[numberOfReadFunctions].EndPos=curlen;  
	while (fgets(sig+curlen,bs-1-curlen,fin) != 0) {
	  if(sig[curlen] == '}')
	    break;
	  curlen=strlen(sig);
	}
#else
	strcpy(sig+len,buf);
	readFunctions[numberOfReadFunctions].EndPos=strlen(sig);  
	sscanf(sig+len,"%s _%s",buf,fname);
#endif
	flen=strlen(fname);
	if(flen>0) {
	  if(flen>2 && fname[flen-2]=='_') {
	    flen-=2;
	  }
	  readFunctions[numberOfReadFunctions].Name = malloc((flen+1-ret_str)*sizeof(char));
	  strncpy(readFunctions[numberOfReadFunctions].Name,fname+ret_str,flen-ret_str);
	  readFunctions[numberOfReadFunctions].Name[flen-ret_str]='\0';
	  
#if defined(IDL_I_HH)
	  len+=slen+strlen(buf)+1+strlen(buf1)+2+flen+ret_str-1; //spaces+virtual+space+return+space+space+*+name
#elif defined(IDL_I_CC)
	  len+=2+strlen(buf)+2+strlen(classname)+2+strlen(readFunctions[numberOfReadFunctions].Name)-1; //return+space+space+name
	  if(fname[0] == '*') {
	    len+=2;
	  }
	  if(fname[flen-2]=='_') {
	    len+=2;
	  }
#else
	  len+=slen+strlen(buf)+2+flen; //spaces+return+space+_+name
#endif
	  readFunctions[numberOfReadFunctions].NumPos=len;
	  if(fname[flen]=='_') {
	    readFunctions[numberOfReadFunctions].NumPos=len+2; //+_+num
	  }
	  
	  len=strlen(sig);
	  readFunctions[numberOfReadFunctions].Signature = malloc((len+1)*sizeof(char));
	  strncpy(readFunctions[numberOfReadFunctions].Signature,sig,len);
	  readFunctions[numberOfReadFunctions].Signature[len]='\0';

	  numberOfReadFunctions++;
	}
      }
    }
  }
  fclose (fin);
  return;
}

void get_signature(const char* num, ClassInfo *data)
{
  static char result[bs];
  int currPos = 0;
  int currPos_sig = 0;
  //int argtype;
  int i, j;
  static char buf[bs];
  static char buf1[bs];
  int ret = 0;
  int found = 0;
  int currPos_num = 0;
  ValueInfo* aRetVal = NULL;
  ValueInfo* aArgVal = NULL;
  char *cp;

  aRetVal = currentFunction->ReturnValue;

  add_to_sig(result,"\n",&currPos);
  if (currentFunction->Signature) {
#if ! defined(IDL_I_CC)
    add_to_sig(result,"        ",&currPos);
#endif
    add_to_sig(result,"//C++: ",&currPos);
    add_to_sig(result,currentFunction->Signature,&currPos);
    add_to_sig(result,"\n",&currPos);
  }
  
  if(IsClass(aRetVal) && ret == 0) {
    found = 0;
    for(i = 0; strcmp(wrapped_classes[i],"") != 0 && found == 0; i++) {
      if(strcmp(wrapped_classes[i],currentFunction->ReturnValue->Class) == 0)
        found = 1;
    }
    if(!found)
      ret = 1;
  }
  
  for (j = 0; j < vtkWrap_CountWrappedParameters(currentFunction); j++) {
    aArgVal = currentFunction->Parameters[j];
    if(IsFunction(aArgVal))
      ret == 1;
    if(IsClass(aArgVal) && ret == 0) {
      found = 0;
      for(i = 0; strcmp(wrapped_classes[i],"") != 0 && found == 0; i++) {
        if(strcmp(wrapped_classes[i],aArgVal->Class) == 0)
          found = 1;
      }
      if(!found)
        ret = 1;
    }
  }
  
  if (IsArray(aRetVal) && !IsClass(aRetVal) && !IsString(aRetVal) && currentFunction->HintSize == 0) {
    ret = 1;
  }
  

  if(ret) {
    add_to_sig(result,"//\n",&currPos);
    /*currentFunction->Signature = (const char*)realloc((void*)currentFunction->Signature,
      (size_t)(currPos+1));*/
    //strcpy((char*)currentFunction->Signature,result);
    cp = (char *)malloc(currPos+1);
    strcpy(cp, result);
    currentFunction->Signature = cp;
    return;
  }

#if defined(IDL_I_CC)
  add_to_sig(result,"struct CreateEventName(",&currPos);
  add_to_sig(result,currentFunction->Name,&currPos);
  if( strlen(num)!=0 ) {
    add_to_sig(result,num,&currPos);
  }
  add_to_sig(result,")",&currPos);
  add_to_sig(result,": public SALOME_Event\n",&currPos);
  add_to_sig(result,"{\n",&currPos);
  if(!IsVoid(aRetVal)) {
    //output_typedef(result,&currPos,MAX_ARGS,aRetVal, currentFunction->ReturnValue->Class);
    add_to_sig(result,"  typedef ",&currPos);
    if (vtkWrap_IsConst(aRetVal))
      add_to_sig(result, " const ",&currPos);
    if (vtkWrap_IsObject(aRetVal))
      add_to_sig(result, " ::",&currPos);
    add_to_sig(result, aRetVal->Class,&currPos);
    if (vtkWrap_IsPointer(aRetVal) || vtkWrap_IsArray(aRetVal))
      add_to_sig(result, "*",&currPos);
    add_to_sig(result," TResult;\n",&currPos);
    add_to_sig(result,"  TResult myResult;\n",&currPos);
  }
  
  //output_typedef(result, &currPos, 0, 0x309, data->Name); //!!??
  add_to_sig(result,"  typedef ::",&currPos);
  add_to_sig(result,data->Name,&currPos);
  add_to_sig(result,"* TObj;\n",&currPos);
  add_to_sig(result,"  TObj myObj;\n",&currPos);
  
  for (i = 0; i < vtkWrap_CountWrappedParameters(currentFunction); i++) {
    aArgVal = currentFunction->Parameters[i];
    //output_typedef(result, &currPos, i, currentFunction->ArgTypes[i],
    //               currentFunction->ArgClasses[i]);
    output_typedef(result, &currPos, i, aArgVal, currentFunction->ArgClasses[i]);
    sprintf(buf," TParam%d;\n",i);
    add_to_sig(result,buf,&currPos);
    sprintf(buf,"  TParam%d myParam%d;\n",i,i);
    add_to_sig(result,buf,&currPos);
  }
    
  add_to_sig(result,"\n",&currPos);  
  add_to_sig(result,"  CreateEventName(",&currPos);  
  add_to_sig(result,currentFunction->Name,&currPos);
  if( strlen(num)!=0 ) {
    add_to_sig(result,num,&currPos);
  }
  add_to_sig(result,")",&currPos);
  add_to_sig(result,"(TObj theObj",&currPos);  
  for (i = 0; i < vtkWrap_CountWrappedParameters(currentFunction); i++) {
    sprintf(buf,", TParam%d theParam%d",i,i);
    add_to_sig(result,buf,&currPos);
  }
  add_to_sig(result,"):\n",&currPos);  
  add_to_sig(result,"  myObj(theObj)",&currPos);  
  for (i = 0; i < vtkWrap_CountWrappedParameters(currentFunction); i++) {
    sprintf(buf,", myParam%d(theParam%d)",i,i);
    add_to_sig(result,buf,&currPos);
  }
  add_to_sig(result,"\n",&currPos);  
  add_to_sig(result,"  { }\n",&currPos);  
  add_to_sig(result,"\n",&currPos);  
  add_to_sig(result,"  virtual void Execute()\n",&currPos);  
  add_to_sig(result,"  {\n",&currPos);  
  add_to_sig(result,"    ",&currPos);  
  if(!IsVoid(aRetVal)/* && !IsString(aRetVal)*/) {
    add_to_sig(result,"myResult = ",&currPos);  
  }
  //if(IsString(aRetVal)) {
  //add_to_sig(result,"const char* ret = ",&currPos);  
  //}
  add_to_sig(result,"myObj->",&currPos);  
  add_to_sig(result,currentFunction->Name,&currPos);  
  add_to_sig(result,"(",&currPos);  
  for (i = 0; i < vtkWrap_CountWrappedParameters(currentFunction); i++) {
    aArgVal = currentFunction->Parameters[i];
    if(i!=0)
      add_to_sig(result,", ",&currPos);  
    if(IsClass(aArgVal) && IsPtr(aArgVal)) {
      add_to_sig(result,"*",&currPos);  
    }
    if (IsString(aArgVal) && IsConst(aArgVal) && strcmp(aArgVal->Class, "vtkStdString") != 0)
      sprintf(buf,"checkNullStr(myParam%d)",i);
    else
      sprintf(buf,"myParam%d",i);
    add_to_sig(result,buf,&currPos);
  }
  add_to_sig(result,");\n",&currPos);  
  //if(IsString(aRetVal)) {
  //add_to_sig(result,"    myResult = (ret==NULL)?NULL:CORBA::string_dup(\"\");\n",&currPos);  
  //}
  add_to_sig(result,"  }\n",&currPos);  
  
  add_to_sig(result,"};\n",&currPos);  
  add_to_sig(result,"//\n",&currPos);  
#endif
  currPos_sig=currPos;
  
#if ! defined(IDL_I_CC)
  add_to_sig(result,"        ",&currPos);
#endif

  output_temp(result,&currPos,MAX_ARGS,aRetVal,
              currentFunction->ReturnValue->Class,0);

#if defined(IDL_I_CC)
  add_to_sig(result,data->Name,&currPos);
  add_to_sig(result,"_i::",&currPos);
#endif
#if ! defined(IDL_I_HH) && ! defined(IDL_I_CC)
  add_to_sig(result,"_",&currPos);
#endif
  add_to_sig(result,currentFunction->Name,&currPos);

  if( strlen(num)!=0 ) {
    add_to_sig(result,num,&currPos);
  }
  currPos_num=currPos;
  add_to_sig(result," ",&currPos);

  /* print the arg list */
  add_to_sig(result,"(",&currPos);

  for (i = 0; i < vtkWrap_CountWrappedParameters(currentFunction); i++) {
    aArgVal = currentFunction->Parameters[i];
    if( i != 0 ) {
      add_to_sig(result,", ",&currPos);	  
    }
    /*    output_temp(result, &currPos, i, currentFunction->ArgTypes[i],
                (char*)currentFunction->ArgClasses[i],
                currentFunction->ArgCounts[i]);*/
    output_temp(result, &currPos, i, aArgVal,  aArgVal->Class, vtkWrap_CountWrappedParameters(currentFunction));
  }

  add_to_sig(result,")",&currPos);
#if defined(IDL_I_CC)
  add_to_sig(result," {\n",&currPos);
  add_to_sig(result,"  try {\n",&currPos);
  for (i = 0; i < vtkWrap_CountWrappedParameters(currentFunction); i++) {
    aArgVal = currentFunction->Parameters[i];
    if(IsClass(aArgVal)) {
      sprintf(buf,"    PARAVIS_Base_i* i_temp%d = GET_SERVANT(temp%d);\n",i,i);
      add_to_sig(result,buf,&currPos);
    }
    
    if(IsArray(aArgVal) && !IsString(aArgVal) && !IsClass(aArgVal) && !IsVoid(aArgVal) ) {
      sprintf(buf,"    CORBA::ULong j_temp%d;\n",i);
      add_to_sig(result,buf,&currPos);
      sprintf(buf,"    CORBA::ULong l_temp%d = temp%d.length();\n",i,i);
      add_to_sig(result,buf,&currPos);
      add_to_sig(result,"    ",&currPos);	      
      
      if(IsFloat(aArgVal))
        add_to_sig(result,"float",&currPos);
      
      if(IsDouble(aArgVal))
        add_to_sig(result,"double",&currPos);
      
      if(IsChar(aArgVal)) {
	if (IsUnsigned(aArgVal))
	  add_to_sig(result,"unsigned ",&currPos);
        add_to_sig(result,"char",&currPos);
      }

      if(IsvtkIdType(aArgVal)) {
        if (IsUnsigned(aArgVal))
          add_to_sig(result,"unsigned ",&currPos);
        add_to_sig(result,"vtkIdType",&currPos);
      } else {
        if(IsShortInt(aArgVal)) {
          if (IsUnsigned(aArgVal))
            add_to_sig(result,"unsigned ",&currPos);
          add_to_sig(result,"int",&currPos);
        }
      }
      
      if(IsLong(aArgVal)) {
        if (IsUnsigned(aArgVal))
          add_to_sig(result,"unsigned ",&currPos);
        add_to_sig(result,"long",&currPos);
      }
      sprintf(buf,"* a_temp%d = new ",i);
      add_to_sig(result,buf,&currPos);
      
      if(IsFloat(aArgVal))
        add_to_sig(result,"float",&currPos);
      
      if(IsDouble(aArgVal))
        add_to_sig(result,"double",&currPos);
      
      if(IsChar(aArgVal)) {
	if (IsUnsigned(aArgVal))
	  add_to_sig(result,"unsigned ",&currPos);
        add_to_sig(result,"char",&currPos);
      }

      if(IsvtkIdType(aArgVal)) {
        if (IsUnsigned(aArgVal))
          add_to_sig(result,"unsigned ",&currPos);
        add_to_sig(result,"vtkIdType",&currPos);
      } else {
        if(IsShortInt(aArgVal)) {
          if (IsUnsigned(aArgVal))
            add_to_sig(result,"unsigned ",&currPos);
          add_to_sig(result,"int",&currPos);
        }
      }
      
      if(IsLong(aArgVal)) {
        if (IsUnsigned(aArgVal))
          add_to_sig(result,"unsigned ",&currPos);
        add_to_sig(result,"long",&currPos);
      }
      
      sprintf(buf,"[l_temp%d];\n",i);
      add_to_sig(result,buf,&currPos);
      
      sprintf(buf,"    for(j_temp%d=0;j_temp%d<l_temp%d;j_temp%d++) {\n",i,i,i,i);
      add_to_sig(result,buf,&currPos);
      
      sprintf(buf,"      a_temp%d[j_temp%d]=temp%d[j_temp%d];\n",i,i,i,i);
      add_to_sig(result,buf,&currPos);
      
      add_to_sig(result,"    }\n",&currPos);
    }

    if(IsString(aArgVal)) {
      sprintf(buf,"    char *c_temp%d = CORBA::string_dup(temp%d);\n",i,i);
      add_to_sig(result,buf,&currPos);
    }

    if(IsVoid(aArgVal)) {
      sprintf(buf,"    long v_temp%d;\n",i);
      add_to_sig(result,buf,&currPos);
      
      sprintf(buf,"    temp%d.operator>>=(v_temp%d);\n",i,i);
      add_to_sig(result,buf,&currPos);
    }
  }  
  add_to_sig(result,"    ",&currPos);

  if(IsArray(aRetVal) && !IsClass(aRetVal) && !IsString(aRetVal)) {
    add_to_sig(result,"CORBA::ULong i_ret;\n",&currPos); 
    add_to_sig(result,"    PARAVIS::",&currPos); 
    AddTypeArray(aRetVal,result,&currPos);
    add_to_sig(result,"_var s_ret = new ",&currPos);
    AddTypeArray(aRetVal,result,&currPos);
    add_to_sig(result,"();\n",&currPos);
    sprintf(buf,"    s_ret->length(%d);\n",currentFunction->HintSize);
    add_to_sig(result,buf,&currPos);
    add_to_sig(result,"    ",&currPos); 
  }

  if(IsArray(aRetVal) && IsUnsigned(aRetVal)) {
    add_to_sig(result,"unsigned ",&currPos);
  }
  
  if(IsFloat(aRetVal)) {
    if(IsArray(aRetVal)) {
      add_to_sig(result,"float* a_ret = ",&currPos);
    } else {
      add_to_sig(result,"CORBA::Float ret = ",&currPos);
    }
  }
  
  if(IsDouble(aRetVal)) {
    if(IsArray(aRetVal)) {
      add_to_sig(result,"double* a_ret = ",&currPos);
    } else {
      add_to_sig(result,"CORBA::Double ret = ",&currPos);
    }
  }
  
  if(IsShortInt(aRetVal)) {
    if(IsArray(aRetVal)) {
      if(IsvtkIdType(aRetVal)) {
        add_to_sig(result,"vtkIdType",&currPos);
      } else {
	if(IsShort(aRetVal)) {
	  add_to_sig(result,"short",&currPos);
	} else {
	  add_to_sig(result,"int",&currPos);
	}
      }
      add_to_sig(result,"* a_ret = ",&currPos);
    } else {
      add_to_sig(result,"CORBA::Short ret = ",&currPos);
    }
  }
  
  if(IsLong(aRetVal)) {
    if(IsArray(aRetVal)) {
      add_to_sig(result,"long* a_ret = ",&currPos);
    } else {
      add_to_sig(result,"CORBA::Long ret = ",&currPos);
    }
  }
  
  //if(IsChar(aRetVal)) {
    if(IsString(aRetVal)) {
      add_to_sig(result,"char * ret = CORBA::string_dup(\"\");\n",&currPos);
      add_to_sig(result,"    const char * cret = ",&currPos);
    } else if (IsChar(aRetVal)) { //!!! vsv
      add_to_sig(result,"CORBA::Char ret = ",&currPos);	    
    }
    //}

  if(IsBoolean(aRetVal)) {
    add_to_sig(result,"CORBA::Boolean ret = ",&currPos);
  }
  
  if(IsVoidPtr(aRetVal)) {
    add_to_sig(result,"void * v_ret = ",&currPos);
  }
  
  if(IsClass(aRetVal)) {
    add_to_sig(result,"::",&currPos);
    add_to_sig(result,currentFunction->ReturnValue->Class,&currPos);
    add_to_sig(result,"* a",&currPos);
    add_to_sig(result,currentFunction->ReturnValue->Class,&currPos);
    add_to_sig(result," = ",&currPos);
  }
  if(IsVoid(aRetVal)) {
    add_to_sig(result,"if(getVTKObject() != NULL) ProcessVoidEvent",&currPos);
  } else {
    add_to_sig(result,"(getVTKObject() != NULL) ? ProcessEvent",&currPos);
  }
  add_to_sig(result,"(\n",&currPos);
  add_to_sig(result,"      new CreateEventName(",&currPos);
  add_to_sig(result,currentFunction->Name,&currPos);
  if( strlen(num)!=0 ) {
    add_to_sig(result,num,&currPos);
  }
  add_to_sig(result,")",&currPos);
  
  add_to_sig(result,"((::",&currPos);
  add_to_sig(result,data->Name,&currPos);	      
  add_to_sig(result,"*)",&currPos);
  add_to_sig(result,"getVTKObject()\n",&currPos);
  
  for (i = 0; i < vtkWrap_CountWrappedParameters(currentFunction); i++) {
    aArgVal = currentFunction->Parameters[i];
    add_to_sig(result,"      , ",&currPos);
    
    //if(IsClass(currentFunction->ArgTypes[i]) && IsPtr(currentFunction->ArgTypes[i])) {
    //add_to_sig(result,"*(",&currPos);
    //}
    
    if(IsClass(aArgVal)) {
      //sprintf(buf,"(i_temp%d != NULL)?dynamic_cast< ::%s*>(i_temp%d->getVTKObject()):NULL",i,currentFunction->ArgClasses[i],i);
      sprintf(buf,"(i_temp%d != NULL)?dynamic_cast< ::%s*>(i_temp%d->getVTKObject()):NULL",i,aArgVal->Class,i);
    } else {
      if(IsArray(aArgVal) && !IsString(aArgVal) && !IsVoid(aArgVal)) {
        sprintf(buf,"a_temp%d",i);
      } else {
        if(IsVoidPtr(aArgVal)) {
          sprintf(buf,"(void*)v_temp%d",i);
        } else {
          if(IsString(aArgVal)) {
            sprintf(buf,"c_temp%d",i);
          } else {
            sprintf(buf,"temp%d",i);
          }
        }
      }
    }
    
    add_to_sig(result,buf,&currPos);
    //if(IsClass(currentFunction->ArgTypes[i]) && IsPtr(currentFunction->ArgTypes[i])) {
    //add_to_sig(result,")",&currPos);
    //}
    add_to_sig(result,"\n",&currPos);
  }
  
  add_to_sig(result,"      )\n",&currPos);	      
  add_to_sig(result,"    )",&currPos);
  if(!IsVoid(aRetVal)) {
    add_to_sig(result,":",&currPos);
    if(IsClass(aRetVal) || IsString(aRetVal) || IsPtr(aRetVal) || IsArray(aRetVal))
      add_to_sig(result,"NULL",&currPos);
    else
      add_to_sig(result,"0",&currPos);
  }
  add_to_sig(result,";\n",&currPos);
  if(IsString(aRetVal)) {
    add_to_sig(result,"    if(cret!=NULL) ret=CORBA::string_dup(cret);\n",&currPos);
  }
  
  if(IsClass(aRetVal)) {
    add_to_sig(result,"    if(a",&currPos);
    add_to_sig(result,currentFunction->ReturnValue->Class,&currPos);	      
    add_to_sig(result," == NULL) {\n",&currPos);
    add_to_sig(result,"      return PARAVIS::",&currPos);	      
    add_to_sig(result,currentFunction->ReturnValue->Class,&currPos);	      
    add_to_sig(result,"::_nil();\n",&currPos);	      
    add_to_sig(result,"    }\n",&currPos);	      
    add_to_sig(result,"    ",&currPos);	      
    add_to_sig(result,"PARAVIS_Base_i* aPtr = ::CreateInstance(a",&currPos);	      
    add_to_sig(result,currentFunction->ReturnValue->Class,&currPos);	      
    add_to_sig(result,", a",&currPos);	      
    add_to_sig(result,currentFunction->ReturnValue->Class,&currPos);	      
    add_to_sig(result,"->GetClassName());\n",&currPos);
    add_to_sig(result,"    aPtr->Init(a",&currPos);	      
    add_to_sig(result,currentFunction->ReturnValue->Class,&currPos);	      
    add_to_sig(result,");\n",&currPos);	      
  }
    
  for (i = 0; i < vtkWrap_CountWrappedParameters(currentFunction); i++) {
    aArgVal = currentFunction->Parameters[i];
    if(IsArray(aArgVal) && !IsVoid(aArgVal) && !IsString(aArgVal) && !IsClass(aArgVal)) {
      if (!IsIn(aArgVal)) {
        sprintf(buf,"    for(j_temp%d=0;j_temp%d<l_temp%d;j_temp%d++) {\n",i,i,i,i);
        add_to_sig(result,buf,&currPos);
        
        sprintf(buf,"      temp%d[j_temp%d]=a_temp%d[j_temp%d];\n",i,i,i,i);
        add_to_sig(result,buf,&currPos);
        
        add_to_sig(result,"    }\n",&currPos);
      }
      sprintf(buf,"    delete [] a_temp%d;\n",i);
      add_to_sig(result,buf,&currPos);
    }
  }

  if (IsVoid(aRetVal) && !IsVoidPtr(aRetVal)) {
    add_to_sig(result,"    return;\n",&currPos); 
  } else {
    if(IsClass(aRetVal)) {
      add_to_sig(result,"    return aPtr->_this();\n",&currPos);
    } else {
      if(IsVoid(aRetVal)) {
        add_to_sig(result,"    CORBA::Any* ret = new CORBA::Any;\n",&currPos);
        add_to_sig(result,"    (*ret) <<= v_ret;\n",&currPos);
        add_to_sig(result,"    return ret;\n",&currPos); 
      } else {
        if(IsArray(aRetVal) && !IsString(aRetVal)) {
          sprintf(buf,"    for(i_ret=0; i_ret<%d; i_ret++) {\n",currentFunction->HintSize);
          add_to_sig(result,buf,&currPos); 
          add_to_sig(result,"      s_ret[i_ret]=a_ret[i_ret];\n",&currPos); 
          add_to_sig(result,"    }\n",&currPos); 
          add_to_sig(result,"    return s_ret._retn();\n",&currPos); 
        } else {
          add_to_sig(result,"    return ret;\n",&currPos); 
        }
      }
    }
  }

  add_to_sig(result,"  } catch(...) {\n",&currPos);
  add_to_sig(result,"    MESSAGE(\"",&currPos);
  //add_to_sig(result,data->ClassName,&currPos);	      
  //add_to_sig(result,"::",&currPos);
  add_to_sig(result,currentFunction->Name,&currPos);	      
  add_to_sig(result," - Unknown exception was occured!!!\");\n",&currPos);

  if (IsVoid(aRetVal)&& !IsVoidPtr(aRetVal)) {
    add_to_sig(result,"    return;\n",&currPos);
  } else{
    if(IsString(aRetVal)) {
      add_to_sig(result,"    return CORBA::string_dup(\"\");\n",&currPos);
    } else {
      if(IsClass(aRetVal)) {
        add_to_sig(result,"    return PARAVIS::",&currPos);
        add_to_sig(result,currentFunction->ReturnValue->Class,&currPos);
        add_to_sig(result,"::_nil();\n",&currPos);
      } else {
        if(IsArray(aRetVal) && !IsVoid(aRetVal)) {
          add_to_sig(result,"    PARAVIS::",&currPos);
          AddTypeArray(aRetVal,result,&currPos);
          add_to_sig(result,"_var s_ret = new ",&currPos);
          AddTypeArray(aRetVal,result,&currPos);
          add_to_sig(result,"();\n",&currPos);
          sprintf(buf,"    s_ret->length(%d);\n",currentFunction->HintSize);
          add_to_sig(result,buf,&currPos);
          add_to_sig(result,"    return s_ret._retn();\n",&currPos);
        } else {
          add_to_sig(result,"    return 0;\n",&currPos);
        }
      }
    }
  }

  add_to_sig(result,"  }\n",&currPos);
  add_to_sig(result,"}\n",&currPos);
#else
  add_to_sig(result,";\n",&currPos);
#endif

  if(ret)
    {
      result[currPos_sig]='\0';
      /*currentFunction->Signature = (const char*)realloc((void*)currentFunction->Signature,
        (size_t)(currPos_sig+1));*/
      cp = (char *)malloc(currPos_sig+1);
      strcpy(cp, result);
      currentFunction->Signature = cp;
      //strcpy((char*)currentFunction->Signature,result);

      return;
    }

  //if(strcmp(currentFunction->Name,"NewInstance") == 0) {
  //currentFunction->Signature[0]='\0';
  //return;
  //}

  for(i = 0; i < numberOfReadFunctions; i++) {
    if(strcmp(readFunctions[i].Name,currentFunction->Name) == 0) {
      j=readFunctions[i].EndPos-readFunctions[i].NumPos;
      strncpy(buf,result+currPos_num,j);
      strncpy(buf1,readFunctions[i].Signature+readFunctions[i].NumPos,j);
      buf[j]=0;
      buf1[j]=0;
      if(strcmp(buf,buf1) == 0) {
        //((char*)currentFunction->Signature)[0]='\0';
	currentFunction->Signature = EmptyString;
        return;
      }
    }
  }
  /*currentFunction->Signature = (const char*)realloc((void*)currentFunction->Signature,
    (size_t)(currPos+1));*/
  //strcpy((char*)currentFunction->Signature, result);
  cp = (char *)malloc(currPos+1);
  strcpy(cp, result);
  currentFunction->Signature = cp;
}

void outputFunction2(FILE *fp, ClassInfo *data)
{
  int i, j, k, is_vtkobject, fnum, backnum;//, is_static, occ, goto_used;
  //int all_legacy;
  FunctionInfo *theFunc;
  FunctionInfo *backFunc;
  const char *theName;
  unsigned int theType;
  const char *backName;
  int backType;
  char static num[8];
  //int isSMObject = 0;
  int found = 0;
  ValueInfo* aArgVal = 0;
  ValueInfo* aBackArgVal = 0;


#if defined(IDL_I_HH)
  fprintf(fp,"#include \"SALOMEconfig.h\"\n");
  fprintf(fp,"#include CORBA_SERVER_HEADER(PARAVIS_Gen_%s)\n",data->Name);
  fprintf(fp,"\n");
  fprintf(fp,"#include \"SALOME_GenericObj_i.hh\"\n");
  fprintf(fp,"#include \"PARAVIS_Gen_i.hh\"\n");
  fprintf(fp,"#include \"PARAVIS_Gen_Types.hh\"\n");
  fprintf(fp,"#include \"vtkSmartPointer.h\"\n");
  fprintf(fp,"#include \"SALOME_Event.h\"\n");

  for(i=0;i<data->NumberOfSuperClasses;i++)
  {
    fprintf(fp,"#include \"PARAVIS_Gen_%s_i.hh\"\n",data->SuperClasses[i]);
  }

  fprintf(fp,"\nclass %s;\n",data->Name);
  fprintf(fp,"\nnamespace PARAVIS\n{\n\n");
#elif defined(IDL_I_CC)
  fprintf(fp,"#include \"SALOME_GenericObj_i.hh\"\n");
  fprintf(fp,"#include \"PARAVIS_Gen_%s_i.hh\"\n",data->Name);
  fprintf(fp,"#include \"PV_Tools.h\"\n");
  fprintf(fp,"#include \"SALOME_Event.h\"\n");
  fprintf(fp,"#include <%s.h>\n",data->Name);
#else
  fprintf(fp,"#include \"PARAVIS_Gen.idl\"\n");
  fprintf(fp,"#include \"PARAVIS_Gen_Types.idl\"\n");

  for(i=0;i<data->NumberOfSuperClasses;i++)
  {
    fprintf(fp,"#include \"PARAVIS_Gen_%s.idl\"\n",data->SuperClasses[i]);
  }

  fprintf(fp,"\nmodule PARAVIS\n{\n\n");
#endif

  is_vtkobject = ((strcmp(data->Name,"vtkObjectBase") == 0) || 
                  (data->NumberOfSuperClasses != 0));

  for(i = 0; i < data->NumberOfSuperClasses; i++) {
    read_class_functions(data->SuperClasses[i],data->Name,fp);
  }

  /* create a idl signature for each method */
  for (fnum = 0; fnum < numberOfWrappedFunctions; fnum++)
    {
      theFunc = wrappedFunctions[fnum];
      currentFunction = theFunc;
      //printf("#### %i)Function %s\n", fnum, theFunc->Name);
      
      /* names of idl methods should be unique */
      num[0]='\0';
      j=-1;
      for (i = 0; i < numberOfReadFunctions; i++) {
        if(strcmp(currentFunction->Name,readFunctions[i].Name) == 0)
          j++;
      }
      for (i = 0; i < fnum; i++)
        {
          if( strcmp(currentFunction->Name,wrappedFunctions[i]->Name) ==  0 )
            j++;
        }
      
      if(j<0) {
        for (i = fnum+1; i < numberOfWrappedFunctions; i++) {
          if( strcmp(currentFunction->Name,wrappedFunctions[i]->Name) ==  0 ) {
            j=0;
            break;
          }
        }
      } else {
        j++;
      }
      
      if(j>=0)
        {
          sprintf(num,"_%d",j);
        }
      get_signature(num,data);
    }
  /* create external type declarations for all object
     return types */
  for (fnum = 0; fnum < numberOfWrappedFunctions; fnum++)
    {
      theFunc = wrappedFunctions[fnum];
      currentFunction = theFunc;
      theName = NULL;
      theType = 0;
      
      for (i = vtkWrap_CountWrappedParameters(theFunc); i >= 0; i--)
        {
          if (i==0)/* return type */
            {
              aArgVal = theFunc->ReturnValue;
              //theType = theFunc->ReturnType;
              //theName = (char*)theFunc->ReturnClass;
            }
          else /* arg type */
            {
              aArgVal = theFunc->Parameters[i-1];
              //theType = theFunc->ArgTypes[i-1];
              //theName = (char*)theFunc->ArgClasses[i-1];
            }
          theType = aArgVal->Type & VTK_PARSE_BASE_TYPE;
          theName = aArgVal->Class;
          /* check for object types */
          /*if ((theType & VTK_PARSE_BASE_TYPE == 0x309)||
            (theType & VTK_PARSE_BASE_TYPE == 0x109))*/
          if ((theType == VTK_PARSE_OBJECT) || (theType == VTK_PARSE_OBJECT_REF))
            {
              /* check that we haven't done this type (no duplicate declarations) */
              for (backnum = fnum; backnum >= 0; backnum--) 
                {
                  backFunc = wrappedFunctions[backnum];
                  backName = NULL;
                  backType = 0;
                  if (backnum == fnum)
                    k = i+1;
                  else
                    k = 0;
                  for (j = vtkWrap_CountWrappedParameters(backFunc); j >= k; j--)
                    {
                      if (j==0) /* return type */
                        {
                          aBackArgVal = backFunc->ReturnValue;
                          //backType = backFunc->ReturnType;
                          //backName = (char*)backFunc->ReturnClass;
                        }
                      else /* arg type */
                        {
                          aBackArgVal = backFunc->Parameters[j-1];
                          //backType = backFunc->ArgTypes[j-1];
                          //backName = (char*)backFunc->ArgClasses[j-1];
                        }
                      backType = aBackArgVal->Type & VTK_PARSE_BASE_TYPE;
                      backName = aBackArgVal->Class;
                      /*if (((backType % VTK_PARSE_BASE_TYPE == 0x309)||
                        (backType % VTK_PARSE_BASE_TYPE == 0x109)))*/
                      if ((backType == VTK_PARSE_OBJECT) || (backType == VTK_PARSE_OBJECT_REF))
                       {
                          if(strcmp(theName,backName) == 0)
                            {
                              break;
                            }
                        }
                    }
                  if (j >= k)
                    {
                      break;
                    }
                }
              if (backnum < 0 && strcmp(data->Name,theName) != 0)
                {
                  found = 0;
                  for(j = 0; strcmp(wrapped_classes[j],"") != 0 && found == 0; j++)
                    {
                      if(strcmp(wrapped_classes[j],theName) == 0)
                        found = 1;
                    }
                  if(found)
                    {
#if defined(IDL_I_HH)
                      fprintf(fp,"    class %s_i;\n",theName);
#elif defined(IDL_I_CC)
                      fprintf(fp,"#include \"PARAVIS_Gen_%s_i.hh\"\n",theName);
                      fprintf(fp,"#include <%s.h>\n",theName);
#else
                      //fprintf(fp,"#include \"PARAVIS_Gen_%s.idl\"\n",theName);
                      fprintf(fp,"    interface %s;\n",theName);
#endif
                    }
                }
            }
        }
    }
  
  //fprintf(fp,"\nmodule PARAVIS\n{\n");
#if defined(IDL_I_HH)
  fprintf(fp,"\n    class %s_i : public virtual POA_PARAVIS::%s, public virtual PARAVIS::PARAVIS_Base_i",data->Name,data->Name);
  //for(i = 0; i < data->NumberOfSuperClasses; i++) {
  //  fprintf(fp,", public virtual %s_i",data->SuperClasses[i]);
  //}
  //fprintf(fp,", public virtual SALOME::GenericObj_i");
  fprintf(fp," {");
  fprintf(fp,"\n    public:\n");
  fprintf(fp,"\n        %s_i();\n",data->Name);
  if(strcmp(data->Name,"vtkSMSessionProxyManager") != 0) {
    fprintf(fp,"\n        ::vtkObjectBase* GetNew();\n");
  }

#elif defined(IDL_I_CC)
  fprintf(fp,"extern PARAVIS::PARAVIS_Base_i* CreateInstance(::vtkObjectBase* Inst, const QString&);\n");
  fprintf(fp,"\nnamespace PARAVIS\n{\n");
  fprintf(fp,"typedef %s_i current_interface;\n",data->Name);
  fprintf(fp,"#define CreateEventName(Function) Event%s ##Function\n",data->Name);
  fprintf(fp,"%s_i::%s_i() {\n",data->Name,data->Name);
  //fprintf(fp,"    Init(::%s::New());\n",data->Name);
  fprintf(fp,"}\n");
  fprintf(fp,"\n");
  
  if(strcmp(data->Name,"vtkSMSessionProxyManager") != 0) {
    fprintf(fp,"::vtkObjectBase* %s_i::GetNew() {\n", data->Name);
    if(strcmp(data->Name,"vtkSMProxyManager") == 0) {
      fprintf(fp,"  return ::%s::GetProxyManager();\n",data->Name);
    } else {
      fprintf(fp,"  return ::%s::New();\n",data->Name);
    }
    fprintf(fp,"}\n");
  }
#else
  fprintf(fp,"\n    interface %s : PARAVIS_Base",data->Name);
  fprintf(fp,"\n    {\n");
#endif

  for(i = 0; i < numberOfReadFunctions; i++) {
    fprintf(fp, "%s\n", readFunctions[i].Signature);
  }

  for (fnum = 0; fnum < numberOfWrappedFunctions; fnum++) {
    fprintf(fp,"%s\n",wrappedFunctions[fnum]->Signature);
  }

#if defined(IDL_I_HH)
  fprintf(fp,"    };\n");
  fprintf(fp,"}\n");
#elif defined(IDL_I_CC)
  fprintf(fp,"};\n");
#else
  fprintf(fp,"    };\n");
  fprintf(fp,"};\n");
#endif

  return;
}

void outputFunction(FILE *fp, ClassInfo *data)
{
  int i;
  //int args_ok = 1;
  ValueInfo* aRetVal = NULL;//currentFunction->ReturnValue;
  ValueInfo* aArgVal = NULL;
  unsigned int aType;
  unsigned int argtype;

  fp = fp;
  /* some functions will not get wrapped no matter what else,
     and some really common functions will appear only in vtkObjectPython */
  if (currentFunction->IsOperator || 
      currentFunction->ArrayFailure ||
      !currentFunction->IsPublic ||
      !currentFunction->Name)
    {
      return;
    }
  //printf("#### Check %s\n", currentFunction->Name);
 
  /* check to see if we can handle the args */
  for (i = 0; i < vtkWrap_CountWrappedParameters(currentFunction); i++)
    {
      aArgVal = currentFunction->Parameters[i];
      /*printf("  Argument: %s ", vtkWrap_GetTypeName(aArgVal));
      if (vtkWrap_IsArray(aArgVal)) {
        printf("!!!! Argument %i is array\n", i);
        return;
        }*/
      if (vtkWrap_IsStream(aArgVal)) {
        //printf("!!!! Argument %i is stream\n", i);
        return;
      }
      if (IsPtr(aArgVal)) {
        //printf("!!!! Argument %i is pointer value\n", i);
        return;
      }
      if (IsUnknown(aArgVal)) {
        //printf("!!!! Argument %i is unknown value\n", i);
       return;
      }
      if (vtkWrap_IsVoidPointer(aArgVal)) {
        //printf("!!!! Argument %i is void pointer\n", i);
        return;
      }
      if (vtkWrap_IsVoidFunction(aArgVal)) {
        //printf("!!!! Argument %i is void function\n", i);
        return;
      }
      argtype = (aArgVal->Type & VTK_PARSE_INDIRECT);
      if (argtype == VTK_PARSE_POINTER_POINTER){
        //printf("!!!! Argument %i is pointer to pointer\n", i);
        return;
      }
      if (vtkWrap_IsNonConstRef(aArgVal)) {
        //printf("!!!! Argument %i is non const ref\n", i);
        return;
      }
      if (vtkWrap_IsSpecialObject(aArgVal)) {
        //printf("!!!! Argument %i is special object\n", i);
        return;
      }
      /*if (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE == 9) args_ok = 0;
      if ((currentFunction->ArgTypes[i] % 0x10) == 8) args_ok = 0;
      if (((currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE)/0x100 != 0x3)&&
          (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE != 0x109)&&
          ((currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE)/0x100)) args_ok = 0;
      if (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE == 0x313) args_ok = 0;
      if (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE == 0x314) args_ok = 0;
      if (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE == 0x31A) args_ok = 0;
      if (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE == 0x31B) args_ok = 0;
      if (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE == 0x31C) args_ok = 0;
      if (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE == 0x315) args_ok = 0;
      if (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE == 0x316) args_ok = 0;*/
    }
  aRetVal = currentFunction->ReturnValue;
  if (aRetVal) {
    //printf("#### Return type: %s\n", vtkWrap_GetTypeName(aRetVal));
    aType = aRetVal->Type & VTK_PARSE_BASE_TYPE;
    if (IsPtr(aRetVal)) {
      //printf("!!!! Return Value is pointer\n");
      return;
    }
    /* eliminate unsigned char * and unsigned short * */
    argtype = (aRetVal->Type & VTK_PARSE_UNQUALIFIED_TYPE);
    if ((argtype == VTK_PARSE_UNSIGNED_CHAR_PTR) || (argtype == VTK_PARSE_UNSIGNED_SHORT)) { 
      //printf("!!!! Return Value is unsigned char or short\n");
      return;
    }

    if ((aType == VTK_PARSE_UNKNOWN)) {
      //printf("!!!! Return Value is unknown\n");
      return;
    }
    argtype = (aRetVal->Type & VTK_PARSE_INDIRECT);
    if (argtype == VTK_PARSE_POINTER_POINTER){
      //printf("!!!! Return value is pointer to pointer\n", i);
      return;
    }
    if (vtkWrap_IsSpecialObject(aRetVal)) {
      //printf("!!!! Return is special object\n", i);
      return;
    }
  }
  /*if ((aRetVal % 0x10) == 0x8) args_ok = 0;
  if (aRetVal % VTK_PARSE_BASE_TYPE == 0x9) args_ok = 0;
  if (((aRetVal % VTK_PARSE_BASE_TYPE)/0x100 != 0x3)&&
      (aRetVal % VTK_PARSE_BASE_TYPE != 0x109)&&
      ((aRetVal % VTK_PARSE_BASE_TYPE)/0x100)) args_ok = 0;
  */

  /*if (aRetVal % VTK_PARSE_BASE_TYPE == 0x313) args_ok = 0;
  if (aRetVal % VTK_PARSE_BASE_TYPE == 0x314) args_ok = 0;
  if (aRetVal % VTK_PARSE_BASE_TYPE == 0x31A) args_ok = 0;
  if (aRetVal % VTK_PARSE_BASE_TYPE == 0x31B) args_ok = 0;
  if (aRetVal % VTK_PARSE_BASE_TYPE == 0x31C) args_ok = 0;
  if (aRetVal % VTK_PARSE_BASE_TYPE == 0x315) args_ok = 0;
  if (aRetVal % VTK_PARSE_BASE_TYPE == 0x316) args_ok = 0;
  

  if (vtkWrap_CountWrappedParameters(currentFunction) && 
      (currentFunction->ArgTypes[0] == 0x5000)
      &&(vtkWrap_CountWrappedParameters(currentFunction) != 0x1)) args_ok = 0;*/

  /* make sure we have all the info we need for array arguments in */
  /*for (i = 0; i < vtkWrap_CountWrappedParameters(currentFunction); i++)
    {
    if (((currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE)/0x100 == 0x3)&&
        (currentFunction->ArgCounts[i] <= 0)&&
        (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE != 0x309)&&
        (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE != 0x303)&&
        (currentFunction->ArgTypes[i] % VTK_PARSE_BASE_TYPE != 0x302)) args_ok = 0;
        }*/

  /* if we need a return type hint make sure we have one */
  //switch (aRetVal % VTK_PARSE_BASE_TYPE)
  /*switch (aRetVal->Type % VTK_PARSE_BASE_TYPE)
    {
    case 0x301: case 0x307: case 0x30A: case 0x30B: case 0x30C: case 0x30D: case 0x30E:
    case 0x304: case 0x305: case 0x306:
      args_ok = currentFunction->HaveHint;
      break;
      }*/
  
  /* make sure it isn't a Delete or New function */
  if (!strcmp("Delete",currentFunction->Name) ||
      !strcmp("New",currentFunction->Name))
    {
      return;
    }

  if (vtkWrap_IsDestructor(data, currentFunction) || vtkWrap_IsConstructor(data, currentFunction)) {
    //printf("!!!! Return Value is constructor or destructor\n");
    return;
  }
  
  /* check for New() function */
  if (!strcmp("New",currentFunction->Name) && vtkWrap_CountWrappedParameters(currentFunction) == 0)
    {
      class_has_new = 1;
    }

  if (currentFunction->IsPublic && //args_ok && 
      strcmp(data->Name,currentFunction->Name) &&
      strcmp(data->Name, currentFunction->Name + 1))
    {
      //printf("#### %i Function %s\n", numberOfWrappedFunctions, currentFunction->Name);
      wrappedFunctions[numberOfWrappedFunctions] = currentFunction;
      numberOfWrappedFunctions++;
    }
  
  return;
}

/* print the parsed structures */
int main(int argc, char *argv[])
{
  OptionInfo *options;
  FileInfo *file_info;
  ClassInfo *data;
  FILE *fp;
  int i;

  /* get command-line args and parse the header file */
  file_info = vtkParse_Main(argc, argv);

  /* get the command-line options */
  options = vtkParse_GetCommandLineOptions();

  /* get the output file */
  fp = fopen(options->OutputFileName, "w");

  if (!fp)
    {
    fprintf(stderr, "Error opening output file %s\n", options->OutputFileName);
    exit(1);
    }

  /* get the main class */
  if ((data = file_info->MainClass) == NULL)
    {
    fclose(fp);
    exit(0);
    }

  /* get the hierarchy info for accurate typing */
  if (options->HierarchyFileName)
    {
    hierarchyInfo = vtkParseHierarchy_ReadFile(options->HierarchyFileName);
    }

#if defined(IDL_I_HH)
  fprintf(fp, "// idl wrapper interface for %s object implementation\n//\n", data->Name);
#elif defined(IDL_I_CC)
  fprintf(fp, "// idl wrapper implementation for %s object\n//\n", data->Name);
#else
  fprintf(fp, "// idl wrapper for %s object\n//\n", data->Name);
#endif
  for (i = 0;1;i++)
    {
      if(strlen(Copyright[i]) != 0)
        {
          fprintf(fp,"%s\n",Copyright[i]);
        }
      else
        {
          break;
        }
    }
  fprintf(fp,"\n");
#if defined(IDL_I_HH)
  fprintf(fp,"#ifndef PARAVIS_Gen_%s_i_HeaderFile\n",data->Name);
  fprintf(fp,"#define PARAVIS_Gen_%s_i_HeaderFile\n",data->Name);
  fprintf(fp,"\n");
#elif defined(IDL_I_CC)
#else
  fprintf(fp,"#ifndef __PARAVIS_Gen_%s__\n",data->Name);
  fprintf(fp,"#define __PARAVIS_Gen_%s__\n",data->Name);
  fprintf(fp,"\n");
#endif

  /* insert function handling code here */
  for (i = 0; i < data->NumberOfFunctions; i++)
    {
      currentFunction = data->Functions[i];
#ifdef VTK_LEGACY_REMOVE
      if(currentFunction->IsLegacy)
	continue;
#endif
      outputFunction(fp, data);
    }
  //printf("#### NbFunctions %i\n", numberOfWrappedFunctions);
  //if (data->NumberOfSuperClasses || (!data->IsAbstract))
  //if (numberOfWrappedFunctions)
  // {
      outputFunction2(fp, data);
      // }

#if defined(IDL_I_HH)
  fprintf(fp,"\n#endif\n");
#elif defined(IDL_I_CC)
#else
  fprintf(fp,"\n#endif\n");
#endif

  vtkParse_Free(file_info);

  return 0;
}
