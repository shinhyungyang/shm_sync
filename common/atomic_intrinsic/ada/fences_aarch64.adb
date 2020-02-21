with System.Machine_Code; use System.Machine_Code;

package body Fences is

  procedure Memory_Fence(F: Fences) is
  begin
    case F is
      when SS =>
        Asm("dmb ishst", Volatile => True);
      when LL =>
        Asm("dmb ishld", Volatile => True);
      when LL_SS =>
        Asm("dmb ishld", Volatile => True);
        Asm("dmb ishst", Volatile => True);
      when LS =>
        Asm("dmb ishld", Volatile => True);
      when LL_LS =>
        Asm("dmb ishld", Volatile => True);
      when LL_LS_SS =>
        Asm("dmb ishld", Volatile => True);
        Asm("dmb ishst", Volatile => True);
      when others =>
        Asm("dmb ish", Volatile => True);
    end case;
  end Memory_Fence;
end Fences;

