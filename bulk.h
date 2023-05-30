#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>

#include <boost/signals2/signal.hpp>

struct Bulk_t {
    std::string header;
    std::string cmd;
};

namespace bs = boost::signals2;
using signal_t = bs::signal<void(const std::vector<Bulk_t> &)>;

struct Bulk {
    enum actions {
        new_bulk, upd_queue, new_buf, upd_buf, end_buf
    };

    explicit Bulk(size_t sz) : sz(sz) {
        std::string line;
        while(std::cin >> line) {
            input(line);
        }
    }

    bs::connection connect(const signal_t::slot_type &subscriber) {
        return signal.connect(subscriber);
    }

    void result() {
        signal(info);
    }

    const std::vector<Bulk_t> &info = bulk_queue;

private:
    void input(const std::string &line) {
        actions action;
        if(line == "{") {
            if(stack++ == 0)
                cmd_cnt = 0;
            return;
        } else if(line == "}") {
            if(--stack != 0)
                return;
            cmd_cnt = 0;
            action = end_buf;
        } else if(stack != 0) {
            action = cmd_cnt++ != 0 ? upd_buf : new_buf;
        } else {
            if(cmd_cnt == sz) cmd_cnt = 0;
            action = cmd_cnt++ != 0 ? upd_queue : new_bulk;
        }

        handling(line, action);
    }

    void handling(const std::string &line, actions action) {
        switch(action) {
            case new_bulk  :
                bulk_queue.push_back(bulk(line));
                break;
            case upd_queue :
                bulk_queue.back().cmd += ", " + line;
                break;
            case new_buf   :
                buffer = bulk(line);
                break;
            case upd_buf   :
                buffer.cmd += ", " + line;
                break;
            case end_buf   :
                bulk_queue.push_back(buffer);
                break;
        }
    }

    Bulk_t bulk(const std::string &line) {
        auto t = std::chrono::system_clock::now().time_since_epoch();
        auto utc = std::chrono::duration_cast<std::chrono::microseconds>(t).count();
        return {std::to_string(utc) + ".log", "bulk: " + line};
    }

    Bulk_t buffer;
    std::vector<Bulk_t> bulk_queue;
    size_t stack = 0, cmd_cnt = 0, sz;
    signal_t signal;
};

struct ToPrint {
    explicit ToPrint(Bulk &bulk) {
        connection = bulk.connect(std::bind(&ToPrint::print, this, std::placeholders::_1));
    }

    void print(const std::vector<Bulk_t> &info) const {
        for(const auto &i: info) {
            std::cout << i.cmd << '\n';
        }
    }

    ~ToPrint() {
        connection.disconnect();
    }

private:
    bs::connection connection;
};

struct ToFile {
    explicit ToFile(Bulk &bulk) {
        connection = bulk.connect(std::bind(&ToFile::print, this, std::placeholders::_1));
    }

    void print(const std::vector<Bulk_t> &info) const {
        for(const auto &i: info) {
            std::ofstream file(i.header);
            file << i.cmd;
            file.close();
        }
    }

    ~ToFile() {
        connection.disconnect();
    }

private:
    bs::connection connection;
};