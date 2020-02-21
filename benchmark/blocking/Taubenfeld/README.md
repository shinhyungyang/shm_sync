## Fair TAS-lock

This benchmark constitutes the implementation of a *fair TAS lock* with the
help of G. Taubenfeld's Fair Synchronization Algorithm [1].

The Fair Synchronization Algorithm provides a way to transfer a non-blocking or
wait-free data structure without fairness guarantees into a similar
data structure satisfying a strong fairness requirement. The fairness
requirement prevents a thread from accessing a resource twice while another
thread is waiting.

In the context of this benchmark, the *fair section* of the Fair
Synchronization Algorithm is used as the critical section of a TAS-lock. The
result of this combination is a mutual exclusion lock with the fairness
guarantees of the Fair Synchronization Algorithm (see [1], Section 5).

The fairness property of the Fair Synchronization Algorithm is weaker than the
FIFO-fairness guaranteed by FIFO locks. As proven in [1], the FIFO fairness
requirement, the progress requirement and concurrency requirement cannot be
mutually satisfied. Note that locks do not have a concurrency requirement and
hence can support FIFO-fairness.

### References
[1] Gadi Taubenfeld, Fair synchronization, J. Parallel Distrib. Comput.,
    Vol. 97, pp. 1-10, 2016, ([pdf](http://www.faculty.idc.ac.il/gadi/MyPapers/2016T-FullVersionFairSynchronization.pdf)).
