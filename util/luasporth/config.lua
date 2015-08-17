require "cjson"

tbl_2 = {0, 4, 7, 11, 12}
tbl_1 = {0, 11, 7, 2, 9}

choice = {tbl_2, tbl_1}
t = 1
pos = 0
base = 70

function f0()
    val = choice[t][pos + 1]
    if(pos == 0) then base = base + 2 end
    pos = pos + 1
    pos = (pos % 5)
    if (base > 82) then
        base = 70
        if(t == 1) then t = 2 else t = 1 end
    end
    return val + base
end

function f1()
end

function f2()
end

function f3()
end
