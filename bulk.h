#include <iostream>
#include <chrono>
#include <queue>
#include <fstream>

struct Bulk{
    enum  actions {new_bulk, upd_queue, new_dyn, upd_dyn, end_dyn};
    using block_t = std::pair<std::string, std::string>;

    explicit Bulk(size_t sz):sz(sz){
        std::string line;
        while (std::cin >> line) {
            input(line);
        }
        output();
    }

private:
    void input(const std::string &line) {
        actions action;
        if (line == "{") {
            if(stack++ == 0)
                cmd_cnt = 0;
            return;
        }
        else if(line == "}") {
            if(--stack != 0)
                return;
            cmd_cnt = 0;
            action = end_dyn;
        }
        else if(stack != 0) {
            action = cmd_cnt++ != 0 ? upd_dyn : new_dyn;
        }
        else {
            if(cmd_cnt == sz) cmd_cnt = 0;
            action = cmd_cnt++ != 0 ? upd_queue : new_bulk;
        }

        handling(line, action);
    }

    void handling(const std::string& line, actions action){
        switch (action) {
            case new_bulk  : blocks_queue.push(bulk(line));              break;
            case upd_queue : blocks_queue.back().second += ", " + line;  break;
            case new_dyn   : buffer = bulk(line);                        break;
            case upd_dyn   : buffer.second += ", " + line;               break;
            case end_dyn   : blocks_queue.push(buffer) ;                 break;
        }
    }

    void output() {
        while (!blocks_queue.empty()) {
            std::ofstream file(blocks_queue.front().first);
            file << blocks_queue.front().second;
            file.close();
            std::cout << blocks_queue.front().second << '\n';
            blocks_queue.pop();
        }
    }

    block_t bulk(const std::string &line) {
        auto t = std::chrono::system_clock::now().time_since_epoch();
        auto utc = std::chrono::duration_cast<std::chrono::microseconds>(t).count();
        return {std::to_string(utc) + ".log", "bulk: " + line};
    }

    block_t buffer;
    std::queue<block_t> blocks_queue;
    size_t stack = 0, cmd_cnt = 0, sz;
};