set(BINDIR ${BINDIR}/blocking)

if (BUILD_ADA)
  file(MAKE_DIRECTORY ${BINDIR}/monitors)
  add_subdirectory(monitors)
  add_subdirectory(POLock)
  add_subdirectory(PO_FIFO_Lock)
  add_subdirectory(Ada_TAS_MPMC)
endif()

if (BUILD_CXX)
  add_subdirectory(ArrayLock)
  add_subdirectory(Taubenfeld)
  add_subdirectory(FilterLock)
  add_subdirectory(PetersonLock)
  add_subdirectory(TASLock)
  add_subdirectory(TATASLock)
  add_subdirectory(POSIXMutexLock)
  add_subdirectory(CLHQueueLock)
  add_subdirectory(C++11MutexLock)
  add_subdirectory(C++11MutexStack)
endif()
