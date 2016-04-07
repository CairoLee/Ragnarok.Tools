--n”{‚µ‚Ü‚·
--mag‚ğ•ÏX‚·‚é‚ÆŠg‘å—¦‚ª•Ï‚í‚è‚Ü‚·
--When "mag" is changed, magnifying power changes.

mag = 3.0

for AID = 0, GetNumAct()-1 do
  for FID = 0, GetNumFrame(AID)-1 do
    for PID = 0, GetNumPat(AID, FID)-1 do
      XOffs, YOffs = GetXYOffs(AID, FID, PID)
      SetXYOffs(AID, FID, PID, XOffs * mag, YOffs * mag)
      ChangeMagnify(AID, FID, PID, mag, mag)
    end
  end
end
