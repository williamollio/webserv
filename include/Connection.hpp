#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <map>

#define NUM_FDS 200

/// The connection end point of the server. Handles all the connections.
class Connection {
    /// First: Port --- Second: Server fd.
    std::map<int, int> _server_fds;
    /// The timeout in milliseconds.
    int                _timeout;
    /// The number of filedescriptors inside of the polling array.
    nfds_t             _nfds;
    /// The polling array.
    struct pollfd      _fds[NUM_FDS];

    /// Compresses the polling array. Upon return, all removable contents are removed.
    void _clear_poll_array();

    /**
     * Returns whether the given filedescriptor is a server socket.
     *
     * @param fd The filedescriptor to check
     * @return True, if the given fd is a server socket's fd
     */
    bool _is_serving_fd(int fd);

public:
     Connection();
    ~Connection();

    void establishConnection();
};

#endif /* CONNECTION_HPP */
