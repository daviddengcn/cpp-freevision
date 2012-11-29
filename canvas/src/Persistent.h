// Persistent.h: interface for the TPersistent class.
// by David, 2002.4
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <afx.h>

#if !defined(_PERSISTENT_H_)
#define _PERSISTENT_H_

class TPersistent : public CObject
{
public:
	virtual void Assign(TPersistent* Source);
	TPersistent();
	virtual ~TPersistent();

	DECLARE_DYNAMIC(TPersistent);
protected:
	virtual void AssignTo(TPersistent* Dest);
};

#endif // !defined(_PERSISTENT_H_)
