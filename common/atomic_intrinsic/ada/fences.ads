package Fences is
  pragma Preelaborate;

  type Fences is (SS, SL, SL_SS, LS, LS_SS, LS_SL, LS_SL_SS, LL, LL_SS, LL_SL, LL_SL_SS, LL_LS, LL_LS_SS, LL_LS_SL, LL_LS_SL_SS);

  procedure Memory_Fence(F: Fences) with Inline;

end Fences;
