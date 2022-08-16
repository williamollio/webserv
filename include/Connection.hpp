#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <map>
#include <sys/poll.h>
#include "HTTPReader.hpp"

#define NUM_FDS 2000

/// The connection end point of the server. Handles all the connections.
class Connection {
    /// First: Port --- Second: Server fd.
    std::map<int, int>        _server_fds;
    /// A mapping which maps a file descriptor to its responsible object.
    std::map<int, Runnable *> _fd_mapping;
    /// First: client fd --- Second: server fd
    std::map<int, int>         _connection_pairs;
    /// The list with all allocated HTTPReader instances for the garbage collector.
    std::list<HTTPReader *>    _readers;
    /// The timeout in milliseconds.
    int                        _timeout;
    /// The number of file descriptors inside of the polling array.
    nfds_t                     _nfds;
    /// The polling array.
    struct pollfd              _fds[NUM_FDS];
    /// The currently active connection.
    static Connection *        currentInstance;
    unsigned long              connections;

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

    /**
     * Prints the contents of the polling error if DEBUG is 2.
     */
    void printPollArray() _NOEXCEPT;

    /**
     * Denies an active connection. The connection on the given file descriptor is closed, the
     * file descriptor is afterwards removed from the polling array. An error message using the
     * given error code is sent before terminating the connection.
     *
     * @param fd The file descriptor to be removed.
     * @param errorCode The error code explaining the reason of the termination.
     */
	void denyConnection(int fd, int errorCode = 429) _NOEXCEPT;

    /**
     * Garbage collects the HTTPReader instances using a variant of the mark and sweep algorithm.
     */
    void _clean_readers();

    /**
     * Removes the given file descriptor from the polling array and removes the file descriptor
     * to Runnable mapping.
     *
     * @param fd The file descriptor to remove.
     * @return Whether the file descriptor was successfully removed.
     */
    bool remove_fd(int fd);

public:
     Connection();
    ~Connection();

    void establishConnection();

    /**
     * Adds a file descriptor to the polling array. The given file descriptor is mapped to the
     * given Runnable pointer, which must not be null. If the given file descriptor is already
     * mapped, the mapping is replaced by the given one. Note: the old Runnable is not notified
     * about the removal of the mapping.
     *
     * @param fd The file descriptor that should be added to the poll array.
     * @param reader The object that is responsible for the file descriptor.
     * @param read Whether the file descriptor should be checked for reading or writing.
     * @return If the file descriptor was successfully added to the polling array.
     */
    bool add_fd(int fd, Runnable * reader, bool read = true);

    /**
     * Returns the currently active Connection instance.
     *
     * @return The instance that is currently handling the polling loop.
     */
    static Connection & getInstance();
};

#endif /* CONNECTION_HPP */
