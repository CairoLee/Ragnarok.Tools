--Mirror copy from left hand to right hand
--左側のactを右側にミラーコピー

if (GetNumAct() % 8) ~= 0 then
  return;
end;

for i = 0, GetNumAct()/8 - 1 do
  --1st. simple copy
  CopyAct(i*8+1, i*8+7);
  CopyAct(i*8+2, i*8+6);
  CopyAct(i*8+3, i*8+5);

  --2nd. change position & mirror
  for AID = i*8+5, i*8+7 do
    for FID = 0, GetNumFrame(AID)-1 do
      for PID = 0, GetNumPat(AID, FID)-1 do
        XOffs, YOffs = GetXYOffs(AID, FID, PID);
        Mir = IsMirror(AID, FID, PID);
        XOffs = -1 * XOffs;
        Mir = not Mir;
        SetXYOffs(AID, FID, PID, XOffs, YOffs);
        SetMirror(AID, FID, PID, Mir);
      end;
      XOffs, YOffs = GetExtXYOffs(AID, FID);
      XOffs = -1 * XOffs;
      SetExtXYOffs(AID, FID, XOffs, YOffs);
    end;
  end;
end;
