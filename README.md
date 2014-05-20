features
========

A suite of feature implementations. This is a collection of relatively
indepenent program features. Each feature aims to represent current best
practice for that feature. Some implementations will be more succesful in that
aim than others.

Goals
=====

1 For each feature to provide "example" quality documentation and references.
2 For it to be possible to selectively enable or copy each feature independent
  of the others.


In the pursuit of 2., if the resulting api's are to perverse, "features" will
provide 'common' or stub facilities through a single dependency. Idealy, simply
a header dependency.

Requirements
============

- SHALL NOT force a particular memory allocation strategy (eg, malloc/free)
- SHALL NOT introduce global state.
- MAY require that the caller provide "upcalls" to conventional facilities like
  malloc/free etc.,

