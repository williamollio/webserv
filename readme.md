# Webserv

[Subject PDF](https://github.com/williamollio/webserv/blob/master/subject.pdf)

# How does it work?
## Setup
- compile with ``make``
- run with ``webserv`` without any options
- pick a browser of your choice and visit the server's website by entering the address specified in the ``*.conf`` file (standard is localhost)
- have fun! (changes on the configuration file will only be applied after a server restart)

## Configuration
The server can be configured with a configuration-file. This file is either specified at the startup or will be autogenerated by the server if not specified.

### Config-File 
- currently we only support one ``server``:
  - it can be specified with the keyword ``server {...}``
  - the server allows following parameters:
    - ``server_names`` _specifies server-addresses_
    - ``cgi_ext``_specifies of the extenstion of the cgi's_
    - ``cgi_bin``_maps extensions to binaries_
    - ``cgi_methods``_methods allowed on CGI requests_
    - ``cgi_root``_specifies path in which requests are always directed_
    - ``port`` _specifies ports to listen to_
    - ``loc`` _predeclares the locations_
    - ``error_pages``_specifies the pages rendered depending on the error code returned_
    - ``accept_files``_manages the access right to upload files_
    - ``root`` _specifies the root folder of the server_
    - ``index`` _specifies the default file of the server_
    - ``upload`` _determine specific folder to store files uploaded by default_
    - ``client_max_body_size`` _specifies the size payload sent by the client
    - ``location`` can be specified further _(after declaration only!)_:
      - ``directory_listing`` _lists the files/folders of the location_
      - ``root`` _specifies the root folder of the location_
      - ``upload`` _determine specific folder to store files uploaded in the location_
      - ``default_file``_specifies the default file of the location_
      - ``methods`` **[GET / POST / DELETE]** _specifies the methods allowed of the location_
      - ``redirect`` _specifies the path where requests should be redirected to_
- comment with ``# comment to be ignored by the parser``
- a option is legal when formatted like: ``<option> : <input> , <input2> ;`` **_or_** ``<option> { <input> , <input2> }``
- mixing ``{}`` and ``:  ;`` will result in a parser error
- whitespaces and newline are handled the same
- missing any delimiter results in undefined behaviour

[Example configuration file](https://github.com/williamollio/webserv/blob/master/server.conf)
