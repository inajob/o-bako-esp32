-- Ported doodle by @ntsutae for o-bako
-- https://twitter.com/ntsutae/status/1268820823952916486

function setup()
end
t = 0
function loop()
    for x = 0, 256 do
        c = math.abs(t+((x-t)~(x+t))^3)%997<97 and 0 or 256
        color(c,c,c)
        pset(x, t%128)
    end
  t = t +1
end

