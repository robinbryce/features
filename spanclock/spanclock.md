
High resolution inverval timing facility.
-----------------------------------------

A facility for measuring time intervals with as much resolution as possible
on the current platform. The api is designed for use in run time
performance analisis where "stop watch and report" like behaviour is
desired.

Implementation Details
----------------------

The windows implementation is informed primarily by

"Acquiring high-resolution time stamps"

http://msdn.microsoft.com/en-us/library/windows/desktop/dn553408%28v=vs.85%29.aspx

And the api documentation for QueryPerformanceCounter.

api
---

* On the assumption that the majority of implementations require smallish sized
  data structs for spanc_val, I chose to 'pass by value' for api's which
  *manipulate* counter values. It avoids pointer aliasing issues and gives the
  compiler more latitude to re-arrange stuff.
* Where possible, return signature makes the api friendly to composition of operations.


Unresolved
----------

http://support.microsoft.com/?id=274323

Determining the implementation at build time has issues:

1. linux kernel / libc mismatch may mean CLOCK id's are available at build
  'configure' (HAVE_CLOCK_MONOTONIC) but are unavailable at run time.

1. Can't really be resolved without some indirection in the api. I think it's best to enable
all implementations which are valid at configure time. This doesn't prevent the api from
underpining a more sophisticated implementation but doesn't penalize the majority of users
with a more cluttered api.

ERRORs
------

The possiblity of esoteric unlikely error values from system library calls are
ignored. We assume that in this case the "read" values will be clearly invalid
and the performance test re-run. For example the gettimeofday implementation
ignores the return code from gettimeofday.

spanclock_ xxx is designed to be used during development to narrow down
performance issues.

We assume the api user prefers api simplicity over any attempts at hand holding.


ERRORS - Windows
----------------

The underlying implementation on windows will not return non zero for any
api. ie "no errors" is a completely fair assumption always.

Counter rollover - windows
--------------------------

On windows, the counter may run for 100 years from *BOOT* without rolling over.


spanclock_accum
---------------

Accumulates the amount of time spent within two calls to hirestime_accum

Assuming ctr refers to the same address, each 'odd' call results in ctr
being set to the time spent "outside" the interval.  On each "even" call,
ctr is left with the number of ticks spent within the two calls.

spanc_ctr acc={0};
for (;;){

 spanclock_accum(&acc); // A
 
 // do stuff of interest

 spanclock_accum(&acc); // B

 // do other stuff
}

* if acc is examined after A but before B, it represent the time to reach the
  for(;;) loop + the time spent doing "other stuff"

* If it is examined after B it represents the TOTAL time doing
  "stuff of interest" (for all iterations). Also if we ever break out of the 
  for loop, acc will be left with the total time spent doing stuff of interest.




