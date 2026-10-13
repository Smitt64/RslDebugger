/**********************************************************************
*        File Name:   xms.h              April 15,1994            
*       Programmer:   S. Kubrin
*      Description:   
*  Public routines:  
* Private routines:  
***********************************************************************/

#ifndef __XMS_H
#define __XMS_H

/*
          XMS.H
   XMS interface
*/

struct XMS_Move 
     {
      long      Length;          // 32-bit number of bytes to transfer
      unsigned  SourceHandle;    // Handle of source block
      long      SourceOffset;    // 32-bit offset into source 
      unsigned  DestHandle;      // Handle of destination block
      long      DestOffset;      // 32-bit offset into destination block
     };


#ifdef __cplusplus
extern "C" {
#endif

unsigned XMS_Installed(void);
//      THIS ROUTINE MUST BE CALLED BEFORE ANY OTHER ROUTINE
//      OR ALL THE OTHER ROUTINES WILL FAIL WITH ERROR CODE 0x80
//
// Returns:     1 if an XMS driver was found
//              0 if no driver was found

long     XMS_Version(void);
// Returns:     32 bit version, XMS version number in the low 16 bits,
//              Driver internal revision in the high 16 bits

long     XMS_QueryLargestFree(void);
// Returns:     >= 0L   Success, return is size of largest free block
//              < 0     Error code is in the top 8 bits of the return value

long     XMS_QueryTotalFree(void);
// Returns:     >= 0L   Success, return is total amount of free extended memory
//              < 0     Error code is in the top 8 bits of the return value

long     XMS_AllocateExtended(unsigned SizeK);
// Parameters:  SizeK is the size of the block requested in Kb
//
// Returns:     >= 0L   Success, the low word is the handle for the block
//              < 0     Error code is in the top 8 bits of the return value

long     XMS_FreeExtended(unsigned Handle);
// Parameters:  Handle is the handle corresponding to the extended memory
//              block to be freed
// 
// Returns:     0L      Success
//              < 0     Error code in top 8 bits

long     XMS_MoveExtended(struct XMS_Move *pMoveDescription);
// Parameters:  pMoveDescription is a pointer to a structure as defined
//              in the XMS.  XMS_Move is defined in XMS.H
// 
// Returns:     0L      Success
//              < 0     Error code in top 8 bits

long     XMS_LockExtended(unsigned Handle);
// Parameters:  Handle is the handle corresponding to the extended memory
//              block to be locked
//
// Returns:     > 0     Success, value is the physical address of the memory
//              < 0     Error code in top 8 bits
//
// NOTE:        This interface only allows 31 bits of physical address.

long     XMS_UnLockExtended(unsigned Handle);
// Parameters:  Handle is the handle corresponding to the extended memory
//              block to be unlocked
// 
// Returns:     0L      Success
//              < 0     Error code in top 8 bits

long     XMS_GetHandleLength(unsigned Handle);
// Parameters:  Handle is the handle corresponding to the extended memory
//              block for which the length is required
//
// Returns:     > 0     Success, the return value is the length of the
//                      block in Kb
//              < 0     Error code in top 8 bits

long     XMS_GetHandleInfo(unsigned Handle);
// Parameters:  Handle is the handle corresponding to the extended memory
//              block for which the information is required
//
// Returns:     > 0     Success, the low byte (bits 7:0) contains the
//                      number of free EMB handles in the system, bits 15:8 
//                      contain the lock count for the handle
//              < 0     Error code in top 8 bits

long     XMS_ReallocateExtended(unsigned Handle, unsigned NewSizeK);
// Parameters:  Handle is the handle which is to be reallocated
//              NewSizeK is the desired size in Kb
// 
// Returns:     0L      Success
//              < 0     Error code in top 8 bits

char     XMSERROR(long x);
// Parameters:  x is a return code from one of the XMS functions which
//              indicated an error
// 
// Returns:     The error code is returned as a 'char'


#ifdef __cplusplus
}
#endif

#define  XMSERROR(val)  (char)((val) >> 24)

#endif

/* <-- EOF --> */