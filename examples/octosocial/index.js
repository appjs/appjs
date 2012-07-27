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

  $info.error = function(label){
    this.removeClass('success').addClass('error');
    $label.text(label);
  };
  $info.succeed = function(label){
    this.removeClass('error').addClass('success');
    $label.text(label);
  };
  $.fn.disable = function(){
    this.attr('disabled','disabled');
  };
  $.fn.enable = function(){
    this.removeAttr('disabled','disabled');
  };

  $(window).on('keydown', function(e){
    if (e.keyCode === KEY_F12) {
      window.frame.openDevTools();
    }
  });

  $username.focus();

  $('#login-form').submit(function(e){
    e.preventDefault();

    $info.succeed('Logging in...');
    $buttons.disable();

    github.authenticate({
      type: 'basic',
      username: $username.val(),
      password: $password.val()
    });

    github.user.get({}, function(err, result) {
      if (err) {
        $info.error('Login Failed. Try Again.');
        $buttons.enable();
      } else {
        loggedIn(result);
      }
    });
  });

  function loggedIn(result){
    $info.succeed('Logged in!');
    $('#user-avatar').append('<img src="'+result.avatar_url+'" width="64" height="64">');
    $('#user-name').text(result.name);
    $('#login-section').hide();
    $('#profile-section').show();
    ['Followers', 'Following'].forEach(function(type){
      populate(type, { user: result.login });
    });
  }

  function appendAvatar(item){
    this.append('<li class="span2"><img src="'+item.avatar_url+'" width="64" height="64" title="'+item.name+'"></li>');
  }

  function populate(type, user){
    github.user['get'+type](user, function(err, result){
      if (err) return window.console.log(err);

      var container = $('#'+type.toLowerCase());
      $('.count', container).text(result.length);
      result.forEach(appendAvatar, $('.thumbnails', container));
    });
  }
});
