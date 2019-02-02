-- draw utility
function cls()
    color(0,0,0)
    fillrect(0,0,128,128)
end

-- mode
M_IDLE = 0
M_MOVE = 1
M_ANIM = 2

mode = M_IDLE
moveDirection = 0
-- object
function createBlock(value)
    return {
        dx = 0,
        dy = 0,
        value = value,
    }
end

function setColor(val)
    if   val == 2 then color(255,255,255)
    elseif val == 4 then color(255,0,0)
    elseif val == 8 then color(0,255,0)
    elseif val == 16 then color(255,0,255)
    elseif val == 32 then color(255,255,0)
    elseif val == 64 then color(0,255,255)
    elseif val == 128 then color(0,128,128)
    elseif val == 256 then color(128,0,0)
    elseif val == 512 then color(0,128,0)
    elseif val == 1024 then color(0,0,128)
    end
end

function drawBlock(x,y,block)
    color(255,255,255)
    setColor(block.value)
    drawrect(16 + x * 24 + block.dx, 16 + y * 24 + block.dy, 23, 23)
    color(255,255,255)
    text(block.value, 16 + x * 24 + block.dx+2, 16 + y * 24 + block.dy+8)
    -- color(255,0,0)
    -- drawrect(16 + x * 24, 16 + y * 24, 23, 23)
end

stage = {
    {nil, nil, nil, nil},
    {nil, nil, nil, nil},
    {nil, nil, nil, nil},
    {nil, nil, nil, nil}
}
stage[1][1] = createBlock(2)

function isFilled()
    local filled = true
    for x = 1,4 do
        for y = 1,4 do
            if stage[x][y] == nil then
                filled = false
            end
        end
    end
    return filled
end

function addBlock()
    local b = createBlock(math.floor(2^math.random(2)))
    if isFilled() then
        return false
    end
    local x,y
    repeat
        x = math.random(4)
        y = math.random(4)
    until stage[x][y] == nil
    stage[x][y] = b
    return true
end

function _move(x, y, px, py)
    local moved = false
    if stage[x][y] ~=nil then
        if stage[x + px][y + py] == nil then
            stage[x + px][y + py] = stage[x][y]
            if     px > 0 then stage[x + px][y + py].dx = -24
            elseif px < 0 then stage[x + px][y + py].dx = 24
            elseif py > 0 then stage[x + px][y + py].dy = -24
            elseif py < 0 then stage[x + px][y + py].dy = 24
            end
            stage[x][y] = nil
            moved = true
        elseif stage[x][y].value == stage[x + px][y + py].value then
            stage[x + px][y + py].value = stage[x + px][y + py].value*2
            stage[x][y] = nil
        end
    end
    return moved
end

function moveLeft()
    local moved = false
    for x = 2,4 do
        for y = 1,4 do
            moved = _move(x ,y ,-1, 0) or moved
        end
    end
    return moved
end
function moveUp()
    local moved = false
    for x = 1,4 do
        for y = 2,4 do
            moved = _move(x ,y ,0, -1) or moved
        end
    end
    return moved
end
function moveRight()
    local moved = false
    for x = 3,1,-1 do
        for y = 1,4 do
            moved = _move(x ,y ,1, 0) or moved
        end
    end
    return moved
end
function moveDown()
    local moved = false
    for x = 1,4 do
        for y = 3,1,-1 do
            moved = _move(x ,y ,0, 1) or moved
        end
    end
    return moved
end

function setup()
    cls()
end

function loop()
    if mode == M_IDLE then
        if btn(0) == 2 then
            moveDirection = 0
            mode = M_MOVE
        end
        if btn(1) == 2 then
            moveDirection = 1
            mode = M_MOVE
        end
        if btn(2) == 2 then
            moveDirection = 2
            mode = M_MOVE
        end
        if btn(3) == 2 then
            moveDirection = 3
            mode = M_MOVE
        end
    end
    if mode == M_MOVE then
        local moved = false
        if     moveDirection == 0 then moved = moveLeft()
        elseif moveDirection == 1 then moved = moveRight() 
        elseif moveDirection == 2 then moved = moveUp() 
        elseif moveDirection == 3 then moved = moveDown()
        end
        if moved then
            mode = M_ANIM
        else
            mode = M_IDLE
            addBlock()
        end
    end
    
    local animated = false
    -- animation
    for x = 1,4 do
        for y = 1,4 do
            if stage[x][y] ~= nil then
                if stage[x][y].dx > 0 then
                    stage[x][y].dx = stage[x][y].dx - 4
                    animated = true
                elseif stage[x][y].dx < 0 then
                    stage[x][y].dx = stage[x][y].dx + 4
                    animated = true
                end
                if stage[x][y].dy > 0 then
                    stage[x][y].dy = stage[x][y].dy - 4
                    animated = true
                elseif stage[x][y].dy < 0 then
                    stage[x][y].dy = stage[x][y].dy + 4
                    animated = true
                end
            end
        end
    end
    if mode == M_ANIM and animated == false then
        mode = M_MOVE
    end
    
    cls()
    color(255,0,255)
    drawrect(16, 16, 24*4, 24*4)
    for x = 1,4 do
        for y = 1,4 do
            if stage[x][y] ~= nil then
                drawBlock(x - 1,y - 1, stage[x][y])
            end
        end
    end
    color(255,255,255)
    text(mode, 0,0)
end