// Persistent.cpp: implementation of the TPersistent class.
// by David, 2006.7
//
//////////////////////////////////////////////////////////////////////

#include <afx.h>
#include "Persistent.h"

IMPLEMENT_DYNAMIC(TPersistent, CObject)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TPersistent::TPersistent()
{

}

TPersistent::~TPersistent()
{

}

void TPersistent::AssignTo(TPersistent *Dest)
{
}

void TPersistent::Assign(TPersistent *Source)
{
	if(Source!=NULL)
		Source->AssignTo(this);
}
