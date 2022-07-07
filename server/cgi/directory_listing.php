#!/usr/bin/php
<?PHP
  // Original PHP code by Chirp Internet: www.chirpinternet.eu
  // Please acknowledge use of this code by including this header.

  function getFileList($dir)
  {
    // array to hold return value
    $retval = array();

    // add trailing slash if missing
    if(substr($dir, -1) != "/") $dir .= "/";

    // open pointer to directory and read list of files
    $d = @dir($dir) or die("getFileList: Failed opening directory $dir for reading");
    while(false !== ($entry = $d->read())) {
      // skip hidden files
      if($entry[0] == ".") continue;
      if(is_dir("$dir$entry")) {
        $retval[] = array(
          "name" => "$dir$entry/",
          "type" => filetype("$dir$entry"),
          "size" => 0,
          "lastmod" => filemtime("$dir$entry")
        );
      } elseif(is_readable("$dir$entry")) {
        $retval[] = array(
          "name" => "$dir$entry",
          "type" => mime_content_type("$dir$entry"),
          "size" => filesize("$dir$entry"),
          "lastmod" => filemtime("$dir$entry")
        );
      }
    }
    $d->close();

    return $retval;
  }
?>
Content-Type: text/html

<h1>List all files with links</h1>

<table class="collapse" border="1">
<thead>
<tr><th>Name</th><th>Type</th><th>Size</th><th>Last Modified</th></tr>
</thead>
<tbody>
<?PHP
  $dirlist = getFileList("./");
  foreach($dirlist as $file) {
    echo "<tr>\n";
    echo "<td><a href=\"{$file['name']}\">",basename($file['name']),"</a></td>\n";
    echo "<td>{$file['type']}</td>\n";
    echo "<td>{$file['size']}</td>\n";
    echo "<td>",date('r', $file['lastmod']),"</td>\n";
    echo "</tr>\n";
  }
?>
</tbody>