<?php
if ($_SERVER['SERVER_NAME'] == 'lindat.mff.cuni.cz')
  header('Location: ' . $_SERVER['HTTP_X_FORWARDED_PROTOCOL'] . '://lindat.mff.cuni.cz/services' . dirname($_SERVER['PHP_SELF']) . '/run.php', TRUE, 301);
else
  header('Location: ' . (isset($_SERVER['HTTPS']) ? 'https://' : 'http://') . $_SERVER['SERVER_NAME'] . dirname($_SERVER['PHP_SELF']) . '/run.php', TRUE, 301);
exit;
?>
