-- jump game

function setup()
end

count = 0
x = 16
y = 100
yv = 0
jumpflag = false
enemy = {}
preventcount = 0
modetitle = 0
modegame = 1
modeover = 2
mode = modetitle

function mkenemy(x, y)
    return {x = x, y = y}
end
enemy[#enemy] = mkenemy(128, 100);
test = {x=0, y=0}

function loop()
    if mode == modetitle then
        title()
    elseif mode == modegame then
        game()
    elseif mode == modeover then
        over()
    end
end

function title()
    color(0,0,0)
    fillrect(0,0,128,128)
    color(255,255,255)
    text("JUMP", 32, 50)

    if btn(5) == 1 or btn(4) == 1 then
        mode = modegame
        enemy = {}
    end
end

function over()
    color(255,255,255)
    text("GAME OVER", 32, 50)
    if btn(5) == 1 or btn(4) == 1 then
        mode = modetitle
    end
end

function game()
    color(0,0,0)
    fillrect(0,0,128,128)

    if (btn(5) == 1 or btn(4) == 1) and not(jumpflag) then
        yv = -7
        jumpflag = true
    end
    if math.abs(yv) < 8 then
        yv = yv + 0.3
    end
    y = y + yv
    if y > 100 then
        y = 100
        jumpflag = false
    end

    if math.random(100) == 1 and preventcount == 0 then
      enemy[#enemy + 1] = mkenemy(128, 100);
      preventcount = 32
    end

    -- hit check
    for i, e in pairs(enemy) do
        cx = x + 8
        cy = y + 8
        cex = e.x + 8
        cey = e.y + 8
        if math.abs(cx - cex) < 8 and math.abs(cy - cey) < 8 then
            mode = modeover
        end
    end

    spr(x, math.floor(y), 16, 16, 0, 0)
    nextenemy = {}
    for i, e in pairs(enemy) do
        e.x = e.x - 1
        if e.x > -16 then
            nextenemy[#nextenemy + 1] = e
        end
        spr(e.x, e.y, 16, 16, 16, 0)
    end
    nextenemy = enemy
    color(100,100,0)
    fillrect(0,116,128,10)

    count = count + 1
    if preventcount > 0 then
        preventcount = preventcount - 1
    end
end
