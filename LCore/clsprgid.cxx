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

const ClassIdent        Object::identifier(EricMHopper_0,
					   EH_ClassNum(EH_ClassNum::LCore,
						       0UL));
const ClassIdent ProgrammerNum::identifier(EricMHopper_0,
					   EH_ClassNum(EH_ClassNum::LCore,
						       1UL));
const ClassIdent      ClassNum::identifier(EricMHopper_0,
					   EH_ClassNum(EH_ClassNum::LCore,
						       2UL));
const ClassIdent    ClassIdent::identifier(EricMHopper_0,
					   EH_ClassNum(EH_ClassNum::LCore,
						       3UL));
const LCore_ClassIdent      EH_ClassNum::identifier(4UL);
const LCore_ClassIdent LCore_ClassIdent::identifier(5UL);

/*  This is a list of class #'s. Individual constants will be defined in
    seperate files to reduce the amount that is mandatorily linked into your
    program. The numbers here are here so there is one place to look to find
    and unused class #

Object			0UL
ProgrammerNum		1UL
ClassNum		2UL
ClassIdent		3UL
EH_ClassNum		4UL
LCore_ClassIdent	5UL
EH_ClassIdent		6UL
Protocol		7UL
ReferenceCounting	8UL

** End of class #'s */
