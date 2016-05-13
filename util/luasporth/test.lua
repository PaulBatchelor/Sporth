print("hello")
tbl = {7, 3, 5}

f = io.open("out.txt", "w")
math.randomseed(1234)
io.write(string.format("the number is %g\n", math.random(10)))

for k,v in pairs(tbl) do
    f:write(string.format("%d ", v))
end

io.close(f)

add_ftable("tbl", #tbl, tbl)
