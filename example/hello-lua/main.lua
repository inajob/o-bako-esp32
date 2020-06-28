function setup()
end

c = 0

function loop()
    color(0, 0, 0)
    fillrect(0, 0, 128, 128)
    color(255, 255, 255)
    text("Hello Lua", 10, c%128)
    c = c + 1
end
