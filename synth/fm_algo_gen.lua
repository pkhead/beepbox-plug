--[[
algorithms:
0.  1 <- (2 3 4)
1.  1 <- (2 3 <- 4)
2.  1 <- 2 <- (3 4)
3.  1 <- (2 3) <- 4
4.  1 <- 2 <- 3 <- 4
5.  1 <- 3  2 <- 4
6.  1  2 <- (3 4)
7.  1  2 <- 3 <- 4
8.  (1 2) <- 3 <- 4
9.  (1 2) <- (3 4)
10. 1  2  3 <- 4
11. (1 2 3) <- 4
12. 1  2  3  4


feedback types:
0.  1 G
1.  2 G
2.  3 G
3.  4 G
4.  1 G  2 G
5.  3 G  4 G
6.  1 G  2 G  3 G
7.  2 G  3 G  4 G
8.  1 G  2 G  3 G  4 G
9.  1 -> 2
10. 1 -> 3
11. 1 -> 4
12. 2 -> 3
13. 2 -> 4
14. 3 -> 4
15. 1 -> 3  2 -> 4
16. 1 -> 4  2 -> 3
17. 1 -> 2 -> 3 -> 4
--]]

local algorithms = {
    -- 0.  1 <- (2 3 4)
    {
        carriers = 1,
        op1 = {2, 3, 4},
    },

    -- 1.  1 <- (2 3 <- 4)
    {
        carriers = 1,
        op1 = {2, 3},
        op3 = {4},
    },

    -- 2.  1 <- 2 <- (3 4)
    {
        carriers = 1,
        op1 = {2},
        op2 = {3, 4},
    },

    -- 3.  1 <- (2 3) <- 4
    {
        carriers = 1,
        op1 = {2, 3},
        op2 = {4},
        op3 = {4},
    },

    -- 4.  1 <- 2 <- 3 <- 4
    {
        carriers = 1,
        op1 = {2},
        op2 = {3},
        op3 = {4},
    },

    -- 5.  1 <- 3  2 <- 4
    {
        carriers = 2,
        op1 = {3},
        op2 = {4},
    },

    -- 6.  1  2 <- (3 4)
    {
        carriers = 2,
        op2 = {3, 4},
    },

    -- 7.  1  2 <- 3 <- 4
    {
        carriers = 2,
        op2 = {3},
        op3 = {4},
    },

    -- 8.  (1 2) <- 3 <- 4
    {
        carriers = 2,
        op1 = {3},
        op2 = {3},
        op3 = {4},
    },

    -- 9.  (1 2) <- (3 4)
    {
        carriers = 2,
        op1 = {3, 4},
        op2 = {3, 4},
    },

    -- 10. 1  2  3 <- 4
    {
        carriers = 3,
        op3 = {4},
    },

    -- 11. (1 2 3) <- 4
    {
        carriers = 3,
        op1 = {4},
        op2 = {4},
        op3 = {4},
    },

    -- 12. 1  2  3  4
    {
        carriers = 4,
    }
}

local feedback_types = {
    -- 0.  1 G
    {
        op1 = 1
    },

    -- 1.  2 G
    {
        op2 = 2
    },

    -- 2.  3 G
    {
        op3 = 3
    },

    -- 3.  4 G
    {
        op4 = 4
    },

    -- 4.  1 G  2 G
    {
        op1 = 1,
        op2 = 2
    },

    -- 5.  3 G  4 G
    {
        op3 = 3,
        op4 = 4,
    },

    -- 6.  1 G  2 G  3 G
    {
        op1 = 1,
        op2 = 2,
        op3 = 3
    },

    -- 7.  2 G  3 G  4 G
    {
        op2 = 2,
        op3 = 3,
        op4 = 4,
    },

    -- 8.  1 G  2 G  3 G  4 G
    {
        op1 = 1,
        op2 = 2,
        op3 = 3,
        op4 = 4,
    },

    -- 9.  1 -> 2
    {
        op2 = 1,
    },

    -- 10. 1 -> 3
    {
        op3 = 1
    },

    -- 11. 1 -> 4
    {
        op4 = 1
    },

    -- 12. 2 -> 3
    {
        op3 = 2
    },

    -- 13. 2 -> 4
    {
        op4 = 2,
    },

    -- 14. 3 -> 4
    {
        op4 = 3
    },

    -- 15. 1 -> 3  2 -> 4
    {
        op3 = 1,
        op4 = 2
    },

    -- 16. 1 -> 4  2 -> 3
    {
        op4 = 1,
        op3 = 2
    },

    -- 17. 1 -> 2 -> 3 -> 4
    {
        op4 = 3,
        op3 = 2,
        op2 = 1,
    }
}

local function write_lines_to_file(file_name, lines)
    local f = assert(io.open(file_name, "w"), "could not open file")
    for _, line in ipairs(lines) do
        f:write(line)
        f:write("\n")
    end
    f:close()
end

-- generate header
local algo_signatures = {}
local func_count = 0

do
    local lines = {"#ifndef _fm_algo_h_", "#define _fm_algo_h_", "#include \"fm.h\"", ""}

    for algo_index, _ in ipairs(algorithms) do
        algo_signatures[algo_index] = {}

        for fdbk_index, _ in ipairs(feedback_types) do
            table.insert(algo_signatures[algo_index], ("double fm_algo%02d%02d(fm_voice_s *voice, const double feedback_amp)"):format(algo_index-1, fdbk_index-1))
            func_count = func_count + 1
        end
    end

    lines[#lines+1] = "typedef double (*fm_algo_f)(fm_voice_s *voice, const double feedback_amp);"
    lines[#lines+1] = "extern fm_algo_f fm_algorithm_table["..func_count.."];"

    lines[#lines+1] = "#endif"

    write_lines_to_file("src/fm_algo.h", lines)
end

do
    local lines = {"#include \"fm_algo.h\""}

    for algo_index, algo_data in ipairs(algorithms) do    
        for fdb_index, fdb_data in ipairs(feedback_types) do
            lines[#lines+1] = algo_signatures[algo_index][fdb_index] .. " {"
            for op=4, 1, -1 do
                local mod_gen_list = {}
                for _, mod_op in ipairs(algo_data["op" .. op] or {}) do
                    mod_gen_list[#mod_gen_list+1] = " + "
                    mod_gen_list[#mod_gen_list+1] = "op"..(mod_op-1).."_scaled"
                end
                local mod_gen = table.concat(mod_gen_list)

                local fdb_str
                if fdb_data["op"..op] then
                    fdb_str = " + feedback_amp * voice->op_states["..(fdb_data["op"..op]-1).."].output"
                else
                    fdb_str = ""
                end

                lines[#lines+1] = "    double op"..(op-1).."_scaled = voice->op_states["..(op-1).."].expression * (voice->op_states[" .. (op-1) .. "].output = fm_calc_op("
                lines[#lines+1] = "        voice->op_states["..(op-1).."].phase" .. mod_gen .. fdb_str
                lines[#lines+1] = "    ));"
            end

            local carriers = {}
            for i=1, algo_data.carriers do
                table.insert(carriers, "op"..(i-1).."_scaled")
            end

            lines[#lines+1] = "    return " .. table.concat(carriers, " + ") .. ";"
            lines[#lines+1] = "}"
        end
    end

    lines[#lines+1] = "fm_algo_f fm_algorithm_table["..(func_count).."] = {"

    for algo_index, algo_data in ipairs(algorithms) do
        for fdb_index, fdb_data in ipairs(feedback_types) do
            lines[#lines+1] = ("    fm_algo%02d%02d,"):format(algo_index - 1, fdb_index - 1)
        end
    end

    lines[#lines+1] = "};"
    
    write_lines_to_file("src/fm_algo.c", lines)
end