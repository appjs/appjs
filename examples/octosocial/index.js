var app = require('appjs'),
    github = new (require('github'))({ version: '3.0.0' }),
    KEY_F12 = process.platform === 'darwin' ? 63247 : 123;

app.serveFilesFrom(__dirname + '/assets');

var window = app.createWindow({
  width: 460,
  height: 640,
  resizable: false,
  disableSecurity: true,
  icons: __dirname + '/assets/icons'
});

window.on('create', function(){
  window.frame.show();
  window.frame.center();
});

window.on('ready', function(){
  var $ = window.$,
      $username = $('input[name=username]'),
      $password = $('input[name=password]'),
      $info = $('#info-login'),
      $label = $info.find('span'),
      $buttons = $('input, button');

  $(window).on('keydown', function(e){
    if (e.keyCode === KEY_F12) {
      window.frame.openDevTools();
    }
  });

  $username.focus();

  $('#login-form').submit(function(e){
    e.preventDefault();

    $info.removeClass('error').addClass('success');
    $label.text('Logging in...');
    $buttons.attr('disabled', true);

    github.authenticate({
      type: 'basic',
      username: $username.val(),
      password: $password.val()
    });

    github.user.get({}, function(err, result) {
      if (err) {
        $info.removeClass('success').addClass('error');
        $label.text('Login Failed. Try Again.');
        $buttons.removeAttr('disabled');
      } else {
        loggedIn(result);
      }
    });
  });

  function loggedIn(result){
    $label.text('Logged in!');
    $('#user-avatar').append('<img src="'+result.avatar_url+'" width="64" height="64">');
    $('#user-name').text(result.name);
    $('#login-section').hide();
    $('#profile-section').show();
    ['Followers', 'Following'].forEach(function(type){
      github.user['get'+type]({ user: result.login }, populate.bind(null, type.toLowerCase()));
    });
  }

  function appendAvatar(item){
    var img = $('<img src="'+item.avatar_url+'" width="64" height="64" title="'+item.name+'">');
    var li = $('<li class="hidden span2"/>').appendTo(this).append(img);
    img.on('load', function(){
      li.removeClass('hidden');
    });
  }

  function populate(type, err, result){
    if (err) {
      window.console.log(err);
    } else {
      var container = $('#'+type);
      $('.count', container).text(result.length);
      result.forEach(appendAvatar, $('.thumbnails', container));
    }
  }
});
