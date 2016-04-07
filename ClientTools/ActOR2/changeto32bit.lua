--change to 32bit fullcolor

for AID = 0, GetNumAct()-1 do
  for FID = 0, GetNumFrame(AID)-1 do
    for PID = 0, GetNumPat(AID, FID)-1 do
      SprNo = GetSprNo(AID, FID, PID)
      SetSprNoType(AID, FID, PID, SprNo, 1)
    end
  end
end
