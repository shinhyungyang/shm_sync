**Hazard pointers** is a memory management system proposed by Maged M. Michael,
which provides a solution for the ABA problem in lock-free data structures [^1].
[^1]: Maged M. Michael. Hazard Pointers: Safe Memory Reclamation for Lock-Free
      Objects. _IEEE Transactions on Parallel and Distributed Systems_,
      15(6):491-504, 2004.

Our implementation of **harzard pointers** is based on the second edition of
_C++ Concurrency in Action_ by Anthony Williams.
