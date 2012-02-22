<?php
if ($_FILES["audio_file"]["size"] < 20000000 && $_FILES["config_file"]["size"] < 2000000) {
  if ($_FILES["audio_file"]["error"] > 0 || $_FILES["config_file"]["error"]) {
    echo "error: file upload failed";
  } else {
    $id_string = uniqid("aim-o-matic", true);
    $ext = pathinfo($_FILES["audio_file"]["name"], PATHINFO_EXTENSION);
    move_uploaded_file($_FILES["audio_file"]["tmp_name"], "/home/ubuntu/upload/" . $id_string . "." . $ext);
    move_uploaded_file($_FILES["config_file"]["tmp_name"], "/home/ubuntu/upload/" . $id_string . ".config");
    echo "Results will be avaiable shortly, here:<br>";
    echo "<a href='results/" . $id_string . "/'>" . $id_string . "</a>";
  }
} else {
  echo "error: file too large";
}

?>
