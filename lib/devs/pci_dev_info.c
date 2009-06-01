// The code below is provided as a sample of one way to access the Class Code
// table information.  Feel free to use it as-is, or modify it to suit your
// needs.
//
// Code created by William H. Avery III (aka Highlander) at Altitech

///////////////////////////////////////////////////////////////////////////////
//
// Lookup descriptions based on three separate inputs
//
// Args: baseid   - PCI base class code
//       subid    - PCI sub class code
//       progif   - PCI prog IF code
//       basedesc - Returns base class description
//       subdesc  - Returns sub class description
//       progdesc - Returns prog IF description
//
// Returns
//   nothing (all "desc" strings will contain PCI descriptions or "")
//
///////////////////////////////////////////////////////////////////////////////

void 
GetPCIClass3 (
  unsigned char		baseid,
  unsigned char		subid,
  unsigned char		progid,
  char **		basedesc,
  char **		subdesc,
  char **		progdesc
)
{
	int	i ;

	*basedesc = *subdesc = *progdesc = "" ;

	for ( i=0 ; i < PCI_CLASSCODETABLE_LEN ; ++i )
	{
		if ( PciClassCodeTable[i].BaseClass == baseid )
		{
			if ( !(**basedesc) )
				*basedesc = PciClassCodeTable[i].BaseDesc ;
			if ( PciClassCodeTable[i].SubClass == subid )
			{
				if ( !(**subdesc) )
					*subdesc = PciClassCodeTable[i].SubDesc ;
				if ( PciClassCodeTable[i].ProgIf == progid )
				{
					*progdesc = PciClassCodeTable[i].ProgDesc ;
					break ;
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
//  Lookup descriptions based on the contents of the full 32-bit Header
//  Class Code Register (including the Revision Code, which is ignored).
//
//////////////////////////////////////////////////////////////////////////

void
GetPCIClass1 (
  long		classcode,
  char **	base,
  char **	sub,
  char **	prog
)
{
	unsigned char	baseid, subid, progid ;

	baseid  = ((classcode >> 24) & 0xFF) ;
	subid  =  ((classcode >> 16) & 0xFF) ;
	progid  = ((classcode >>  8) & 0xFF) ;

	GetPCIClass3 (baseid,subid,progid,base,sub,prog) ;
}
