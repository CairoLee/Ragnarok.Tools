--Set gradation
--           RRGGBB
startcol = 0xff0000
endcol   = 0xffbdbd
startpid = 0x10
endpid   = 0x17


sr = (startcol / 65536) % 0x100
sg = (startcol / 256) % 0x100
sb = startcol % 0x100
sh, ss, sv = RGB2HSV(sr, sg, sb)

er = (endcol / 65536) % 0x100
eg = (endcol / 256) % 0x100
eb = endcol % 0x100
eh, es, ev = RGB2HSV(er, eg, eb)

dh = (eh - sh) / (endpid - startpid)
ds = (es - ss) / (endpid - startpid)
dv = (ev - sv) / (endpid - startpid)

for i = 0, (endpid-startpid)-1 do
  r, g, b = HSV2RGB(sh + dh*i, ss + ds*i, sv + dv*i)
  SetPalCol(startpid + i, r*65536+g*256+b)
end
