#pragma hdrstop
#include "LCore/Object.h"

#ifndef _LCORE_ClassTypes_H_
#  include "LCore/ClassTypes.h"
#endif

#ifndef _LCORE_Programmers_H_
#  include "LCore/Programmers.h"
#endif

#ifndef _LCORE_HopClTypes_H_
#  include "LCore/HopClTypes.h"
#endif

 /* UNIX:@@:ClassProgID.cc:@@: */
/* MSDOS:@@:CLSPRGID.CPP:@@: */

const ProgrammerNum EricMHopper_0(0UL);
const ProgrammerNum WinterFire_0(2UL);
const ProgrammerNum StPaulSoftware_0(3UL);

const ClassIdent        Object::identifier(EricMHopper_0, ClassNum(0UL));
const ClassIdent ProgrammerNum::identifier(EricMHopper_0, ClassNum(1UL));
const ClassIdent      ClassNum::identifier(EricMHopper_0, ClassNum(2UL));
const ClassIdent    ClassIdent::identifier(EricMHopper_0, ClassNum(3UL));
const EH0_ClassIdent    EH_ClassNum::identifier(4UL);
const EH0_ClassIdent EH0_ClassIdent::identifier(5UL);

/*  This is a list of class #'s. Individual constants will be defined in
    seperate files to reduce the amount that is mandatorily linked into your
    program. The numbers here are here so there is one place to look to find
    and unused class #

Applicator		6UL
PrinterApplicator	7UL
Container		8UL
ForwardCursor		9UL
BiDirCursor		10UL
Predicate		11UL
Collection		12UL
InsertFwdCursor		13UL
InsertBDCursor		14UL
SLL_Node		15UL
SL_List			16UL
SLL_Cursor		17UL
DLL_Node		18UL
DL_List			19UL
DLL_Cursor		20UL
Protocol		21UL
Sortable		22UL
ReferenceCounting	23UL
Hashable		24UL
Key			25UL
Association		26UL
_StrRep			27UL
String			28UL
Array			29UL
ArrayCursor		30UL
SimpleArray		31UL
Date			32UL
DateConverter		33UL
BinaryDtConv		34UL
VerboseDtConv		35UL
MMDDYYDtConv		36UL
EH_ClassIdent		37UL
GenericAssoc		38UL
Dictionary		39UL

** End of class #'s */
