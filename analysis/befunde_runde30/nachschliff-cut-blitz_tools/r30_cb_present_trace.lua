-- r30_cb_present_trace.lua — Runde 30 Nachschliff, Spur cut-blitz: DYNAMISCHER Beleg im Original
-- (PCSX-Redux, -interpreter -debugger; gestartet ueber pcsx_drive.py des Skills
-- re15-pcsx-watchpoint mit PCSX_LUA=<diese Datei>).
--
-- Frage: zeichnet die Present-Routine FUN_8002137c in dem Bild, in dem sie den Kamera-Apply
-- FUN_80021bbc ruft, noch die OT (DrawOTag) und wechselt sie den Puffer? Und setzt ausser dem
-- Apply jemand die Blickmatrix (FUN_80053ca4)?
--
-- Exec-Haltepunkte (Rueckruf gibt IMMER true zurueck — false LOESCHT den Haltepunkt):
--   0x8002137c  Present-Eintritt      -> vorigen Present abschliessen, neuen beginnen
--   0x80021558  jal FUN_80021bbc      -> Apply in diesem Present
--   0x8002157c  jal FUN_80043870      -> Hintergrund-LoadImage (Normalpfad)
--   0x800215bc  jal DrawOTag (1. von 3)
--   0x800215f8  sb v0,DAT_800aca34    -> Pufferwechsel
--   0x80021d2c  jal FUN_80013c50      -> BG von CD (im Apply)
--   0x80021e8c  jal FUN_80053ca4      -> Blickmatrix (im Apply)
--   0x80053ca4  Eintritt Blickmatrix  -> Aufrufer (ra) zaehlen
-- Ausgabe: eine Zeile je Present MIT Apply, danach alle 600 UI-Bilder eine Summenzeile.
-- Stoppmarke fuer pcsx_drive.py: ">>> SETTER" (deren Abbruchbedingung) nach MAX_APPLY Applies.
local band = bit.band
local OUT = os.getenv("PCSX_WATCH_LOG") or [[C:\workspace\git\reAi_v2\shots\r30_cb_present.txt]]
local MAX_APPLY = tonumber(os.getenv("R30_CB_MAX_APPLY") or "12")

local out, frames = nil, 0
local p = 0
local cur = nil
local st = { pres = 0, apply = 0, apply_draw = 0, apply_toggle = 0, apply_bgcopy = 0,
             apply_cam = 0, apply_cd = 0, norm = 0, norm_draw = 0, norm_toggle = 0 }
local camcalls = {}
local done = false

local function u8(m, a) return m[band(a, 0x1fffff)] end
local function s16(m, a)
  local o = band(a, 0x1fffff); local v = m[o] + m[o + 1] * 256
  if v >= 0x8000 then v = v - 0x10000 end; return v
end

local function abschliessen()
  if cur == nil then return end
  local m = PCSX.getMemPtr()
  local buf_nach = u8(m, 0x800aca34)
  st.pres = st.pres + 1
  if cur.apply then
    st.apply = st.apply + 1
    if cur.draw then st.apply_draw = st.apply_draw + 1 end
    if cur.toggle then st.apply_toggle = st.apply_toggle + 1 end
    if cur.bgcopy then st.apply_bgcopy = st.apply_bgcopy + 1 end
    if cur.cam then st.apply_cam = st.apply_cam + 1 end
    if cur.cd then st.apply_cd = st.apply_cd + 1 end
    out:write(string.format(
      "P%d APPLY cut %d->%d | DrawOTag=%s Pufferwechsel=%s BG-LoadImage=%s | Apply: CD-BG=%s Blickmatrix=%s | Puffer %d->%d\n",
      p, cur.fe4, s16(m, 0x800b0fe4), tostring(cur.draw), tostring(cur.toggle),
      tostring(cur.bgcopy), tostring(cur.cd), tostring(cur.cam), cur.buf, buf_nach))
    out:flush()
  else
    st.norm = st.norm + 1
    if cur.draw then st.norm_draw = st.norm_draw + 1 end
    if cur.toggle then st.norm_toggle = st.norm_toggle + 1 end
  end
end

local function hp(addr, name, fn)
  local ok = pcall(function()
    _G.__cb_bps[#_G.__cb_bps + 1] = PCSX.addBreakpoint(addr, 'Exec', 4, name, function()
      pcall(fn)
      return true
    end)
  end)
  out:write(string.format("Haltepunkt @0x%08x %-14s %s\n", addr, name, tostring(ok)))
end

function DrawImguiFrame()
  frames = frames + 1
  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    _G.__cb_bps = {}
    hp(0x8002137c, "present", function()
      abschliessen()
      local m = PCSX.getMemPtr()
      p = p + 1
      cur = { buf = u8(m, 0x800aca34), fe4 = s16(m, 0x800b0fe4) }
    end)
    hp(0x80021558, "apply", function() if cur then cur.apply = true end end)
    hp(0x8002157c, "bg_loadimage", function() if cur then cur.bgcopy = true end end)
    hp(0x800215bc, "drawotag", function() if cur then cur.draw = true end end)
    hp(0x800215f8, "toggle", function() if cur then cur.toggle = true end end)
    hp(0x80021d2c, "cd_bg", function() if cur then cur.cd = true end end)
    hp(0x80021e8c, "apply_cam", function() if cur then cur.cam = true end end)
    hp(0x80053ca4, "fun_80053ca4", function()
      local ra = PCSX.getRegisters().GPR.r[31]
      local k = string.format("%08x", tonumber(ra))
      camcalls[k] = (camcalls[k] or 0) + 1
    end)
    out:flush()
  end
  if frames % 600 ~= 0 then return end
  pcall(function()
    local z = {}
    for k, v in pairs(camcalls) do z[#z + 1] = k .. ":" .. v end
    out:write(string.format(
      "UI%6d | Presents %d (normal %d: DrawOTag %d, Wechsel %d) | Apply %d: DrawOTag %d, Wechsel %d, BG-LoadImage %d, CD-BG %d, Blickmatrix %d | FUN_80053ca4 ra: %s\n",
      frames, st.pres, st.norm, st.norm_draw, st.norm_toggle, st.apply, st.apply_draw,
      st.apply_toggle, st.apply_bgcopy, st.apply_cd, st.apply_cam, table.concat(z, " ")))
    out:flush()
    if st.apply >= MAX_APPLY and not done then
      done = true
      out:write(">>> SETTER fertig (Stoppmarke fuer pcsx_drive.py)\n")
      out:flush()
    end
  end)
end
