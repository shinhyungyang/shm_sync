package body PO_FIFO_Lock is
   protected body FIFO_Lock is
     entry Lock when Guard is
     begin
       Guard := False;
     end Lock;
     procedure Unlock is
     begin
       Guard := True;
     end Unlock;
   end FIFO_Lock;
end PO_FIFO_Lock;
