// PARAVIS : ParaView wrapper SALOME module
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#include "vtkParse.h"
#include "vtkWrapIDL.h"

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

#define bs 8192

int numberOfWrappedFunctions = 0;
FunctionInfo *wrappedFunctions[1000];
extern FunctionInfo *currentFunction;

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

void AddNotReturnArg(int Type, char *Result, int *CurrPos) {
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  ;
#else
  if(IsIn(Type))
    add_to_sig(Result,"in ",CurrPos);
  else
    add_to_sig(Result,"inout ",CurrPos);
#endif
}

int IsFunction(int Type) {
  return (Type == 0x5000);
}

int IsConst(int Type) {
  return ((Type % 0x2000) >= 0x1000);
}

void AddConst(char *Result, int *CurrPos) {
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  add_to_sig(Result,"const ",CurrPos);
#else
  add_to_sig(Result,"in ",CurrPos);
#endif
}

int IsPtr(int Type) {
  return ((Type % 0x1000)/0x100 == 0x1);
}

int IsIn(int Type) {
  return 1;
  //return ((Type % 0x1000)/0x100 < 1 || (Type % 0x1000)/0x100 > 7);
}

int IsUnknown(int Type) {
  return ((Type % 0x1000)/0x100 == 0x8);
}

void AddAtomArg(int I, int Type, char *TypeIDL, char *TypeCorba, char *Result, int *CurrPos) {
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

int IsArray(int Type) {
  return ((Type % 0x1000)/0x100 == 0x3);
}

void AddArrayArg(int I, int Type, char *TypeIDL, char *Result, int *CurrPos) {
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

int IsBoolean(int Type) {
  return ((Type % 0x10) == 0xE);
}

void AddBooleanAtomArg(int I, int Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"boolean","Boolean",Result,CurrPos);
}

int IsChar(int Type) {
  return ((Type % 0x10) == 0x3 || (Type % 0x10) == 0xD);
}

void AddCharAtomArg(int I, int Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"char","Char",Result,CurrPos);
}

int IsString(Type) {
  return (IsChar(Type) && IsArray(Type));
}

void AddStringArg(int I, char *Result, int *CurrPos) {
#if defined(IDL_I_HH) || defined(IDL_I_CC)
  add_to_sig(Result,"char ",CurrPos);
#else
  add_to_sig(Result,"string ",CurrPos);
#endif
}

int IsFloat(int Type) {
  return ((Type % 0x10) == 0x1);
}

void AddFloatAtomArg(int I, int Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"float","Float",Result,CurrPos);
}

int IsFloatArray(int Type) {
  return (IsFloat(Type) && IsArray(Type));
}

void AddFloatArrayArg(int I, int Type, char *Result, int *CurrPos) {
  AddArrayArg(I,Type,"float",Result,CurrPos);
}

int IsDouble(int Type) {
  return ((Type % 0x10) == 0x7);
}

void AddDoubleAtomArg(int I, int Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"double","Double",Result,CurrPos);
}

int IsDoubleArray(int Type) {
  return (IsDouble(Type) && IsArray(Type));
}

void AddDoubleArrayArg(int I, int Type, char *Result, int *CurrPos) {
  AddArrayArg(I,Type,"double",Result,CurrPos);
}

int IsvtkIdType(int Type) {
  return((Type % 0x10) == 0xA);
}

int IsShort(int Type) {
  return ((Type % 0x10) == 0x4 || (Type % 0x10) == 0x5 || (Type % 0x10) == 0xA);
}

void AddShortAtomArg(int I, int Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"short","Short",Result,CurrPos);
}

int IsShortArray(int Type) {
  return (IsShort(Type) && IsArray(Type));
}

void AddShortArrayArg(int I, int Type, char *Result, int *CurrPos) {
  AddArrayArg(I,Type,"short",Result,CurrPos);
}

int IsLong(int Type) {
  return ((Type % 0x10) == 0x6 || (Type % 0x10) == 0xB || (Type % 0x10) == 0xC);
}

void AddLongAtomArg(int I, int Type, char *Result, int *CurrPos) {
  AddAtomArg(I,Type,"long","Long",Result,CurrPos);
}

int IsLongArray(int Type) {
  return (IsLong(Type) && IsArray(Type));
}

void AddLongArrayArg(int I, int Type, char *Result, int *CurrPos) {
  AddArrayArg(I,Type,"long",Result,CurrPos);
}

int IsClass(int Type) {
  return ((Type % 0x10) == 0x9);
}

void AddClassArg(int I, int Type, char *Class, char *Result, int *CurrPos) {
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

int _IsVoid(int Type) {
  return ((Type % 0x10) == 0x2);
}

int IsVoid(int Type) {
  return (_IsVoid(Type) && (!IsPtr(Type)));
}

void AddVoid(char *Result, int *CurrPos) {
  add_to_sig(Result,"void ",CurrPos);
}

int IsVoidPtr(int Type) {
  return (_IsVoid(Type) && (IsPtr(Type) || IsArray(Type)));
}

void AddVoidArg(int I, char *Result, int *CurrPos, int Type) {
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

void AddTypeArray(int Type, char *Result, int *CurrPos) {
  if(IsShort(Type))
    add_to_sig(Result,"short",CurrPos);
  if(IsLong(Type))
    add_to_sig(Result,"long",CurrPos);
  if(IsFloat(Type))
    add_to_sig(Result,"float",CurrPos);
  if(IsDouble(Type))
    add_to_sig(Result,"double",CurrPos);
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

void output_type(char* result, int *currPos, int i, int aType, char *Id)
{
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
  
  if(IsShort(aType)) {
    if(IsArray(aType)) {
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
      add_to_sig(result,"long",currPos);
    } else {
      add_to_sig(result,"CORBA::Long",currPos);
    }
  }
  
  if(IsChar(aType)) {
    if(IsString(aType)) {
      if(IsReturnArg(i))
	add_to_sig(result,"const ",currPos);
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
    add_to_sig(result,"*",currPos);
  }
}

void output_typedef(char* result, int *currPos, int i, int aType, char *Id)
{
  add_to_sig(result,"  typedef ",currPos);
  output_type(result,currPos,i,aType,Id);
}

void output_temp(char* result, int *currPos, int i, int aType, char *Id, int aCount)
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

  if(IsShort(aType)) {
    if(IsShortArray(aType)) {
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
      AddCharAtomArg(i,aType,result,currPos);
    }
  }

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
  int i;
  int j;
  int flen=0;
  int num=0;
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
      fprintf(fp,sig);
    }
    if(strncmp("#include <vtk",sig,13)==0) {
      fprintf(fp,sig);
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
      fgets(buf,bs-1,fin);
      if(strlen(buf) > 1) {
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

void get_signature(const char* num, FileInfo *data)
{
  static char result[bs];
  int currPos = 0;
  int currPos_sig = 0;
  int argtype;
  int i, j;
  static char buf[bs];
  static char buf1[bs];
  int ret = 0;
  int found = 0;
  int currPos_num = 0;

  add_to_sig(result,"\n",&currPos);
  if (currentFunction->Signature) {
#if ! defined(IDL_I_CC)
    add_to_sig(result,"        ",&currPos);
#endif
    add_to_sig(result,"//C++: ",&currPos);
    add_to_sig(result,currentFunction->Signature,&currPos);
    add_to_sig(result,"\n",&currPos);
  }

  if(IsClass(currentFunction->ReturnType) && ret == 0) {
    found = 0;
    for(i = 0; strcmp(wrapped_classes[i],"") != 0 && found == 0; i++) {
      if(strcmp(wrapped_classes[i],currentFunction->ReturnClass) == 0)
	found = 1;
    }
    if(!found)
      ret = 1;
  }

  for (j = 0; j < currentFunction->NumberOfArguments; j++) {
    if(IsFunction(currentFunction->ArgTypes[j]))
      ret == 1;
    if(IsClass(currentFunction->ArgTypes[j]) && ret == 0) {
      found = 0;
      for(i = 0; strcmp(wrapped_classes[i],"") != 0 && found == 0; i++) {
	if(strcmp(wrapped_classes[i],currentFunction->ArgClasses[j]) == 0)
	  found = 1;
      }
      if(!found)
	ret = 1;
    }
  }

  if (IsArray(currentFunction->ReturnType) && !IsClass(currentFunction->ReturnType) && !IsString(currentFunction->ReturnType) && currentFunction->HintSize == 0) {
    ret = 1;
  }

  if(ret) {
    add_to_sig(result,"//\n",&currPos);
    currentFunction->Signature = realloc(currentFunction->Signature,
					 (size_t)(currPos+1));
    strcpy(currentFunction->Signature,result);
    
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
    if(!IsVoid(currentFunction->ReturnType)) {
      output_typedef(result,&currPos,MAX_ARGS,currentFunction->ReturnType,
		     currentFunction->ReturnClass);
      add_to_sig(result," TResult;\n",&currPos);
      add_to_sig(result,"  TResult myResult;\n",&currPos);
    }
    
    output_typedef(result, &currPos, 0, 0x309,
		   data->ClassName);
    add_to_sig(result," TObj;\n",&currPos);
    add_to_sig(result,"  TObj myObj;\n",&currPos);
    
    for (i = 0; i < currentFunction->NumberOfArguments; i++) {
      output_typedef(result, &currPos, i, currentFunction->ArgTypes[i],
		     currentFunction->ArgClasses[i]);
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
    for (i = 0; i < currentFunction->NumberOfArguments; i++) {
      sprintf(buf,", TParam%d theParam%d",i,i);
      add_to_sig(result,buf,&currPos);
    }
    add_to_sig(result,"):\n",&currPos);  
    add_to_sig(result,"  myObj(theObj)",&currPos);  
    for (i = 0; i < currentFunction->NumberOfArguments; i++) {
      sprintf(buf,", myParam%d(theParam%d)",i,i);
      add_to_sig(result,buf,&currPos);
    }
    add_to_sig(result,"\n",&currPos);  
    add_to_sig(result,"  { }\n",&currPos);  
    add_to_sig(result,"\n",&currPos);  
    add_to_sig(result,"  virtual void Execute()\n",&currPos);  
    add_to_sig(result,"  {\n",&currPos);  
    add_to_sig(result,"    ",&currPos);  
    if(!IsVoid(currentFunction->ReturnType)/* && !IsString(currentFunction->ReturnType)*/) {
      add_to_sig(result,"myResult = ",&currPos);  
    }
    //if(IsString(currentFunction->ReturnType)) {
    //add_to_sig(result,"const char* ret = ",&currPos);  
    //}
    add_to_sig(result,"myObj->",&currPos);  
    add_to_sig(result,currentFunction->Name,&currPos);  
    add_to_sig(result,"(",&currPos);  
    for (i = 0; i < currentFunction->NumberOfArguments; i++) {
      if(i!=0)
	add_to_sig(result,", ",&currPos);  
      if(IsClass(currentFunction->ArgTypes[i]) && IsPtr(currentFunction->ArgTypes[i])) {
	add_to_sig(result,"*",&currPos);  
      }
      sprintf(buf,"myParam%d",i);
      add_to_sig(result,buf,&currPos);
    }
    add_to_sig(result,");\n",&currPos);  
    //if(IsString(currentFunction->ReturnType)) {
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

  output_temp(result,&currPos,MAX_ARGS,currentFunction->ReturnType,
	      currentFunction->ReturnClass,0);

#if defined(IDL_I_CC)
  add_to_sig(result,data->ClassName,&currPos);
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

  for (i = 0; i < currentFunction->NumberOfArguments; i++) {
    if( i != 0 ) {
      add_to_sig(result,", ",&currPos);	  
    }
    output_temp(result, &currPos, i, currentFunction->ArgTypes[i],
		currentFunction->ArgClasses[i],
		currentFunction->ArgCounts[i]);
  }

  add_to_sig(result,")",&currPos);
#if defined(IDL_I_CC)
  add_to_sig(result," {\n",&currPos);
  add_to_sig(result,"  try {\n",&currPos);
  for (i = 0; i < currentFunction->NumberOfArguments; i++) {
    if(IsClass(currentFunction->ArgTypes[i])) {
      sprintf(buf,"    PARAVIS_Base_i* i_temp%d = GET_SERVANT(temp%d);\n",i,i);
      add_to_sig(result,buf,&currPos);
    }
      
    if(
       IsArray(currentFunction->ArgTypes[i])
       && !IsString(currentFunction->ArgTypes[i])
       && !IsClass(currentFunction->ArgTypes[i])
       && !IsVoid(currentFunction->ArgTypes[i])
       ) {
      sprintf(buf,"    CORBA::ULong j_temp%d;\n",i);
      add_to_sig(result,buf,&currPos);
      sprintf(buf,"    CORBA::ULong l_temp%d = temp%d.length();\n",i,i);
      add_to_sig(result,buf,&currPos);
      add_to_sig(result,"    ",&currPos);	      
      
      if(IsFloat(currentFunction->ArgTypes[i]))
	add_to_sig(result,"float",&currPos);
      
      if(IsDouble(currentFunction->ArgTypes[i]))
	add_to_sig(result,"double",&currPos);
      
      if(IsvtkIdType(currentFunction->ArgTypes[i])) {
	add_to_sig(result,"vtkIdType",&currPos);
      } else {
	  if(IsShort(currentFunction->ArgTypes[i])) {
	    add_to_sig(result,"int",&currPos);
	  }
      }
      
      if(IsLong(currentFunction->ArgTypes[i]))
	add_to_sig(result,"long",&currPos);
      
      sprintf(buf,"* a_temp%d = new ",i);
      add_to_sig(result,buf,&currPos);

      if(IsFloat(currentFunction->ArgTypes[i]))
	add_to_sig(result,"float",&currPos);
      
      if(IsDouble(currentFunction->ArgTypes[i]))
	add_to_sig(result,"double",&currPos);
      
      if(IsvtkIdType(currentFunction->ArgTypes[i])) {
	add_to_sig(result,"vtkIdType",&currPos);
      } else {
	if(IsShort(currentFunction->ArgTypes[i])) {
	  add_to_sig(result,"int",&currPos);
	}
      }

      if(IsLong(currentFunction->ArgTypes[i]))
	add_to_sig(result,"long",&currPos);
      
      sprintf(buf,"[l_temp%d];\n",i);
      add_to_sig(result,buf,&currPos);
      
      sprintf(buf,"    for(j_temp%d=0;j_temp%d<l_temp%d;j_temp%d++) {\n",i,i,i,i);
      add_to_sig(result,buf,&currPos);
      
      sprintf(buf,"      a_temp%d[j_temp%d]=temp%d[j_temp%d];\n",i,i,i,i);
      add_to_sig(result,buf,&currPos);
      
      add_to_sig(result,"    }\n",&currPos);
    }

    if(IsString(currentFunction->ArgTypes[i])) {
      sprintf(buf,"    char *c_temp%d = CORBA::string_dup(temp%d);\n",i,i);
      add_to_sig(result,buf,&currPos);
    }

    if(IsVoid(currentFunction->ArgTypes[i])) {
      sprintf(buf,"    long v_temp%d;\n",i);
      add_to_sig(result,buf,&currPos);
      
      sprintf(buf,"    temp%d.operator>>=(v_temp%d);\n",i,i);
      add_to_sig(result,buf,&currPos);
    }
  }  
  add_to_sig(result,"    ",&currPos);

  if(IsArray(currentFunction->ReturnType) && !IsClass(currentFunction->ReturnType) && !IsString(currentFunction->ReturnType)) {
    add_to_sig(result,"CORBA::ULong i_ret;\n",&currPos); 
    add_to_sig(result,"    PARAVIS::",&currPos); 
    AddTypeArray(currentFunction->ReturnType,result,&currPos);
    add_to_sig(result,"_var s_ret = new ",&currPos);
    AddTypeArray(currentFunction->ReturnType,result,&currPos);
    add_to_sig(result,"();\n",&currPos);
    sprintf(buf,"    s_ret->length(%d);\n",currentFunction->HintSize);
    add_to_sig(result,buf,&currPos);
    add_to_sig(result,"    ",&currPos); 
  }
  
  if(IsFloat(currentFunction->ReturnType)) {
    if(IsArray(currentFunction->ReturnType)) {
      add_to_sig(result,"float* a_ret = ",&currPos);
    } else {
      add_to_sig(result,"CORBA::Float ret = ",&currPos);
    }
  }
  
  if(IsDouble(currentFunction->ReturnType)) {
    if(IsArray(currentFunction->ReturnType)) {
      add_to_sig(result,"double* a_ret = ",&currPos);
    } else {
      add_to_sig(result,"CORBA::Double ret = ",&currPos);
    }
  }
  
  if(IsShort(currentFunction->ReturnType)) {
    if(IsArray(currentFunction->ReturnType)) {
      if(IsvtkIdType(currentFunction->ReturnType)) {
	add_to_sig(result,"vtkIdType",&currPos);
      } else {
	add_to_sig(result,"int",&currPos);
      }
      add_to_sig(result,"* a_ret = ",&currPos);
    } else {
      add_to_sig(result,"CORBA::Short ret = ",&currPos);
    }
  }
  
  if(IsLong(currentFunction->ReturnType)) {
    if(IsArray(currentFunction->ReturnType)) {
      add_to_sig(result,"long* a_ret = ",&currPos);
    } else {
      add_to_sig(result,"CORBA::Long ret = ",&currPos);
    }
  }
  
  if(IsChar(currentFunction->ReturnType)) {
    if(IsString(currentFunction->ReturnType)) {
      add_to_sig(result,"char * ret = CORBA::string_dup(\"\");\n",&currPos);
      add_to_sig(result,"    const char * cret = ",&currPos);
    } else {
      add_to_sig(result,"CORBA::Char ret = ",&currPos);	    
    }
  }

  if(IsBoolean(currentFunction->ReturnType)) {
    add_to_sig(result,"CORBA::Boolean ret = ",&currPos);
  }
  
  if(IsVoidPtr(currentFunction->ReturnType)) {
    add_to_sig(result,"void * v_ret = ",&currPos);
  }
  
  if(IsClass(currentFunction->ReturnType)) {
    add_to_sig(result,"::",&currPos);
    add_to_sig(result,currentFunction->ReturnClass,&currPos);
    add_to_sig(result,"* a",&currPos);
    add_to_sig(result,currentFunction->ReturnClass,&currPos);
    add_to_sig(result," = ",&currPos);
  }
  if(IsVoid(currentFunction->ReturnType)) {
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
  add_to_sig(result,data->ClassName,&currPos);	      
  add_to_sig(result,"*)",&currPos);
  add_to_sig(result,"getVTKObject()\n",&currPos);
  
  for (i = 0; i < currentFunction->NumberOfArguments; i++) {
    add_to_sig(result,"      , ",&currPos);
    
    //if(IsClass(currentFunction->ArgTypes[i]) && IsPtr(currentFunction->ArgTypes[i])) {
    //add_to_sig(result,"*(",&currPos);
    //}
    
    if(IsClass(currentFunction->ArgTypes[i])) {
      sprintf(buf,"(i_temp%d != NULL)?dynamic_cast< ::%s*>(i_temp%d->getVTKObject()):NULL",i,currentFunction->ArgClasses[i],i);
    } else {
      if(
	 IsArray(currentFunction->ArgTypes[i])
	 && !IsString(currentFunction->ArgTypes[i])
	 && !IsVoid(currentFunction->ArgTypes[i])
	 ) {
	sprintf(buf,"a_temp%d",i);
      } else {
	if(IsVoidPtr(currentFunction->ArgTypes[i])) {
	  sprintf(buf,"(void*)v_temp%d",i);
	} else {
	  if(IsString(currentFunction->ArgTypes[i])) {
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
  if(!IsVoid(currentFunction->ReturnType)) {
    add_to_sig(result,":",&currPos);
    if(IsClass(currentFunction->ReturnType) || IsString(currentFunction->ReturnType) || IsPtr(currentFunction->ReturnType) || IsArray(currentFunction->ReturnType))
      add_to_sig(result,"NULL",&currPos);
    else
      add_to_sig(result,"0",&currPos);
  }
  add_to_sig(result,";\n",&currPos);
  if(IsString(currentFunction->ReturnType)) {
    add_to_sig(result,"    if(cret!=NULL) ret=CORBA::string_dup(cret);\n",&currPos);
  }
  
  if(IsClass(currentFunction->ReturnType)) {
    add_to_sig(result,"    if(a",&currPos);
    add_to_sig(result,currentFunction->ReturnClass,&currPos);	      
    add_to_sig(result," == NULL) {\n",&currPos);
    add_to_sig(result,"      return PARAVIS::",&currPos);	      
    add_to_sig(result,currentFunction->ReturnClass,&currPos);	      
    add_to_sig(result,"::_nil();\n",&currPos);	      
    add_to_sig(result,"    }\n",&currPos);	      
    add_to_sig(result,"    ",&currPos);	      
    add_to_sig(result,"PARAVIS_Base_i* aPtr = ::CreateInstance(a",&currPos);	      
    add_to_sig(result,currentFunction->ReturnClass,&currPos);	      
    add_to_sig(result,", a",&currPos);	      
    add_to_sig(result,currentFunction->ReturnClass,&currPos);	      
    add_to_sig(result,"->GetClassName());\n",&currPos);
    add_to_sig(result,"    aPtr->Init(a",&currPos);	      
    add_to_sig(result,currentFunction->ReturnClass,&currPos);	      
    add_to_sig(result,");\n",&currPos);	      
  }
    
  for (i = 0; i < currentFunction->NumberOfArguments; i++) {
    if(
       IsArray(currentFunction->ArgTypes[i])
       && !IsVoid(currentFunction->ArgTypes[i])
       && !IsString(currentFunction->ArgTypes[i])
       && !IsClass(currentFunction->ArgTypes[i])
       ) {
      if (!IsIn(currentFunction->ArgTypes[i])) {
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

  if (IsVoid(currentFunction->ReturnType) && !IsVoidPtr(currentFunction->ReturnType)) {
    add_to_sig(result,"    return;\n",&currPos); 
  } else {
    if(IsClass(currentFunction->ReturnType)) {
      add_to_sig(result,"    return aPtr->_this();\n",&currPos);
    } else {
      if(IsVoid(currentFunction->ReturnType)) {
	add_to_sig(result,"    CORBA::Any* ret = new CORBA::Any;\n",&currPos);
	add_to_sig(result,"    (*ret) <<= v_ret;\n",&currPos);
	add_to_sig(result,"    return ret;\n",&currPos); 
      } else {
	if(IsArray(currentFunction->ReturnType) && !IsString(currentFunction->ReturnType)) {
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

  if (IsVoid(currentFunction->ReturnType)&& !IsVoidPtr(currentFunction->ReturnType)) {
    add_to_sig(result,"    return;\n",&currPos);
  } else{
    if(IsString(currentFunction->ReturnType)) {
      add_to_sig(result,"    return CORBA::string_dup(\"\");\n",&currPos);
    } else {
      if(IsClass(currentFunction->ReturnType)) {
	add_to_sig(result,"    return PARAVIS::",&currPos);
	add_to_sig(result,currentFunction->ReturnClass,&currPos);
	add_to_sig(result,"::_nil();\n",&currPos);
      } else {
	if(IsArray(currentFunction->ReturnType) && !IsVoid(currentFunction->ReturnType)) {
	  add_to_sig(result,"    PARAVIS::",&currPos);
	  AddTypeArray(currentFunction->ReturnType,result,&currPos);
	  add_to_sig(result,"_var s_ret = new ",&currPos);
	  AddTypeArray(currentFunction->ReturnType,result,&currPos);
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
      currentFunction->Signature = realloc(currentFunction->Signature,
					   (size_t)(currPos_sig+1));
      strcpy(currentFunction->Signature,result);

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
	currentFunction->Signature[0]='\0';
	return;
      }
    }
  }

  currentFunction->Signature = realloc(currentFunction->Signature,
                                       (size_t)(currPos+1));
  strcpy(currentFunction->Signature,result);
}

void outputFunction2(FILE *fp, FileInfo *data)
{
  int i, j, k, is_static, is_vtkobject, fnum, occ, backnum, goto_used;
  int all_legacy;
  FunctionInfo *theFunc;
  FunctionInfo *backFunc;
  char *theName;
  int theType;
  char *backName;
  int backType;
  char static num[8];
  //int isSMObject = 0;
  int found = 0;

#if defined(IDL_I_HH)
  fprintf(fp,"#include \"SALOMEconfig.h\"\n");
  fprintf(fp,"#include CORBA_SERVER_HEADER(PARAVIS_Gen_%s)\n",data->ClassName);
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

  fprintf(fp,"\nclass %s;\n",data->ClassName);
  fprintf(fp,"\nnamespace PARAVIS\n{\n\n");
#elif defined(IDL_I_CC)
  fprintf(fp,"#include \"SALOME_GenericObj_i.hh\"\n");
  fprintf(fp,"#include \"PARAVIS_Gen_%s_i.hh\"\n",data->ClassName);
  fprintf(fp,"#include \"PV_Tools.h\"\n");
  fprintf(fp,"#include \"SALOME_Event.h\"\n");
  fprintf(fp,"#include <%s.h>\n",data->ClassName);
#else
  fprintf(fp,"#include \"PARAVIS_Gen.idl\"\n");
  fprintf(fp,"#include \"PARAVIS_Gen_Types.idl\"\n");

  for(i=0;i<data->NumberOfSuperClasses;i++)
  {
    fprintf(fp,"#include \"PARAVIS_Gen_%s.idl\"\n",data->SuperClasses[i]);
  }

  fprintf(fp,"\nmodule PARAVIS\n{\n\n");
#endif

  is_vtkobject = ((strcmp(data->ClassName,"vtkObjectBase") == 0) || 
                  (data->NumberOfSuperClasses != 0));

  for(i = 0; i < data->NumberOfSuperClasses; i++) {
    read_class_functions(data->SuperClasses[i],data->ClassName,fp);
  }

  /* create a idl signature for each method */
  for (fnum = 0; fnum < numberOfWrappedFunctions; fnum++)
    {

    theFunc = wrappedFunctions[fnum];
    currentFunction = theFunc;

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

    for (i = theFunc->NumberOfArguments; i >= 0; i--)
      {
	if (i==0)/* return type */
	  {
	    theType = theFunc->ReturnType;
	    theName = theFunc->ReturnClass;
	  }
	else /* arg type */
	  {
	    theType = theFunc->ArgTypes[i-1];
	    theName = theFunc->ArgClasses[i-1];
	  }
	/* check for object types */
	if ((theType % 0x1000 == 0x309)||
	    (theType % 0x1000 == 0x109))
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
		for (j = backFunc->NumberOfArguments; j >= k; j--)
		  {
		    if (j==0) /* return type */
		      {
			backType = backFunc->ReturnType;
			backName = backFunc->ReturnClass;
		      }
		    else /* arg type */
		      {
			backType = backFunc->ArgTypes[j-1];
			backName = backFunc->ArgClasses[j-1];
		      }
		    if (((backType % 0x1000 == 0x309)||
			 (backType % 0x1000 == 0x109)))
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
	    if (backnum < 0 && strcmp(data->ClassName,theName) != 0)
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
  fprintf(fp,"\n    class %s_i : public virtual POA_PARAVIS::%s, public virtual PARAVIS::PARAVIS_Base_i",data->ClassName,data->ClassName);
  //for(i = 0; i < data->NumberOfSuperClasses; i++) {
  //  fprintf(fp,", public virtual %s_i",data->SuperClasses[i]);
  //}
  //fprintf(fp,", public virtual SALOME::GenericObj_i");
  fprintf(fp," {");
  fprintf(fp,"\n    public:\n");
  fprintf(fp,"\n        %s_i();\n",data->ClassName);
#elif defined(IDL_I_CC)
  fprintf(fp,"extern PARAVIS::PARAVIS_Base_i* CreateInstance(::vtkObjectBase* Inst, const QString&);\n");
  fprintf(fp,"\nnamespace PARAVIS\n{\n");
  fprintf(fp,"typedef %s_i current_inderface;\n",data->ClassName);
  fprintf(fp,"#define CreateEventName(Function) Event%s ##Function\n",data->ClassName);
  fprintf(fp,"%s_i::%s_i() {\n",data->ClassName,data->ClassName);
  fprintf(fp,"    Init(::%s::New());\n",data->ClassName);
  fprintf(fp,"}\n");
  fprintf(fp,"\n");
#else
  fprintf(fp,"\n    interface %s : PARAVIS_Base",data->ClassName);
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

void outputFunction(FILE *fp, FileInfo *data)
{
  int i;
  int args_ok = 1;
 
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
  
  /* check to see if we can handle the args */
  for (i = 0; i < currentFunction->NumberOfArguments; i++)
    {
    if (currentFunction->ArgTypes[i] % 0x1000 == 9) args_ok = 0;
    if ((currentFunction->ArgTypes[i] % 0x10) == 8) args_ok = 0;
    if (((currentFunction->ArgTypes[i] % 0x1000)/0x100 != 0x3)&&
        (currentFunction->ArgTypes[i] % 0x1000 != 0x109)&&
        ((currentFunction->ArgTypes[i] % 0x1000)/0x100)) args_ok = 0;
    if (currentFunction->ArgTypes[i] % 0x1000 == 0x313) args_ok = 0;
    if (currentFunction->ArgTypes[i] % 0x1000 == 0x314) args_ok = 0;
    if (currentFunction->ArgTypes[i] % 0x1000 == 0x31A) args_ok = 0;
    if (currentFunction->ArgTypes[i] % 0x1000 == 0x31B) args_ok = 0;
    if (currentFunction->ArgTypes[i] % 0x1000 == 0x31C) args_ok = 0;
    if (currentFunction->ArgTypes[i] % 0x1000 == 0x315) args_ok = 0;
    if (currentFunction->ArgTypes[i] % 0x1000 == 0x316) args_ok = 0;
    }
  if ((currentFunction->ReturnType % 0x10) == 0x8) args_ok = 0;
  if (currentFunction->ReturnType % 0x1000 == 0x9) args_ok = 0;
  if (((currentFunction->ReturnType % 0x1000)/0x100 != 0x3)&&
      (currentFunction->ReturnType % 0x1000 != 0x109)&&
      ((currentFunction->ReturnType % 0x1000)/0x100)) args_ok = 0;


  /* eliminate unsigned char * and unsigned short * */
  if (currentFunction->ReturnType % 0x1000 == 0x313) args_ok = 0;
  if (currentFunction->ReturnType % 0x1000 == 0x314) args_ok = 0;
  if (currentFunction->ReturnType % 0x1000 == 0x31A) args_ok = 0;
  if (currentFunction->ReturnType % 0x1000 == 0x31B) args_ok = 0;
  if (currentFunction->ReturnType % 0x1000 == 0x31C) args_ok = 0;
  if (currentFunction->ReturnType % 0x1000 == 0x315) args_ok = 0;
  if (currentFunction->ReturnType % 0x1000 == 0x316) args_ok = 0;

  if (currentFunction->NumberOfArguments && 
      (currentFunction->ArgTypes[0] == 0x5000)
      &&(currentFunction->NumberOfArguments != 0x1)) args_ok = 0;

  /* make sure we have all the info we need for array arguments in */
  for (i = 0; i < currentFunction->NumberOfArguments; i++)
    {
    if (((currentFunction->ArgTypes[i] % 0x1000)/0x100 == 0x3)&&
        (currentFunction->ArgCounts[i] <= 0)&&
        (currentFunction->ArgTypes[i] % 0x1000 != 0x309)&&
        (currentFunction->ArgTypes[i] % 0x1000 != 0x303)&&
        (currentFunction->ArgTypes[i] % 0x1000 != 0x302)) args_ok = 0;
    }

  /* if we need a return type hint make sure we have one */
  switch (currentFunction->ReturnType % 0x1000)
    {
    case 0x301: case 0x307: case 0x30A: case 0x30B: case 0x30C: case 0x30D: case 0x30E:
    case 0x304: case 0x305: case 0x306:
      args_ok = currentFunction->HaveHint;
      break;
    }
  
  /* make sure it isn't a Delete or New function */
  if (!strcmp("Delete",currentFunction->Name) ||
      !strcmp("New",currentFunction->Name))
    {
    args_ok = 0;
    }
  
  /* check for New() function */
  if (!strcmp("New",currentFunction->Name) &&
      currentFunction->NumberOfArguments == 0)
    {
    class_has_new = 1;
    }

  if (currentFunction->IsPublic && args_ok && 
      strcmp(data->ClassName,currentFunction->Name) &&
      strcmp(data->ClassName, currentFunction->Name + 1))
    {
    wrappedFunctions[numberOfWrappedFunctions] = currentFunction;
    numberOfWrappedFunctions++;
    }

  return;
}

/* print the parsed structures */
void vtkParseOutput(FILE *fp, FileInfo *data)
{
  int i;

#if defined(IDL_I_HH)
  fprintf(fp, "// idl wrapper interface for %s object implementation\n//\n", data->ClassName);
#elif defined(IDL_I_CC)
  fprintf(fp, "// idl wrapper implementation for %s object\n//\n", data->ClassName);
#else
  fprintf(fp, "// idl wrapper for %s object\n//\n", data->ClassName);
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
  fprintf(fp,"#ifndef PARAVIS_Gen_%s_i_HeaderFile\n",data->ClassName);
  fprintf(fp,"#define PARAVIS_Gen_%s_i_HeaderFile\n",data->ClassName);
  fprintf(fp,"\n");
#elif defined(IDL_I_CC)
#else
  fprintf(fp,"#ifndef __PARAVIS_Gen_%s__\n",data->ClassName);
  fprintf(fp,"#define __PARAVIS_Gen_%s__\n",data->ClassName);
  fprintf(fp,"\n");
#endif

  /* insert function handling code here */
  for (i = 0; i < data->NumberOfFunctions; i++)
    {
      currentFunction = data->Functions + i;
      outputFunction(fp, data);
    }

  if (data->NumberOfSuperClasses || !data->IsAbstract)
    {
    outputFunction2(fp, data);
    }

#if defined(IDL_I_HH)
  fprintf(fp,"\n#endif\n");
#elif defined(IDL_I_CC)
#else
  fprintf(fp,"\n#endif\n");
#endif
  return;
}
