# Webserv

<strong><ins>Starting day</ins></strong> : 7 June

[Subject PDF](https://github.com/williamollio/webserv/blob/master/subject.pdf)

# How does it work?
## setup
- compile with ``make``
- run with ``webserv`` without any options
- pick a browser of your choice and visit the server's website by entering the address specified in the ``*.conf`` file (standard is localhost)
- have fun!

## configuration
The server can be configured with a configuration-file. This file is either specified at the startup or will be autogenerated by the server if not specified.

### config-file 
- currently we only support one ``server``:
  - it can be specified with the keyword ``server {...}``
  - the server allows following parameters:
    - ``server_names`` _specifies server-addresses_
    - ``ports``        _specifies ports to listen to_
    - ``root``         _specifies the root folder of the server_
    - ``location`` can be specified further _(after declaration only!)_:
      - ``directory_listing`` 
      - ``default_file``
      - ``methods`` **[GET / POST / DELETE]**
    - ``error_pages``
    - ``accept_files``
    - ``cgi_path``
    - ``cgi_extension``
- comment with ``# comment to be ignored by the parser``
- a option is legal when formatted like: ``<option> : <input> , <input2> ;`` **_or_** ``<option> { <input> , <input2> }``
- mixing ``{}`` and ``:  ;`` will result in a parser error
- whitespaces and newline are handled the same
- missing any delimiter results in undefined behaviour


# Suggestions

- At least one meeting per week to restructure the planning
- Set two reviewers for each pull request
- Daily push/merge
- Communication is key

# Steps

1. Setting up the connection/sockets ✅
2. I/O Multiplexing ✅
3. HTTPReader (c.f. RFC 7230 - 7235)
4. CGI ✅
5. HTTPSender (c.f. RFC 7230 - 7235)
6. ConfigReader

# On-going tasks

- <strong>mhahn</strong> : CGI
- <strong>jkasper</strong> : ConfigurationReader / HTTPReader
- <strong>wollio</strong> : HTTPSender

# To do
- <strong> HTTPReader </strong>: POST Request on Safari & Chrome throwing error (Jascha)
- <strong> HTTPSender </strong> : Handle rules for locations/methods (William), Directory listing (William)
- <strong> HTTPReader </strong> : File creation on chunked request/without filename (William)
