with Timer_Binding;

package body PO_Lock is
   protected body Lock is
     procedure CriticalSection (Uin_delay : in Integer) is
     begin
       Timer_Binding.Useful_Work_Ns(Uin_delay); -- Useful work (Nano second)
       null; -- Critical section code here...
     end CriticalSection;
   end Lock;
end PO_Lock;
