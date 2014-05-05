//======================================================================
//
// ClockRes
// By Mark Russinovich
// SysInternals - www.sysinternals.com
//
// This program is *very* simple. Just use GetSystemTimeAdjustment 
// to query the system clock interval.
// 
//======================================================================
#include <windows.h>



//----------------------------------------------------------------------
//
// Main
//
//----------------------------------------------------------------------
void main( int argc, char *argv[] )
{
    DWORD adjustment, clockInterval;
    BOOL  adjustmentDisabled;

    printf("\nClockRes - View the system clock resolution\n");
    printf("By Mark Russinovich\n");
    printf("SysInternals - www.sysinternals.com\n\n");
    GetSystemTimeAdjustment( &adjustment,
                             &clockInterval,
                             &adjustmentDisabled );
    printf( "The system clock interval is %.06f ms\n",
            (float) clockInterval / 10000 );

}
