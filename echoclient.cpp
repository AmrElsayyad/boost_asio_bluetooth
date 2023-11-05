/* clienthttpget.cpp */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <boost/program_options.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include "wrapper.h"

using std::cout;
using std::endl;
using std::string;

namespace po = boost::program_options;

boost::mutex global_stream_lock;

/* reads from keypress, doesn't echo */
int getch(void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

class MyConnection : public Connection {
  private:
    void OnAccept(const std::string &addr, uint8_t channel) {
        global_stream_lock.lock();
        std::cout << "[OnAccept] " << addr << ":" << channel << "\n";
        global_stream_lock.unlock();

        // Start the next receive
        Recv();
    }

    void OnConnect(const std::string &addr, uint8_t channel) {
        global_stream_lock.lock();
        std::cout << "[OnConnect] " << addr << ":" << channel << "\n";
        global_stream_lock.unlock();

        // Start the next receive
        Recv();

        std::string str = "hello world!";

        std::vector<uint8_t> request;
        std::copy(str.begin(), str.end(), std::back_inserter(request));
        Send(request);
    }

    void OnSend(const std::vector<uint8_t> &buffer) {
        global_stream_lock.lock();
        std::cout << "[OnSend] " << buffer.size() << " bytes\n";
        for (size_t x = 0; x < buffer.size(); x++) {
            std::cout << (char)buffer[x];
            if ((x + 1) % 16 == 0) std::cout << "\n";
        }
        std::cout << "\n";
        global_stream_lock.unlock();
    }

    void OnRecv(std::vector<uint8_t> &buffer) {
        global_stream_lock.lock();
        std::cout << "[OnRecv] " << buffer.size() << " bytes\n";
        for (size_t x = 0; x < buffer.size(); x++) {
            std::cout << (char)buffer[x];
            if ((x + 1) % 16 == 0) std::cout << "\n";
        }
        std::cout << "\n";
        global_stream_lock.unlock();

        // Start the next receive
        Recv();
    }

    void OnTimer(const boost::posix_time::time_duration &delta) {
        global_stream_lock.lock();
        std::cout << "[OnTimer] " << delta << std::endl;
        global_stream_lock.unlock();
    }

    void OnError(const boost::system::error_code &error) {
        global_stream_lock.lock();
        std::cout << "[OnError] " << error << "\n";
        global_stream_lock.unlock();
    }

  public:
    MyConnection(boost::shared_ptr<Hive> hive) : Connection(hive) {}

    ~MyConnection() {}
};

int main(int argc, char *argv[]) {
    po::options_description desc("Usage: " + string(argv[0]) +
                                 " <server-mac-address>\nOptions");
    desc.add_options()("help,h", "display this help and exit");

    // Hide the `files` options in a separate description
    po::options_description desc_hidden("Hidden options");
    desc_hidden.add_options()("server-mac-address", po::value<string>(),
                              "server MAC address");

    // This description is used for parsing and validation
    po::options_description cmdline_options;
    cmdline_options.add(desc).add(desc_hidden);

    // And this one to display help
    po::options_description visible_options;
    visible_options.add(desc);

    po::positional_options_description pos;
    pos.add("server-mac-address", 1);

    po::variables_map vm;
    try {
        // Only parse the options, so we can catch the explicit `--files`
        auto parsed = po::command_line_parser(argc, argv)
                          .options(cmdline_options)
                          .positional(pos)
                          .run();

        // Make sure there were no non-positional `server-mac-address` options
        for (auto const &opt : parsed.options) {
            if ((opt.position_key == -1) &&
                (opt.string_key == "server-mac-address")) {
                throw po::unknown_option("server-mac-address");
            }
        }

        po::store(parsed, vm);
        po::notify(vm);
    } catch (const po::error &e) {
        std::cerr << "Couldn't parse command line arguments properly:\n";
        std::cerr << e.what() << '\n' << '\n';
        std::cerr << visible_options << '\n';
        return 1;
    }

    if (vm.count("help") || !vm.count("server-mac-address")) {
        std::cout << desc << "\n";
        return 1;
    }

    boost::shared_ptr<Hive> hive(new Hive());

    boost::shared_ptr<MyConnection> connection(new MyConnection(hive));
    connection->Connect(vm["server-mac-address"].as<string>(), 13);

    while (1) {
        hive->Poll();
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }

    hive->Stop();

    return 0;
}
