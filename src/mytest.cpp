#include "stdafx.h"

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

#include "mytest.h"

BOOL GetDriveGeometry(DISK_GEOMETRY *pdg)
{
  HANDLE hDevice;               // handle to the drive to be examined 
  BOOL bResult;                 // results flag
  DWORD junk;                   // discard results

  hDevice = CreateFile("\\\\.\\PhysicalDrive0",  // drive to open
                    0,                // no access to the drive
                    FILE_SHARE_READ | // share mode
                    FILE_SHARE_WRITE, 
                    NULL,             // default security attributes
                    OPEN_EXISTING,    // disposition
                    0,                // file attributes
                    NULL);            // do not copy file attributes

  if (hDevice == INVALID_HANDLE_VALUE) // cannot open the drive
  {
    return (FALSE);
  }

  bResult = DeviceIoControl(hDevice,  // device to be queried
      IOCTL_DISK_GET_DRIVE_GEOMETRY,  // operation to perform
                             NULL, 0, // no input buffer
                            pdg, sizeof(*pdg),     // output buffer
                            &junk,                 // # bytes returned
                            (LPOVERLAPPED) NULL);  // synchronous I/O

  CloseHandle(hDevice);

  return (bResult);
}

int mytest()
{
  DISK_GEOMETRY pdg;            // disk drive geometry structure
  BOOL bResult;                 // generic results flag
  ULONGLONG DiskSize;           // size of the drive, in bytes

  bResult = GetDriveGeometry (&pdg);

  if (bResult) 
  {
    printf("Cylinders = %I64d\n", pdg.Cylinders);
    printf("Tracks/cylinder = %ld\n", (ULONG) pdg.TracksPerCylinder);
    printf("Sectors/track = %ld\n", (ULONG) pdg.SectorsPerTrack);
    printf("Bytes/sector = %ld\n", (ULONG) pdg.BytesPerSector);

    DiskSize = pdg.Cylinders.QuadPart * (ULONG)pdg.TracksPerCylinder *
      (ULONG)pdg.SectorsPerTrack * (ULONG)pdg.BytesPerSector;
    printf("Disk size = %I64d (Bytes) = %I64d (Gb)\n", DiskSize,
           DiskSize / (1024 * 1024 * 1024));
  } 
  else 
  {
    printf ("GetDriveGeometry failed. Error %ld.\n", GetLastError ());
  }

  return ((int)bResult);
}