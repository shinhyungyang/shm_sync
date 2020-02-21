package PO_Lock is
   protected type Lock is
     procedure CriticalSection (Uin_delay : in Integer);
   end Lock;
end PO_Lock;
