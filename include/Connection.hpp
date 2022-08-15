#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <map>
#include <sys/poll.h>
#include "HTTPReader.hpp"

#define NUM_FDS 200

/// The connection end point of the server. Handles all the connections.
class Connection {
    /// First: Port --- Second: Server fd.
    std::map<int, int> _server_fds;
    std::map<int, Runnable *> _fd_mapping;
    /// First: client fd --- Second: server fd
    std::map<int, int>      _connection_pairs;
    std::list<HTTPReader *> _readers;
    /// The timeout in milliseconds.
    int                _timeout;
    /// The number of file descriptors inside of the polling array.
    nfds_t             _nfds;
    /// The polling array.
    struct pollfd      _fds[NUM_FDS];
    static Connection * currentInstance;

    /// Compresses the polling array. Upon return, all removable contents are removed.
    void _clear_poll_array();

    /**
     * Returns whether the given file descriptor is a server socket.
     *
     * @param fd The file descriptor to check
     * @return True, if the given fd is a server socket's fd
     */
    bool _is_serving_fd(int fd);

    /**
     * Accepts an incoming connection.
     *
     * @param i The index inside of the polling array of the file descriptor to handle
     */
    void accept(nfds_t i);

    /**
     * Handles a file descriptor.
     *
     * @param i The index inside of the polling array of the file descriptor to handle
     */
    void handle(nfds_t i);

    void printPollArray()                                       _NOEXCEPT;
	void denyConnection(const int fd, HTTPReader * reader = NULL , const int errorCode = 429) _NOEXCEPT;

    bool remove_fd(int fd);

public:
     Connection();
    ~Connection();

    void establishConnection();
    bool add_fd(int fd, Runnable * reader, bool read = true);

    static Connection & getInstance();
};

#endif /* CONNECTION_HPP */
