M_DRAW = 0
M_MENU = 1
M_PALETTE = 2
M_LOAD = 3
M_SAVE = 4
mode = 0
grid = true
col = 0
cursor = 0

digit = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
}

function cls()
    color(0,0,0)
    fillrect(0,0,128,128) 
end

function menu()
  if btn(2) == 2 and cursor > 0 then
    cursor = cursor - 1
  end
  if btn(3) == 2 and cursor < 4 then
    cursor = cursor + 1
  end
  if btn(5) == 2 then
    if cursor == 0 then
      mode = M_PALETTE
    elseif cursor == 1 then
      grid = true
      mode = M_DRAW
    elseif cursor == 2 then
      grid = false
      mode = M_DRAW
    elseif cursor == 3 then
      mode = M_LOAD
    elseif cursor == 4 then
      mode = M_SAVE
    end
  end

  cls()
  color(255,255,255)
  text("Palette", 8, 8)
  text("Grid On", 8, 8*2)
  text("Grid Off", 8, 8*3)
  text("Load", 8, 8*4)
  text("Save", 8, 8*5)
  text(">", 0, cursor * 8 + 8)
end

function drawPalette()
  if btn(0) == 2 and col > 0 then
    col = col - 1
  end
  if btn(1) == 2 and col < 255 then
    col = col + 1
  end
  if btn(2) == 2 and col > 16 then
    col = col - 16
  end
  if btn(3) == 2 and col < 255-16 then
    col = col + 16
  end

  x = 0
  y = 0
  ty = 0
  for p = 0,255 do
    color(p)
    fillrect(x*8,y*8,8,8)
    x = x+1
    if x*8 >= 128 then
      x = 0
      y = y + 1
    end
  end
  color(255,255,255)
  drawrect(col%16 * 8,math.floor(col/16)*8,8,8)
end

lx = 0
ly = 0
function load()
    if btn(0) == 2 and lx > 0 then
        lx = lx - 1
    end
    if btn(1) == 2 and lx < 7 then
        lx = lx + 1
    end
    if btn(2) == 2 and ly > 0 then
        ly = ly - 1
    end
    if btn(3) == 2 and ly < 7 then
        ly = ly + 1
    end
    if btn(5) == 2 then
        mode = M_DRAW
        cls()
        spr(0,0,128,128,0,0)
        for x = 0,15 do
          for y = 0,15 do
            r,g,b,index = pget(lx * 16 + x,ly * 16 + y)
            digit[y*16 + x + 1] = index
          end
        end
    end
    if btn(6) == 2 then -- cancel
        mode = M_DRAW
    end

    cls()
    spr(0,0,128,128,0,0)
    color(0,255,0)
    drawrect(lx * 16, ly * 16, 16, 16)
end

function save()
    if btn(0) == 2 and lx > 0 then
        lx = lx - 1
    end
    if btn(1) == 2 and lx < 7 then
        lx = lx + 1
    end
    if btn(2) == 2 and ly > 0 then
        ly = ly - 1
    end
    if btn(3) == 2 and ly < 7 then
        ly = ly + 1
    end
    if btn(5) == 2 then
        mode = M_DRAW
        -- todo: how to save?
        cls()
        spr(0,0,128,128,0,0)
        for x = 0,15 do
          for y = 0,15 do
            color(digit[y*16 + x + 1])
            pset(x + lx * 16,y + ly * 16)
          end
        end
        savebmp("/load-test/sprite.bmp")
        run("/load-test/color.lua") -- reload
    end
    if btn(6) == 2 then -- cancel
        mode = M_DRAW
    end

    cls()
    spr(0,0,128,128,0,0)

    for x = 0,15 do
      for y = 0,15 do
        color(digit[y*16 + x + 1])
        pset(x + lx * 16,y + ly * 16)
      end
    end

    color(0,255,0)
    drawrect(lx * 16, ly * 16, 16, 16)
end

function setup()
    cls()
end


px = 0
py = 0

function drawRun()
    if btn(0) == 2 and px > 0 then
        px = px - 1
    end
    if btn(1) == 2 and px < 15 then
        px = px + 1
    end
    if btn(2) == 2 and py > 0 then
        py = py - 1
    end
    if btn(3) == 2 and py < 15 then
        py = py + 1
    end

    if btn(4) == 2 then
        mode = M_MENU
    end
    
    if btn(5) == 2 then
        digit[py * 16 + px + 1] = col
    end
    if btn(6) == 2 then
        digit[py * 16 + px + 1] = 0
    end
    
    cls()

    for y = 0, 15 do
        for x = 0, 15 do
            color(digit[y*16+x + 1])
            fillrect(x * 6 + 1, y * 6 + 1, 6, 6)
        end
    end
    for y = 0, 15 do
        for x = 0, 15 do
            color(digit[y*16+x + 1])
            fillrect(x +128-16 -8, y + 128-16-8 , 1, 1)
        end
    end
    color(255,255,255)
    drawrect(0, 0, 6*16+1, 6*16+1)

    if grid then
      color(255,255,255)
      for i = 0, 16 do
          drawrect(0, i * 6,6*16,1)
          drawrect(i * 6, 0,1, 6*16)
      end
    end

    color(0,255,0)
    drawrect(px*6, py*6 ,6, 6)
end

function loop()
    if mode == M_DRAW then
      drawRun()
    elseif mode == M_MENU then
      menu()
    elseif mode == M_LOAD then
      load()
    elseif mode == M_SAVE then
      save()
    elseif mode == M_PALETTE then
      drawPalette()
      if btn(5) == 2 then
          mode = M_DRAW
      end
    end
end
