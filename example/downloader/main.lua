json = require("/downloader/json.lua")

M_INIT = 0
M_FETCH = 1
M_GAMELIST = 2
M_FETCHGAMEINFO = 3
M_GAMEINFO = 4
M_GAMEDL = 5

mode = M_INIT
gameList = {}
cursor = 1
gameName = ""
gameInfo = {}
files = {}

function cls()
    color(0,0,0)
    fillrect(0,0,128,128) 
end

function drawStatusText(s)
  cls()
  color(0,255,0)
  text(s, 0, 0)
end

function fetchGames()
  collectgarbage("collect")
  local file = httpsget("raw.githubusercontent.com","/inajob/o-bako-esp32/master/example/games.json")
  return json.decode(file)
end

function storeFile(game, fname)
  collectgarbage("collect")
  return httpsgetfile("raw.githubusercontent.com","/inajob/o-bako-esp32/master/example/" .. game .. "/" .. fname, "/" .. game .. "/" .. fname)
end

function fetchFile(game, fname)
  collectgarbage("collect")
  return httpsget("raw.githubusercontent.com","/inajob/o-bako-esp32/master/example/" .. game .. "/" .. fname)
end

function fetchGameInfo(game)
  local file = fetchFile(game, "game.json")
  return json.decode(file)
end

function drawGameList(x, y, games, cursor)
  for i,v in pairs(games) do
    text(v, 8 + x, (i - 1) * 8 + y)
  end 
  text(">", 0 + x, (cursor - 1) * 8 + y)
end

function drawGameInfo(x, y, game)
  text("title:" .. game["title"],x ,y)
  text("files:",x ,y + 8)
  for i,v in pairs(game["files"]) do
    text(v, 8 + x , (i - 1) * 8 + 16 + y)
  end 
end

function setup()
    cls()
end

function loop()
  if mode == M_INIT then
    cls()
    drawStatusText("fetch Game List ...")
    mode = M_FETCH
  elseif mode == M_FETCH then
    local games = fetchGames()
    gameList = games["games"]
    cls()
    mode = M_GAMELIST
  elseif mode == M_GAMELIST then
    cls()
    color(0,255,0)
    if btn(2) == 2 then
      cursor = cursor - 1
      if cursor == 0 then
        cursor = #gameList
      end
    end
    if btn(3) == 2 then
      cursor = cursor + 1
      if cursor > #gameList then
        cursor = 1;
      end
    end
    if btn(4) == 2 or btn(5) == 2 then
      gameName = gameList[cursor]
      mode = M_FETCHGAMEINFO
      cls()
      drawStatusText("fetch GameInfo " .. gameName .. "...")
    end
    drawGameList(0, 16, gameList, cursor)
  elseif mode == M_FETCHGAMEINFO then
    gameInfo = fetchGameInfo(gameName)
    mode = M_GAMEINFO
  elseif mode == M_GAMEINFO then
    cls()
    color(0,30,0)
    fillrect(4,4,128-4,128-4)
    color(0,255,0)
    drawrect(4,4,128-4,128-4)
    drawGameInfo(8,8, gameInfo)
    if btn(4) == 2 or btn(5) == 2 then
      mode = M_GAMEDL
      files = gameInfo["files"]
      drawStatusText("Game download...")
    end
    if btn(1) == 2 or btn(6) == 2 then
      mode = M_GAMELIST
    end
  elseif mode == M_GAMEDL then
    head = table.remove(files, 1)
    drawStatusText("download..." .. head)
    storeFile(gameName, head)
    if #files == 0 then
      mode = M_GAMELIST
    end
  end
end
