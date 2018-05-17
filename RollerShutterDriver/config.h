const char page_config[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <title>Roller shutter</title>
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
    .save {
        width: 100px;
        height: 40px;
        margin-top: 50px;
    }
    .reset-settings {
        margin-top: 100px;
    }
</style>
<body>
    <div class="menu">
        <a href="/">Main</a>
        <a href="/config">Config</a>
    </div>
    <hr class="hr">
    <form action="/save" method="POST">
        Wifi name: <input type="text" name="wifiname" value="{wifiname}"><br>
        Wifi pass: <input type="password" name="wifipassword" value="{wifipass}"><br>
        <input class="save" type="submit" value="Save">
    </form>

    <form class="reset-settings" action="/resetsettings" method="POST">
        <input type="submit" value="Reset settings">
    </form>
</body>
</html>
)=====";