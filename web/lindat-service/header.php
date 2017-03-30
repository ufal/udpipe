<!DOCTYPE html>
<html>
  <head>
    <title>UDPipe</title>
    <meta charset="utf-8">

    <link rel="stylesheet" href="//maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css">
    <link rel="stylesheet" href="//maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap-theme.min.css">
    <link rel="stylesheet" href="//maxcdn.bootstrapcdn.com/font-awesome/4.4.0/css/font-awesome.min.css">

    <link rel="stylesheet" href="branding/public/css/lindat.css" type="text/css" />
    <link rel="stylesheet" href="bootstrap-select.min.css" type="text/css" />
    <link rel="stylesheet" href="udpipe.css" type="text/css" />

    <script src="//code.jquery.com/jquery-1.11.3.min.js" type="text/javascript"></script>
    <script src="//maxcdn.bootstrapcdn.com/bootstrap/3.3.4/js/bootstrap.min.js"></script>
    <script src="//cdn.rawgit.com/google/code-prettify/master/loader/run_prettify.js" type="text/javascript"></script>

    <script src="bootstrap-select.min.js" type="text/javascript"></script>
    <script src="blob.min.js" type="text/javascript"></script>
    <script src="filesaver.min.js" type="text/javascript"></script>
    <script src="js-treex-view.min.js" type="text/javascript"></script>
  </head>

  <body id="lindat-services">
    <?php require('branding/header.htm')?>
    <div class="lindat-common">
      <div class="container">
        <!-- breadcrumb -->
        <ul class="breadcrumb">
          <li><a href="//lindat.mff.cuni.cz/">LINDAT/CLARIN</a></li>
          <li><a href="//lindat.mff.cuni.cz/services/">Services</a></li>
          <li class="active">UDPipe</li>
        </ul>

        <!-- Service title -->
        <h1 class="text-center">UDPipe</h1>

        <!-- menu -->
        <ul class="nav nav-tabs text-center" style="margin-bottom: 10px">
          <li <?php if ($main_page == 'info.php') echo ' class="active"'?>><a href="info.php"><span class="fa fa-info-circle"></span> About</a></li>
          <li <?php if ($main_page == 'run.php') echo ' class="active"'?>><a href="run.php"><span class="fa fa-cogs"></span> Run</a></li>
          <li <?php if ($main_page == 'api-reference.php') echo ' class="active"'?>><a href="api-reference.php"><span class="fa fa-list"></span> REST API Documentation</a></li>
        </ul>
