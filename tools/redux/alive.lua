-- alive.lua — Laeuft das Spiel wirklich, oder haengt es?
--
-- ANLASS: Ich hatte aus ZWEI PC-Stichproben (beide 0x80062130) auf eine Haenge-Schleife
-- geschlossen. Der Nutzer sagt, es laufe. Zwei Stichproben sind kein Beleg — wenn das eine
-- Schleife ist, die ohnehin jedes Bild durchlaufen wird, sieht "haengt" genauso aus.
--
-- RICHTIGER TEST: den Programmzaehler ueber viele Bilder abtasten und die ANZAHL
-- VERSCHIEDENER Werte zaehlen. Haenger = eine Handvoll. Laufendes Spiel = hunderte.
-- Dazu ein Zaehler, wie oft sich der Pad-Schreibvorgang ueberhaupt ereignet.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\alive_out.txt]]
local frames = 0
local out = nil
local fertig = false
local gesehen = {}
local verschiedene = 0
local proben = 0
local padwrites = 0
local bp = nil

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end

function DrawImguiFrame()
  if fertig then return end
  frames = frames + 1
  if frames < 200 then return end

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("Lebendigkeitstest\n")
    pcall(function()
      bp = PCSX.addBreakpoint(0x800AC76C, 'Write', 4, 'padcount', function()
        padwrites = padwrites + 1
        return true   -- MESSUNG: false LOESCHT den Haltepunkt (1 Treffer, dann tot)
      end)
    end)
  end

  local ok, err = pcall(function()
    local pc = tonumber(PCSX.getRegisters().pc)
    proben = proben + 1
    if gesehen[pc] == nil then
      gesehen[pc] = 1
      verschiedene = verschiedene + 1
    else
      gesehen[pc] = gesehen[pc] + 1
    end

    if frames % 300 == 0 then
      local m = PCSX.getMemPtr()
      out:write(string.format(
        "Bild %5d | %4d Proben, %4d VERSCHIEDENE pc | Pad-Schreibvorgaenge: %d | pc jetzt %08x | Modus %d\n",
        frames, proben, verschiedene, padwrites, pc, u8(m, 0x800b5359)))
      out:flush()
    end

    if frames >= 2000 then
      -- die haeufigsten pc-Werte zeigen
      local liste = {}
      for k, v in pairs(gesehen) do liste[#liste+1] = { pc = k, n = v } end
      table.sort(liste, function(a, b) return a.n > b.n end)
      out:write("\nHaeufigste Programmzaehler-Werte:\n")
      for i = 1, math.min(8, #liste) do
        out:write(string.format("   %08x  %dx\n", liste[i].pc, liste[i].n))
      end
      out:write(string.format("\nERGEBNIS: %d verschiedene pc-Werte bei %d Proben, %d Pad-Schreibvorgaenge\n",
                verschiedene, proben, padwrites))
      out:write(verschiedene > 50 and "-> das Spiel LAEUFT\n" or "-> das Spiel HAENGT\n")
      out:close()
      fertig = true
      PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then
    out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush(); fertig = true
  end
end
