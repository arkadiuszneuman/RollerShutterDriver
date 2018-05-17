const char page_index[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <title>Roller shutter driver</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" type="text/css" media="screen" href="main.css" />
</head>
<style>
    body {
        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif
    }
    .menu a {
        margin-left: 30px;
    }
    .hr {
        border: 1px solid gray;
    }
    .status {
        margin: 50px;
        font-size: larger;
    }
</style>
<body>
    <div class="menu">
        <a href="/">Main</a>
        <a href="/config">Config</a>
    </div>
    <hr class="hr">
    <div class="status">{status}</div>
</body>
</html>
)=====";