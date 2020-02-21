with System.Machine_Code; use System.Machine_Code;

package body Fences is

  procedure Memory_Fence(F: Fences) is
  begin
    case F is
      when SS =>
        Asm("sfence", Volatile => True);
      when LL =>
        Asm("lfence", Volatile => True);
      when LL_SS =>
        Asm("lfence", Volatile => True);
        Asm("sfence", Volatile => True);
      when others =>
        Asm("mfence", Volatile => True);
    end case;
  end Memory_Fence;
end Fences;
